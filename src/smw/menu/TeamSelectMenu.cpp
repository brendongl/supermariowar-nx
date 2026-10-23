#include "TeamSelectMenu.h"

#include "ResourceManager.h"
#include "ui/MI_Image.h"
#include "ui/MI_TeamSelect.h"
#include "ui/MI_Text.h"

extern CResourceManager* rm;


UI_TeamSelectMenu::UI_TeamSelectMenu()
    : UI_Menu()
{
    miTeamSelect = new MI_TeamSelect(&rm->spr_player_select_background, 0, 0);
    miTeamSelect->SetAutoModify(true);

    AddControl(miTeamSelect, NULL, NULL, NULL, NULL);

    setInitialFocus(miTeamSelect);
    SetCancelCode(MENU_CODE_TO_MAIN_MENU);
}

void UI_TeamSelectMenu::ResetTeamSelect()
{
    miTeamSelect->Reset();
}

short UI_TeamSelectMenu::GetTeamCount() const
{
    return miTeamSelect->OrganizeTeams();
}
