#include "PlayChoiceMenu.h"

#include "Game.h"
#include "GameMode.h"
#include "GameValues.h"
#include "input.h"
#include "ResourceManager.h"
#include "map.h"
#include "ui/MI_Button.h"
#include "ui/ModernMenu.h"
#include "uicontrol.h"

#include <algorithm>
#include <string>

extern CGameValues game_values;
extern CResourceManager* rm;
extern CMap* g_map;
extern SDL_Surface* blitdest;

namespace {
class ChoiceFrame : public UI_Control {
public:
    ChoiceFrame() : UI_Control(0, 0) {}
    void Draw() override {
        ModernMenu::header("HOW DO YOU WANT TO PLAY?");
        ModernMenu::panel(10, 52, 464, 340, 10, 42, 34);
        rm->menu_font_large.draw(30, 68, "QUICK START");
        rm->menu_font_small.draw(30, 96, "Jump in. The game picks the rest.");
        rm->menu_font_small.draw(30, 140, "STAGE");
        rm->menu_font_large.draw(112, 136, "Random stage for your roster");
        rm->menu_font_small.draw(30, 184, "MODE");
        rm->menu_font_large.draw(112, 180, "8 easy-to-learn favourites");
        rm->menu_font_small.draw(30, 228, "LENGTH");
        rm->menu_font_large.draw(112, 224, "Short, about 3 minutes");
        rm->menu_font_small.draw(30, 272, "ITEMS & CPU");
        rm->menu_font_large.draw(164, 268, "Classic / Medium");

        ModernMenu::panel(486, 52, 368, 340);
        rm->menu_font_large.draw(506, 68, "CUSTOM MATCH");
        rm->menu_font_small.draw(506, 96, "Pick mode, stage and every rule.");
        rm->menu_font_small.draw(506, 140, "22 MODES");
        rm->menu_font_small.draw(506, 164, "24 WIDE STAGES FOR 5-8 PLAYERS");
        rm->menu_font_small.draw(506, 188, "290 ORIGINAL STAGES FOR 2-4");
        rm->menu_font_small.draw(506, 224, "TOURNAMENT / TOUR");
        rm->menu_font_small.draw(506, 248, "Play a full series of matches.");
        ModernMenu::footer("A Select    B Back to fighters    Y Re-roll    X Quick settings");
    }
};

const char* description(GameModeType mode)
{
    switch (mode) {
        case game_mode_classic: return "Last player with lives wins.";
        case game_mode_frag: return "First to the KO target wins.";
        case game_mode_coins: return "Grab the most coins.";
        case game_mode_stomp: return "Stomp the enemies.";
        case game_mode_star: return "Hold the star to score.";
        case game_mode_koth: return "Hold the zone to score.";
        case game_mode_frenzy: return "Grab cards for power-ups.";
        case game_mode_survival: return "Last survivor wins.";
        default: return "Win before your opponents.";
    }
}
} // namespace

UI_PlayChoiceMenu::UI_PlayChoiceMenu()
{
    AddNonControl(new ChoiceFrame());
    auto* quick = new MI_Button(&rm->spr_selectfield, 24, 344, "LET'S GO!", 436);
    quick->SetCode(MENU_CODE_QUICK_REVEAL);
    auto* custom = new MI_Button(&rm->spr_selectfield, 500, 300, "CUSTOM MATCH", 340);
    custom->SetCode(MENU_CODE_TO_GAME_SETUP_MENU);
    auto* series = new MI_Button(&rm->spr_selectfield, 500, 344, "TOURNAMENT / TOUR", 340);
    series->SetCode(MENU_CODE_BACK_TO_MATCH_SELECTION_MENU);
    AddControl(quick, series, custom, nullptr, custom);
    AddControl(custom, series, series, quick, nullptr);
    AddControl(series, custom, quick, quick, nullptr);
    setInitialFocus(quick);
    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);
}

MenuCodeEnum UI_PlayChoiceMenu::SendInput(CPlayerInput* input)
{
    for (short p = 0; p < MAX_INPUT_PLAYERS; ++p) {
        if (input->outputControls[p].menu_random.fPressed)
            return MENU_CODE_QUICK_REVEAL;
        if (input->outputControls[p].menu_scrollfast.fPressed)
            return MENU_CODE_TO_GAME_SETUP_MENU;
    }
    return UI_Menu::SendInput(input);
}

class UI_QuickRevealMenu::RevealControl : public UI_Control {
public:
    RevealControl() : UI_Control(0, 0) {}

    void reset(CGameMode* newMode, const char* newMapName) {
        mode = newMode;
        mapName = newMapName;
        timer = 0;
        thumbnail = g_map->createThumbnailSurface(false, true);
    }

    void Update() override { if (timer < 180) ++timer; }
    bool finished() const { return timer >= 180; }

    void Draw() override {
        ModernMenu::header("TONIGHT'S MATCH");
        ModernMenu::panel(26, 54, 468, 326);
        const SDL_Rect src{0, 0, thumbnail.getWidth(), thumbnail.getHeight()};
        const SDL_Rect dst{42, 70, 432, 240};
        thumbnail.drawStretch(src, blitdest, dst);
        rm->menu_font_large.drawCentered(260, 338, mapName);

        ModernMenu::panel(508, 54, 320, 326, 10, 42, 34);
        rm->menu_font_small.draw(524, 68, "MODE");
        rm->menu_mode_large.draw(652, 112, {mode->gamemode * 32, 0, 32, 32});
        rm->menu_font_large.drawCentered(668, 170, mode->GetModeName());
        rm->menu_font_small.drawChopCentered(668, 214, 270, description(mode->gamemode));
        const int count = std::max(1, 3 - timer / 60);
        ModernMenu::strip(686, 350, 128, 20, 192, 0, 24);
        rm->menu_font_small.drawCentered(750, 352, ("STARTING IN " + std::to_string(count)).c_str());
        ModernMenu::footer("A Start now    Y Re-roll    B Back");
    }

    MenuCodeEnum SendInput(CPlayerInput* input) override {
        for (short p = 0; p < MAX_INPUT_PLAYERS; ++p) {
            if (input->outputControls[p].menu_select.fPressed) return MENU_CODE_START_GAME;
            if (input->outputControls[p].menu_random.fPressed) return MENU_CODE_QUICK_REROLL;
            if (input->outputControls[p].menu_cancel.fPressed) return MENU_CODE_BACK_TO_PLAY_CHOICE;
        }
        return MENU_CODE_NONE;
    }

private:
    CGameMode* mode = nullptr;
    std::string mapName;
    gfxSprite thumbnail;
    int timer = 0;
};

UI_QuickRevealMenu::UI_QuickRevealMenu()
{
    reveal = new RevealControl();
    reveal->SetAutoModify(true);
    AddControl(reveal, nullptr, nullptr, nullptr, nullptr);
    setInitialFocus(reveal);
    SetCancelCode(MENU_CODE_BACK_TO_PLAY_CHOICE);
}

void UI_QuickRevealMenu::ResetReveal(CGameMode* mode, const char* mapName)
{
    reveal->reset(mode, mapName);
    ResetMenu();
}

bool UI_QuickRevealMenu::Finished() const
{
    return reveal->finished();
}
