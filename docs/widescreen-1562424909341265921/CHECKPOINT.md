# Conversation 1562424909341265921 — current checkpoint

Repo /home/brendongl/projects/supermariowar-nx, eight-players; wide-pack commit
1e432607, exit-fix commit 59c079e6 and UI/Quick Start commit 5593e6ff.
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

Implement the approved UI mockups. No public push/release is authorized. Physical
four-controller pairing remains missing evidence, not a failed test. Toad was
authorized for this project test only; no blanket cafe fleet permission exists.

## UI implementation gate — 2026-10-23

- Implemented a reusable pixel UI layer and genuine 864x480 rendering for the
  welcome, fighter-select and match-setup states. Legacy submenus stay at 640x480.
- Welcome now has the approved button plate, eight live fighter sprites, player
  count/controller hint, credits footer and cafe mark.
- Fighter select now renders eight live cards, eight team colours, ready/CPU state,
  skin names and an X shortcut that adds/removes CPU slots 5–8.
- Match setup now has the approved mode list, native 16:9 wide-stage preview,
  separate rules controls and footer hints. The controls remain functional rather
  than being baked into a screenshot.
- Normal PC build passes. ASan menu-capture run produced all three target screens
  without sanitizer/runtime/SDL errors. Evidence: `build-ui-capture.SO6HUf`.
- Files changed: GSSplashScreen.cpp, GSMenu.cpp, GameSettingsMenu.cpp,
  TeamSelectMenu.cpp, MI_TeamSelect.cpp/.h, MI_MapPreview.cpp,
  ui/ModernMenu.cpp/.h, ui/CMakeLists.txt, this checkpoint.
- Missing evidence: input/navigation regression, sustained ASan gameplay, Switch
  build and Toad hardware rendering. These are not failed tests.
- No active helpers. Next: implement the approved Quick Start/play-choice/reveal
  flow, then run navigation, sanitizer, Switch-build and Toad gates.

## Quick Start and navigation gate — 2026-10-23

- Added the approved post-roster play-choice screen with Quick Start, Custom Match
  and Tournament/Tour routes. Quick Start is the default; Y rerolls and X opens
  the full settings screen.
- Added the three-second reveal screen with the selected native 16:9 stage,
  one of the eight approved modes, plain-language rules, A start-now, Y reroll
  and B back. The revealed stage is now the stage that gameplay actually loads.
- Quick Start chooses only Classic, Frag, Coins, Stomp, Star, KOTH, Frenzy or
  Survival. Existing player-count map filtering keeps 5–8 players on the 24
  approved wide maps.
- Fixed a navigation defect found during validation: Down from Quick Start first
  jumped to Tournament/Tour instead of Custom Match.
- Deterministic eight-player captures pass for both routes. Reviewed play-choice,
  reveal and setup renders: `build-ui-flow-quick.aj9UBJ` and
  `build-ui-flow-custom.4aaTOm`.
- Automated sanitizer navigation passed this whole route: B back to fighters →
  roster again → X settings → B back → Y reveal → Y reroll → B back → A Quick
  Start → eight-player wide gameplay → main menu. It emitted `[ui-nav] complete`,
  exited zero and had zero ASan/UBSan findings. Evidence: `build-ui-nav2.sI7IXc`.
- 61/61 unit tests pass. A first long-soak invocation used the base PC data folder,
  which has no wide maps, and stopped with `No maps available for this player
  count`; this is a failed harness/data setup, not a failed game test or sanitizer
  finding. The corrected merged-data soak is running next.
- The `smw` ASan target compiles and links. An attempted all-target build reached
  the unrelated level-editor link and failed on existing RTTI symbols; this is
  not a game-target test failure, and no claim is made yet about whether it is a
  baseline editor issue.
- Files additionally changed: `MenuCode.h`, `uimenu.h`, `PlayChoiceMenu.cpp/.h`,
  `menu/CMakeLists.txt`, `GSMenu.cpp/.h`.
- No active helpers. Next: unit tests, sustained sanitizer gameplay, Switch build,
  then Toad visual/input/gameplay/exit validation and restoration.

## UI gameplay soak gate — 2026-10-23

- Merged-data eight-player ASan/UBSan soak passed 13 Quick Start matches in 180
  seconds with zero sanitizer findings and zero stuck warnings. Evidence:
  `build-ui-soak-merged.kOzBLz`.
- That cache only contained 0smw Wide, so it proves the repeated UI/gameplay loop
  but not random selection across the whole approved pool. A fresh merged data
  tree was created without replacing shared paths and verified to contain 24 wide
  maps, 24 wide backgrounds and 23 per-stage wide tilesets. A final soak against
  that tree is the next gate.
- Fixed series compatibility before the final build: Tournament/Tour/World/
  Minigame roster confirmation continues into its legacy setup instead of looping
  back to play choice; series selection also refreshes the >4-player restrictions.
- Final fresh-data ASan/UBSan soak passed 14 Quick Start matches in 180 seconds,
  randomly exercising 0smw, Boo is Back, The hotter the better, Sky Garden,
  ssbmjunglejapes, mountain waterfall, Block! Block!, Ninja Gaiden, Haunted
  Temple and other approved wide stages. All matches reported width 27 and eight
  players; zero sanitizer findings and zero stuck warnings. Evidence:
  `build-ui-soak-final.GXwY28`.
- Remaining missing evidence: full pool-transition check is still running;
  Switch build and Toad hardware rendering/input/gameplay/exit are not yet tested.

## Final UI hardware gate — 2026-10-23

- Final Switch Release NRO built successfully. SHA256:
  `dece0fdc44761db5573d72039d39f68af9feb8a33a62f40f6d12d951a4494d89`.
  The downloaded deployed copy matched that hash exactly.
- Toad rendered the new welcome screen, eight-card fighter screen, play-choice
  screen, native 16:9 match setup and reveal screen correctly. The tested route
  covered B back to fighter select, X to full settings, B back, Y reveal, Y
  reroll and A start-now.
- Hardware validation found that Switch's alternate menu action was still bound
  to L. The final build binds it to physical X and keeps L as a compatibility
  alias. X then opened Match Setup on Toad as shown in `final2-x-settings.jpg`.
- Hardware validation also found the initial reveal descriptions clipped. They
  were shortened and the final two reveal captures fit cleanly. Y changed the
  selection from Jungle Wide / Frag Limit to Boo is Back Wide / Stomp, and the
  match then loaded Boo is Back Wide with all eight HUD slots. Evidence:
  `build-ui-toad.NN2ac8/final2-reveal1.jpg`, `final2-reveal2.jpg` and
  `final2-gameplay.jpg`.
- The exact final candidate closed from live gameplay through HOME/X/A and
  returned to HOME without an error dialog or relaunch. This adds a clean-exit
  pass for the final UI binary to the three earlier exit-fix passes.
- Toad was restored to its original NRO and its SHA256 was reverified as
  `d8e8432069695ca03f62f9c8264e720052bc1a7fd2a79faa1b6b11d0bbd0ebdc`.
  Test-created settings were removed, HOME was visually checked, the virtual
  controller was cleared/detached, and lease
  `1380599b-2856-41d7-be11-3382b74e79d2` was completed.
- Verified results: 61/61 unit tests (185 assertions), full automated sanitizer
  navigation, 14-match fresh-data eight-player sanitizer soak, Switch Release
  build, exact deployed hash, final hardware navigation/reroll/gameplay/exit and
  restoration all pass.
- The first fresh-data pool command omitted the required `autotest` marker, so it
  waited at Welcome for 35 minutes without reaching an assertion. It was stopped;
  this was a failed harness setup, not a game failure. The corrected run used a
  new temporary HOME and passed under ASan/UBSan: 2→4→5→8→4→8 players exposed
  290→290→24→24→290→24 maps, and every browser entry plus 100 manual and 100
  Quick random selections per count had the correct 20/27-tile width. Evidence:
  `build-ui-pools-fixed.Oqy0Dx/run.log`.
- Missing evidence, not failures: physical four-controller pairing and sustained
  hardware frame timing.
- No helpers, active test processes, or console leases. Next: commit the UI
  changes, and do not push or release.

## User controller-test deployment — 2026-10-23

- At Brendon's request, installed final NRO
  `dece0fdc44761db5573d72039d39f68af9feb8a33a62f40f6d12d951a4494d89`
  on Toad and verified the on-console hash.
- Confirmed the game directory has no `autotest`, `options.toml` or injected
  `controls.sdl2.bin`; normal human-controller play is active.
- Launched successfully to the new Welcome screen. Cleared/detached the virtual
  controller (`controllerAttached:false`) so it cannot occupy a player slot.
- Candidate is intentionally left installed and running for Brendon's physical
  controller test. Lease `56ade542-8acf-4d00-8759-659a413468e1` was completed.
- Remaining evidence: Brendon's physical-controller result. No active helpers,
  test processes or console leases. No public push/release.
