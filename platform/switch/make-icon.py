#!/usr/bin/env python3
"""Builds the 256x256 Switch icon from the game's own menu artwork."""
import sys
from PIL import Image

menu = "data/gfx/packs/Classic/menu/"
out = sys.argv[1] if len(sys.argv) > 1 else "platform/switch/icon.jpg"

bg = Image.open(menu + "menu_background.png").convert("RGB")
bg = bg.crop((80, 0, 560, 480)).resize((256, 256), Image.NEAREST)

logo = Image.open(menu + "menu_smw.png").convert("RGBA")
px = logo.load()
for y in range(logo.height):
    for x in range(logo.width):
        r, g, b, _ = px[x, y]
        if (r, g, b) == (255, 0, 255):
            px[x, y] = (0, 0, 0, 0)
scale = 240 / logo.width
logo = logo.resize((240, round(logo.height * scale)), Image.NEAREST)
bg.paste(logo, ((256 - logo.width) // 2, (256 - logo.height) // 2), logo)
bg.save(out, "JPEG", quality=95)
