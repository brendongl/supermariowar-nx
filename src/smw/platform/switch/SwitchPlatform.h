#pragma once

#include <cstdint>

namespace switch_platform {
void init();
void exit();
// Logs a heartbeat with the frame rate and the time spent updating/drawing (update) and
// presenting (flip), so hangs and slowdowns show up in smw.log
void onFrame(uint64_t updateNs, uint64_t flipNs);
bool showEightPlayerPairing();
} // namespace switch_platform
