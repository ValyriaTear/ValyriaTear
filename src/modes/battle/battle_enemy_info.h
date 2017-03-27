////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_ENEMY_INFO_HEADER__
#define __BATTLE_ENEMY_INFO_HEADER__

namespace vt_battle
{

//! \brief Data used to load and place enemies on battle grounds.
struct BattleEnemyInfo {
    BattleEnemyInfo():
        enemy_id(0),
        position_x(0.0f),
        position_y(0.0f)
    {}

    BattleEnemyInfo(uint32_t id):
        enemy_id(id),
        position_x(0.0f),
        position_y(0.0f)
    {}

    BattleEnemyInfo(uint32_t id, float x, float y):
        enemy_id(id),
        position_x(x),
        position_y(y)
    {}

    //! \brief  The enemy id. See: enemies.lua
    uint32_t enemy_id;

    //! \brief The enemy position in the battle ground, in pixels.
    float position_x;
    float position_y;
};

} // namespace vt_battle

#endif // __BATTLE_ENEMY_INFO_HEADER__
