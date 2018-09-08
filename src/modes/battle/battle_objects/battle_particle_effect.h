////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_PARTICLE_EFFECT_HEADER__
#define __BATTLE_PARTICLE_EFFECT_HEADER__

#include "battle_object.h"

#include "engine/video/particle_effect.h"

namespace vt_battle
{

namespace private_battle
{

//! \brief A class representing particle effects used as battle objects:
//! spell effects, attack effects, ...
class BattleParticleEffect : public BattleObject
{
public:
    BattleParticleEffect(const std::string& effect_filename);

    //! Used to be drawn at the right time by the battle mode.
    void DrawSprite();

    //! Permits to start the effect.
    bool Start() {
        return _effect.Start();
    }

    //! Tells whether the effect can be removed from memory.
    bool CanBeRemoved() const {
        return !_effect.IsAlive();
    }

    void Update() {
        _effect.Update();
    }

protected:
    //! The particle effect class used internally
    vt_mode_manager::ParticleEffect _effect;
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_PARTICLE_EFFECT_HEADER__
