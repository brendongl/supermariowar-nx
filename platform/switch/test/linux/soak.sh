#!/usr/bin/env bash
# Headless auto-test soak of the host build under AddressSanitizer + UBSan.
#   platform/switch/test/linux/soak.sh [seconds] [output-dir]
# Env: SMW_SIDE_PANELS=1 for the Switch layout, SMW_AUTOTEST_LINES for extra autotest options.
# Exits non-zero if a sanitizer reported anything or no match finished.
set -uo pipefail
cd "$(dirname "$0")/../../../.."
SECS=${1:-600}
OUT=$(realpath -m "${2:-build-linux-asan/soak}")
IMAGE=smw-linux-test
mkdir -p "$OUT/home/.local/share/supermariowar"
# Extra autotest lines (e.g. SMW_AUTOTEST_LINES="players=8") go after "fast"
printf 'fast\n%s\n' "${SMW_AUTOTEST_LINES:-}" > "$OUT/home/.local/share/supermariowar/autotest"

docker image inspect $IMAGE >/dev/null 2>&1 || docker build -q -t $IMAGE platform/switch/test/linux >/dev/null
run() { docker run --rm -u "$(id -u):$(id -g)" -e HOME=/tmp -v "$PWD":/src -w /src "$@"; }

if [ ! -f build-linux-asan/build.ninja ]; then
    san="-fsanitize=address,undefined -fno-omit-frame-pointer"
    run $IMAGE cmake -S . -B build-linux-asan -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DNO_NETWORK=ON -DSMW_AUTOTEST=ON -DSDL_UNIX_CONSOLE_BUILD=ON \
        -DCMAKE_C_FLAGS="$san" -DCMAKE_CXX_FLAGS="$san" -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined" >/dev/null
fi
run $IMAGE cmake --build build-linux-asan --target smw >/dev/null || exit 1

docker run --rm -u "$(id -u):$(id -g)" -e HOME=/home/t \
    -e SDL_VIDEO_DRIVER=offscreen -e SDL_AUDIO_DRIVER=dummy \
    -e ASAN_OPTIONS=detect_leaks=0 -e UBSAN_OPTIONS=print_stacktrace=1 ${SMW_SIDE_PANELS:+-e SMW_SIDE_PANELS=1} \
    -v "$OUT/home":/home/t -v "$PWD":/src -w /src $IMAGE \
    timeout "$SECS" build-linux-asan/smw --datadir /src/data > "$OUT/smw.log" 2>&1

matches=$(grep -c '\[match\] #' "$OUT/smw.log")
ubsan=$(grep -c 'runtime error' "$OUT/smw.log")
asan=$(grep -c 'ERROR: AddressSanitizer' "$OUT/smw.log")
stuck=$(grep -c '\[autotest\]\[warning\]' "$OUT/smw.log")
echo "matches=$matches ubsan=$ubsan asan=$asan stuck-warnings=$stuck log=$OUT/smw.log"
[ "$ubsan" -eq 0 ] && [ "$asan" -eq 0 ] && [ "$matches" -gt 0 ]
