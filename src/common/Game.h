#pragma once

#include <string_view>

/// Creates the user-writable settings directory, if it doesn't exist yet.
void ensureSettingsDir();


class App {
public:
    /// Width of the play area: 640, or 864 while a widescreen map is being played
    static inline int screenWidth = 640;
    static constexpr int menuWidth = 640;
    static constexpr int maxScreenWidth = 864;
    static constexpr int screenHeight = 480;
    static constexpr int menuTransparency = 72;

    static void registerSdlMetadata(std::string_view app_title);
};
