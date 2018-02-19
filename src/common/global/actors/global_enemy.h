////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_ENEMY_HEADER__
#define __GLOBAL_ENEMY_HEADER__

#include "global_actor.h"

namespace vt_script
{
class ReadScriptDescriptor;
}

namespace vt_global
{

/** ****************************************************************************
*** \brief Representation of enemies that fight in battles
***
*** The game handles enemies a little different than most RPGs. Instead of an
*** enemy always having the same statistics for health, phys_atk, etc., enemy
*** stats are randomized so that the same type of enemy does not always have
*** the exact same stats. Guassian random values are applied to each enemy's
*** "base" stats before the player begins battle with that enemy, making
*** the enemy tougher or weaker than the base level depending on the outcome. Some
*** enemies (notably bosses) do not have this randomization applied to their stats
*** in order to make sure that bosses are challenging, but not overwhemlingly strong
*** or surprisingly weak.
***
*** Enemies have one to several different skills that they may use in battle. An enemy
*** has to have at least one skill defined for it, otherwise they would not be able to
*** perform any action in battle. Enemy's may also carry a small chance of dropping an
*** item or other object after they are defeated.
*** ***************************************************************************/
class GlobalEnemy : public GlobalActor
{
public:
    explicit GlobalEnemy(uint32_t id);
    virtual ~GlobalEnemy() override
    {
    }

    /** \brief Enables the enemy to be able to use a specific skill
    *** \param skill_id The integer ID of the skill to add to the enemy
    *** \returns whether the skill was added successfully.
    ***
    *** This method should be called only <b>after</b> the Initialize() method has been invoked. The
    *** purpose of this method is to allow non-standard skills to be used by enemies under certain
    *** circumstances. For example, in scripted battle sequences where an enemy may become stronger
    *** and gain access to new skills after certain criteria are met. Normally you would want to define
    *** any skills that you wish an enemy to be able to use within their Lua definition file.
    **/
    bool AddSkill(uint32_t skill_id) override;

    /** \brief Uses random variables to calculate which objects, if any, the enemy dropped.
    **/
    std::vector<std::shared_ptr<GlobalObject>> DetermineDroppedObjects();

    //! \name Class member access functions
    //@{
    uint32_t GetDrunesDropped() const {
        return _drunes_dropped;
    }

    uint32_t GetSpriteWidth() const {
        return _sprite_width;
    }

    uint32_t GetSpriteHeight() const {
        return _sprite_height;
    }

    std::vector<vt_video::AnimatedImage>* GetBattleAnimations() {
        return &_battle_animations;
    }
    //@}

protected:
    //! \brief The dimensions of the enemy's battle sprite in pixels
    uint32_t _sprite_width, _sprite_height;

    //! \brief The amount of drunes that the enemy will drop
    uint32_t _drunes_dropped;

    /** \brief Dropped object containers
    *** These two vectors are of the same size. _dropped_objects contains the IDs of the objects that the enemy
    *** may drop. _dropped_chance contains a value from 0.0f to 1.0f that determines the probability of the
    *** enemy dropping that object.
    **/
    //@{
    std::vector<uint32_t> _dropped_objects;
    std::vector<float> _dropped_chance;
    //@}

    /** \brief Contains all of the possible skills that the enemy may possess
    *** This container holds the IDs of all skills that the enemy may execute in battle.
    *** The Initialize() function uses this data to populates the GlobalActor _skills container.
    **/
    std::vector<uint32_t> _skill_set;

    /** \brief The battle sprite animations for the enemy
    *** Each enemy has four animations representing damage levels of 0%, 33%, 66%, and 100%. This vector thus
    *** always has a size of four holding each of these image frames. The first element contains the 0%
    *** damage frame, the second element contains the 33% damage frame, and so on.
    **/
    std::vector<vt_video::AnimatedImage> _battle_animations;

    /** \brief Initializes the enemy stats and skills
    ***
    *** This function sets the enemy's experience level, modifies its stats using Gaussian
    *** random values, and constructs the skills that the enemy is capable of using.
    ***
    *** \note Certain enemies can skip the stat randomization step.
    **/
    void _Initialize();
}; // class GlobalEnemy : public GlobalActor

} // namespace vt_global

#endif // __GLOBAL_ENEMY_HEADER__
