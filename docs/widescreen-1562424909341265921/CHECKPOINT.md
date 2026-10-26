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

## Eight physical-controller UI gate — 2026-10-23

- Expanded local input from four to eight controller slots. On Switch, controller
  slot N now remains mapped to player N for all eight human players; network play
  is still explicitly capped at four.
- Replaced the legacy player type strip with eight PLAYER / CPU / NONE cards.
  Only controller 1 can edit this roster, and the roster always retains at least
  two active slots and one human.
- Fighter select now limits choice to 16 recognizable characters. L/R changes
  fighter, ZL/ZR changes colour, Y randomizes within the curated list, and every
  human controller owns its own card. Dice Guy was renamed Rollie in the asset,
  docs and UI; Rollie's name has a large gold treatment.
- Player Setup, match/series setup, stage browser, mode options, Play Choice and
  Quick Reveal are Player-1-only. Quick Reveal has no timer or automatic start
  path; it remains until P1 presses A.
- Deterministic eight-human ASan/UBSan navigation passed: P2 input was rejected
  on protected menus; P8 changed fighter and colour through shoulder actions;
  all eight players readied; the reveal waited at least 240 frames; P2 A was
  ignored; P1 A started a 27-wide match with 8 players and 8 teams. No sanitizer
  finding. Evidence: build-eight-flow.5Fm2ic and its menu/gameplay captures.
- Visual review passed for Player Setup, all-eight fighter cards, highlighted
  ROLLIE treatment and the non-countdown Quick Reveal.
- Two earlier test invocations failed only because their fresh data staging
  omitted or misaddressed `platform/widescreen/data`; both stopped with `Empty
  map directory` / `No maps available for this player count`. These were harness
  setup failures, not game failures or missing sanitizer evidence.
- Files changed: README.md; Rollie skin rename; switch side-panel generator;
  GameValues/input/uimenu; GSMenu; Main/Match/Game/Mode/MapFilter/PlayChoice
  menus; MI_PlayerSelect and MI_TeamSelect; this checkpoint. `servers.toml`
  remains pre-existing, untracked and untouched.
- Active work: normal PC build and unit suite. Missing evidence, not failures:
  Switch Release build and physical eight-controller validation on Toad.
- Next: finish unit tests, build a fresh Switch NRO, deploy it to authorized Toad
  through switchctl, and leave normal controller play enabled for Brendon.
## Final Switch build gate — 2026-10-23

- Verified: fresh Release `smw_nro` build completed successfully in `build-switch-eight.i6QEdz`.
- Artifact: `src/smw/smw.nro`, 64,775,803 bytes, SHA-256 `e19c825310dbbd969e66c046444041a0010a4aeb918a15d705607b78058260cc`.
- Verified: RomFS contains `SipNPlay_Rollie.png`, excludes the old Dice Guy filename, and the exact Rollie PNG payload is embedded in the NRO.
- Verified: `git diff --check` passes. Existing PC evidence remains 61/61 CTest, deterministic eight-human ASAN/UBSAN flow, and eight-CPU ASAN/UBSAN soak.
- Remaining evidence: install this exact NRO on Toad and physically validate eight paired controllers. This is missing hardware evidence, not a failed test.
- Files changed: the eight-controller/UI files listed in the preceding gate; `servers.toml` remains untouched and untracked.
- Active helpers: none. Active build jobs: none.
- Next action: reserve Toad, close the old build safely, install and hash-verify this NRO, clear automation/control overrides, launch normally, and leave it ready for Brendon.

## Toad deployment gate — blocked by console connectivity

- Reserved Toad under lease `c69cfb35-5d71-4a77-8c08-714f1385f5d3` and captured HOME before writing anything.
- The managed API then returned empty replies. The NRO upload failed with `Broken pipe` after **0 bytes**, so the installed file was not changed or partially overwritten.
- A broker refresh confirmed Toad changed from `horizon` to `offline`; a later fleet status check still showed it offline.
- Missing evidence: final NRO installed/hash-verified and physical eight-controller behavior. No new game test failed because the candidate never reached the console.

## Four/eight-player pairing flow implementation gate — 2026-10-25

- The normal local flow now starts in four-player mode. Player 1 can press `+`
  or `-` on Player Setup to request eight-player mode; the Switch build invokes
  the native controller-support applet for 5–8 controllers before exposing all
  eight roster slots. Pressing `+` or `-` again returns to four-player mode and
  clears slots 5–8.
- Player Setup is a dedicated P1-only roster screen. Four-player mode uses four
  large cards; eight-player mode uses eight compact cards. The fighter screen
  adapts from one centered row for up to four active players to a centered 4x2
  grid for five through eight, and its status now reads `N PLAYERS / N READY`.
- Verified: fresh normal and ASan/UBSan PC builds completed, all 61 CTest tests
  passed, and the deterministic visible flow passed under the sanitizers from
  default four-player setup through the
  eight-player roster, eight independently controlled fighter cards, P1-only
  Play Choice and Quick Reveal, and an eight-player wide-stage match. Player 2
  was rejected on protected screens and Player 8 changed fighter and colour.
  The bounded sanitizer run ended by timeout during healthy gameplay after
  `[eight-input] complete`, with no ASan/UBSan or failure marker. Evidence:
  ignored build artifacts `build-ui-demo/eight-input-{normal,asan}.log`.
- One initial dynamic run lacked the widescreen data overlay and correctly
  stopped with `No maps available for this player count`. This was a failed
  test-data setup, not a game failure. A fresh data bundle with the overlay
  passed. The bounded final run ended by the test timeout during healthy
  gameplay after logging `[eight-input] complete`; it had no failure marker.
- Files changed: `README.md`, `GameValues`, input handling, `MenuCode`, `GSMenu`,
  `MainMenu`, `MI_PlayerSelect`, `MI_TeamSelect`, and Switch platform pairing
  integration. `servers.toml` remains pre-existing, untracked and untouched.
- Missing evidence, not failures: compiling the current code as an NRO (the
  `/opt/devkitpro` mount is currently unavailable), seeing the native pairing
  applet on hardware, and validating eight physical controllers.
- Active helpers: none. Active test processes and console leases: none.
- The requested 19.3-second demonstration video was rendered from the verified
  captures, checked at every segment, and saved as the ignored artifact
  `build-ui-demo/eight-player-flow-demo.mp4` (671,435 bytes, SHA-256
  `d84a3eb4a9707c0f163e4c811e7dc750f792b2c12804a80df68025762423c45c`).
- GitHub gate: implementation and README were committed as `90bf3e99` (`Add
  native four-to-eight player setup flow`) and pushed to `origin/eight-players`.
  The public `v1.1.0-test1` pre-release was deliberately left unchanged because
  this revision has no current NRO/native-applet hardware evidence.
- Active helpers, builds, tests and console leases: none. Next action when the
  Switch toolchain is available is a fresh Release NRO build followed by a
  leased hardware check of pairing cancellation, 5–8 returned controllers and
  the exact 4→8→4 flow. King of the Hill remains unchanged; deliver the video
  now and book the requested visual-proposal reminder.
- Files changed: checkpoint only after commit `eb4f673c` (`Add eight-controller player flow`).
- Active helpers/builds: none.
- Next action: when Toad's managed connection returns, acquire a new Toad lease and resume at upload; do not rebuild or repeat completed PC gates.

## Final Toad deployment — 2026-10-23

- Toad returned online. Queued lease `efd77d24-63fb-4de3-8e5e-7f556d7258f8` waited for the existing MD2 icon-swap owner and acquired normally; no owner was bypassed.
- Verified HOME before writing. Deleted `options.toml` and `controls.sdl2.bin`; `autotest` was already absent.
- Uploaded the final 64,775,803-byte NRO. Console-side SHA-256 exactly matches `e19c825310dbbd969e66c046444041a0010a4aeb918a15d705607b78058260cc`.
- Launched Super Mario War and visually verified the new Welcome screen with “Up to 8 players”. Cleared and detached the sys-autopilot virtual controller; `/status` reports `controllerAttached:false`.
- Final console directory contains the NRO, logs, filters and maps only: no autotest/options/controls override. Game is intentionally left running at Welcome for Brendon's physical test.
- Remaining evidence: physical eight-controller pairing, per-player L/R and ZL/ZR fighter controls, P1-only menu enforcement, manual Quick-start A, and all eight players moving in-match. These are missing user hardware results, not failed tests.
- Files changed: checkpoint only after commits `eb4f673c` and `7bbd262a`. Active helpers/builds: none.
- Next action: receive Brendon's physical test result and immediately fix any observed defect before release.

## Community preview preparation — 2026-10-24

- User authorized ongoing GitHub progress updates, GBAtemp thread updates and public requests for hardware testers.
- Verified the live GBAtemp release thread is `685705`; the logged-in Akumara firefox-web profile can read and reply. The thread currently has 176 views, 0 replies and 4 likes.
- Fresh 8-second ASan/UBSan visual sweep loaded all 23 converted maps at 8 players: 23/23 passed, no sanitizer errors or overlap warnings. Evidence: `build-wide-soak.w7YIWQ`.
- Reported defect: “grey box is off” lacks a stage name or screenshot. Toad log/screenshot retrieval was blocked because its live status was `horizon-noapi`; managed GETs returned empty replies. No console change was made. Local visual inspection makes Greden's grey-green platform the likely subject, but its artwork and top collision both align exactly with the original after the intended +96 px center shift, so this is not yet a confirmed diagnosis or failed test.
- GitHub docs corrected locally: README now describes eight physical controller slots and the fixed exit; ROADMAP records the 24-stage development pack; CHANGELOG gains an unreleased NX development section.
- Prepared and ZIP-tested `build-community-test.DBoiRd/SuperMarioWar-NX-v1.1.0-test1.zip` (48,505,861 bytes, SHA-256 `73c65ef25626ca008897164efb404cf4afaad77435d53d0f90619187d8b7edc8`). Embedded NRO remains `e19c825310dbbd969e66c046444041a0010a4aeb918a15d705607b78058260cc`. The package includes a focused eight-controller checklist and asks for the exact stage/screenshot if the grey-box issue appears.
- Remaining: identify the grey-box defect from user evidence; publish the branch and experimental GitHub pre-release; reply to GBAtemp with progress, screenshots, download and tester request; verify both public pages.
- Files changed: README.md, ROADMAP.md, CHANGELOG, this checkpoint. Active helpers: none.
- Next action: commit documentation, publish the `eight-players` branch and `v1.1.0-test1` pre-release, then post and verify the GBAtemp tester update. Treat the grey-box report as open until exact evidence identifies it.

## Community preview published — 2026-10-24

- Pushed branch `eight-players` through commit `e867ee8f` and published the public GitHub pre-release `v1.1.0-test1`: https://github.com/brendongl/supermariowar-nx/releases/tag/v1.1.0-test1
- Verified the stable GitHub release remains `v1.0.0`; the tester build is explicitly marked as a pre-release.
- Uploaded the tested ZIP, bare NRO and checksum file. GitHub reports the ZIP digest as `73c65ef25626ca008897164efb404cf4afaad77435d53d0f90619187d8b7edc8`; the bare NRO digest remains `e19c825310dbbd969e66c046444041a0010a4aeb918a15d705607b78058260cc`.
- Published and visually verified GBAtemp reply #2 with the feature progress, hardware test checklist, pre-release link and two UI screenshots: https://gbatemp.net/threads/release-super-mario-war-nx-v1-0-0-native-switch-port-4-player-local-60-fps.685705/#post-10932377
- Opened GitHub issue #1 to collect the exact stage name, screenshot/video, player count and collision behavior for the reported misplaced grey box: https://github.com/brendongl/supermariowar-nx/issues/1
- Verified results: public download and forum post are live; forum renders the complete text and both attachments; GitHub assets and hashes match the locally validated package.
- Remaining missing evidence: a physical eight-controller session and enough evidence to identify the grey-box stage. The grey-box report remains unconfirmed; no test currently fails for it.
- Files changed: this checkpoint after documentation commit `e867ee8f`. `servers.toml` remains pre-existing, untracked and untouched. Active helpers, builds and console leases: none.
- Next action: monitor tester replies and GitHub issue #1; reproduce and fix concrete reports, then rerun the relevant PC and Switch gates before promoting a stable release.

## Bowser hardware and UX review — 2026-10-25

- Brendon explicitly authorized Bowser. Lease `886d38ce-c56e-4f16-8c61-5d2156dc4fec` was acquired normally and completed; no other console was touched.
- Bowser's sys-autopilot build supports one injected virtual Pro Controller, not eight independent virtual pads. With rail Joy-Cons already owning Player 1, the injected pad became Player 2. Its Right/Up/A inputs were correctly ignored on the P1-only roster screen, giving a real hardware check of the menu ownership gate.
- Installed and hash-verified test NRO `e19c825310dbbd969e66c046444041a0010a4aeb918a15d705607b78058260cc`. An eight-player/eight-team `Xijar_Greden Wide` King of the Hill soak reached 59.9 fps over the second full minute after startup, with no crash. The first interval averaged 53.0 fps because it included a 3.67-second startup/load maximum; sustained update time was 4.00 ms and flip time 12.68 ms.
- The reported grey box was reproduced as the translucent King of the Hill scoring zone. It is a dynamic mode object, not generated stage artwork. Quick Start randomizes KOTH size from 2x2 through 7x7; on dense wide stages a large zone can be accepted over mostly solid scenery, which makes it look misplaced. This is a confirmed presentation/placement problem; the exact user screenshot is still missing.
- One harness invocation used `map=Greden Wide` instead of the exact `Xijar_Greden Wide` filename. The log reported `ERROR: Couldn't open map` and retained the previously selected Death Island Wide map. Correcting the name loaded Greden and passed. This was a test-input failure, not a game crash.
- Fresh ASan/UBSan captures at 2 and 5 active players, plus a targeted eight-player Greden KOTH run, were sanitizer-clean. UX findings: inactive fighter cards consume most of the screen at 2–5 players and their `SET ON PLAYER SETUP` text overflows; the roster screen mixes eight slot cards with legacy Start/Quick/Options/Controls/Exit controls; the eight-player fighter grid itself and Quick Reveal remain clear and legible.
- Added Game Lab SMW todos #9 (adaptive fighter layout), #10 (streamlined roster screen), and #11 (safer Quick KOTH zone size/placement). Ultimate SysModule todo #12 tracks optional eight-pad HDLS automation; it would improve repeatable hardware UI testing but would not replace a physical-controller pass. No production source changed in this gate.
- Bowser cleanup verified: original NRO `0ba1dc0015a155612ff96fc32d3db88e6d3fda0100d407939c4af3fdec761bf6`, options `f7f894c0af425483c52d8a4ccead33decec08bb05acad8dec51e12f2fe82adc8`, and controls `d5abbaa51fed52c039812bb8cb24e1ca2f7b4406788639975c28bb8598a5800a` restored exactly; `autotest` absent; virtual pad detached; HOME idle.
- Files changed: this checkpoint only; `servers.toml` remains pre-existing, untracked and untouched. Active helpers, builds and console leases: none.
- Next action: after Brendon chooses the UI direction, implement the adaptive fighter layout first, then simplify roster setup, then constrain Quick Start KOTH zones; rerun 2/5/8-player visual captures, sanitizer navigation and Bowser hardware smoke before the next pre-release.

## Eight-player identity and spawn mapping — 2026-10-25

- Added one shared identity table for the roster, fighter cards and native Switch controller-pairing applet: P1 red, P2 green, P3 yellow, P4 blue, P5 purple, P6 orange, P7 cyan and P8 pink.
- Existing maps expose four compatible team-spawn regions, so eight teams map as `1,2,3,4,1,2,3,4`. The existing 20-attempt/furthest-point chooser still separates the two players sharing each region.
- Verified results: fresh Release build passed; 63/63 CTest cases and 220/220 assertions passed. A deterministic eight-player runtime checked all eight default team/color identities and the exact region sequence before exiting successfully. A separate ASan/UBSan build and runtime repeated that check with no sanitizer finding.
- Failed tests: none. Earlier runtime timeouts came from an incorrectly placed autotest marker; one later shell wrapper returned status 1 after the game itself exited 0 because its reporting pipeline found no failure text. Direct log validation passed. These were harness issues, not game failures.
- Missing evidence: the current machine has no usable `/opt/devkitpro` Switch compiler, so the native pairing-applet code has not been compiled into a fresh NRO or seen on Switch hardware in this gate. The libnx field names were checked against its source earlier. Physical eight-controller validation remains pending.
- Files changed: `src/common/PlayerIdentity.h`, common/test CMake lists, `ModernMenu.cpp`, `SwitchPlatform.cpp`, `player.cpp`, and deterministic test hooks in `GSMenu.cpp`/`GSGameplay.cpp`.
- Active helpers, background builds and console leases: none.
- Next action: when the Switch toolchain is available, build a fresh NRO and verify the eight pairing colors plus one live 5–8-controller spawn session before updating the public pre-release.

## Interactive spawn atlas — 2026-10-25

- Built an interactive static page from the 24 shipped widescreen `.map` files. It shows each stage image, exact serialized spawn rectangles, the eight player colors, optional tile grid, search, large view and rerollable example eight-player openings using the game's 20-attempt / 96-pixel separation rule.
- Data finding: every current widescreen map serializes identical allowed areas for region IDs 1–4. The runtime mapping remains P1/P5→1, P2/P6→2, P3/P7→3 and P4/P8→4, but today those region IDs do not produce different authored zones. The page states this explicitly.
- Verified results: 24/24 maps parsed; 24/24 864×480 images loaded; four populated region sets per map; all current sets independently compared equal. Desktop and 400 px Playwright checks passed player selection, reroll, search, detail dialog, responsive width and browser-console checks. Existing independent 23-map conversion verification still passes 6,900 center cells and authored side objects.
- Published owner-private Sites version 1 at `https://smw-spawn-atlas.dongletrk.chatgpt.site`; production reports deployment succeeded and generated a production screenshot. The anonymous canary and `webget` correctly reach the Sign in with ChatGPT gate (HTTP 401 before login), so anonymous content/asset probing is blocked by the deliberate owner-only access policy. This is missing unauthenticated evidence, not a failed page test.
- Resolved tooling failures: local QA first hit an unrelated service already using port 8765; rerun on an unused port passed. The first Sites archive used unsupported static directory `.`; versioned source was corrected to `dist/`, pushed, saved and deployed successfully.
- Files changed: `tools/widescreen/map_info.py`, `spawn_viewer.py`, `README.md`, approved `00.png`, and this checkpoint. Deployed source is also committed in the separate Sites repository at `/home/brendongl/projects/smw-spawn-atlas` commit `12b26edb40f74a278e533c4d08c1212035efff76`.
- Active helpers, test processes and console leases: none.
- Next action: Brendon reviews the atlas and decides whether regions 1–4 should be authored as genuinely different player zones before the next NRO build.

## Distinct spawn zones — 2026-10-25

- Authored four distinct left-to-right spawn zones in all 24 widescreen maps.
  Each map chooses its three boundaries from its own legal spawn cells so the
  four zones have similar safe-space counts. The regions are disjoint and their
  union exactly equals the previous legal player-spawn area; normal region 0 and
  item spawning are unchanged.
- Player pairing remains P1/P5→1, P2/P6→2, P3/P7→3 and P4/P8→4. The map no-spawn
  grid and serialized runtime rectangles are both updated, so later editor saves
  preserve the authored regions.
- Verified results: all 24 binaries parse and reload; the four regions differ on
  every map; all 24 runtime launches placed eight players inside the expected
  regions; every same-region pair was at least 96 px apart. The independent map
  verifier still passes 23 maps and 6,900 preserved center cells. Fresh Release
  build passes 63/63 CTests. Fresh Switch NRO builds successfully, contains the
  exact bytes of all 24 maps once, and has SHA-256
  `2ea291f4510bd09d9bc6e0b338e69f9120ba51932fd1951bc2f0a8c430c77bee`.
- Atlas version 2 is live at `https://smw-spawn-atlas.dongletrk.chatgpt.site`.
  Desktop and 400 px Playwright checks pass all 24 maps, region selection,
  reroll, search, large view, responsive width and console checks. Sites reports
  deployment succeeded; anonymous access reaches the intended ChatGPT sign-in
  gate, so authenticated production content remains missing canary evidence.
- Harness failures: one 24-map batch used a non-autotest PC build, so every run
  waited at Welcome and timed out without loading a map. A proof run then used a
  stale autotest binary which lacked the new spawn logging hook. Both were test
  setup failures; the fresh current autotest build passed 24/24 runtime maps.
- Files changed: all 24 wide `.map` files; `author_spawn_regions.py`,
  `map_info.py`, `spawn_viewer.py`, this README and checkpoint. Separate Sites
  source commit: `063b867`. No helpers, build jobs or console leases remain.
- Missing evidence, not a failure: this exact NRO has not run on Switch hardware
  and physical eight-controller behavior remains unverified. No console was
  touched in this gate.

## Fox hardware spawn sweep — 2026-10-25 (in progress)

- Lease `712153fd-73ee-4308-aa44-cc216c28673b`; exact candidate NRO SHA-256 `2ea291f4510bd09d9bc6e0b338e69f9120ba51932fd1951bc2f0a8c430c77bee` uploaded and verified on-console.
- Backed up Fox's original NRO (`5fdd5150e14b15d6c903fe0359190ccef820f00f1c3dfd4aff95698f9e3a6cc6`) plus its pre-existing empty autotest marker, options, controls and two logs before changing anything.
- Verified 12/24 wide maps so far: each launched an actual 27-tile, 8-player/8-team match, produced a hardware screenshot, had no map-open or autotest warning, and closed cleanly.
- Failed game tests: none. A first nine-second harness attempt closed two maps before the match-start log and was discarded; rerunning with 14 seconds passed them.
- Blocked/missing evidence: before map 13, the managed launcher timed out without a launch event and Fox's sys-autopilot began resetting/ignoring API requests. The event collector records clean exits through map 12 and no launch for map 13, so this is currently a console-control interruption, not evidence of a game crash. Fox cannot yet be restored while its API is unavailable.
- Files changed: this checkpoint only. Active helpers: none. Active console: Fox lease above; evidence under `build-fox-spawn-test.LqTQ4I`.
- Next action: continue the remaining 12 maps on Bowser under a separate authorized lease while leaving Fox untouched; restore both consoles exactly when their control APIs are available.

## Combined Fox/Bowser hardware spawn sweep — 2026-10-25

- Verified all 24 widescreen maps on real Switch hardware with the exact candidate NRO SHA-256 `2ea291f4510bd09d9bc6e0b338e69f9120ba51932fd1951bc2f0a8c430c77bee`.
- Fox passed maps 1–12; Bowser passed maps 13–24. Every map reached its intended 27-tile stage with 8 CPU players and 8 teams, produced a screenshot, logged no map-open or autotest warning, and closed cleanly through qlhook. A manually reviewed 24-image contact sheet shows all intended stages rendered with eight HUD slots.
- Bowser produced no new Atmosphere crash, fatal or ERPT report. Bowser was restored byte-for-byte to its original NRO (`0ba1dc0015a155612ff96fc32d3db88e6d3fda0100d407939c4af3fdec761bf6`), options, controls and logs; the test-created autotest marker was deleted, the virtual pad detached, HOME left idle and lease `99698a6e-34b3-4c7a-ada9-cbc97fb5c8ef` released.
- Failed game tests: none. The two short-timer results were discarded and passed on the corrected rerun. Map 13 passed on Bowser after Fox failed to start it.
- Missing evidence: these runs used eight CPU players, so eight physical controllers and native pairing colors remain unverified. Switch logs prove the map and 8-player/8-team runtime, but do not print each spawn coordinate; the exact per-region coordinates remain covered by the 24/24 PC runtime gate.
- Cleanup blocked on Fox: after map 12's recorded clean exit, its API changed to connection-reset / `horizon-noapi`, consistent with sleep or the API closing. No map-13 launch event exists. Fox's pre-test backup is safe, but the candidate NRO and test files cannot be restored until the console is physically woken. Lease `712153fd-73ee-4308-aa44-cc216c28673b` remains active. This is a cleanup block, not a game failure.
- Added Ultimate SysModule todo #15 to keep unattended qlhook-only test sweeps awake; qlhook launches do not refresh the console's input-idle clock and this lease had `keepAwake:false`.
- Evidence: Fox `build-fox-spawn-test.LqTQ4I`, Bowser `build-bowser-spawn-test.jVmfhk`. Active helpers: none.
- Next action: physically wake Fox once, restore its original NRO/settings/logs by saved hashes, delete the test marker, detach the virtual pad and release the lease. After that, the only product evidence still missing is a real 5–8 physical-controller session.

## Test stop — 2026-10-25

- Brendon asked to stop tests and follow-ups. Cancelled all three pending follow-ups for this thread; no further automatic checks are booked.
- Closed Fox lease `712153fd-73ee-4308-aa44-cc216c28673b` with a device note recording the deferred cleanup. No console action was attempted while Fox remained `horizon-noapi`.
- Verified results remain unchanged: all 24 hardware map loads passed; Bowser is restored. Failed game tests: none.
- Deferred cleanup, not a test failure: Fox still has candidate NRO/test files because it was asleep/API-closed. Its exact original NRO/settings/log backups remain in `build-fox-spawn-test.LqTQ4I` for a later explicitly requested restoration.
- Files changed: this checkpoint only. Active helpers, console leases and scheduled follow-ups: none. Next action: none until Brendon asks to resume.


## Eight virtual-controller test — 2026-10-26 (gate 1)

- Bowser lease `459e324b-1fe1-440d-80ba-539a7ddd174f` is active and exclusive. Baseline has one physical rail controller plus one pre-existing HDLS virtual controller, reproducing the two-controller ceiling at the controller-service layer.
- Extended the local sys-autopilot source to keep up to eight independent HDLS controller handles/states, accept `controller: 1..8` on input endpoints, attach a requested controller count, detach all controllers, and report the virtual-controller count. Legacy single-controller calls remain compatible.
- Verified results: the full sys-autopilot host suite passes. Failed tests: none. Missing evidence: the Switch build, deployment, eight-pad enumeration and SMW UI flow have not yet been run.
- Files changed in `/home/brendongl/projects/sys-autopilot`: input implementation/header/HTTP routes, status HTTP/MCP, README and host stubs. `.claude/` was pre-existing and untouched. Active helpers: none.
- Next action: compile the sysmodule for Switch, back up Bowser's current sysmodule and game state, then deploy and validate controller enumeration.

### Gate 2 — Switch build and Bowser backup

- Switch build passed in the pinned devkitPro image. Candidate sysmodule SHA-256: `6bb8f887bfb7dafa530e9de0ede0f0956f8b8400b944e75ceb00c854cc35f485`.
- Backed up Bowser before deployment: sysmodule `c575054e920d683cb95f5d7f0310e2b53493f07da6a694cf6d72102029f614da`, SMW NRO `0ba1dc0015a155612ff96fc32d3db88e6d3fda0100d407939c4af3fdec761bf6`, options and controls at their previously recorded hashes. Captured baseline status, input topology and crash-report list.
- Failed tests: none. Missing evidence: candidate has not yet booted or enumerated multiple pads. Active lease remains Bowser; no helpers.
- Next action: deploy the candidate sysmodule, reboot, verify health, then attach seven virtual pads.

### Gate 3 — partial Bowser enumeration; hardware blocked

- Candidate sysmodule was hash-verified on Bowser and booted normally on FW 23.0.1. It reported seven HDLS virtual handles attached. At HOME, Horizon exposed four virtual npads plus the one physical rail controller (five visible npads); the remaining assignment behavior still needs testing while SMW requests eight slots.
- Before the current SMW NRO upload started, Bowser went offline about one minute after reboot. Its pre-test status had `keepAwake:false` and no post-boot physical input; this timing is consistent with its short idle sleep, but a crash report cannot be ruled out until the console is physically woken. The upload transferred zero bytes, so Bowser's original SMW NRO was not changed.
- Failed operations: the game upload and following hash/launch commands failed with connection refused because the console was already offline. This is not a game test failure; the game never launched.
- Missing evidence: post-wake crash-report comparison, eight-slot assignment inside SMW, independent input from players 1–8, and cleanup restoration. Active helpers: none. Bowser lease remains active.
- Next action: after one physical wake, enable timed keep-awake first, inspect crash reports, continue the controller/game test, then restore the exact original sysmodule and SMW files.

## Bowser virtual-controller validation — 2026-10-26

- Bowser lease `459e324b-1fe1-440d-80ba-539a7ddd174f` completed. The current SMW candidate NRO `2ea291f4510bd09d9bc6e0b338e69f9120ba51932fd1951bc2f0a8c430c77bee` booted and ran a real eight-participant Jungle Bridge Wide match with four independent virtual human controllers plus four CPUs.
- Verified UI behavior: controller 2 could not change the Player-1-only roster; L/R changed only that controller's fighter; ZL/ZR changed only that controller's color; each human had to ready; controller 2 could not start the Quick Start reveal; controller 1 could. The native pairing applet showed P1-P4 with the intended colors/labels and P5-P8 empty.
- The game reached 8-player mode because the four HDLS pads plus Bowser's physical rail controller met the five-controller minimum. The current build therefore did not reproduce the cafe report that only two controllers could connect. The exact cafe build remains unknown, so that report is missing build evidence rather than disproven.
- Hardware/tool limit: current Nintendo HDLS accepts four independently controllable virtual devices. Handles 5-7 could be created, but subsequent state updates failed with HID result `0x1c24ca` (module 18, description 202). Eight all-virtual controllers cannot be validated by this HDLS method; a different Bluetooth/HID injection route or physical controllers is required.
- Failed operation: the first upload attempt met Bowser's one-minute idle sleep and transferred zero bytes. After physical wake, no new crash/fatal report existed. Keep-awake was enabled and the rerun passed. This was a test-harness setup error, not a game crash.
- New crash/fatal reports during the full test: none. Bowser was restored and hash-verified to original sysmodule `c575054e920d683cb95f5d7f0310e2b53493f07da6a694cf6d72102029f614da`, game NRO `0ba1dc0015a155612ff96fc32d3db88e6d3fda0100d407939c4af3fdec761bf6`, options and controls; virtual pads cleared; keep-awake off; HOME idle; lease released.
- Added four-controller HDLS support to sys-autopilot in local commit `960dd49`; full host suite and Switch build pass, and all four controller indices were hardware-verified. Ultimate SysModule todo #12 now records that eight requires a non-HDLS method.
- Files changed: sys-autopilot input/status API, README and stubs in commit `960dd49`; this checkpoint. Active helpers, console leases and background processes: none.
- Remaining evidence: a real 6-8 physical-controller session and the exact build/config that showed the cafe two-controller problem. Next action: test the candidate with 6-8 physical controllers or distribute it as a tester pre-release before cafe deployment.


## Bowser post-test crash diagnosis — 2026-10-26 (gate 1)

- Corrected the prior conclusion: Bowser did create two new ERPT system reports after cleanup, at 06:42:04 and 06:49:30 UTC. Both are error 2162-0004 (`ForcedShutdownDetected`), proving two hard power-offs after the console became unresponsive. Atmosphere crash/fatal folders remained unchanged; the old crash API omitted ERPT, which is why the first check missed this evidence.
- The previous boot log shows the initiating failure immediately after keep-awake was disabled: repeated Awake → sleep → intermediate wake cycles, followed each time by mDNS/socket errors. Bowser's installed sys-autopilot is commit `8cddd5e`; it predates commit `e0c0397`, which keeps filesystem/network IPC suspended through intermediate PSC wake states until the console reaches full Awake. This exact race matches the observed log.
- Implemented ERPT discovery and MessagePack `ErrorCode` parsing in sys-autopilot's crash-report API so forced shutdowns can no longer be silently omitted. Full host suite passes after adding an ERPT regression fixture.
- Failed tests: the first two host-suite runs caught incomplete ERPT test integration (missing `utime.h`, then a truncated UUID path); both code/test defects were fixed. The complete suite now passes.
- Missing evidence: the current sys-autopilot build has not yet been compiled/deployed on Bowser and its sleep/wake fix has not been hardware-verified there.
- Files changed: sys-autopilot crash model/parser/HTTP/MCP/tool description and MCP test; this checkpoint. Active helpers: none. Active Bowser lease: `a9b7bc46-b54b-4cfd-8575-67af2fb03afb`; timed keep-awake is on during debugging.
- Next action: build the current sysmodule, back up Bowser again, install it, confirm ERPT reports appear through the API, then perform one controlled sleep/wake test with physical recovery available.

### Gate 2 — corrected sysmodule installed

- Committed ERPT reporting as sys-autopilot `cf23fab` (`Include forced shutdowns in crash reports`). The installed build also contains prior fixes `3599c54` (release backlight sessions) and `e0c0397` (wait for full PSC Awake before resuming IPC), plus four-controller support `960dd49`.
- Built a clean versioned Switch sysmodule and installed it on Bowser. On-console SHA-256 matches `59a70addee8b9eb317df6d16d14f76b7134b65d367480583efb3f0446abaee10`; `/status` reports `2.1.0-24-gcf23fab`. The exact pre-fix original remains backed up at SHA-256 `c575054e920d683cb95f5d7f0310e2b53493f07da6a694cf6d72102029f614da`.
- Hardware verification passed for boot, status, hash and ERPT parsing: Bowser now reports both new 2162-0004 forced shutdowns through `/crash-reports`. No new report was created by three controlled reboots.
- Failed tests: none in this gate. Missing evidence: one real sleep and physical wake cycle is still required to exercise the fixed PSC transition path.
- Files changed: sys-autopilot commit `cf23fab`; this checkpoint. Active helpers: none. Bowser lease remains active and timed keep-awake remains on until the controlled test begins.
- Next action: disable keep-awake, command one sleep, have Brendon press Bowser's power button once, then inspect status/logs/ERPT and either keep the fix or restore the exact backup.

### Gate 3 — exact cleanup reproduction failed; second fix ready

- The first controlled sleep/wake with no HDLS use passed: Bowser returned fully Awake, API healthy, and no new ERPT/crash/fatal report appeared.
- The exact cleanup reproduction then attached four virtual controllers, verified four npads, detached all, verified zero, and slept. Bowser froze/offlined again on physical wake. This is a failed hardware test and proves commit `e0c0397` alone does not fix the post-controller-test crash.
- Root cause narrowed further: `input_detach_all()` detached the virtual devices but left hid's HDLS transfer-memory work buffer mapped. Ordinary sleep passed because that buffer had never been opened. Releasing it only from the subsequent PSC sleep callback is too late on Bowser/FW 23.0.1.
- Implemented and committed sys-autopilot `dfef370` (`Release HDLS work buffer after controller detach`): the last successful detach now immediately releases the HDLS work buffer; attach failures clean it up too. Full host suite and Switch build pass. Candidate version `2.1.0-25-gdfef370`, SHA-256 `23d023bd331cff662aff3e15a937e04bae27d2bef45a283c8c9d3f579661993c`.
- Missing evidence: Bowser is currently offline awaiting the instructed 12-second power-button recovery, so `dfef370` has not been installed or hardware-tested. The earlier `cf23fab` remains installed.
- Files changed: sys-autopilot commit `dfef370`; this checkpoint. Active helpers: none. Bowser lease `a9b7bc46-b54b-4cfd-8575-67af2fb03afb` remains active.
- Next action: once Bowser is online, upload/hash-verify `dfef370`, reboot, repeat attach-four → detach-all → verify release log → sleep → physical wake, then inspect ERPT and either accept or continue debugging.

### Gate 3 correction and final candidate — 2026-10-26

- Correction to the preceding gate: the exact four-controller cleanup cycle did **not** crash. Bowser woke at 14:05:10, restored listening/API, briefly cycled once, and then auto-slept normally at 14:05:30 because keep-awake was off. It woke again at 14:11:54. No new ERPT/crash/fatal report exists. The earlier offline observation was missing evidence, not a failed test.
- Installed version `2.1.0-25-gdfef370`, on-console SHA-256 `23d023bd331cff662aff3e15a937e04bae27d2bef45a283c8c9d3f579661993c`. Attached four HDLS pads, verified four, detached all, verified zero, and verified the new `HDLS work buffer released after last detach` log before sleep.
- Bowser is now in the final controlled sleep with timed keep-awake persisted for ten minutes, which will prevent the quick post-wake re-sleep that confused the previous check.
- Failed tests: none after correcting the classification. Missing evidence: one normal physical wake and the post-wake status/log/report check for `dfef370`. Active helper: none; Bowser lease remains active.
- Next action: Brendon presses power once; verify API, full Awake state, exact hash, zero virtual pads and no new reports, disable keep-awake, then release the lease.

### Bowser post-test crash — complete

- Final exact-path hardware test passed on sys-autopilot `2.1.0-25-gdfef370`: attached four virtual controllers, verified four; detached all, verified zero; logged immediate HDLS work-buffer release; entered sleep; physically woke; returned PSC Awake/API healthy.
- No new ERPT, crash or fatal report was created. Newest ERPT remains the second original forced shutdown at 06:49:30 UTC. Installed sysmodule hash is `23d023bd331cff662aff3e15a937e04bae27d2bef45a283c8c9d3f579661993c`.
- Root cause: Bowser's restored old sys-autopilot `8cddd5e` treated intermediate PSC wake states as fully awake and resumed SD/network IPC too early. Commit `e0c0397` fixes that. `dfef370` additionally releases HDLS work memory immediately after the final controller detach. `cf23fab` makes ERPT forced shutdowns visible through the crash API so future checks cannot miss them.
- Verified results: full host suite passed; Switch build passed; three reboots passed; plain sleep/wake passed; two attach-four/detach-all sleep/wake cycles passed; exact hash/version and zero virtual pads confirmed. Failed final tests: none. Missing evidence: none for this reported crash path.
- Bowser was left at HOME with no running app, zero virtual controllers and keep-awake off. Lease `a9b7bc46-b54b-4cfd-8575-67af2fb03afb` was released. Active helpers/background tasks: none.
- Files changed: sys-autopilot commits `cf23fab` and `dfef370`; SMW checkpoint commits through this entry. Next action: continue the separate eight-physical-controller validation when controllers are available.
