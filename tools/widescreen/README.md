# Approved widescreen stage pack

23 approved conversions, plus the existing 0smw Wide. Candidate 12, Rat Race,
was dropped. `stages.json` records the source names and approved review revision;
`approved/` preserves those images. The screenshots are references, not map files.

## Runtime behavior

- 2–4 enabled players: 290 original maps, 20 by 15 tiles.
- 5–8 enabled players: 24 converted maps, 27 by 15 tiles (864 by 480).
- Both manual selection/random and Quick Game obey the same width restriction.
- All eight slots support human controllers or CPU players, with separate teams.
  Scripted tours/worlds/minigames require <=4 players.
- `platform/widescreen/data` is installed on desktop and merged into Switch RomFS.
  The upstream `data` submodule is unchanged.

## Authoring

The approved image centers began at pixel 112, a half-tile offset. Playable maps
shift the center to column 3 (pixel 96). Source center tiles, collisions, objects,
warps, hazards and map settings remain serialized game data. Side art is a custom
seven-column atlas; `layouts.py` explicitly defines its collision and live block
placements. Question/music/throw/donut blocks use engine objects, with their
painted duplicates removed. The requested center edits in candidates 3 and 9 are
copied from actual source tiles. Moving-platform maps were excluded.

1. Build the Linux test image from `platform/switch/test/linux/Dockerfile` and
   configure `build-linux-asan` with `SMW_AUTOTEST=ON` (see existing soak script).
2. Run `python3 tools/widescreen/build_assets.py` (requires Pillow). It prepares
   atlases, backgrounds, `build-wide-author/manifest.txt`, edit files, provenance,
   and a merged `build-wide-author/data` directory.
3. Start the test binary in the Docker repository mount `/src`, with
   `--datadir /src/build-wide-author/data`, `SMW_AUTHOR=/src/build-wide-author/manifest.txt`,
   offscreen SDL video and dummy audio. Use an isolated HOME with an empty
   `.local/share/supermariowar/autotest` file to skip the splash screen.
   The author hook loads, widens, serializes and fully reloads every map.
4. Run `python3 tools/widescreen/verify_maps.py`. It independently parses binary
   maps and checks all 6,900 original center cells, requested edits, live side
   blocks, shifted items and hazard counts.
5. Copy only the 23 generated `* Wide.map` files named in `stages.json` from
   `build-wide-author/data/maps/` to `platform/widescreen/data/maps/`.
6. Run `python3 tools/widescreen/author_spawn_regions.py` after copying the maps.
   This reapplies the four balanced team regions to regenerated map binaries.
7. Rebuild desktop and Switch binaries; the maps are now packaged with both.

Regeneration is deterministic from the approved images and layout file; it does
not call an image-generation API. The atlas is intentionally static except for
objects explicitly promoted to engine blocks.

## Spawn atlas

`python3 tools/widescreen/author_spawn_regions.py` partitions each wide map's
legal player-spawn cells into four distinct, map-balanced horizontal zones.
The tool updates both the no-spawn grid and the serialized runtime rectangles,
so the zones survive later editor saves. Region 0 and item spawning are unchanged.

`python3 tools/widescreen/spawn_viewer.py OUTPUT` builds the interactive static
spawn review page from the 24 shipped wide maps. It reads the serialized spawn
rectangles, copies the approved stage images and provides player-color filters,
tile-grid overlays and repeatable example eight-player rolls. The example dots
apply the game's 20-attempt / 96-pixel separation rule; they are possible rolls,
not fixed spawn points.

The runtime maps P1/P5 to region 1, P2/P6 to 2, P3/P7 to 3 and P4/P8 to 4. The deployed
owner-private page is https://smw-spawn-atlas.dongletrk.chatgpt.site.

## Verification

`SMW_CHECK_MAP_POOLS=1` on the test binary checks every browser entry and 100
manual/Quick Game random selections at player counts 2, 4, 5, 8, 4, 8.
`python3 tools/widescreen/soak_pack.py 45` runs bounded eight-CPU tests on all 23
maps, with three independent processes and fresh HOME directories. Results include
binary hashes, actual arena width/player/team counts, frames, overlap warnings,
and sanitizer reports. Optional stage IDs follow the duration.

`SMW_TEST_MODE=Greed` selects that mode in test builds. For original-map regression,
set `SMW_TEST_PLAYERS=4 SMW_TEST_ORIGINAL=1`. These hooks do not affect normal play.

2026-10-23 results: all maps loaded and rendered, full sweep 22/23 initially passed;
The Pit exposed a pre-existing ceiling-spike collision bug when Greed made a hit
nonfatal. After correcting that resolution, its targeted 90-second eight-CPU
retest passed four matches with no overlap/sanitizer warnings. Three original maps
also passed four-player regressions. 61 unit tests / 185 assertions pass, including
legacy four-slot settings and invalid-array rollback. Normal PC and Switch builds
succeed. The initial Fox test was blocked by a reservation and the isolated emulator
aborted before loading the game; Toad follow-up results are recorded below.

Hardware follow-up: Toad passed manual 4→5→8→4 selection and gameplay checks.
Application exit failed on both preview and original builds; original restored.
See `docs/widescreen-1562424909341265921/toad-validation.md` for scope and evidence.
