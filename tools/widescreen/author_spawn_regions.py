#!/usr/bin/env python3
"""Give the four team spawn regions distinct, balanced horizontal zones.

The playable cells come from region 0, which remains unchanged. Each map gets
three map-specific cuts chosen to balance the count of legal spawn cells while
keeping every zone at least three tiles wide. Regions 1-4 partition region 0;
the item region (5) is untouched.
"""

import argparse
import struct
from pathlib import Path

from map_info import read


HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
DEFAULT_MAPS = ROOT / "platform/widescreen/data/maps"


def split_areas(areas, bounds):
    result = []
    for left, top, width, height, _size in areas:
        right = left + width
        clipped_left = max(left, bounds[0])
        clipped_right = min(right, bounds[1] - 1)
        if clipped_left <= clipped_right:
            clipped_width = clipped_right - clipped_left
            size = (clipped_width + 1) * (height + 1)
            result.append((clipped_left, top, clipped_width, height, size))
    return result


def choose_cuts(areas, map_width):
    per_column = [0] * map_width
    for left, _top, width, height, _size in areas:
        for x in range(left, left + width + 1):
            per_column[x] += height + 1

    total = sum(per_column)
    target = total / 4
    best = None
    # Three tiles provides 96 px of horizontal separation for a paired region;
    # the normal distance chooser can also separate the pair vertically.
    for cut1 in range(3, map_width - 8):
        for cut2 in range(cut1 + 3, map_width - 5):
            for cut3 in range(cut2 + 3, map_width - 2):
                cuts = (0, cut1, cut2, cut3, map_width)
                counts = [sum(per_column[cuts[i]:cuts[i + 1]]) for i in range(4)]
                if min(counts) < 16:
                    continue
                widths = [cuts[i + 1] - cuts[i] for i in range(4)]
                score = sum((count - target) ** 2 for count in counts)
                score += 4 * sum((width - map_width / 4) ** 2 for width in widths)
                candidate = (score, cuts, counts)
                if best is None or candidate < best:
                    best = candidate
    if best is None:
        raise ValueError("could not find four populated spawn zones")
    return best[1], best[2]


def encode_spawnareas(regions):
    values = bytearray()
    for region in regions:
        values += struct.pack("<i", len(region))
        for area in region:
            values += struct.pack("<5i", *area)
    return values


def author(path):
    parsed = read(path)
    base = parsed["spawnareas"][0]
    cuts, counts = choose_cuts(base, parsed["width"])
    team_regions = [split_areas(base, (cuts[i], cuts[i + 1])) for i in range(4)]
    if [sum(area[4] for area in region) for region in team_regions] != counts:
        raise AssertionError(f"area weights do not match cuts for {path}")

    data = bytearray(parsed["_data"])
    for y, row in enumerate(parsed["_spawn_offsets"]):
        for x, offset in enumerate(row):
            for team in range(4):
                outside = not (cuts[team] <= x < cuts[team + 1])
                data[offset + team + 1] = int(bool(parsed["nospawn"][y][x][0]) or outside)

    regions = [base, *team_regions, parsed["spawnareas"][5]]
    start = parsed["_spawnareas_start"]
    end = parsed["_spawnareas_end"]
    rewritten = data[:start] + encode_spawnareas(regions) + data[end:]
    temporary = path.with_suffix(path.suffix + ".tmp")
    temporary.write_bytes(rewritten)
    temporary.replace(path)
    return cuts, counts


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("maps", nargs="?", type=Path, default=DEFAULT_MAPS)
    args = parser.parse_args()
    paths = sorted(args.maps.glob("* Wide.map"))
    if not paths:
        raise SystemExit(f"no wide maps found in {args.maps}")
    for path in paths:
        cuts, counts = author(path)
        print(f"{path.name}: cuts={cuts[1:-1]} cells={tuple(counts)}")
    print(f"Authored four distinct spawn zones in {len(paths)} maps")


if __name__ == "__main__":
    main()
