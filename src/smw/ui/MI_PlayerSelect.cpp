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
        for (short state : game_values.playercontrol) {
            if (state > 0) ++active;
            if (state == 1) ++humans;
        }
        if (active < 2 || humans < 1)
            game_values.playercontrol[iSelectedPlayer] = 1;
    };
    {
        if (keys.menu_right.fPressed) {
            if (++iSelectedPlayer > 7)
                iSelectedPlayer = 0;

            SetImagePosition();
        }

        if (keys.menu_left.fPressed) {
            if (--iSelectedPlayer < 0)
                iSelectedPlayer = 7;

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

        if (keys.menu_select.fPressed || keys.menu_cancel.fPressed) {
            miModifyImage->setVisible(false);
            fModifying = false;
            return MENU_CODE_UNSELECT_ITEM;
        }
    }

    return MENU_CODE_NONE;
}

void MI_PlayerSelect::SetImagePosition()
{
    miModifyImage->SetPosition(m_pos.x + iPlayerPosition[iSelectedPlayer] - 22, m_pos.y - 7);
}

void MI_PlayerSelect::Update()
{
    miModifyImage->Update();
}

void MI_PlayerSelect::Draw()
{
    if (!m_visible)
        return;

    constexpr int cardWidth = 96;
    constexpr int gap = 7;
    for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        const int x = m_pos.x + iPlayer * (cardWidth + gap);
        const bool current = fModifying && iPlayer == iSelectedPlayer;
        const short state = game_values.playercontrol[iPlayer];
        ModernMenu::panel(x, m_pos.y, cardWidth, 96,
            current ? 68 : 16, current ? 86 : 22, current ? 150 : 52);
        if (current) ModernMenu::strip(x + 5, m_pos.y + 5, cardWidth - 10, 18, 232, 152, 16);
        rm->menu_font_small.drawCentered(x + cardWidth / 2, m_pos.y + 8,
            ("PLAYER " + std::to_string(iPlayer + 1)).c_str());

        const char* stateName = state == 1 ? "PLAYER" : state == 2 ? "CPU" : "NONE";
        const Uint8 r = state == 1 ? 40 : state == 2 ? 48 : 56;
        const Uint8 g = state == 1 ? 152 : state == 2 ? 92 : 56;
        const Uint8 b = state == 1 ? 64 : state == 2 ? 184 : 72;
        ModernMenu::strip(x + 8, m_pos.y + 40, cardWidth - 16, 30, r, g, b);
        rm->menu_font_large.drawCentered(x + cardWidth / 2, m_pos.y + 45, stateName);
        if (current)
            rm->menu_font_small.drawCentered(x + cardWidth / 2, m_pos.y + 76, "UP/DOWN");
    }
    rm->menu_font_small.drawCentered(m_pos.x + iWidth / 2, m_pos.y + 102,
        "P1 ONLY   LEFT/RIGHT SLOT   UP/DOWN TYPE   A DONE");
}
