#!/usr/bin/env bash
# Packs a forwarder NSP: a HOME menu icon that starts sdmc:/switch/supermariowar/smw.nro
# through nx-hbloader in application mode. This is the NSP that works on hardware today
# (the standalone NSP from build-nsp.sh crashes the console's fatal module on launch,
# see issue tracker). Copy build-switch/src/smw/smw.nro to the SD card path below.
#
# Needs: Docker (devkitpro/devkita64), hacPack (HACPACK=path,
# https://github.com/DarkMatterCore/hacPack), NTON's prebuilt forwarder exefs and logo
# (NTON_ASSETS=path to nton/assets from https://github.com/rlaphoenix/nton) and the keys
# from your own console (PROD_KEYS=path, default ~/.switch/prod.keys).
#   platform/switch/nsp/build-forwarder.sh [version]
set -euo pipefail
cd "$(dirname "$0")/../../.."
ROOT=$PWD
VER=${1:-$(sed -n 's/^set(SMW_SWITCH_VERSION "\([0-9.]*\)".*/\1/p' cmake/PlatformSwitch.cmake)}
TID=017397C52C004000
NRO_PATH=sdmc:/switch/supermariowar/smw.nro
NAME="Super Mario War"
AUTHOR="72dpiarmy & contributors (Switch port: supermariowar-nx)"
HACPACK=${HACPACK:-hacpack}
PROD_KEYS=$(realpath "${PROD_KEYS:-$HOME/.switch/prod.keys}")
OUT=${OUT:-build-switch/nsp}

[ -d "${NTON_ASSETS:-}/exefs" ] || { echo "set NTON_ASSETS to nton/assets" >&2; exit 1; }
[ -f "$PROD_KEYS" ] || { echo "missing keys file (set PROD_KEYS)" >&2; exit 1; }
command -v "$HACPACK" >/dev/null || { echo "hacpack not found (set HACPACK)" >&2; exit 1; }
HACPACK=$(realpath "$(command -v "$HACPACK")")

W=$ROOT/$OUT/forwarder
rm -rf "$W"; mkdir -p "$W"/exefs "$W"/control "$W"/romfs "$W"/out "$W"/nca
cp "$NTON_ASSETS"/exefs/* "$W/exefs/"
docker run --rm -u "$(id -u):$(id -g)" -v "$W":/w devkitpro/devkita64 \
    /opt/devkitpro/tools/bin/nacptool --create "$NAME" "$AUTHOR" "$VER" /w/control/control.nacp --titleid=$TID

python3 - "$W/exefs/main.npdm" "$W/control/control.nacp" "$TID" <<'PY'
import struct, sys
npdm_path, nacp_path, tid = sys.argv[1], sys.argv[2], int(sys.argv[3], 16)
n = bytearray(open(npdm_path, 'rb').read())
aci, _, acid, _ = struct.unpack_from('<IIII', n, 0x70)
assert n[aci:aci + 4] == b'ACI0' and n[acid + 0x200:acid + 0x204] == b'ACID'
struct.pack_into('<Q', n, aci + 0x10, tid)          # ACI0 program id
struct.pack_into('<QQ', n, acid + 0x210, tid, tid)  # ACID program id range
open(npdm_path, 'wb').write(n)
n = bytearray(open(nacp_path, 'rb').read())
struct.pack_into('<Q', n, 0x3038, tid)           # PresenceGroupId
struct.pack_into('<Q', n, 0x3070, tid + 0x1000)  # AddOnContentBaseId
struct.pack_into('<Q', n, 0x3078, tid)           # SaveDataOwnerId
for i in range(8):
    struct.pack_into('<Q', n, 0x30B0 + 8 * i, tid)  # LocalCommunicationId
n[0x3025] = 0  # StartupUserAccount: none
n[0x3034] = 0  # Screenshot: allowed
n[0x3035] = 2  # VideoCapture: enabled
open(nacp_path, 'wb').write(n)
PY
cp platform/switch/icon.jpg "$W/control/icon_AmericanEnglish.dat"
printf '%s' "$NRO_PATH" > "$W/romfs/nextNroPath"
printf '%s' "$NRO_PATH" > "$W/romfs/nextArgv"

cd "$W"
"$HACPACK" -k "$PROD_KEYS" -o out --type nca --ncatype program --titleid $TID \
    --exefsdir exefs --romfsdir romfs --logodir "$NTON_ASSETS/logo" --keygeneration 1 > hacpack.log
PROGRAM=$(ls out/*.nca)
"$HACPACK" -k "$PROD_KEYS" -o out --type nca --ncatype control --titleid $TID \
    --romfsdir control --keygeneration 1 >> hacpack.log
CONTROL=$(ls out/*.nca | grep -v "$PROGRAM")
"$HACPACK" -k "$PROD_KEYS" -o out --type nca --ncatype meta --titleid $TID --titletype application \
    --programnca "$PROGRAM" --controlnca "$CONTROL" --keygeneration 1 >> hacpack.log
mv out/*.nca nca/
"$HACPACK" -k "$PROD_KEYS" -o . --type nsp --ncadir nca --titleid $TID >> hacpack.log
FINAL="$ROOT/$OUT/SuperMarioWar-NX-v$VER-forwarder [$TID].nsp"
mv "${TID,,}.nsp" "$FINAL"
ls -la "$FINAL"
