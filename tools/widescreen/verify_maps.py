#!/usr/bin/env python3
"""Check serialized maps independently of the engine authoring tool."""
import json
from pathlib import Path
from map_info import read
from layouts import LAYOUTS
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
ASSETS=ROOT/'build-wide-author/data'
def tile(m,c):return tuple((m['tilesets'].get(c[k],c[k]),c[k+1],c[k+2]) for k in range(0,12,3))
checks=[]
for s in json.loads((HERE/'stages.json').read_text()):
 n=s['stage'];src=read(ROOT/'data/maps'/(s['name']+'.map'));dst=read(ASSETS/'maps'/(s['name']+' Wide.map'))
 assert dst['width']==27
 changed={(x,y):(sx,sy) for x,y,sx,sy in LAYOUTS[n]['copies']}
 for y in range(15):
  for x in range(20):
   dx=x+3;sx,sy=changed.get((dx,y),(dx,y));sc=src['cells'][sy][sx-3];dc=dst['cells'][y][dx]
   assert tile(src,sc)==tile(dst,dc),(n,x,y,'tiles')
   assert sc[12:]==dc[12:],(n,x,y,'object')
   assert src['collision'][sy][sx-3]==dst['collision'][y][dx],(n,x,y,'collision')
   assert src['warps'][y][x]==dst['warps'][y][dx],(n,x,y,'warp')
 for a,y,kind in LAYOUTS[n]['objects']:
  x=a if a<3 else a+20
  assert dst['cells'][y][x][12]==kind,(n,x,y,'side object')
 assert len(src['hazards'])==len(dst['hazards']),(n,'hazards')
 assert [(t,x+3,y) for t,x,y in src['items']]==dst['items'],(n,'items')
 assert (ASSETS/'gfx/packs/Classic/backgrounds'/dst['background']).exists()
 checks.append({'stage':n,'center_cells_checked':300,'live_side_blocks':len(LAYOUTS[n]['objects']),'result':'pass'})
print(json.dumps(checks,indent=2));print(f'PASS: {len(checks)} maps, 6900 center cells plus authored block placements')
