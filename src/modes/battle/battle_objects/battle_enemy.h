////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_ENEMY_HEADER__
#define __BATTLE_ENEMY_HEADER__

#include "battle_actor.h"

#include "common/global/actors/global_enemy.h"

namespace vt_battle
{

namespace private_battle
{

/** ****************************************************************************
*** \brief Represents the entity for an enemy in the battle
***
*** This class is a wrapper around a GlobalEnemy object.
*** ***************************************************************************/
class BattleEnemy : public BattleActor
{
public:
    BattleEnemy(uint32_t enemy_id);

    ~BattleEnemy();

    bool IsEnemy() const {
        return true;
    }

    void ChangeState(ACTOR_STATE new_state);

    float GetSpriteWidth() const {
        return _global_enemy->GetSpriteWidth();
    }

    float GetSpriteHeight() const {
        return _global_enemy->GetSpriteHeight();
    }

    /** \brief Changes the battle enemy's current sprite animation image
    *** \param alias The alias text used to identify the animation to change
    ***
    *** \note Not all forms of battle sprites have multiple animations or any animations at all. For
    *** example, enemies typically only have a standard set of unanimated damage frames for their
    *** sprites. The reason this method is defined for all actors is so that the same skills may be
    *** reused for both characters and enemies, since some skill implementations will wish to call
    *** this method on the actor performing the skill.
    ***
    *** \warning This function is not a pure virtual one because it needs to be accessed by Lua
    *** and there is an issue with using Luabind on pure virtual functions.
    **/
    void ChangeSpriteAnimation(const std::string &alias);

    void Update();

    //! \brief Draws the damage blended enemy sprite image on to the battle field
    void DrawSprite();

    vt_global::GlobalEnemy* GetGlobalEnemy() {
        return _global_enemy;
    }

    //! \brief See BattleActor::DrawStaminaIcon()
    void DrawStaminaIcon(const vt_video::Color &color = vt_video::Color::white) const;

protected:
    //! \brief A pointer to the global enemy object which the battle enemy represents
    vt_global::GlobalEnemy* _global_enemy;

    //! \brief A pointer to the enemy battle animations
    //! Do not delete it, the global enemy instance will take care of it.
    std::vector<vt_video::AnimatedImage>* _sprite_animations;

    //! \brief Contains the identifier text of the current sprite animation
    std::string _sprite_animation_alias;

    //! \brief Set to true when the actor is in the ACTING state and the execution of the action is complete
    bool _action_finished;
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_ENEMY_HEADER__
