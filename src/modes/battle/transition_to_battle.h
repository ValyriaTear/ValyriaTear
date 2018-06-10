////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __TRANSITION_BATTLE_HEADER__
#define __TRANSITION_BATTLE_HEADER__

#include "engine/mode_manager.h"

namespace vt_battle
{

class BattleMode;

//! \brief Handles transition from an event or a mode to the battle mode
//! Must be called without fade transition, as it will do it.
class TransitionToBattleMode : public vt_mode_manager::GameMode
{
public:
    TransitionToBattleMode(BattleMode* BM, bool is_boss = false);

    ~TransitionToBattleMode();

    void Update();

    void Draw();

    void Reset();

private:
    //! \brief The screen capture of the moment of the encounter
    vt_video::StillImage _screen_capture;

    //! \brief The transition timer, used to display the encounter visual effect
    vt_system::SystemTimer _transition_timer;

    //! \brief Used to display the effect
    float _position;

    //! \brief Tells whether the boss trigger sound is to be played or not.
    bool _is_boss;

    //! \brief The Battle mode to trigger afterward. Must not be nullptr.
    BattleMode* _BM;
};

} // namespace vt_battle

#endif // __TRANSITION_BATTLE_HEADER__
