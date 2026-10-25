#include "doctest.h"

#include "PlayerIdentity.h"

#include <set>
#include <tuple>

TEST_CASE("eight player identities have unique colors")
{
    constexpr std::array<PlayerIdentityColor, 8> expected{{
        {208, 32, 48}, {112, 184, 24}, {240, 184, 16}, {48, 128, 208},
        {128, 56, 208}, {232, 104, 16}, {16, 168, 184}, {232, 80, 152},
    }};
    std::set<std::tuple<int, int, int>> colors;
    for (std::size_t player = 0; player < kPlayerIdentityColors.size(); ++player) {
        const PlayerIdentityColor color = kPlayerIdentityColors[player];
        colors.emplace(color.r, color.g, color.b);
        CHECK(color.r == expected[player].r);
        CHECK(color.g == expected[player].g);
        CHECK(color.b == expected[player].b);
    }

    CHECK(kPlayerIdentityColors.size() == 8);
    CHECK(colors.size() == 8);
}

TEST_CASE("eight teams map onto four compatible spawn regions")
{
    constexpr std::array<short, 8> expected{{1, 2, 3, 4, 1, 2, 3, 4}};
    for (short team = 0; team < 8; ++team)
        CHECK(playerSpawnRegion(team) == expected[team]);

    CHECK(playerSpawnRegion(-1) == 0);
}
