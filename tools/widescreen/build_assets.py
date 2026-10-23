#!/usr/bin/env python3
"""Prepare approved side art and a manifest for SMW_AUTHOR (engine serializer).
Run from any directory; Pillow is the only dependency. Does not invoke image AI.
"""
from pathlib import Path
import hashlib,json,struct
from PIL import Image
from layouts import LAYOUTS, ERASE_OFFSETS, CLEAR_RECTS, ART_COPIES
from map_info import read
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
OUT=ROOT/'build-wide-author'
OUT.mkdir(exist_ok=True)
ASSETS=ROOT/'platform/widescreen/data'
ASSETS.mkdir(parents=True,exist_ok=True)
def mapx(a):return a if a<3 else a+20
manifest=[];report=[]
for s in json.loads((HERE/'stages.json').read_text()):
    n=s['stage'];layout=LAYOUTS[n];name=s['name'];source=ROOT/'data/maps'/f'{name}.map'
    info=read(source);assert info['width']==20
    approved=Image.open(HERE/'approved'/f'{n:02}.png').convert('RGB');assert approved.size==(864,480)
    # Move the original from x112 to x96. Extend the outer right by half a tile.
    # Registration left a narrow black border on some approved PNGs. Never
    # repeat that border into the arena: reflect the last valid art pixels.
    # Winterland has a masked bottom section, so its registration border
    # is black for only 416 rows and needs an explicit measured cut.
    cut={7:853}.get(n,864)
    while cut>832 and sum(max(p)<10 for p in approved.crop((cut-1,0,cut,480)).getdata())>432:
        cut-=1
    aligned=Image.new('RGB',(864,480));aligned.paste(approved.crop((16,0,cut,480)),(0,0))
    tail=864-(cut-16)
    aligned.paste(approved.crop((cut-tail,0,cut,480)).transpose(Image.Transpose.FLIP_LEFT_RIGHT),(cut-16,0))
    bg=Image.open(ROOT/'data/gfx/packs/Classic/backgrounds'/s['background']).convert('RGB')
    assert bg.size==(640,480)
    widebg=Image.new('RGB',(864,480));widebg.paste(bg,(96,0))
    widebg.paste(bg.crop((0,0,96,480)).transpose(Image.Transpose.FLIP_LEFT_RIGHT),(0,0))
    widebg.paste(bg.crop((512,0,640,480)).transpose(Image.Transpose.FLIP_LEFT_RIGHT),(736,0))
    bgname=f'{Path(s["background"]).stem}_Review{n:02}Wide.png'
    widebg.save(ASSETS/'gfx/packs/Classic/backgrounds'/bgname)
    atlas=Image.new('RGB',(224,480));atlas.paste(aligned.crop((0,0,96,480)),(0,0));atlas.paste(aligned.crop((736,0,864,480)),(96,0))
    types=[[0]*7 for _ in range(15)]
    for x0,y0,x1,y1,t in layout['rects']:
        assert 0<=x0<=x1<7 and 0<=y0<=y1<15 and 0<=t<=19
        for y in range(y0,y1+1):
            for x in range(x0,x1+1):types[y][x]=t
    clean=Image.new('RGB',(224,480));clean.paste(widebg.crop((0,0,96,480)),(0,0));clean.paste(widebg.crop((736,0,864,480)),(96,0))
    for box in CLEAR_RECTS.get(n,[]):atlas.paste(clean.crop(box),box[:2])
    for a,y,sx,sy in ART_COPIES.get(n,[]):
        atlas.paste(approved.crop((112+(sx-3)*32,sy*32,144+(sx-3)*32,(sy+1)*32)),(a*32,y*32))
    edits=[]
    for a,y,kind in layout['objects']:
        x=mapx(a)
        # All live blocks are drawn by the game, never left painted in the atlas.
        dx,dy=ERASE_OFFSETS.get(n,{}).get((a,y),(0,0))
        # Clip to the same side: do not touch the preserved original middle.
        lo,hi=(0,96) if a<3 else (96,224)
        box=(max(lo,a*32+min(0,dx)),max(0,y*32+min(0,dy)),min(hi,(a+1)*32+max(0,dx)),min(480,(y+1)*32+max(0,dy)))
        atlas.paste(clean.crop(box),box[:2])
        types[y][a]=0
        templates=[(sx+3,sy) for sy,row in enumerate(info['cells']) for sx,c in enumerate(row) if c[12]==kind and not c[13]]
        if templates:
            sx,sy=templates[0];edits.append(f'object {x} {y} {sx} {sy}')
        else:
            assert kind!=1;edits.append(f'block {x} {y} {kind} 0')
    for dx,dy,sx,sy in layout['copies']:
        edits.append(f'copy {dx} {dy} {sx} {sy}')
    setname=f'WideReview{n:02}';folder=ASSETS/'gfx/packs/Classic/tilesets'/setname;folder.mkdir(exist_ok=True)
    for fn,size in [('large.png',(224,480)),('medium.png',(112,240)),('small.png',(56,120))]:
        atlas.resize(size,Image.Resampling.NEAREST).save(folder/fn)
    vals=[c for row in types for c in row];(folder/'tileset.tls').write_bytes(struct.pack('<106i',105,*vals))
    (OUT/f'{n:02}.edits').write_text('\n'.join(edits)+'\n')
    # Paths inside the build container, rooted at the mounted repository.
    output=f'maps/{name} Wide.map'
    manifest.append('|'.join([f'maps/{name}.map',output,bgname,setname,f'/src/build-wide-author/{n:02}.edits']))
    report.append(dict(stage=n,output=output,source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),approved_sha256=hashlib.sha256((HERE/'approved'/f'{n:02}.png').read_bytes()).hexdigest(),collision_cells=sum(c!=0 for c in vals),live_side_blocks=len(layout['objects']),center_edits=layout['copies']))
(OUT/'manifest.txt').write_text('\n'.join(manifest)+'\n')
(OUT/'provenance.json').write_text(json.dumps(report,indent=2)+'\n')
print(f'Prepared {len(report)} stage atlases and authoring manifests')

# Build a self-contained desktop test data directory without changing the upstream submodule.
import shutil
for source in (ROOT/'data', ROOT/'platform/switch/data', ASSETS):
    shutil.copytree(source,OUT/'data',dirs_exist_ok=True,ignore=shutil.ignore_patterns('.git'))
