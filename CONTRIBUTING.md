# Contributing

Thanks for helping. Bug reports with a log attached are just as valuable as code.

## Layout

| Path | What it is |
|---|---|
| `src/` | The game, shared with upstream |
| `src/smw/platform/switch/` | Switch start-up and shutdown: RomFS, SD card logging, frame heartbeat |
| `cmake/PlatformSwitch.cmake` | Switch build settings |
| `platform/switch/build-deps.sh` | Builds SDL 3, SDL3_image, SDL3_mixer and toml11 for the Switch |
| `platform/switch/patches/` | Patches applied to SDL |
| `platform/switch/data/` | Extra game data copied over `data/` in the Switch build (the Dice Guy skin) |
| `platform/switch/build.sh` | Builds `smw.nro` |
| `platform/switch/nsp/` | NSP packaging (NPDM, NACP, hacBrewPack) |
| `platform/switch/test/` | Emulator smoke run and the Linux sanitizer soak |

Keep Switch-only code behind `#ifdef __SWITCH__` (or `NINTENDO_SWITCH` in CMake), and keep changes to shared code small and general, so they can also go upstream to [mmatyas/supermariowar](https://github.com/mmatyas/supermariowar).

## Building

See the README. Everything runs in Docker (`devkitpro/devkita64`), so you don't need devkitPro installed.

## Testing a change

1. **Sanitizer soak (any PC with Docker).** `platform/switch/test/linux/soak.sh 600` builds the game for Linux with AddressSanitizer and UndefinedBehaviorSanitizer, then lets four CPU players play random Quick Games, uncapped, for 10 minutes. It fails on any sanitizer report. CI runs it on every push.
2. **Emulator smoke run.** `platform/switch/test/eden-run.sh build-switch/src/smw/smw.nro 300 /tmp/run 30` boots the NRO in the [Eden](https://eden-emu.dev) emulator under Xvfb with software Vulkan and saves a screenshot every 30 seconds. It is slow, but catches start-up and rendering problems.
3. **Hardware.** Put an empty `autotest` file in `sdmc:/switch/supermariowar/` to run the same soak on a console, then read `smw.log`. Test with real controllers before calling input changes done.

## Pull requests

- One topic per pull request, with a description of what changed, why, and how you tested it.
- Say what you tested on: sanitizer soak, emulator, or a console (with firmware version).
- Don't commit console keys, NSPs or other build output.
