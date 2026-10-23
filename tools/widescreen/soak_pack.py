#!/usr/bin/env python3
"""Bounded per-map CPU regressions; keeps independent homes, logs and frames."""
import concurrent.futures,json,os,re,subprocess,sys,hashlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
OUT=Path(subprocess.check_output(['mktemp','-d',str(ROOT/'build-wide-soak.XXXXXX')],text=True).strip())
rows=json.loads((ROOT/'tools/widescreen/stages.json').read_text())
seconds=int(sys.argv[1]) if len(sys.argv)>1 else 35
players=int(os.environ.get('SMW_TEST_PLAYERS','8'))
original=os.environ.get('SMW_TEST_ORIGINAL')=='1'
stages=set(map(int,sys.argv[2:]))
if stages:rows=[s for s in rows if s['stage'] in stages]
print(f'OUTPUT {OUT}',flush=True)
def run(s):
 n=s['stage'];out=OUT/f'{n:02}';home=out/'home/.local/share/supermariowar';home.mkdir(parents=True)
 (home/'autotest').write_text(f'fast\nplayers={players}\nmap={s["name"]}{"" if original else " Wide"}\n')
 inner='/src/'+str(out.relative_to(ROOT));log=out/'smw.log'
 cmd=['docker','run','--rm','-u',f'{os.getuid()}:{os.getgid()}','-e',f'HOME={inner}/home','-e',f'SMW_TEST_MODE={os.environ.get("SMW_TEST_MODE","")}','-e','SDL_VIDEO_DRIVER=offscreen','-e','SDL_AUDIO_DRIVER=dummy','-e','ASAN_OPTIONS=detect_leaks=0:abort_on_error=1','-e','UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1','-e',f'SMW_CAPTURE_DIR={inner}','-v',f'{ROOT}:/src','-w','/src','smw-linux-test','timeout','--signal=TERM','--kill-after=5',str(seconds),'build-linux-asan/smw','--datadir','/src/build-wide-author/data']
 with log.open('w') as f:p=subprocess.run(cmd,stdout=f,stderr=subprocess.STDOUT)
 text=log.read_text(errors='replace');
 arena=re.findall(r'\[match\] arena-width=(\d+) players=(\d+) teams=(\d+)',text)
 result=dict(stage=n,players=players,original=original,arena=arena,mode=os.environ.get('SMW_TEST_MODE','random'),binary_sha256=hashlib.sha256((ROOT/'build-linux-asan/smw').read_bytes()).hexdigest(),code=p.returncode,matches=len(re.findall(r'\[match\] map=',text)),warnings=text.count('[autotest][warning]'),sanitizer=bool(re.search('ERROR: AddressSanitizer|runtime error:|AddressSanitizer:DEADLYSIGNAL',text)),frames=len(list(out.glob('frame-*.png'))),path=str(out))
 result['passed']=p.returncode in (0,124) and result['matches']>0 and result['frames']>0 and bool(arena) and all(int(w)==(20 if original else 27) and int(p)==players for w,p,_ in arena) and not result['warnings'] and not result['sanitizer']
 print(json.dumps(result),flush=True);return result
with concurrent.futures.ThreadPoolExecutor(max_workers=3) as pool:results=list(pool.map(run,rows))
(OUT/'results.json').write_text(json.dumps(results,indent=2)+'\n')
print(f'RESULT {sum(r["passed"] for r in results)}/{len(results)} pass',flush=True)
sys.exit(0 if all(r['passed'] for r in results) else 1)
