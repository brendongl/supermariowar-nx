#pragma once

namespace switch_platform {
void init();
void exit();
// Logs a heartbeat with the average frame rate, so hangs and slowdowns show up in smw.log
void onFrame();
} // namespace switch_platform
