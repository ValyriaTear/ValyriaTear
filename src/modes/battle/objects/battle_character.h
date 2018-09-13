////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_CHARACTER_HEADER__
#define __BATTLE_CHARACTER_HEADER__

#include "battle_actor.h"

namespace vt_global {
class GlobalCharacter;
}

namespace vt_battle
{

namespace private_battle
{

/** ****************************************************************************
*** \brief Represents a player-controlled character in the battle
***
*** Character actors have a series of animated images that reflect their current
*** state and actions. Each character also has a custom set of progressive damage
*** battle portraits (5 in total) that are drawn when the character is selected.
*** ***************************************************************************/
class BattleCharacter : public BattleActor
{
public:
    BattleCharacter(vt_global::GlobalCharacter *character);

    ~BattleCharacter();

    bool IsEnemy() const {
        return false;
    }

    void ChangeState(ACTOR_STATE new_state);

    float GetSpriteWidth() const {
        return _current_sprite_animation ? _current_sprite_animation->GetWidth() : 0.0f;
    }

    float GetSpriteHeight() const {
        return _current_sprite_animation ? _current_sprite_animation->GetHeight() : 0.0f;
    }

    /** \brief Changes the battle character's current sprite animation image
    *** \param alias The alias text used to identify the animation to change
    ***
    *** \note Not all forms of battle sprites have multiple animations or any animations at all. For
    *** example, enemies typically only have a standard set of unanimated damage frames for their
    *** sprites. The reason this method is defined for all actors is so that the same skills may be
    *** reused for both characters and enemies, since some skill implementations will wish to call
    *** this method on the actor performing the skill.
    **/
    void ChangeSpriteAnimation(const std::string &alias);

    //! \brief Changes the action and target selection text to reflect the character's current state
    void ChangeActionText();

    //! \brief Returns true if the player may select a command for the character to execute
    bool CanSelectCommand() const {
        return (_state == ACTOR_STATE_IDLE) || (_state == ACTOR_STATE_COMMAND);
    }

    //! \brief Updates the state of the character. Must be called every frame loop.
    void Update();

    //! \brief Draws the character's current sprite animation frame
    void DrawSprite();

    //! \brief Draws the character's damage-blended face portrait
    void DrawPortrait();

    /** \brief Draws the character's status in the bottom area of the screen
    *** \param order The order position of the character [0-3] used to determine draw positions
    *** \param character_command Tells which character the command menu is open for, if any. (can be nullptr)
    **/
    void DrawStatus(uint32_t order, BattleCharacter* character_command);

    vt_global::GlobalCharacter *GetGlobalCharacter() {
        return _global_character;
    }

    const std::string &GetSpriteAnimationAlias() const {
        return _sprite_animation_alias;
    }

protected:
    //! \brief A pointer to the global character object which the battle character represents
    vt_global::GlobalCharacter* _global_character;

    //! \brief Retrains the last HP and SP values that were rendered to text
    uint32_t _last_rendered_hp, _last_rendered_sp;

    //! \brief Contains the identifier text of the current sprite animation
    std::string _sprite_animation_alias;

    //! \brief The animation name before being attacked,
    //! used to return to it after the hurt or dodge animation.
    std::string _before_attack_sprite_animation;

    //! \brief The Animated image pointer from the global character
    //! Used to avoid calling the global character std::map find calls on each loops
    //! Don't delete it, it's just a reference to the global manager animated images
    vt_video::AnimatedImage* _current_sprite_animation;

    //! The current weapon animation loaded for the given weapon
    vt_video::AnimatedImage _current_weapon_animation;

    //! \brief Rendered text of the character's name
    vt_video::TextImage _name_text;

    //! \brief Rendered text of the character's current hit points
    vt_video::TextImage _hit_points_text;

    //! \brief Rendered text of the character's current skill points
    vt_video::TextImage _skill_points_text;

    //! \brief Rendered text of the character's currently selected action
    vt_video::TextImage _action_selection_text;

    //! \brief Rendered icon of the character's currently selected action
    vt_video::StillImage _action_selection_icon;
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_CHARACTER_HEADER__
