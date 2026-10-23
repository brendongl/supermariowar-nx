#pragma once

#include <SDL3/SDL.h>


class GraphicsSDL {
public:
    ~GraphicsSDL();

    bool init(bool fullscreen);

    void flipScreen() const;
    void changeFullScreen(bool fullcreen) const;
    void setTitle(const char*) const;
    void showErrorBox(const char*) const;
    void takeScreenshot() const;

    // Optional 16:9 side panels around the 640x480 game: a HUD panel on the left
    // and a decorative border on the right (see SidePanels below). A widescreen map
    // (864 px) fills the screen instead, so the panels are off while one is played.
    bool sidePanelsAvailable() const { return sdl_left_surface != nullptr; }
    bool sidePanelsEnabled() const;
    SDL_Surface* leftPanel() const { return sdl_left_surface; }
    void setPanelBackgrounds(SDL_Surface* left, SDL_Surface* right);

private:
    // surface -> texture -> renderer -> window
    SDL_Window* sdl_window = nullptr;
    SDL_Renderer* sdl_renderer = nullptr;
    SDL_Surface* sdl_screen_surface = nullptr;
    SDL_Texture* sdl_screen_texture = nullptr;

    SDL_Surface* sdl_left_surface = nullptr;   // redrawn every frame (HUD)
    SDL_Surface* sdl_left_background = nullptr;
    SDL_Texture* sdl_left_texture = nullptr;
    SDL_Texture* sdl_right_texture = nullptr;  // static border

    mutable int presented_width = 0;  // logical width last given to the renderer
};

