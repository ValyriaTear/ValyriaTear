////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "modes/battle/battle_objects/battle_particle_effect.h"

#include "utils/utils_common.h"

namespace vt_battle
{

namespace private_battle
{

BattleParticleEffect::BattleParticleEffect(const std::string &effect_filename):
    BattleObject()
{
    if(!_effect.LoadEffect(effect_filename))
        PRINT_WARNING << "Invalid battle particle effect file requested: "
                      << effect_filename << std::endl;
}

void BattleParticleEffect::DrawSprite()
{
    if(!_effect.IsAlive())
        return;

    _effect.Move(GetXLocation(), GetYLocation());
    _effect.Draw();
}

} // namespace private_battle

} // namespace vt_battle
