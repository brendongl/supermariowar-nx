#include "ModernMenu.h"

#include "Game.h"
#include "ResourceManager.h"

#include <algorithm>

extern SDL_Surface* blitdest;
extern CResourceManager* rm;

namespace {
Uint32 color(Uint8 r, Uint8 g, Uint8 b)
{
    return SDL_MapSurfaceRGB(blitdest, r, g, b);
}
}

namespace ModernMenu {

void fill(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_Rect rect{x, y, w, h};
    SDL_FillSurfaceRect(blitdest, &rect, color(r, g, b));
}

void panel(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b)
{
    fill(x + 3, y + 3, w, h, 0, 0, 0);
    fill(x, y, w, h, 0, 0, 0);
    fill(x + 2, y + 2, w - 4, h - 4, 232, 236, 248);
    fill(x + 4, y + 4, w - 8, h - 8, 0, 0, 0);
    fill(x + 5, y + 5, w - 10, h - 10, r, g, b);
}

void strip(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b)
{
    fill(x, y, w, h, 0, 0, 0);
    fill(x + 1, y + 1, w - 2, h - 2, r, g, b);
    fill(x + 2, y + 1, w - 4, 1,
        static_cast<Uint8>(std::min(255, r + 48)),
        static_cast<Uint8>(std::min(255, g + 48)),
        static_cast<Uint8>(std::min(255, b + 48)));
}

void backdrop()
{
    fill(0, 0, App::screenWidth, App::screenHeight, 10, 12, 32);
    if (App::screenWidth <= App::menuWidth) {
        rm->menu_backdrop.draw(0, 0);
        return;
    }

    // The legacy image has transparent padding around a 570x424 illustration.
    const int side = 147;
    const SDL_Rect leftSrc{36, 28, 64, 424};
    const SDL_Rect leftDst{0, 0, side, App::screenHeight};
    rm->menu_backdrop.drawStretch(leftSrc, blitdest, leftDst);
    const SDL_Rect centerSrc{36, 28, 570, 424};
    const SDL_Rect centerDst{side, 0, 570, App::screenHeight};
    rm->menu_backdrop.drawStretch(centerSrc, blitdest, centerDst);
    const SDL_Rect rightSrc{542, 28, 64, 424};
    const SDL_Rect rightDst{side + 570, 0, App::screenWidth - side - 570, App::screenHeight};
    rm->menu_backdrop.drawStretch(rightSrc, blitdest, rightDst);
}

void header(const char* title)
{
    panel(8, 6, App::screenWidth - 16, 36);
    rm->menu_font_large.draw(22, 11, title);
}

void footer(const char* hints, const char* action)
{
    fill(0, App::screenHeight - 30, App::screenWidth, 30, 14, 18, 42);
    fill(0, App::screenHeight - 30, App::screenWidth, 2, 72, 84, 140);
    rm->menu_font_small.draw(14, App::screenHeight - 21, hints);
    if (action) {
        const int width = rm->menu_font_small.getWidth(action) + 24;
        strip(App::screenWidth - width - 12, App::screenHeight - 25, width, 20, 192, 0, 24);
        rm->menu_font_small.drawCentered(App::screenWidth - width / 2 - 12, App::screenHeight - 21, action);
    }
}

void teamColor(short team, Uint8& r, Uint8& g, Uint8& b)
{
    constexpr Uint8 colors[8][3] = {
        {208, 32, 48}, {112, 184, 24}, {240, 184, 16}, {48, 128, 208},
        {128, 56, 208}, {232, 104, 16}, {16, 168, 184}, {232, 80, 152},
    };
    team = std::clamp<short>(team, 0, 7);
    r = colors[team][0]; g = colors[team][1]; b = colors[team][2];
}

} // namespace ModernMenu
