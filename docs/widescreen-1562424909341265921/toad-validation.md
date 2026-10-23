# Toad hardware validation — 2026-10-23

User explicitly requested testing on idle Toad in conversation 1562424909341265921.
Quiet lease 8af5c9d8-d6ba-40a4-a3c9-e9ef07b97308 was acquired and released.
Exit-fix validation used quiet lease 9d66ab7d-86cd-46f8-b616-f23e05fc3133.
Firmware 22.5.0, sys-autopilot 1.5.0; controls were injected through its virtual
controller. No physical multi-controller pairing test was performed.

## Passed

- Uploaded NRO matched SHA256 3a78897f2ce90786fa7350a98600f1fb9f0d3fb1b8b8221477f0d4a573879e3a on the console.
- Four players (one human/three CPUs): original Flying Fortress loaded and played.
- Enabled fifth slot in the menu: selection changed automatically to Greden Wide;
  five-player gameplay displayed the expanded arena and five HUD slots.
- Enabled slots 6–8: Quick Game ran with eight players on Greden Wide.
- Manual eight-player map selection offered Haunted Temple Wide; it ran with
  eight players. Both eight-player matches continued running without an observed
  gameplay crash. This was a smoke test, not an all-mode/all-map endurance test.
- Disabled slots 5–8: picker returned to original Heinous Heights, which played
  with four players. Thus the 4→5→8→4 transition works on hardware.
- Remote movement/jump/menu/exit-match inputs were exercised.

## Exit failure and resolution

Leaving the application with B from the menu originally displayed Horizon's
software-closed error. NTON's nx-hbloader source confirmed the cause: when an NRO
returns with an empty next-load path, the forwarder rereads its embedded path and
launches the same NRO again. Mesa/EGL cannot initialize a second time in this
process, producing the null Data Abort below.

The preview crash report is 01792752844_017397c52c004000.log: result 2168-0002,
Data Abort at null. Exact-build symbolization gives:

    memset -> tmemCreate -> _nvInitialize -> nouveau_device_new
    -> nouveau_switch_screen_create -> switch_initialize -> eglInitialize
    -> SDL_EGL_LoadLibrary -> SDL_GL_LoadLibrary
    -> SDL_CreateWindowWithProperties -> GraphicsSDL::init -> main_game

The original pre-test NRO reproduced the same failure. Its SHA256 was:
d8e8432069695ca03f62f9c8264e720052bc1a7fd2a79faa1b6b11d0bbd0ebdc.
Its crash report is
01792753316_017397c52c004000.log, also 2168-0002 / null Data Abort. Therefore
the exit failure predates the widescreen conversion.

Three attempted fixes stopped the relaunch crash but still produced Horizon's
software-closed message: direct `svcExitProcess()`, `appletExit()` followed by the
syscall, and full `__appExit()` followed by the syscall. None generated a new
Atmosphere crash report; direct process termination itself was being classified
as abnormal.

The passing fix follows official libnx and Homebrew Menu behavior for a forwarded
NRO: set `__nx_applet_exit_mode = 1` during normal platform cleanup, then return
normally so libnx performs the application self-exit handshake. Candidate NRO
SHA256 `421cce9bb1807d3d1f4d5f1dc3b66b5573e738c7a883043722ed8c7aaf03668b`
passed three launch -> splash -> main-menu -> B-exit cycles. Each returned directly
to HOME without an error screen or NRO relaunch, and the crash-report directory
remained unchanged.

An additional eight-player autoplay smoke test selected Jungle Bridge Wide; the
live log recorded `arena-width=27 players=8 teams=8`. A final normal launch and
B-exit after that test also returned cleanly to HOME.

No sustained frame-timing run was performed, so no hardware FPS claim is made.
Raw logs/reports and original binary backup remain in build-toad-test.mUDoYr;
exit-fix evidence is in build-exit-test.Xf0kPw.

## Cleanup

Original NRO restored at /switch/supermariowar/smw.nro; temporary autotest and all
test-created options.toml, controls.sdl2.bin and servers.toml removed. No such
settings files existed before the test. HOME was visually verified with no game
playing. Virtual controller cleared and detached (status confirmed false). No
reboot, firmware/sysmodule changes, or public release.

The first two overwrite attempts failed while the game had restarted and remained
open; the existing file stayed intact, confirmed by hash. Closing via HOME/X/A
allowed restoration. Toad 1.5.0 does not support /files/move, /process/list or
/crash-reports; use directory listing for reports and do not repeat these calls.

Final exit evidence is in build-exit-test.Xf0kPw. Game Lab #4 remains open until
the widescreen feature is released.
