# Conversation 1562424909341265921 — current checkpoint

Repo /home/brendongl/projects/supermariowar-nx, eight-players; feature commit1e432607.
23 approved conversions (+0smw Wide=24), Rat Race excluded. No public push/release.
Pre-existing servers.toml remains untracked and untouched. No helpers.

## Completed PC/build gates

- Preserved approved v2/v3/v4 references, names and revisions in tools/widescreen.
- Authored 23 binary 27x15 maps using engine serializer. 0smw Wide now shares the same overlay: 24 wide maps total.
- Separate live blocks and collision data; original center preserved except requested music/yellow-block edits. Independent verification checks 6,900 center cells, source items/warps/hazards, live side blocks.
- Manual picker/random and Quick Game width filtering implemented and integration-tested at 2,4,5,8,4,8 players (290 normal / 24 wide).
- Eight-slot selector: slots 5–8 CPU-only, random skins, individual teams. Scripted tours/worlds/minigames hidden above four. Controller input still limited to four humans.
- Full 23-map eight-CPU ASan/UBSan sweep: 22 initially passed; The Pit failed overlap detection in Greed. Fixed nonfatal ceiling spike resolution, targeted 90-second retest passed four matches. No sanitizer errors in the full sweep.
- Three original maps passed four-player regressions. Two maps passed five-player boundary tests (three matches each). Winterland final-art retest passed two eight-CPU matches.
- Removed right registration strips, including partially masked Winterland strip. Actual game captures in gameplay-preview.jpg.
- 61 unit tests / 185 assertions pass. Fixed discovered FileIO dangling filename and transactional legacy four-slot TOML parsing.
- Normal PC build and Switch NRO build succeed. Fixed NRO asset dependency so image/map-only changes actually repack RomFS. Verified all 140 shared asset files occur byte-for-byte in the final NRO.


## Hardware gate — latest

User authorized Toad explicitly. Test details/evidence: toad-validation.md.
PASS: exact NRO hash on console; manual 4→5→8→4 transition, original/wide map
selection and live 4/5/8-player gameplay. Two eight-player wide maps exercised.
Remote virtual-controller input tested; physical four-controller pairing NOT tested.
FAIL: application exit shows software-closed error on BOTH preview and original
pre-test NRO. Null Data Abort; preview symbols point to GPU initialization in
main_game. Forwarder/reload involvement suspected, not established. No FPS evidence.

## Cleanup confirmed

Original Toad NRO restored and hash verified d8e8432069695ca03f62f9c8264e720052bc1a7fd2a79faa1b6b11d0bbd0ebdc.
HOME idle visually verified. Test-created settings and temp NRO removed; virtual
controller cleared/detached, lease8af5c9d8-d6ba-40a4-a3c9-e9ef07b97308 completed.
No active helpers, processes, leases or follow-ups. Game Lab #4 remains open.
Raw evidence/backup: build-toad-test.mUDoYr. Current preview artifact remains local
in build-wide-delivery.dbUO9l, not installed on Toad. Final preview NRO hash
3a78897f2ce90786fa7350a98600f1fb9f0d3fb1b8b8221477f0d4a573879e3a.

## Next action

Investigate/reproduce the existing application-exit/reload failure before release.
Do not conflate this failure with the passing map conversion tests. Obtain a new
lease before device actions; Toad was authorized for this test, no blanket cafe
fleet permission. Isolated Eden startup gate previously blocked; do not repeat
unavailable probes. Toad1.5.0 lacks /files/move, /process/list, /crash-reports.
