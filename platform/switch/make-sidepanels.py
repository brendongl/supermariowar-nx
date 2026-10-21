#!/usr/bin/env python3
"""Draws the 16:9 side panel art (platform/switch/data/gfx/sidepanels/{left,right}.png).

The left panel (160x480) sits behind the in-game score boxes; the right one (53x480)
is a plain border. Text uses the game's own bitmap font so it matches the HUD.
    platform/switch/make-sidepanels.py
"""
import os
from PIL import Image

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..")
OUT = os.path.join(ROOT, "platform", "switch", "data", "gfx", "sidepanels")
FONT = os.path.join(ROOT, "data", "gfx", "packs", "Classic", "fonts", "font_small.png")
DICE_GUY = os.path.join(ROOT, "platform", "switch", "data", "gfx", "skins", "SipNPlay_DiceGuy.png")

BG = (28, 32, 56)
BRICK = (36, 42, 72)
MORTAR = (22, 25, 44)
GOLD = (232, 176, 48)
GOLD_DARK = (150, 104, 24)
MAGENTA = (255, 0, 255)


def bricks(w, h):
    im = Image.new("RGB", (w, h), BG)
    px = im.load()
    for y in range(h):
        row = y // 8
        for x in range(w):
            offset = 8 if row % 2 else 0
            mortar = y % 8 == 7 or (x + offset) % 16 == 15
            px[x, y] = MORTAR if mortar else BRICK
    return im


def divider(im, x):
    for y in range(im.height):
        im.putpixel((x, y), GOLD_DARK)
        im.putpixel((x + 1, y), GOLD)


def load_font():
    font = Image.open(FONT).convert("RGB")
    glyphs, x, w = [], 0, font.width
    while x < w and font.getpixel((x, 0)) != MAGENTA:
        x += 1
    while x < w:
        while x < w and font.getpixel((x, 0)) == MAGENTA:
            x += 1
        start = x
        while x < w and font.getpixel((x, 0)) != MAGENTA:
            x += 1
        if x > start:
            glyphs.append(font.crop((start, 1, x, font.height)))
    return glyphs


def text(im, glyphs, s, cx, y):
    widths = [glyphs[ord(c) - 33].width if c != " " else 6 for c in s]
    x = cx - sum(widths) // 2
    for c, gw in zip(s, widths):
        if c != " ":
            g = glyphs[ord(c) - 33]
            for gy in range(g.height):
                for gx in range(g.width):
                    p = g.getpixel((gx, gy))
                    if p not in (MAGENTA, (255, 255, 255)):  # the font is keyed on white
                        im.putpixel((x + gx, y + gy), p)
        x += gw


def sprite(im, src, frame, x0, y0, scale=1):
    for y in range(32):
        for x in range(32):
            p = src.getpixel((frame * 32 + x, y))
            if p != MAGENTA:
                for dy in range(scale):
                    for dx in range(scale):
                        im.putpixel((x0 + x * scale + dx, y0 + y * scale + dy), p)


def die(im, x0, y0, pips):
    """A 16x16 white die with dark pips."""
    for y in range(16):
        for x in range(16):
            corner = (x in (0, 15)) and (y in (0, 15))
            if corner:
                continue
            edge = x in (0, 15) or y in (0, 15)
            im.putpixel((x0 + x, y0 + y), (20, 20, 30) if edge else (240, 236, 224))
    spots = {1: [(7, 7)], 2: [(3, 3), (11, 11)], 3: [(3, 3), (7, 7), (11, 11)],
             4: [(3, 3), (11, 3), (3, 11), (11, 11)], 5: [(3, 3), (11, 3), (7, 7), (3, 11), (11, 11)],
             6: [(3, 3), (11, 3), (3, 7), (11, 7), (3, 11), (11, 11)]}[pips]
    for sx, sy in spots:
        for dy in (0, 1):
            for dx in (0, 1):
                im.putpixel((x0 + sx + dx, y0 + sy + dy), (192, 32, 32))


def main():
    os.makedirs(OUT, exist_ok=True)
    glyphs = load_font()
    guy = Image.open(DICE_GUY).convert("RGB")

    left = bricks(160, 480)
    divider(left, 158)
    text(left, glyphs, "Super Mario War", 79, 14)
    text(left, glyphs, "NX", 79, 32)
    sprite(left, guy, 0, 8, 440)
    text(left, glyphs, "sipnplay.cafe", 96, 452)
    left.save(os.path.join(OUT, "left.png"))

    right = bricks(53, 480)
    divider(right, 0)
    for i, y in enumerate(range(40, 400, 72)):
        die(right, 20, y, i % 6 + 1)
    sprite(right, guy, 2, 13, 430)
    right.save(os.path.join(OUT, "right.png"))


if __name__ == "__main__":
    main()
