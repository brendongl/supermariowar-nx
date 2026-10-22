#!/usr/bin/env bash
# Packs build-switch/smw.elf (plus the RomFS data) into an installable NSP.
#
# Needs: Docker (devkitpro/devkita64), hacBrewPack (HACBREWPACK=path, build it from
# https://github.com/rlaphoenix/hacBrewPack) and the keys from your own console
# (PROD_KEYS=path, default ~/.switch/prod.keys). Keys are never committed.
#   platform/switch/nsp/build-nsp.sh [version]
set -euo pipefail
cd "$(dirname "$0")/../../.."
ROOT=$PWD
VER=${1:-$(sed -n 's/^set(SMW_SWITCH_VERSION "\([0-9.]*\)".*/\1/p' cmake/PlatformSwitch.cmake)}
TID=017397c52c004000  # must match platform/switch/nsp/app.json
NAME="Super Mario War"
AUTHOR="72dpiarmy & contributors (Switch port: supermariowar-nx)"
HACBREWPACK=${HACBREWPACK:-hacbrewpack}
PROD_KEYS=${PROD_KEYS:-$HOME/.switch/prod.keys}
ELF=${ELF:-build-switch/smw.elf}
OUT=${OUT:-build-switch/nsp}

[ -f "$ELF" ] || { echo "missing $ELF: run platform/switch/build.sh first" >&2; exit 1; }
[ -d build-switch/romfs/data ] || { echo "missing build-switch/romfs: run platform/switch/build.sh first" >&2; exit 1; }
[ -f "$PROD_KEYS" ] || { echo "missing keys file $PROD_KEYS (set PROD_KEYS)" >&2; exit 1; }
command -v "$HACBREWPACK" >/dev/null || { echo "hacbrewpack not found (set HACBREWPACK)" >&2; exit 1; }

W=$OUT/work
rm -rf "$W"; mkdir -p "$W"/exefs "$W"/control
docker run --rm -u "$(id -u):$(id -g)" -v "$ROOT":/src -w /src devkitpro/devkita64 bash -c "
  set -e; T=/opt/devkitpro/tools/bin
  \$T/elf2nso '$ELF' '$W/exefs/main'
  \$T/npdmtool platform/switch/nsp/app.json '$W/exefs/main.npdm'
  \$T/nacptool --create '$NAME' '$AUTHOR' '$VER' '$W/control/control.nacp' --titleid=$TID"

python3 - "$W/control/control.nacp" "$TID" <<'PY'
import struct, sys
path, tid = sys.argv[1], int(sys.argv[2], 16)
n = bytearray(open(path, 'rb').read())
struct.pack_into('<Q', n, 0x3038, tid)           # PresenceGroupId
struct.pack_into('<Q', n, 0x3070, tid + 0x1000)  # AddOnContentBaseId
struct.pack_into('<Q', n, 0x3078, tid)           # SaveDataOwnerId
for i in range(8):
    struct.pack_into('<Q', n, 0x30B0 + 8 * i, tid)  # LocalCommunicationId
n[0x3025] = 0  # StartupUserAccount: none (settings live on the SD card)
n[0x3034] = 0  # Screenshot: allowed
n[0x3035] = 2  # VideoCapture: enabled
open(path, 'wb').write(n)
PY
cp platform/switch/icon.jpg "$W/control/icon_AmericanEnglish.dat"

"$HACBREWPACK" -k "$(realpath "$PROD_KEYS")" --nologo \
    --exefsdir "$W/exefs" --romfsdir build-switch/romfs --controldir "$W/control" \
    --nspdir "$W/nsp" --ncadir "$W/nca" --tempdir "$W/temp" --backupdir "$W/backup" > "$W/hacbrewpack.log"
FINAL="$OUT/SuperMarioWar-NX-v$VER [${TID^^}].nsp"
mv "$W/nsp/$TID.nsp" "$FINAL"
ls -la "$FINAL"
