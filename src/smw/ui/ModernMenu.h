#pragma once

#include <SDL3/SDL.h>

namespace ModernMenu {

constexpr int Width = 864;

void fill(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b);
void panel(int x, int y, int w, int h, Uint8 r = 16, Uint8 g = 22, Uint8 b = 52);
void strip(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b);
void backdrop();
void header(const char* title);
void footer(const char* hints, const char* action = nullptr);
void teamColor(short team, Uint8& r, Uint8& g, Uint8& b);

} // namespace ModernMenu
