#pragma once

#include <array>
#include <cstdint>

struct PlayerIdentityColor {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

// One identity palette used by the roster, fighter cards and Switch pairing UI.
inline constexpr std::array<PlayerIdentityColor, 8> kPlayerIdentityColors{{
    {208, 32, 48},   // P1 red
    {112, 184, 24},  // P2 green
    {240, 184, 16},  // P3 yellow
    {48, 128, 208},  // P4 blue
    {128, 56, 208},  // P5 purple
    {232, 104, 16},  // P6 orange
    {16, 168, 184},  // P7 cyan
    {232, 80, 152},  // P8 pink
}};

inline constexpr std::array<const char*, 8> kPlayerIdentityLabels{{
    "P1  RED", "P2  GREEN", "P3  YELLOW", "P4  BLUE",
    "P5  PURPLE", "P6  ORANGE", "P7  CYAN", "P8  PINK",
}};

// Existing maps contain four team-spawn regions. Eight teams share them in
// opposite pairs while FindSpawnPoint chooses separated positions in a region.
inline constexpr short playerSpawnRegion(short teamId)
{
    return teamId >= 0 ? static_cast<short>((teamId % 4) + 1) : 0;
}
