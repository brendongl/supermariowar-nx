"""Read-only inspection of the 1.8 map fields used by the authoring checks."""
import struct
from pathlib import Path
class Reader:
    def __init__(self,p):self.data=Path(p).read_bytes();self.pos=0
    def take(self,n):v=self.data[self.pos:self.pos+n];assert len(v)==n;self.pos+=n;return v
    def i(self):return struct.unpack('<i',self.take(4))[0]
    def string(self):return self.take(self.i()).rstrip(b'\0').decode()
def read(p):
    r=Reader(p);ver=tuple(r.i() for _ in range(4));assert ver[:2]==(1,8)
    w=r.i() if ver[2]>=1 else 20
    summary=[r.i() for _ in range(13)];sets={}
    for _ in range(r.i()):
        idx=r.i();sets[idx]=r.string()
    cells=[[struct.unpack('<14b',r.take(14)) for x in range(w)] for y in range(15)]
    bg=r.string();switches=[r.i() for _ in range(4)]
    platforms=r.i();assert platforms==0, 'moving platforms unsupported'
    items=[tuple(r.i() for _ in range(3)) for _ in range(r.i())]
    hazards=[r.take(52) for _ in range(r.i())]
    eye=[r.i() for _ in range(3 if ver[2]>=1 or ver[3]>=2 else 1)];music=r.i()
    collision=[];warps=[];spawn=[];spawn_offsets=[]
    for y in range(15):
        c=[];wa=[];s=[];so=[]
        for x in range(w):
            c.append(r.i());wa.append(tuple(r.i() for _ in range(3)))
            so.append(r.pos);s.append(list(r.take(6)))
        collision.append(c);warps.append(wa);spawn.append(s);spawn_offsets.append(so)
    # Supplemental switch state, warp exits, then the six serialized spawn-area sets.
    # Each spawn area stores tile-space left/top/width/height and its selection weight.
    for _ in range(r.i()):r.take(3)
    for _ in range(r.i()):r.take(40)
    spawnareas_start=r.pos;spawnareas=[]
    for _ in range(6):
        spawnareas.append([tuple(r.i() for _ in range(5)) for _ in range(r.i())])
    assert all(0<=c<=19 for row in collision for c in row)
    return dict(width=w,version=ver,cells=cells,collision=collision,warps=warps,
                nospawn=spawn,spawnareas=spawnareas,tilesets=sets,background=bg,
                items=items,hazards=hazards,_data=r.data,
                _spawn_offsets=spawn_offsets,_spawnareas_start=spawnareas_start,
                _spawnareas_end=r.pos)
if __name__=='__main__':
    import sys
    m=read(sys.argv[1]);print(m['tilesets'],m['background'])
    for y in range(15):print(f'{y:2}', ''.join('Q' if c[12]==1 else 'M' if c[12]==3 else 'B' if c[12]>=0 else '.#=ID^v<>i!!!XXXXXXX_'[t] for c,t in zip(m['cells'][y],m['collision'][y])))
