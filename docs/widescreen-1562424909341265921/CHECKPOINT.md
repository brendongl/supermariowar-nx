# Conversation 1562424909341265921 — current checkpoint

Repo /home/brendongl/projects/supermariowar-nx, eight-players; feature commit
1e432607 and exit-fix commit 59c079e6.
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
The original application exit showed a software-closed error on BOTH preview and original
pre-test NRO. Null Data Abort; preview symbols point to GPU initialization in
main_game. Root cause confirmed from NTON nx-hbloader source: after the NRO returns,
an empty next-load path makes the forwarder reread its embedded `nextNroPath` and
launch the game again. The second Mesa/EGL initialization crashes in `tmemCreate`.
No FPS evidence.

Round 1 attempted to perform platform cleanup and call libnx `svcExitProcess()`,
terminating the NTON forwarder process instead of returning to its relaunch loop.
That Switch Release candidate's SHA256 was
`3a82641ecf3b76c3fde4d5c1c428df02bd05741f26b7d55984c17fda10d7e39c`.
Hardware round 1 FAILED: no new Atmosphere crash report was produced, confirming
the relaunch/GPU crash stopped, but Horizon still displayed its software-closed
error because direct `svcExitProcess()` skipped libnx's applet shutdown handshake.
Hardware round 2 FAILED the same visible way and also created no crash report:
calling only `appletExit()` was not a complete libnx shutdown. Round 3 calls the
full libnx `__appExit()` routine (filesystem, time, HID, applet, and service-manager
cleanup) before process termination; it also FAILED with the same Horizon message
and no crash report. The missing mechanism is now verified in official libnx and
Homebrew Menu sources: forwarded NROs set `__nx_applet_exit_mode = 1` and return
normally, allowing libnx's standard cleanup to perform the application self-exit
handshake. Round 4 uses that mechanism. Build and hardware evidence are still
recorded below.

Round 4 PASSED. The final fix sets libnx `__nx_applet_exit_mode = 1` during normal
Switch cleanup and returns normally. Candidate NRO SHA256
`421cce9bb1807d3d1f4d5f1dc3b66b5573e738c7a883043722ed8c7aaf03668b`
passed two launch/splash/main-menu/B-exit cycles on Toad, each returning directly
to HOME with no error and no new crash report. An eight-player Jungle Bridge Wide
smoke test logged `arena-width=27 players=8 teams=8`; the next normal exit also
passed, making three passing exit cycles total. Rebuilt PC suite passes 61 tests / 185 assertions. Switch Release build
passes. Evidence: `build-exit-test.Xf0kPw` and `toad-validation.md`.

## Cleanup confirmed

Original Toad NRO restored and hash verified d8e8432069695ca03f62f9c8264e720052bc1a7fd2a79faa1b6b11d0bbd0ebdc.
HOME idle visually verified. Test-created settings and temp NRO removed; virtual
controller cleared/detached. Lease 9d66ab7d-86cd-46f8-b616-f23e05fc3133 was
completed. No active helpers, processes, leases, or follow-ups.
Game Lab #4 remains open.
Raw evidence/backup: build-toad-test.mUDoYr. Current preview artifact remains local
in build-wide-delivery.dbUO9l, not installed on Toad. Final preview NRO hash
3a78897f2ce90786fa7350a98600f1fb9f0d3fb1b8b8221477f0d4a573879e3a.

## Next action

Report the verified result. No public push/release is authorized. Physical
four-controller pairing remains missing evidence, not a failed test. Toad was
authorized for this test only; no blanket cafe fleet permission exists.
