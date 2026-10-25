#pragma once

#include "uimenu.h"

class MI_PlayerSelect;

/*
    This is the main menu you see at the start of the game.
*/
class UI_MainMenu : public UI_Menu {
public:
    UI_MainMenu();

private:
    MI_PlayerSelect* miPlayerSelect;
};
