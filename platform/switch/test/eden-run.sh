#!/usr/bin/env bash
# Headless smoke run in the Eden emulator with software Vulkan (lavapipe).
# eden-run.sh <nro|nsp> <seconds> <outdir> [screenshot-interval-seconds]
set -uo pipefail
GAME=$1; SECS=${2:-60}; OUT=${3:-/tmp/smw-eden}; EVERY=${4:-10}
EDEN=${EDEN:-$HOME/tools/eden/AppDir/bin/eden-cli}
DISP=${DISP:-:95}
SDMC=${SDMC:-$HOME/.local/share/eden/sdmc}
mkdir -p "$OUT"; rm -f "$OUT"/*.png
rm -f "$SDMC/switch/supermariowar/smw.log"
export SHARUN_ALLOW_SYS_VKICD=1 VK_ICD_FILENAMES=/usr/share/vulkan/icd.d/lvp_icd.json DISPLAY=$DISP
timeout "$SECS" "$EDEN" -g "$GAME" > "$OUT/eden.log" 2>&1 &
pid=$!
t=0
while kill -0 $pid 2>/dev/null; do
    sleep "$EVERY"; t=$((t+EVERY))
    ffmpeg -loglevel error -y -f x11grab -video_size 1280x720 -i "$DISP" -frames:v 1 "$OUT/t$(printf %04d $t).png" 2>/dev/null
done
wait $pid; echo "eden exit=$?"
cp "$SDMC/switch/supermariowar/smw.log" "$OUT/smw.log" 2>/dev/null
