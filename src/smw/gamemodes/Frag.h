#pragma once

#include "GameMode.h"


//Fraglimit
class CGM_Frag : public CGameMode
{
    public:
        CGM_Frag();

        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
        void playerextraguy(CPlayer &player, short iType) override;

        PlayerKillType CheckWinner(CPlayer * player) override;

#if defined(_DEBUG) || defined(SMW_AUTOTEST)
    void setdebuggoal() override {
        goal = 5;
    }
#endif

};
