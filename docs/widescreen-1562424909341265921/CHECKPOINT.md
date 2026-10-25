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
