#include "gfxSDL.h"

#include "gfx.h"
#include "Game.h"

#include "path.h"

#include <SDL3_image/SDL_image.h>

#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <cstdlib>

extern SDL_Surface* screen;

#define GFX_BPP 16
#define GFX_SCREEN_W 640
#define GFX_SCREEN_H 480

template<typename... Args>
[[noreturn]] void throw_error(std::format_string<Args...> fmt, Args&&... args) {
    throw std::runtime_error(std::format(fmt, std::forward<Args>(args)...));
}

namespace {
void initSdl()
{
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK))
        throw_error("SDL error: {}", SDL_GetError());

    const auto sdl_version = SDL_GetVersion();
    printf("[init] SDL %d.%d.%d loaded.\n",
        SDL_VERSIONNUM_MAJOR(sdl_version),
        SDL_VERSIONNUM_MINOR(sdl_version),
        SDL_VERSIONNUM_MICRO(sdl_version));

    const auto sdlimg_version = IMG_Version();
    printf("[gfx] SDL_image %d.%d.%d loaded.\n",
        SDL_VERSIONNUM_MAJOR(sdlimg_version),
        SDL_VERSIONNUM_MINOR(sdlimg_version),
        SDL_VERSIONNUM_MICRO(sdlimg_version));
}

void quitSdl()
{
    SDL_Quit();
}

SDL_Window* createWindow(bool fullscreen)
{
    Uint32 window_flags = SDL_WINDOW_RESIZABLE;
    if (fullscreen)
        window_flags |= SDL_WINDOW_FULLSCREEN;
    int window_w = GFX_SCREEN_W;
    int window_h = GFX_SCREEN_H;
#ifdef __SWITCH__
    // One EGL window covering the screen (720p handheld, 1080p docked). SDL resizes
    // resizable windows on dock changes and the renderer letterboxes the 640x480 game.
    window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if (const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay())) {
        window_w = mode->w;
        window_h = mode->h;
    }
#endif

    SDL_Window* window = SDL_CreateWindow("smw",
        window_w, window_h,
        window_flags);
    if (!window)
        throw_error("Couldn't create window: {}", SDL_GetError());

    return window;
}

const char* findPreferredRendererIndex() {
#ifdef SDL2_FORCE_GLES
    return "opengles";
#endif
    return nullptr;
}

bool wantSidePanels()
{
#ifdef __SWITCH__
    return !SDL_getenv("SMW_NO_SIDE_PANELS");
#else
    return SDL_getenv("SMW_SIDE_PANELS") != nullptr;
#endif
}

int logicalWidth()
{
    return wantSidePanels() ? SidePanels::LeftWidth + GFX_SCREEN_W + SidePanels::RightWidth : GFX_SCREEN_W;
}

SDL_Renderer* createRenderer(SDL_Window* window)
{
    SDL_Renderer* renderer = SDL_CreateRenderer(window, findPreferredRendererIndex());
    if (!renderer)
        throw_error("Couldn't create renderer: {}", SDL_GetError());

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetRenderLogicalPresentation(renderer, logicalWidth(), GFX_SCREEN_H, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    printf("[gfx] Renderer: %s\n", SDL_GetRendererName(renderer));
    return renderer;
}

SDL_Surface* createScreenSurface(int w = GFX_SCREEN_W)
{
    SDL_Surface* surface = SDL_CreateSurface(w, GFX_SCREEN_H, SDL_PIXELFORMAT_ARGB8888);
    if (!surface)
        throw_error("Couldn't create video buffer: {}", SDL_GetError());

    return surface;
}

SDL_Texture* createScreenTexture(SDL_Renderer* renderer, int w = GFX_SCREEN_W)
{
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        w, GFX_SCREEN_H);
    if (!texture)
        throw_error("Couldn't create video texture: {}", SDL_GetError());

    if (!SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST))
        throw_error("Couldn't set up texture scaling mode: %s\n", SDL_GetError());

    return texture;
}
} // namespace


bool GraphicsSDL::init(bool fullscreen)
{
    initSdl();
    sdl_window = createWindow(fullscreen);
    sdl_renderer = createRenderer(sdl_window);
    // Wide enough for widescreen maps; 640 px games use the left part
    sdl_screen_surface = createScreenSurface(App::maxScreenWidth);
    sdl_screen_texture = createScreenTexture(sdl_renderer, App::maxScreenWidth);

    if (wantSidePanels()) {
        sdl_left_surface = createScreenSurface(SidePanels::LeftWidth);
        sdl_left_background = createScreenSurface(SidePanels::LeftWidth);
        sdl_left_texture = createScreenTexture(sdl_renderer, SidePanels::LeftWidth);
        sdl_right_texture = createScreenTexture(sdl_renderer, SidePanels::RightWidth);
        SDL_FillSurfaceRect(sdl_left_background, nullptr, SDL_MapSurfaceRGB(sdl_left_background, 0, 0, 0));
        SDL_FillSurfaceRect(sdl_left_surface, nullptr, SDL_MapSurfaceRGB(sdl_left_surface, 0, 0, 0));
        SDL_Surface* black = createScreenSurface(SidePanels::RightWidth);
        SDL_FillSurfaceRect(black, nullptr, SDL_MapSurfaceRGB(black, 0, 0, 0));
        SDL_UpdateTexture(sdl_right_texture, nullptr, black->pixels, black->pitch);
        SDL_DestroySurface(black);
        printf("[gfx] Side panels enabled (%d + %d + %d)\n", SidePanels::LeftWidth, GFX_SCREEN_W, SidePanels::RightWidth);
    }

    printf("[gfx] Game window initialized (%dx%d, %dbpp)\n",
        sdl_screen_surface->w,
        sdl_screen_surface->h,
        SDL_BITSPERPIXEL(sdl_screen_surface->format));

    screen = sdl_screen_surface;
    return true;
}

bool GraphicsSDL::sidePanelsEnabled() const
{
    return sidePanelsAvailable() && App::screenWidth <= GFX_SCREEN_W;
}

void GraphicsSDL::setPanelBackgrounds(SDL_Surface* left, SDL_Surface* right)
{
    if (!sidePanelsAvailable())
        return;

    if (left) {
        SDL_BlitSurface(left, nullptr, sdl_left_background, nullptr);
        SDL_BlitSurface(sdl_left_background, nullptr, sdl_left_surface, nullptr);
    }
    if (right) {
        SDL_Surface* converted = SDL_ConvertSurface(right, SDL_PIXELFORMAT_ARGB8888);
        if (converted) {
            SDL_UpdateTexture(sdl_right_texture, nullptr, converted->pixels, converted->pitch);
            SDL_DestroySurface(converted);
        }
    }
}

GraphicsSDL::~GraphicsSDL()
{
    SDL_DestroyTexture(sdl_left_texture);
    SDL_DestroyTexture(sdl_right_texture);
    SDL_DestroySurface(sdl_left_surface);
    SDL_DestroySurface(sdl_left_background);
    SDL_DestroyTexture(sdl_screen_texture);
    SDL_DestroySurface(sdl_screen_surface);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);
    quitSdl();
}

void GraphicsSDL::showErrorBox(const char* message) const
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, sdl_window);
}

void GraphicsSDL::setTitle(const char* title) const
{
    SDL_SetWindowTitle(sdl_window, title);
}

void GraphicsSDL::flipScreen() const
{
    SDL_UpdateTexture(sdl_screen_texture, nullptr, sdl_screen_surface->pixels, sdl_screen_surface->pitch);

    // The play area is 640 px, or 864 px on a widescreen map (no side panels then)
    const int playWidth = App::screenWidth;
    const int wantWidth = playWidth > GFX_SCREEN_W ? playWidth : logicalWidth();
    if (wantWidth != presented_width) {
        SDL_SetRenderLogicalPresentation(sdl_renderer, wantWidth, GFX_SCREEN_H, SDL_LOGICAL_PRESENTATION_LETTERBOX);
        presented_width = wantWidth;
    }
    const SDL_FRect src {0, 0, static_cast<float>(playWidth), GFX_SCREEN_H};

    SDL_RenderClear(sdl_renderer);
    if (sidePanelsEnabled()) {
        SDL_UpdateTexture(sdl_left_texture, nullptr, sdl_left_surface->pixels, sdl_left_surface->pitch);
        const SDL_FRect left {0, 0, SidePanels::LeftWidth, GFX_SCREEN_H};
        const SDL_FRect game {SidePanels::LeftWidth, 0, GFX_SCREEN_W, GFX_SCREEN_H};
        const SDL_FRect right {SidePanels::LeftWidth + GFX_SCREEN_W, 0, SidePanels::RightWidth, GFX_SCREEN_H};
        SDL_RenderTexture(sdl_renderer, sdl_left_texture, nullptr, &left);
        SDL_RenderTexture(sdl_renderer, sdl_screen_texture, &src, &game);
        SDL_RenderTexture(sdl_renderer, sdl_right_texture, nullptr, &right);
        // The HUD is drawn again next frame, on a clean background
        SDL_BlitSurface(sdl_left_background, nullptr, sdl_left_surface, nullptr);
    } else {
        SDL_RenderTexture(sdl_renderer, sdl_screen_texture, &src, nullptr);
    }
    SDL_RenderPresent(sdl_renderer);
}

void GraphicsSDL::changeFullScreen(bool fullscreen) const
{
#ifdef __SWITCH__
    // Always full screen; SDL sizes the window to the current output
    return;
#endif
    if (!SDL_SetWindowFullscreen(sdl_window, fullscreen)) {
        fprintf(stderr, "[gfx] Couldn't toggle fullscreen mode: %s\n", SDL_GetError());
        return;
    }
}

void GraphicsSDL::takeScreenshot() const
{
    using std::chrono::system_clock;

    // NOTE: %F and %T don't work on Windows
    constexpr const char* path_format = "screenshots/%Y-%m-%d_%H%M%S.png";

    const std::time_t now = system_clock::to_time_t(system_clock::now());
    std::ostringstream path_ss;
    path_ss << std::put_time(std::localtime(&now), path_format);
    const std::string path = convertPath(path_ss.str());

    if (IMG_SavePNG(sdl_screen_surface, path.c_str()) != 0) {
        fprintf(stderr, "[gfx] Couldn't write the screenshot to file: %s\n", SDL_GetError());
        return;
    }

    printf("[gfx] Screenshot saved to file: %s\n", path.c_str());
}
