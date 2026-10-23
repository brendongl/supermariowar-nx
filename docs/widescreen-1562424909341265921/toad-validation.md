# Toad hardware validation — 2026-10-23

User explicitly requested testing on idle Toad in conversation 1562424909341265921.
Quiet lease 8af5c9d8-d6ba-40a4-a3c9-e9ef07b97308 was acquired and released.
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

## Failed / unresolved

Leaving the application with B from the menu displayed Horizon's software-closed
error. A subsequent startup appeared, so this may involve the forwarder/reload
path, but that cause is not established.

The preview crash report is 01792752844_017397c52c004000.log: result 2168-0002,
Data Abort at null. Exact-build symbolization gives:

    memset -> tmemCreate -> _nvInitialize -> nouveau_device_new
    -> nouveau_switch_screen_create -> switch_initialize -> eglInitialize
    -> SDL_EGL_LoadLibrary -> SDL_GL_LoadLibrary
    -> SDL_CreateWindowWithProperties -> GraphicsSDL::init -> main_game

The original pre-test NRO was restored and its SHA256 independently verified:
d8e8432069695ca03f62f9c8264e720052bc1a7fd2a79faa1b6b11d0bbd0ebdc.
The same visible exit error reproduced with that build; its new report is
01792753316_017397c52c004000.log, also 2168-0002 / null Data Abort. Therefore
this failure is not confined to the widescreen conversion. The older ELF has
not been symbolized; identical root cause is not claimed.

Retrieved logs contained startup output only, so no hardware FPS claim is made.
Raw logs/reports and original binary backup remain in build-toad-test.mUDoYr.

## Cleanup

Original NRO restored at /switch/supermariowar/smw.nro; temporary restore NRO and
all test-created options.toml, controls.sdl2.bin and servers.toml removed. No such
settings files existed before the test. HOME was visually verified with no game
playing. Virtual controller cleared and detached (status confirmed false), lease
released. No reboot, firmware/sysmodule changes, or public release.

The first two overwrite attempts failed while the game had restarted and remained
open; the existing file stayed intact, confirmed by hash. Closing via HOME/X/A
allowed restoration. Toad 1.5.0 does not support /files/move, /process/list or
/crash-reports; use directory listing for reports and do not repeat these calls.

Next: investigate the exit/reload failure separately before release. Game Lab #4
remains open. No active helpers, processes, follow-ups or console leases remain.
