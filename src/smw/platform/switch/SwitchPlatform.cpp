#include "SwitchPlatform.h"

#include "path.h"

#include <switch.h>

#include <SDL3/SDL_timer.h>

#include <algorithm>
#include <cstdio>
#include <filesystem>

namespace fs = std::filesystem;

extern "C" u32 __nx_applet_exit_mode;

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

void onFrame(uint64_t updateNs, uint64_t flipNs)
{
    static uint64_t frames = 0;
    static uint64_t window_start = SDL_GetTicks();
    static uint64_t update_sum = 0, update_max = 0, flip_sum = 0, flip_max = 0;
    frames++;
    update_sum += updateNs;
    flip_sum += flipNs;
    update_max = std::max(update_max, updateNs);
    flip_max = std::max(flip_max, flipNs);
    if (frames == 1)
        printf("[switch] first frame presented\n");
    if (frames % 3600 == 0) {
        const uint64_t now = SDL_GetTicks();
        printf("[switch] frame %llu, %.1f fps, uptime %llus, update avg %.2f max %.2f ms, flip avg %.2f max %.2f ms\n",
            (unsigned long long)frames, 3600000.0 / double(now - window_start),
            (unsigned long long)(now / 1000),
            update_sum / 3600.0 / 1e6, update_max / 1e6, flip_sum / 3600.0 / 1e6, flip_max / 1e6);
        window_start = now;
        update_sum = update_max = flip_sum = flip_max = 0;
    }
}

bool showEightPlayerPairing()
{
    HidLaControllerSupportArg arg;
    HidLaControllerSupportResultInfo result{};
    hidLaCreateControllerSupportArg(&arg);
    arg.hdr.player_count_min = 5;
    arg.hdr.player_count_max = 8;
    arg.hdr.enable_take_over_connection = 1;
    arg.hdr.enable_left_justify = 1;
    arg.hdr.enable_permit_joy_dual = 1;
    arg.enable_explain_text = 1;
    for (int player = 0; player < 8; ++player)
        hidLaSetExplainText(&arg, "Super Mario War", static_cast<HidNpadIdType>(player));

    const Result rc = hidLaShowControllerSupport(&result, &arg);
    printf("[switch] eight-player pairing rc=0x%x players=%d\n", rc, result.player_count);
    return R_SUCCEEDED(rc) && result.player_count >= 5;
}

void exit()
{
    appletSetMediaPlaybackState(false);
    std::fflush(stdout);
    romfsExit();
    // The NTON forwarder reloads its embedded NRO when an NRO returns normally.
    // Ask libnx's standard cleanup to self-exit the application instead.
    __nx_applet_exit_mode = 1;
}
} // namespace switch_platform
