#include "MI_TeamSelect.h"

#include "gfx.h"
#include "Game.h"
#include "GameValues.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "FileList.h"
#include "ui/MI_Image.h"
#include "ui/ModernMenu.h"

extern CGameValues game_values;
extern CResourceManager* rm;
extern SDL_Surface* blitdest;

extern SkinList *skinlist;

/**************************************
 * MI_TeamSelect Class
 **************************************/

MI_TeamSelect::MI_TeamSelect(gfxSprite * spr_background_ref, short x, short y) :
    UI_Control(x, y)
{
    spr = spr_background_ref;
    miImage = new MI_Image(spr, m_pos.x, m_pos.y, 0, 0, 416, 256, 1, 1, 0);

    for (short iTeam = 0; iTeam < 4; iTeam++) {
        iTeamCounts[iTeam] = game_values.teamcounts[iTeam];

        for (short iSlot = 0; iSlot < 3; iSlot++)
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
        const short team = player < MAX_INPUT_PLAYERS ? std::max<short>(0, GetTeam(player)) : player;
        Uint8 r, g, b;
        ModernMenu::teamColor(team, r, g, b);
        ModernMenu::panel(x, y, cardW, cardH, active ? 18 : 14, active ? 24 : 18, active ? 52 : 38);

        const std::string number = "P" + std::to_string(player + 1);
        if (!active) {
            rm->menu_font_small.draw(x + 12, y + 12, number);
            rm->menu_font_large.drawCentered(x + cardW / 2, y + 68, "?");
            rm->menu_font_small.drawCentered(x + cardW / 2, y + 122,
                player >= MAX_INPUT_PLAYERS ? "X  ADD CPU" : "JOIN ON MAIN MENU");
            continue;
        }

        ModernMenu::strip(x + 5, y + 5, cardW - 10, 22, r, g, b);
        rm->spr_menu_boxed_numbers.draw(x + 9, y + 8,
            {player * 16, std::min<short>(team, 7) * 16, 16, 16});
        rm->menu_font_small.draw(x + 30, y + 9, number);
        rm->menu_font_small.drawRightJustified(x + cardW - 10, y + 9,
            game_values.playercontrol[player] == 2 ? "CPU" : "PLAYER");

        ModernMenu::fill(x + 6, y + 28, cardW - 12, 84, 112, 184, 224);
        ModernMenu::fill(x + 6, y + 94, cardW - 12, 18, 72, 152, 48);
        const SDL_Rect src{0, 0, 32, 32};
        const SDL_Rect dst{x + cardW / 2 - 32, y + 45, 64, 64};
        rm->spr_player[player][fReady[player] ? 0 : iAnimationFrame].drawStretch(src, blitdest, dst);

        const char* skinName = game_values.randomskin[player]
            ? "Random" : skinlist->at(game_values.skinids[player]).name.c_str();
        rm->menu_font_large.drawChopCentered(x + cardW / 2, y + 119, cardW - 24, skinName);
        rm->menu_font_small.draw(x + 10, y + 151, "TEAM");
        for (short t = 0; t < 8; ++t) {
            Uint8 tr, tg, tb;
            ModernMenu::teamColor(t, tr, tg, tb);
            const int sx = x + 62 + t * 16;
            if (t == team) ModernMenu::fill(sx - 2, y + 149, 16, 16, 248, 248, 255);
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

    ModernMenu::footer("A Ready    B Back    X CPU    Y Random    L/R Team",
        fAllReady ? "A  START MATCH" : nullptr);
}

MenuCodeEnum MI_TeamSelect::SendInput(CPlayerInput * playerInput)
{
    if (playerInput->outputControls[0].menu_scrollfast.fPressed) {
        short slot = -1;
        for (short p = MAX_INPUT_PLAYERS; p < MAX_PLAYERS; ++p) {
            if (!game_values.playercontrol[p]) { slot = p; break; }
        }
        if (slot >= 0) {
            game_values.playercontrol[slot] = 2;
            game_values.randomskin[slot] = true;
            game_values.skinids[slot] = RANDOM_INT(skinlist->count());
        } else {
            for (short p = MAX_PLAYERS - 1; p >= MAX_INPUT_PLAYERS; --p) {
                if (game_values.playercontrol[p]) { game_values.playercontrol[p] = 0; break; }
            }
        }
        Reset();
        return MENU_CODE_NONE;
    }

    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        COutputControl * playerKeys = &game_values.playerInput.outputControls[iPlayer];

        if (game_values.playercontrol[iPlayer] > 0 && !fReady[iPlayer]) { //if this player is player or cpu
            if (playerKeys->menu_left.fPressed) {
                if (playerKeys->menu_right.fDown)
                    game_values.randomskin[iPlayer] = !game_values.randomskin[iPlayer];
                else
                    FindNewTeam(iPlayer, -1);
            }

            if (playerKeys->menu_right.fPressed) {
                if (playerKeys->menu_left.fDown)
                    game_values.randomskin[iPlayer] = !game_values.randomskin[iPlayer];
                else
                    FindNewTeam(iPlayer, 1);
            }

            //Scroll up/down through player skins
            if (!game_values.randomskin[iPlayer]) {
                if (playerKeys->menu_up.fPressed) {
                    do {
                        if (playerKeys->menu_down.fDown) {
                            game_values.skinids[iPlayer] = RANDOM_INT(skinlist->count());
                        } else {
                            if (--game_values.skinids[iPlayer] < 0)
                                game_values.skinids[iPlayer] = (short)skinlist->count() - 1;
                        }
                    } while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false));
                } else if (playerKeys->menu_up.fDown) {
                    if (iFastScroll[iPlayer] == 0) {
                        if (++iFastScrollTimer[iPlayer] > 40) {
                            iFastScroll[iPlayer] = 1;
                        }
                    } else {
                        if (++iFastScrollTimer[iPlayer] > 5) {
                            do {
                                if (--game_values.skinids[iPlayer] < 0)
                                    game_values.skinids[iPlayer] = (short)skinlist->count() - 1;
                            } while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false));

                            iFastScrollTimer[iPlayer] = 0;
                        }
                    }
                }

                if (playerKeys->menu_down.fPressed) {
                    do {
                        if (playerKeys->menu_up.fDown) {
                            game_values.skinids[iPlayer] = RANDOM_INT( skinlist->count());
                        } else {
                            if (++game_values.skinids[iPlayer] >= skinlist->count())
                                game_values.skinids[iPlayer] = 0;
                        }
                    } while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false));
                } else if (playerKeys->menu_down.fDown) {
                    if (iFastScroll[iPlayer] == 0) {
                        if (++iFastScrollTimer[iPlayer] > 40) {
                            iFastScroll[iPlayer] = 1;
                        }
                    } else {
                        if (++iFastScrollTimer[iPlayer] > 5) {
                            do {
                                if (++game_values.skinids[iPlayer] >= skinlist->count())
                                    game_values.skinids[iPlayer] = 0;
                            } while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false));

                            iFastScrollTimer[iPlayer] = 0;
                        }
                    }
                }

                if ((!playerKeys->menu_up.fDown && !playerKeys->menu_down.fDown) ||
                        (playerKeys->menu_up.fDown && playerKeys->menu_down.fDown)) {
                    iFastScroll[iPlayer] = 0;
                    iFastScrollTimer[iPlayer] = 0;
                }
            } else {
                iFastScroll[iPlayer] = 0;
                iFastScrollTimer[iPlayer] = 0;
            }

            if (playerKeys->menu_random.fPressed) {
                if (playerKeys->menu_scrollfast.fDown) {
                    game_values.randomskin[iPlayer] = !game_values.randomskin[iPlayer];
                } else if (!game_values.randomskin[iPlayer]) {
                    do {
                        game_values.skinids[iPlayer] = RANDOM_INT(skinlist->count());
                    } while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false));
                }
            }
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed) {
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

        if (playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            if (game_values.playercontrol[iPlayer] > 0 && fReady[iPlayer]) {
                fReady[iPlayer] = false;
                fAllReady = false;
            } else {
                if (DEVICE_KEYBOARD != playerInput->inputControls[iPlayer]->iDevice || iPlayer == 0) {
                    fModifying = false;
                    return game_values.matchtype == MatchType::SingleGame || game_values.matchtype == MatchType::QuickGame
                        ? MENU_CODE_TO_MAIN_MENU : MENU_CODE_BACK_TO_MATCH_SELECTION_MENU;
                }
            }
        }
        if (DEVICE_KEYBOARD != playerInput->inputControls[iPlayer]->iDevice) {
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
    for (short iTeam = 0; iTeam < 4; iTeam++) {
        for (short iTeamItem = 0; iTeamItem < iTeamCounts[iTeam]; iTeamItem++) {
            if (iTeamIDs[iTeam][iTeamItem] == iPlayerID) {
                iTeamCounts[iTeam]--;

                for (int iMovePlayer = iTeamItem; iMovePlayer < iTeamCounts[iTeam]; iMovePlayer++)
                    iTeamIDs[iTeam][iMovePlayer] = iTeamIDs[iTeam][iMovePlayer + 1];

                short iNewTeam = iTeam;
                bool fOnlyTeam = true;

                do {
                    iNewTeam += iDirection;

                    if (iNewTeam < 0)
                        iNewTeam = 3;
                    else if (iNewTeam > 3)
                        iNewTeam = 0;

                    fOnlyTeam = true;
                    for (int iMovePlayer = 0; iMovePlayer < 4; iMovePlayer++) {
                        if (iMovePlayer == iNewTeam)
                            continue;

                        if (iTeamCounts[iMovePlayer] > 0) {
                            fOnlyTeam = false;
                            break;
                        }
                    }
                } while (fOnlyTeam);

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
    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        short iTeamID;
        short iSlotID;
        bool fFound = false;
        for (iTeamID = 0; iTeamID < 4; iTeamID++) {
            for (iSlotID = 0; iSlotID < iTeamCounts[iTeamID]; iSlotID++) {
                if (iTeamIDs[iTeamID][iSlotID] == iPlayer) {
                    fFound = true;
                    break;
                }
            }

            if (fFound)
                break;
        }

        if (fFound) {
            //Need to remove the player
            if (game_values.playercontrol[iPlayer] == 0) {
                iTeamCounts[iTeamID]--;

                if (iTeamCounts[iTeamID] > iSlotID) {
                    for (short iSlot = iSlotID; iSlot < iTeamCounts[iTeamID]; iSlot++) {
                        iTeamIDs[iTeamID][iSlot] = iTeamIDs[iTeamID][iSlot + 1];
                    }
                }
            }
        } else {
            //A new player was added so find a spot for him
            if (game_values.playercontrol[iPlayer] > 0) {
                short iLookForNewTeam = iPlayer;

                while (iTeamCounts[iLookForNewTeam] >= 3) {
                    if (++iLookForNewTeam >= 4)
                        iLookForNewTeam = 0;
                }

                iTeamIDs[iLookForNewTeam][iTeamCounts[iLookForNewTeam]] = iPlayer;
                iTeamCounts[iLookForNewTeam]++;

                if (game_values.teamcolors)
                    game_values.colorids[iPlayer] = iLookForNewTeam;
            }
        }
    }

    //Check to see if there is only one team and if so, split them up

    short iCountTeams = 0;
    short iLastTeam = 0;
    for (short iTeamID = 0; iTeamID < 4; iTeamID++) {
        if (iTeamCounts[iTeamID] > 0) {
            iCountTeams++;
            iLastTeam = iTeamID;
        }
    }

    if (iCountTeams == 1) {
        short iLookForNewTeam = iLastTeam;
        if (++iLookForNewTeam >= 4)
            iLookForNewTeam = 0;

        iTeamCounts[iLastTeam]--;
        short iPlayer = iTeamIDs[iLastTeam][iTeamCounts[iLastTeam]];
        iTeamIDs[iLookForNewTeam][iTeamCounts[iLookForNewTeam]] = iPlayer;
        iTeamCounts[iLookForNewTeam]++;

        if (game_values.teamcolors)
            game_values.colorids[iPlayer] = iLookForNewTeam;
    }

    iAnimationTimer = 0;
    iAnimationFrame = 0;

    fAllReady = true;

    for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        if (iPlayer >= MAX_INPUT_PLAYERS) {
            fReady[iPlayer] = true;
            if (game_values.playercontrol[iPlayer] > 0) {
                game_values.colorids[iPlayer] = iPlayer;
                while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false)) {
                    if (++game_values.skinids[iPlayer] >= skinlist->count()) game_values.skinids[iPlayer] = 0;
                }
            }
            continue;
        }
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

        if (game_values.teamcolors)
            game_values.colorids[iPlayer] = GetTeam(iPlayer);
        else
            game_values.colorids[iPlayer] = iPlayer;

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
    for (short iTeam = 0; iTeam < MAX_PLAYERS; ++iTeam)
        game_values.teamcounts[iTeam] = 0;
    for (short iTeam = 0; iTeam < 4; iTeam++) {
        game_values.teamcounts[iTeam] = 0;

        if (iTeamCounts[iTeam] > 0) {
            for (short iTeamSpot = 0; iTeamSpot < 3; iTeamSpot++)
                game_values.teamids[iNumTeams][iTeamSpot] = iTeamIDs[iTeam][iTeamSpot];

            game_values.teamcounts[iNumTeams] = iTeamCounts[iTeam];
            iNumTeams++;
        }
    }

    for (short player = MAX_INPUT_PLAYERS; player < MAX_PLAYERS; ++player) {
        if (!game_values.playercontrol[player]) continue;
        game_values.playercontrol[player] = 2;
        game_values.teamids[iNumTeams][0] = player;
        game_values.teamcounts[iNumTeams++] = 1;
        game_values.colorids[player] = player;
        game_values.randomskin[player] = true;
    }
    return iNumTeams;
}

short MI_TeamSelect::GetTeam(short iPlayerID)
{
    for (short iTeam = 0; iTeam < 4; iTeam++) {
        for (short iSlot = 0; iSlot < iTeamCounts[iTeam]; iSlot++) {
            if (iTeamIDs[iTeam][iSlot] == iPlayerID) {
                return iTeam;
            }
        }
    }

    return -1;
}
