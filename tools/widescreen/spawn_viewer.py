#!/usr/bin/env python3
"""Build the static eight-player spawn review page from the shipped wide maps."""

import argparse
import json
import shutil
from pathlib import Path

from map_info import read


HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
MAPS = ROOT / "platform/widescreen/data/maps"
APPROVED = HERE / "approved"

COLORS = [
    (208, 32, 48, "Red"),
    (112, 184, 24, "Green"),
    (240, 184, 16, "Yellow"),
    (48, 128, 208, "Blue"),
    (128, 56, 208, "Purple"),
    (232, 104, 16, "Orange"),
    (16, 168, 184, "Cyan"),
    (232, 80, 152, "Pink"),
]


def clean_name(source: str) -> tuple[str, str]:
    if source == "0smw":
        return "0smw Wide", "72dpiarmy"
    author, title = source.split("_", 1)
    return f"{title} Wide", author


def collect():
    stages = [{"stage": 0, "name": "0smw"}] + json.loads((HERE / "stages.json").read_text())
    result = []
    for stage in stages:
        number = stage["stage"]
        source = stage["name"]
        path = MAPS / f"{source} Wide.map"
        parsed = read(path)
        title, author = clean_name(source)
        areas = [parsed["spawnareas"][region] for region in range(1, 5)]
        result.append({
            "stage": number,
            "title": title,
            "author": author,
            "file": path.name,
            "image": f"images/{number:02}.png",
            "width": parsed["width"] * 32,
            "height": 480,
            "areas": areas,
            "weights": [sum(area[4] for area in region) for region in areas],
        })
    return result


HTML = r'''<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<title>SMW Spawn Atlas</title>
<style>
:root{--ink:#10182b;--paper:#eef6ff;--panel:#fff;--sky:#65a8f4;--gold:#f4be32;--line:#b9c7dc;--muted:#5f6e84;--shadow:#18233b;--focus:#ffcf45;color-scheme:light}
*{box-sizing:border-box}html{scroll-behavior:smooth}body{margin:0;background:linear-gradient(#6fb4ff 0 190px,#d9edff 190px,#eef6ff 470px);color:var(--ink);font-family:"Trebuchet MS",Verdana,sans-serif;min-width:320px}
body:before{content:"";position:absolute;left:0;right:0;top:132px;height:92px;background:radial-gradient(ellipse at 10% 100%,#fff 0 23%,transparent 24%),radial-gradient(ellipse at 31% 100%,#fff 0 29%,transparent 30%),radial-gradient(ellipse at 57% 100%,#fff 0 21%,transparent 22%),radial-gradient(ellipse at 83% 100%,#fff 0 31%,transparent 32%);opacity:.88;pointer-events:none}
button,input{font:inherit}.shell{width:min(1500px,calc(100% - 32px));margin:auto;position:relative}.hero{padding:42px 0 26px;min-height:190px;display:grid;grid-template-columns:1fr auto;align-items:end;gap:24px}.kicker{font:800 12px/1.2 ui-monospace,SFMono-Regular,Consolas,monospace;letter-spacing:.16em;text-transform:uppercase;color:#14396c}.hero h1{margin:7px 0 6px;font:900 clamp(34px,6vw,74px)/.9 Impact,"Arial Black",sans-serif;letter-spacing:.01em;color:#fff;text-shadow:0 4px 0 #245a9c,3px 0 #245a9c,-3px 0 #245a9c,0 -3px #245a9c}.hero p{max-width:720px;margin:0;color:#12345d;font-weight:700}.hero-stat{text-align:right;background:var(--ink);color:#fff;padding:13px 16px;border:3px solid #fff;box-shadow:5px 5px 0 #245a9c}.hero-stat strong{display:block;font:900 32px/1 Impact,sans-serif;color:var(--gold)}.hero-stat span{font:700 11px/1.3 ui-monospace,monospace;text-transform:uppercase;letter-spacing:.08em}
.controls{position:sticky;top:0;z-index:10;background:rgba(238,246,255,.94);backdrop-filter:blur(10px);border:1px solid var(--line);box-shadow:0 5px 0 rgba(16,24,43,.14);padding:12px;margin:6px 0 24px}.control-row{display:flex;align-items:center;gap:10px;flex-wrap:wrap}.player-strip{display:flex;gap:6px;flex:1;min-width:560px}.player{--pc:#888;position:relative;flex:1;min-width:58px;border:2px solid var(--ink);background:#fff;padding:8px 5px 7px;color:var(--ink);font-weight:900;box-shadow:0 3px 0 var(--ink);cursor:pointer}.player:before{content:"";display:inline-block;width:10px;height:10px;border-radius:50%;background:var(--pc);border:1px solid var(--ink);margin-right:5px;vertical-align:1px}.player[aria-pressed="true"]{transform:translateY(3px);box-shadow:none;background:var(--pc);color:#fff;text-shadow:0 1px 1px #000}.player:nth-child(3)[aria-pressed="true"]{color:#161616;text-shadow:none}.tool{height:40px;border:2px solid var(--ink);background:#fff;color:var(--ink);padding:0 12px;font-weight:800}.tool.search{width:190px}.tool.button{cursor:pointer;background:var(--gold);box-shadow:0 3px 0 var(--ink)}.tool.button:active{transform:translateY(3px);box-shadow:none}.checks{display:flex;gap:14px;align-items:center;margin-top:10px;color:var(--muted);font:700 12px/1 ui-monospace,monospace}.checks label{display:flex;align-items:center;gap:6px}.checks input{accent-color:#246fc4}.readout{margin-left:auto;color:var(--ink)}
.explain{display:grid;grid-template-columns:repeat(3,1fr);gap:1px;background:var(--line);border:1px solid var(--line);margin-bottom:24px}.explain div{background:#fff;padding:13px 15px}.explain b{display:block;font:900 11px/1 ui-monospace,monospace;text-transform:uppercase;letter-spacing:.08em;margin-bottom:5px}.explain span{font-size:13px;color:var(--muted)}
.grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:22px;padding-bottom:60px}.card{background:var(--panel);border:2px solid var(--ink);box-shadow:6px 6px 0 var(--shadow);overflow:hidden;content-visibility:auto;contain-intrinsic-size:500px}.card-head{display:flex;align-items:center;gap:12px;padding:11px 13px;border-bottom:2px solid var(--ink)}.stage-no{font:900 22px/1 Impact,sans-serif;color:#2b6cbb;min-width:38px}.stage-text{min-width:0;flex:1}.stage-text h2{font:900 18px/1.05 "Arial Narrow","Trebuchet MS",sans-serif;margin:0;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}.stage-text p{font:700 10px/1.2 ui-monospace,monospace;color:var(--muted);margin:4px 0 0;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}.zone-count{font:800 10px/1.2 ui-monospace,monospace;text-align:right;color:var(--muted)}.map-wrap{position:relative;background:#000;cursor:zoom-in}.map-wrap canvas{display:block;width:100%;height:auto;image-rendering:pixelated}.map-key{display:flex;align-items:center;gap:8px;padding:9px 12px;border-top:1px solid var(--line);font:700 11px/1 ui-monospace,monospace;color:var(--muted)}.swatch{width:14px;height:14px;border:2px solid var(--ink);background:var(--selected)}.paired{margin-left:auto}
.empty{grid-column:1/-1;text-align:center;padding:80px 20px;color:var(--muted);font-weight:800}dialog{width:min(1200px,96vw);border:3px solid var(--ink);padding:0;box-shadow:10px 10px 0 var(--ink);background:#fff}dialog::backdrop{background:rgba(7,13,26,.84)}.dialog-head{display:flex;align-items:center;justify-content:space-between;padding:10px 14px;border-bottom:2px solid var(--ink);font-weight:900}.close{border:2px solid var(--ink);background:var(--gold);font-weight:900;padding:6px 12px;cursor:pointer}.detail-canvas{display:block;width:100%;height:auto;image-rendering:pixelated}
button:focus-visible,input:focus-visible{outline:4px solid var(--focus);outline-offset:2px}@media(max-width:900px){.hero{grid-template-columns:1fr}.hero-stat{display:none}.grid{grid-template-columns:1fr}.controls{position:relative}.player-strip{min-width:100%;display:grid;grid-template-columns:repeat(4,1fr)}.tool.search{flex:1}.explain{grid-template-columns:1fr}.explain div+div{border-top:1px solid var(--line)}}@media(max-width:480px){.shell{width:min(100% - 20px,1500px)}.hero{padding-top:30px}.hero h1{font-size:44px}.controls{padding:9px}.player{font-size:12px;min-width:0}.card-head{gap:7px}.stage-text h2{font-size:15px}.zone-count{display:none}}@media(prefers-reduced-motion:reduce){html{scroll-behavior:auto}}
</style>
</head>
<body>
<header class="shell hero"><div><div class="kicker">Super Mario War NX · 8-player field guide</div><h1>Spawn Atlas</h1><p>See every place each player can appear across the 24 widescreen stages. Select a player to inspect their eligible area; roll again to preview another possible eight-player opening.</p></div><div class="hero-stat"><strong>24</strong><span>playable<br>widescreen maps</span></div></header>
<main class="shell">
<section class="controls" aria-label="Spawn controls"><div class="control-row"><div class="player-strip" id="players"></div><input class="tool search" id="search" type="search" placeholder="Find a stage…" aria-label="Find a stage"><button class="tool button" id="roll">↻ Roll examples</button></div><div class="checks"><label><input id="areas" type="checkbox" checked> Eligible area</label><label><input id="gridToggle" type="checkbox"> Tile grid</label><span class="readout" id="readout"></span></div></section>
<section class="explain"><div><b>Solid color</b><span>The selected player's complete eligible spawn area, read from the shipped map.</span></div><div><b>Numbered dots</b><span>One possible eight-player opening. The game rerolls to keep players at least 96 px apart.</span></div><div><b>Balanced zones</b><span>Each map has four distinct zones with similar amounts of safe space. P1/P5, P2/P6, P3/P7 and P4/P8 share those zones.</span></div></section>
<section class="grid" id="cards" aria-live="polite"></section>
</main>
<dialog id="detail"><div class="dialog-head"><span id="detailTitle"></span><button class="close" id="closeDetail">Close</button></div><canvas class="detail-canvas" id="detailCanvas" width="864" height="480"></canvas></dialog>
<script>
const MAPS=__MAP_DATA__;
const PLAYERS=__PLAYER_DATA__;
let selected=0,rollSeed=1,showAreas=true,showGrid=false;
const images=new Map(),examples=new Map();
const cards=document.getElementById('cards'),readout=document.getElementById('readout');
const hash=s=>[...s].reduce((h,c)=>Math.imul(h^c.charCodeAt(0),16777619)>>>0,2166136261);
const rngFrom=seed=>{let a=seed>>>0;return()=>{a|=0;a=a+0x6D2B79F5|0;let t=Math.imul(a^a>>>15,1|a);t=t+Math.imul(t^t>>>7,61|t)^t;return((t^t>>>14)>>>0)/4294967296}};
function candidate(areas,rng){const total=areas.reduce((n,a)=>n+a[4],0);let pick=Math.floor(rng()*total),area=areas[0];for(const a of areas){if(pick<a[4]){area=a;break}pick-=a[4]}const [left,top,w,h]=area,aw=w*32+32,ah=h*32+32;return{x:left*32+1+Math.floor(rng()*Math.max(1,aw-22-2)),y:top*32+1+Math.floor(rng()*Math.max(1,ah-25-2))}}
function rollMap(map){const rng=rngFrom(hash(map.file)^Math.imul(rollSeed,2654435761));const placed=[];for(let p=0;p<8;p++){const areas=map.areas[p%4];let best=null,bestDistance=-1;for(let attempt=0;attempt<20;attempt++){const point=candidate(areas,rng);let nearest=1e9;for(const other of placed)nearest=Math.min(nearest,Math.max(Math.abs(other.x-point.x),Math.abs(other.y-point.y)));if(nearest>bestDistance){bestDistance=nearest;best=point}if(nearest>=96)break}placed.push(best)}examples.set(map.file,placed)}
function draw(canvas,map){const ctx=canvas.getContext('2d');ctx.imageSmoothingEnabled=false;ctx.clearRect(0,0,864,480);const img=images.get(map.file);if(img)ctx.drawImage(img,0,0,864,480);if(showGrid){ctx.strokeStyle='rgba(255,255,255,.22)';ctx.lineWidth=1;for(let x=32;x<864;x+=32){ctx.beginPath();ctx.moveTo(x+.5,0);ctx.lineTo(x+.5,480);ctx.stroke()}for(let y=32;y<480;y+=32){ctx.beginPath();ctx.moveTo(0,y+.5);ctx.lineTo(864,y+.5);ctx.stroke()}}if(showAreas){const c=PLAYERS[selected].rgb;ctx.fillStyle=`rgba(${c},.25)`;ctx.strokeStyle=`rgb(${c})`;ctx.lineWidth=2;ctx.setLineDash([7,4]);for(const [l,t,w,h] of map.areas[selected%4]){ctx.fillRect(l*32,t*32,w*32+32,h*32+32);ctx.strokeRect(l*32+1,t*32+1,w*32+30,h*32+30)}ctx.setLineDash([])}const points=examples.get(map.file)||[];points.forEach((point,p)=>{const info=PLAYERS[p];const x=point.x+11,y=point.y+12;ctx.beginPath();ctx.arc(x,y,p===selected?13:10,0,Math.PI*2);ctx.fillStyle=`rgb(${info.rgb})`;ctx.fill();ctx.lineWidth=p===selected?4:2;ctx.strokeStyle=p===2?'#101820':'#fff';ctx.stroke();ctx.fillStyle=p===2?'#101820':'#fff';ctx.font=`900 ${p===selected?12:10}px Trebuchet MS`;ctx.textAlign='center';ctx.textBaseline='middle';ctx.fillText(String(p+1),x,y+.5)})}
function rerender(){document.querySelectorAll('.card').forEach(card=>{const map=MAPS[+card.dataset.index];draw(card.querySelector('canvas'),map);const zones=map.areas[selected%4];card.querySelector('.zone-count').innerHTML=`${zones.length} AREAS<br>${map.weights[selected%4]} TILE POSITIONS`;card.querySelector('.map-key').style.setProperty('--selected',`rgb(${PLAYERS[selected].rgb})`);card.querySelector('.key-label').textContent=`P${selected+1} ${PLAYERS[selected].name} · region ${selected%4+1}`;card.querySelector('.paired').textContent=`paired with P${((selected+4)%8)+1}`})}
function makeCard(map,index){const card=document.createElement('article');card.className='card';card.dataset.index=index;card.dataset.search=`${map.title} ${map.author} ${map.file}`.toLowerCase();card.innerHTML=`<div class="card-head"><span class="stage-no">${map.stage===0?'★':String(map.stage).padStart(2,'0')}</span><div class="stage-text"><h2>${map.title}</h2><p>${map.author} · ${map.file}</p></div><div class="zone-count"></div></div><div class="map-wrap" tabindex="0" role="button" aria-label="Open ${map.title} larger"><canvas width="864" height="480"></canvas></div><div class="map-key"><span class="swatch"></span><span class="key-label"></span><span class="paired"></span></div>`;const open=()=>openDetail(index);card.querySelector('.map-wrap').addEventListener('click',open);card.querySelector('.map-wrap').addEventListener('keydown',e=>{if(e.key==='Enter'||e.key===' '){e.preventDefault();open()}});cards.append(card);const img=new Image();img.onload=()=>{images.set(map.file,img);draw(card.querySelector('canvas'),map)};img.src=map.image}
function filter(){const q=document.getElementById('search').value.trim().toLowerCase();let visible=0;document.querySelectorAll('.card').forEach(card=>{const on=card.dataset.search.includes(q);card.hidden=!on;if(on)visible++});document.querySelector('.empty')?.remove();if(!visible){const empty=document.createElement('div');empty.className='empty';empty.textContent='No stage matches that search.';cards.append(empty)}readout.textContent=`${visible} of ${MAPS.length} maps`}
function openDetail(index){const map=MAPS[index],dialog=document.getElementById('detail');document.getElementById('detailTitle').textContent=`${map.title} · P${selected+1} ${PLAYERS[selected].name}`;draw(document.getElementById('detailCanvas'),map);dialog.showModal()}
const playerStrip=document.getElementById('players');PLAYERS.forEach((p,i)=>{const b=document.createElement('button');b.className='player';b.style.setProperty('--pc',`rgb(${p.rgb})`);b.setAttribute('aria-pressed',i===0);b.textContent=`P${i+1} ${p.name}`;b.onclick=()=>{selected=i;document.querySelectorAll('.player').forEach((x,n)=>x.setAttribute('aria-pressed',n===i));rerender()};playerStrip.append(b)});
MAPS.forEach(map=>rollMap(map));MAPS.forEach(makeCard);rerender();filter();
document.getElementById('roll').onclick=()=>{rollSeed=(rollSeed+1)>>>0;MAPS.forEach(rollMap);rerender()};document.getElementById('areas').onchange=e=>{showAreas=e.target.checked;rerender()};document.getElementById('gridToggle').onchange=e=>{showGrid=e.target.checked;rerender()};document.getElementById('search').oninput=filter;document.getElementById('closeDetail').onclick=()=>document.getElementById('detail').close();document.getElementById('detail').addEventListener('click',e=>{if(e.target===e.currentTarget)e.currentTarget.close()});
</script>
</body></html>'''


def build(output: Path):
    maps = collect()
    output.mkdir(parents=True, exist_ok=True)
    images = output / "images"
    images.mkdir(exist_ok=True)
    for item in maps:
        source = APPROVED / f"{item['stage']:02}.png"
        if not source.exists():
            raise FileNotFoundError(source)
        shutil.copy2(source, images / source.name)
    players = [
        {"name": name, "rgb": f"{r},{g},{b}"}
        for r, g, b, name in COLORS
    ]
    html = HTML.replace("__MAP_DATA__", json.dumps(maps, separators=(",", ":")))
    html = html.replace("__PLAYER_DATA__", json.dumps(players, separators=(",", ":")))
    (output / "index.html").write_text(html)
    (output / "spawn-data.json").write_text(json.dumps(maps, indent=2) + "\n")
    print(f"Built {len(maps)} maps in {output}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    build(args.output.resolve())
