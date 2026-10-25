#include "MainMenu.h"

#include "ResourceManager.h"
#include "ui/MI_PlayerSelect.h"

extern CResourceManager* rm;

UI_MainMenu::UI_MainMenu()
    : UI_Menu()
{
    SetPlayerOneOnly();
    miPlayerSelect = new MI_PlayerSelect(&rm->menu_player_select, 20, 74, "Players", 824, 0);
    miPlayerSelect->SetAutoModify(true);
    AddControl(miPlayerSelect, nullptr, nullptr, nullptr, nullptr);
    setInitialFocus(miPlayerSelect);
}
