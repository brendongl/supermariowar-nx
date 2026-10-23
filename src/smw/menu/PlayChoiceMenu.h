#pragma once

#include "uimenu.h"

class CGameMode;
class MI_Button;

class UI_PlayChoiceMenu : public UI_Menu {
public:
    UI_PlayChoiceMenu();
    MenuCodeEnum SendInput(CPlayerInput* input) override;
};

class UI_QuickRevealMenu : public UI_Menu {
public:
    UI_QuickRevealMenu();
    void ResetReveal(CGameMode* mode, const char* mapName);
    bool Finished() const;

private:
    class RevealControl;
    RevealControl* reveal = nullptr;
};
