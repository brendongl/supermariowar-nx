#!/usr/bin/env bash
# Builds smw.nro for the Switch. Run from the repo root inside devkitpro/devkita64:
#   docker run --rm -v "$PWD":/src -w /src devkitpro/devkita64 platform/switch/build.sh
set -euo pipefail
[ -f .deps-switch/lib/libSDL3.a ] || platform/switch/build-deps.sh
cmake -S . -B build-switch -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/Switch.cmake" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE:-Release}"
cmake --build build-switch --target smw_nro
ls -la build-switch/src/smw/smw.nro
