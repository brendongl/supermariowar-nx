#include "MI_PlayerSelect.h"

#include "GameValues.h"
#include "ResourceManager.h"
#include "ui/MI_Image.h"
#include "ui/ModernMenu.h"

extern CResourceManager* rm;
extern CGameValues game_values;


MI_PlayerSelect::MI_PlayerSelect(gfxSprite * nspr, short x, short y, std::string name, short width, short indent)
    : UI_Control(x, y)
    , spr(nspr)
    , szName(std::move(name))
    , iWidth(width)
    , iIndent(indent)
{
    miModifyImage = std::make_unique<MI_Image>(nspr, m_pos.x, m_pos.y - 6, 32, 128, 78, 78, 4, 1, 8);
    miModifyImage->setVisible(false);

    const short iSpacing = (width - indent - 8 * 34) / 9;
    for (size_t i = 0; i < iPlayerPosition.size(); i++) {
        iPlayerPosition[i] = iSpacing + indent + i * (iSpacing + 34);
    }

    SetImagePosition();
}

MenuCodeEnum MI_PlayerSelect::Modify(bool modify)
{
    miModifyImage->setVisible(modify);
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_PlayerSelect::SendInput(CPlayerInput* playerInput)
{
    COutputControl& keys = playerInput->outputControls[0];
    const auto validateRoster = [this]() {
        int active = 0;
        int humans = 0;
        for (short player = 0; player < game_values.localPlayerLimit; ++player) {
            const short state = game_values.playercontrol[player];
            if (state > 0) ++active;
            if (state == 1) ++humans;
        }
        if (humans < 1) {
            if (game_values.playercontrol[iSelectedPlayer] == 0)
                ++active;
            game_values.playercontrol[iSelectedPlayer] = 1;
        }
        if (active < 2) {
            for (short player = 0; player < game_values.localPlayerLimit; ++player) {
                if (player != iSelectedPlayer && game_values.playercontrol[player] == 0) {
                    game_values.playercontrol[player] = 2;
                    break;
                }
            }
        }
    };
    {
        const short playerLimit = game_values.localPlayerLimit;
        if (keys.menu_player_mode.fPressed)
            return MENU_CODE_TOGGLE_EIGHT_PLAYER_MODE;

        if (keys.menu_right.fPressed) {
            if (++iSelectedPlayer >= playerLimit)
                iSelectedPlayer = 0;

            SetImagePosition();
        }

        if (keys.menu_left.fPressed) {
            if (--iSelectedPlayer < 0)
                iSelectedPlayer = playerLimit - 1;

            SetImagePosition();
        }

        if (keys.menu_up.fPressed) {
            if (--game_values.playercontrol[iSelectedPlayer] < 0)
                game_values.playercontrol[iSelectedPlayer] = 2;

            validateRoster();
        }

        if (keys.menu_down.fPressed) {
            if (++game_values.playercontrol[iSelectedPlayer] > 2)
                game_values.playercontrol[iSelectedPlayer] = 0;

            validateRoster();
        }

        if (keys.menu_select.fPressed) {
            validateRoster();
            return MENU_CODE_TO_MATCH_SELECTION_MENU;
        }
        if (keys.menu_cancel.fPressed) return MENU_CODE_EXIT_APPLICATION;
        if (keys.menu_scrollfast.fPressed) return MENU_CODE_TO_OPTIONS_MENU;
        if (keys.menu_random.fPressed) return MENU_CODE_TO_CONTROLS_MENU;
    }

    return MENU_CODE_NONE;
}

void MI_PlayerSelect::SetImagePosition()
{
    if (iSelectedPlayer >= game_values.localPlayerLimit)
        iSelectedPlayer = game_values.localPlayerLimit - 1;
    const int cardWidth = game_values.localPlayerLimit == 4 ? 190 : 96;
    const int gap = game_values.localPlayerLimit == 4 ? 12 : 7;
    const int total = game_values.localPlayerLimit * cardWidth + (game_values.localPlayerLimit - 1) * gap;
    const int x0 = m_pos.x + (iWidth - total) / 2;
    miModifyImage->SetPosition(x0 + iSelectedPlayer * (cardWidth + gap) + cardWidth / 2 - 39, m_pos.y - 7);
}

void MI_PlayerSelect::Update()
{
    miModifyImage->Update();
}

void MI_PlayerSelect::Draw()
{
    if (!m_visible)
        return;

    ModernMenu::header("PLAYER SETUP");
    const short playerLimit = game_values.localPlayerLimit;
    const int cardWidth = playerLimit == 4 ? 190 : 96;
    const int cardHeight = playerLimit == 4 ? 154 : 132;
    const int gap = playerLimit == 4 ? 12 : 7;
    const int total = playerLimit * cardWidth + (playerLimit - 1) * gap;
    const int x0 = m_pos.x + (iWidth - total) / 2;
    for (short iPlayer = 0; iPlayer < playerLimit; iPlayer++) {
        const int x = x0 + iPlayer * (cardWidth + gap);
        const bool current = fModifying && iPlayer == iSelectedPlayer;
        const short state = game_values.playercontrol[iPlayer];
        ModernMenu::panel(x, m_pos.y, cardWidth, cardHeight,
            current ? 68 : 16, current ? 86 : 22, current ? 150 : 52);
        if (current) ModernMenu::strip(x + 5, m_pos.y + 5, cardWidth - 10, 18, 232, 152, 16);
        rm->menu_font_small.drawCentered(x + cardWidth / 2, m_pos.y + 8,
            ("PLAYER " + std::to_string(iPlayer + 1)).c_str());

        const char* stateName = state == 1 ? "PLAYER" : state == 2 ? "CPU" : "NONE";
        const Uint8 r = state == 1 ? 40 : state == 2 ? 48 : 56;
        const Uint8 g = state == 1 ? 152 : state == 2 ? 92 : 56;
        const Uint8 b = state == 1 ? 64 : state == 2 ? 184 : 72;
        ModernMenu::strip(x + 8, m_pos.y + 48, cardWidth - 16, 38, r, g, b);
        rm->menu_font_large.drawCentered(x + cardWidth / 2, m_pos.y + 55, stateName);
        if (current)
            rm->menu_font_small.drawCentered(x + cardWidth / 2, m_pos.y + 104, "UP / DOWN");
    }
    const char* modeAction = playerLimit == 4 ? "+ / -  PAIR FOR 8 PLAYERS" : "+ / -  RETURN TO 4 PLAYERS";
    ModernMenu::strip(182, 280, 500, 46, playerLimit == 4 ? 28 : 88,
        playerLimit == 4 ? 96 : 54, playerLimit == 4 ? 176 : 108);
    rm->menu_font_large.drawCentered(432, 291, modeAction);
    rm->menu_font_small.drawCentered(432, 345, "PLAYER 1 CONTROLS THIS SCREEN");
    ModernMenu::footer("LEFT/RIGHT Slot    UP/DOWN Type    X Options    Y Controls    B Exit", "A  CONTINUE");
}
