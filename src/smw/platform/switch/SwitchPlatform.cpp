#include "SwitchPlatform.h"

#include "path.h"

#include <switch.h>

#include <SDL3/SDL_timer.h>

#include <cstdio>
#include <filesystem>

namespace fs = std::filesystem;

namespace {
void redirectLogs()
{
    const std::string home = GetHomeDirectory();
    const std::string log = home + "smw.log";
    std::error_code ec;
    fs::rename(log, home + "smw.prev.log", ec);
    if (std::freopen(log.c_str(), "w", stdout))
        std::setvbuf(stdout, nullptr, _IOLBF, 0);
    if (std::freopen(log.c_str(), "a", stderr))
        std::setvbuf(stderr, nullptr, _IONBF, 0);
}

// User-editable map filters ship in RomFS; copy them to the SD card once so edits persist
void seedFilters()
{
    const fs::path src = "romfs:/data/filters";
    const fs::path dst = GetHomeDirectory() + "filters";
    std::error_code ec;
    fs::create_directories(dst, ec);
    for (const auto& entry : fs::directory_iterator(src, ec)) {
        const fs::path target = dst / entry.path().filename();
        if (entry.is_regular_file() && !fs::exists(target))
            fs::copy_file(entry.path(), target, ec);
    }
}
} // namespace

namespace switch_platform {
void init()
{
    romfsInit();
    redirectLogs();
    seedFilters();
    // Keep running while docked with the screen dimmed; the game has no idle state
    appletSetMediaPlaybackState(true);
}

void onFrame()
{
    static uint64_t frames = 0;
    static uint64_t window_start = SDL_GetTicks();
    frames++;
    if (frames == 1)
        printf("[switch] first frame presented\n");
    if (frames % 3600 == 0) {
        const uint64_t now = SDL_GetTicks();
        printf("[switch] frame %llu, %.1f fps, uptime %llus\n",
            (unsigned long long)frames, 3600000.0 / double(now - window_start),
            (unsigned long long)(now / 1000));
        window_start = now;
    }
}

void exit()
{
    appletSetMediaPlaybackState(false);
    std::fflush(stdout);
    romfsExit();
}
} // namespace switch_platform
