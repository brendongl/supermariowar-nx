#include "MI_TeamSelect.h"

#include "gfx.h"
#include "Game.h"
#include "GameValues.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "FileList.h"
#include "ui/MI_Image.h"
#include "ui/ModernMenu.h"

#include <array>
#include <algorithm>
#include <string>
#include <vector>

extern CGameValues game_values;
extern CResourceManager* rm;
extern SDL_Surface* blitdest;

extern SkinList *skinlist;

namespace {
constexpr std::array<const char*, 16> FeaturedSkinNames {
    "Rollie", "Mario", "Luigi", "Peach", "Toad", "Yoshi", "Wario", "Bowser",
    "DK", "Kirby", "Link", "Samus", "Pikachu", "Sonic", "Megaman", "PacMan"
};

const std::vector<short>& featuredSkins()
{
    static std::vector<short> ids;
    if (!ids.empty()) return ids;
    for (const char* name : FeaturedSkinNames) {
        for (size_t index = 0; index < skinlist->count(); ++index) {
            if (skinlist->at(index).name == name) {
                ids.push_back(static_cast<short>(index));
                break;
            }
        }
    }
    if (ids.empty()) ids.push_back(0);
    return ids;
}

short featuredStep(short current, int direction)
{
    const auto& ids = featuredSkins();
    auto it = std::find(ids.begin(), ids.end(), current);
    int position = it == ids.end() ? 0 : static_cast<int>(it - ids.begin());
    position = (position + direction + static_cast<int>(ids.size())) % static_cast<int>(ids.size());
    return ids[position];
}

short randomFeatured()
{
    const auto& ids = featuredSkins();
    return ids[RANDOM_INT(static_cast<int>(ids.size()))];
}

const char* displaySkinName(short skin)
{
    const std::string& name = skinlist->at(skin).name;
    if (name == "DK") return "DONKEY KONG";
    if (name == "Megaman") return "MEGA MAN";
    if (name == "PacMan") return "PAC-MAN";
    if (name == "Rollie") return "ROLLIE";
    return name.c_str();
}
} // namespace

/**************************************
 * MI_TeamSelect Class
 **************************************/

MI_TeamSelect::MI_TeamSelect(gfxSprite * spr_background_ref, short x, short y) :
    UI_Control(x, y)
{
    spr = spr_background_ref;
    miImage = new MI_Image(spr, m_pos.x, m_pos.y, 0, 0, 416, 256, 1, 1, 0);

    for (short iTeam = 0; iTeam < MAX_PLAYERS; iTeam++) {
        iTeamCounts[iTeam] = game_values.teamcounts[iTeam];

        for (short iSlot = 0; iSlot < MAX_PLAYERS; iSlot++)
            iTeamIDs[iTeam][iSlot] = game_values.teamids[iTeam][iSlot];

        fReady[iTeam] = false;
    }

    iAnimationTimer = 0;
    iAnimationFrame = 0;
    iRandomAnimationFrame = 0;
}

MI_TeamSelect::~MI_TeamSelect()
{
    delete miImage;
}

void MI_TeamSelect::Update()
{
    if (++iAnimationTimer > 7) {
        iAnimationTimer = 0;

        iAnimationFrame += 2;
        if (iAnimationFrame > 2)
            iAnimationFrame = 0;

        iRandomAnimationFrame += 32;
        if (iRandomAnimationFrame >= 128)
            iRandomAnimationFrame = 0;
    }
}

void MI_TeamSelect::Draw()
{
    if (!m_visible)
        return;

    ModernMenu::header("CHOOSE YOUR FIGHTER");
    short joined = 0, ready = 0;
    for (short p = 0; p < MAX_PLAYERS; ++p) {
        if (game_values.playercontrol[p] > 0) {
            ++joined;
            if (fReady[p]) ++ready;
        }
    }
    const std::string status = std::to_string(joined) + " / 8 JOINED    "
        + std::to_string(ready) + " / " + std::to_string(joined) + " READY";
    rm->menu_font_small.drawRightJustified(App::screenWidth - 22, 17, status);

    constexpr int cardW = 200, cardH = 190, gapX = 8, gapY = 8;
    constexpr int x0 = 16, y0 = 50;
    for (short player = 0; player < MAX_PLAYERS; ++player) {
        const int x = x0 + (player % 4) * (cardW + gapX);
        const int y = y0 + (player / 4) * (cardH + gapY);
        const bool active = game_values.playercontrol[player] > 0;
        const short team = std::max<short>(0, GetTeam(player));
        Uint8 r, g, b;
        ModernMenu::teamColor(team, r, g, b);
        ModernMenu::panel(x, y, cardW, cardH, active ? 18 : 14, active ? 24 : 18, active ? 52 : 38);

        const std::string number = "P" + std::to_string(player + 1);
        if (!active) {
            rm->menu_font_small.draw(x + 12, y + 12, number);
            rm->menu_font_large.drawCentered(x + cardW / 2, y + 68, "?");
            rm->menu_font_small.drawCentered(x + cardW / 2, y + 122, "SET ON PLAYER SETUP");
            continue;
        }

        ModernMenu::strip(x + 5, y + 5, cardW - 10, 22, r, g, b);
        rm->spr_menu_boxed_numbers.draw(x + 9, y + 8,
            {player * 16, std::min<short>(game_values.colorids[player], 7) * 16, 16, 16});
        rm->menu_font_small.draw(x + 30, y + 9, number);
        rm->menu_font_small.drawRightJustified(x + cardW - 10, y + 9,
            game_values.playercontrol[player] == 2 ? "CPU" : "PLAYER");

        ModernMenu::fill(x + 6, y + 28, cardW - 12, 84, 112, 184, 224);
        ModernMenu::fill(x + 6, y + 94, cardW - 12, 18, 72, 152, 48);
        const SDL_Rect src{0, 0, 32, 32};
        const SDL_Rect dst{x + cardW / 2 - 32, y + 45, 64, 64};
        rm->spr_player[player][fReady[player] ? 0 : iAnimationFrame].drawStretch(src, blitdest, dst);

        const bool rollie = skinlist->at(game_values.skinids[player]).name == "Rollie";
        if (rollie) ModernMenu::strip(x + 16, y + 116, cardW - 32, 28, 232, 152, 16);
        rm->menu_font_large.drawChopCentered(x + cardW / 2, y + 119, cardW - 24,
            displaySkinName(game_values.skinids[player]));
        rm->menu_font_small.draw(x + 10, y + 151, "COLOR");
        for (short t = 0; t < 8; ++t) {
            Uint8 tr, tg, tb;
            ModernMenu::teamColor(t, tr, tg, tb);
            const int sx = x + 62 + t * 16;
            if (t == game_values.colorids[player]) ModernMenu::fill(sx - 2, y + 149, 16, 16, 248, 248, 255);
            ModernMenu::fill(sx, y + 151, 12, 12, tr, tg, tb);
        }
        if (fReady[player]) {
            ModernMenu::strip(x + 8, y + 167, cardW - 16, 18,
                game_values.playercontrol[player] == 2 ? 40 : 48,
                game_values.playercontrol[player] == 2 ? 72 : 152,
                game_values.playercontrol[player] == 2 ? 128 : 40);
            rm->menu_font_small.drawCentered(x + cardW / 2, y + 169,
                game_values.playercontrol[player] == 2 ? "CPU - NORMAL" : "READY!");
        } else {
            ModernMenu::strip(x + 8, y + 167, cardW - 16, 18, 60, 64, 92);
            rm->menu_font_small.drawCentered(x + cardW / 2, y + 169, "PICKING...");
        }
    }

    ModernMenu::footer("A Ready    L/R Fighter    ZL/ZR Color    Y Random    B Back",
        fAllReady ? "A  START MATCH" : nullptr);
}

MenuCodeEnum MI_TeamSelect::SendInput(CPlayerInput * playerInput)
{
    for (short iPlayer = 0; iPlayer < MAX_INPUT_PLAYERS; iPlayer++) {
        COutputControl* playerKeys = &playerInput->outputControls[iPlayer];

        if (game_values.playercontrol[iPlayer] == 1 && !fReady[iPlayer]) {
            if (playerKeys->menu_skin_prev.fPressed || playerKeys->menu_skin_next.fPressed) {
                const int direction = playerKeys->menu_skin_prev.fPressed ? -1 : 1;
                game_values.skinids[iPlayer] = featuredStep(game_values.skinids[iPlayer], direction);
                game_values.randomskin[iPlayer] = false;
                rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false);
            }

            if (playerKeys->menu_color_prev.fPressed || playerKeys->menu_color_next.fPressed) {
                const int direction = playerKeys->menu_color_prev.fPressed ? -1 : 1;
                game_values.colorids[iPlayer] = static_cast<short>(
                    (game_values.colorids[iPlayer] + direction + MAX_PLAYERS) % MAX_PLAYERS);
                rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false);
            }

            // Keep D-pad team selection for custom team modes.
            if (playerKeys->menu_left.fPressed) FindNewTeam(iPlayer, -1);
            if (playerKeys->menu_right.fPressed) FindNewTeam(iPlayer, 1);

            if (playerKeys->menu_random.fPressed) {
                game_values.skinids[iPlayer] = randomFeatured();
                game_values.randomskin[iPlayer] = false;
                rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false);
            }
        }

        if (game_values.playercontrol[iPlayer] == 1 && playerKeys->menu_select.fPressed) {
            fReady[iPlayer] = true;

            if (fAllReady && (DEVICE_KEYBOARD != playerInput->inputControls[iPlayer]->iDevice || iPlayer == 0)) {
                fModifying = false;
                if (game_values.matchtype == MatchType::Tournament ||
                        game_values.matchtype == MatchType::Tour ||
                        game_values.matchtype == MatchType::World ||
                        game_values.matchtype == MatchType::MiniGame) {
                    printf("MI_TeamSelect::SendInput MENU_CODE_TO_GAME_SETUP_MENU\n");
                    return MENU_CODE_TO_GAME_SETUP_MENU;
                }
                printf("MI_TeamSelect::SendInput MENU_CODE_TO_PLAY_CHOICE_MENU\n");
                return MENU_CODE_TO_PLAY_CHOICE_MENU;
            }

            fAllReady = true;
            for (short i = 0; i < MAX_PLAYERS; i++) {
                if (game_values.playercontrol[i] > 0 && !fReady[i]) {
                    fAllReady = false;
                    break;
                }
            }
        }

        if (playerKeys->menu_cancel.fPressed) {
            if (game_values.playercontrol[iPlayer] > 0 && fReady[iPlayer]) {
                fReady[iPlayer] = false;
                fAllReady = false;
            } else if (iPlayer == 0) {
                fModifying = false;
                return game_values.matchtype == MatchType::SingleGame || game_values.matchtype == MatchType::QuickGame
                    ? MENU_CODE_TO_MAIN_MENU : MENU_CODE_BACK_TO_MATCH_SELECTION_MENU;
            }
        }
        if (playerInput->inputControls[iPlayer] && DEVICE_KEYBOARD != playerInput->inputControls[iPlayer]->iDevice) {
            short team = GetTeam(iPlayer);
            gfx_setjoystickteamcolor(joystickFromIndex(playerInput->inputControls[iPlayer]->iDevice), team, fReady[iPlayer] ? 1.0 : 0.5);
        }
    }

    return MENU_CODE_NONE;
}

MenuCodeEnum MI_TeamSelect::Modify(bool modify)
{
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

void MI_TeamSelect::FindNewTeam(short iPlayerID, short iDirection)
{
    for (short iTeam = 0; iTeam < MAX_PLAYERS; iTeam++) {
        for (short iTeamItem = 0; iTeamItem < iTeamCounts[iTeam]; iTeamItem++) {
            if (iTeamIDs[iTeam][iTeamItem] == iPlayerID) {
                iTeamCounts[iTeam]--;

                for (int iMovePlayer = iTeamItem; iMovePlayer < iTeamCounts[iTeam]; iMovePlayer++)
                    iTeamIDs[iTeam][iMovePlayer] = iTeamIDs[iTeam][iMovePlayer + 1];

                const short iNewTeam = static_cast<short>(
                    (iTeam + iDirection + MAX_PLAYERS) % MAX_PLAYERS);

                iTeamIDs[iNewTeam][iTeamCounts[iNewTeam]] = iPlayerID;
                iTeamCounts[iNewTeam]++;

                if (game_values.teamcolors) {
                    game_values.colorids[iPlayerID] = iNewTeam;

                    //Skip skins that are invalid
                    while (!rm->LoadMenuSkin(iPlayerID, game_values.skinids[iPlayerID], iNewTeam, false)) {
                        if (++game_values.skinids[iPlayerID] >= skinlist->count())
                            game_values.skinids[iPlayerID] = 0;
                    }
                }

                return;
            }
        }
    }
}

void MI_TeamSelect::Reset()
{
    // Build an eight-team lobby from the current roster. Players can merge
    // teams with left/right after choosing their fighter.
    for (short iTeam = 0; iTeam < MAX_PLAYERS; ++iTeam) {
        iTeamCounts[iTeam] = 0;
        std::fill(std::begin(iTeamIDs[iTeam]), std::end(iTeamIDs[iTeam]), -1);
    }
    for (short iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer) {
        if (game_values.playercontrol[iPlayer] > 0) {
            iTeamIDs[iPlayer][0] = iPlayer;
            iTeamCounts[iPlayer] = 1;
        }
    }

    iAnimationTimer = 0;
    iAnimationFrame = 0;

    fAllReady = true;

    for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        iFastScroll[iPlayer] = 0;
        iFastScrollTimer[iPlayer] = 0;

        if (game_values.playercontrol[iPlayer] == 1) {
            fReady[iPlayer] = false;
            fAllReady = false;
        } else {
            fReady[iPlayer] = true;
        }

        //If debug build, then ignore the player ready stuff
#ifdef _DEBUG
        fReady[iPlayer] = true;
        fAllReady = true;
#endif

        if (game_values.playercontrol[iPlayer] == 0)
            continue;

        game_values.colorids[iPlayer] = std::clamp<short>(
            game_values.colorids[iPlayer], 0, MAX_PLAYERS - 1);

        const auto& featured = featuredSkins();
        if (std::find(featured.begin(), featured.end(), game_values.skinids[iPlayer]) == featured.end())
            game_values.skinids[iPlayer] = featured[iPlayer % featured.size()];
        game_values.randomskin[iPlayer] = false;

        //Skip skins that are invalid
        //TODO: this loops forever in emscripten build
        while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false)) {
            if (++game_values.skinids[iPlayer] >= skinlist->count())
                game_values.skinids[iPlayer] = 0;
        }
    }
}

short MI_TeamSelect::OrganizeTeams()
{
    iNumTeams = 0;
    for (short iTeam = 0; iTeam < MAX_PLAYERS; ++iTeam) {
        game_values.teamcounts[iTeam] = 0;
        std::fill(std::begin(game_values.teamids[iTeam]), std::end(game_values.teamids[iTeam]), -1);
    }
    for (short iTeam = 0; iTeam < MAX_PLAYERS; iTeam++) {

        if (iTeamCounts[iTeam] > 0) {
            for (short iTeamSpot = 0; iTeamSpot < iTeamCounts[iTeam]; iTeamSpot++)
                game_values.teamids[iNumTeams][iTeamSpot] = iTeamIDs[iTeam][iTeamSpot];

            game_values.teamcounts[iNumTeams] = iTeamCounts[iTeam];
            iNumTeams++;
        }
    }

    return iNumTeams;
}

short MI_TeamSelect::GetTeam(short iPlayerID)
{
    for (short iTeam = 0; iTeam < MAX_PLAYERS; iTeam++) {
        for (short iSlot = 0; iSlot < iTeamCounts[iTeam]; iSlot++) {
            if (iTeamIDs[iTeam][iSlot] == iPlayerID) {
                return iTeam;
            }
        }
    }

    return -1;
}
