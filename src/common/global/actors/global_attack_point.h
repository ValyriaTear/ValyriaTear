////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_ATTACK_POINT_HEADER__
#define __GLOBAL_ATTACK_POINT_HEADER__

#include "common/global/status_effects/status_effect_enums.h"

#include "engine/video/image.h"

#include "utils/ustring.h"

#include <memory>

namespace vt_script
{
class ReadScriptDescriptor;
}

namespace vt_global
{

class GlobalArmor;
class GlobalActor;

/** \name Character Attack Point Positions
*** \brief They represent the index location of the attack points and armor types for characters
**/
enum GLOBAL_POSITION {
    GLOBAL_POSITION_HEAD     = 0,
    GLOBAL_POSITION_TORSO    = 1,
    GLOBAL_POSITION_ARMS     = 2,
    GLOBAL_POSITION_LEGS     = 3,
    GLOBAL_POSITION_INVALID  = 4
};

/** ****************************************************************************
*** \brief Represents the points of attack present on an actor
***
*** An attack point is a location where an actor may be attacked. It is <b>not</b> a numerical
*** quantity. Actors typically have multiple attack points, each with their own resistances and
*** weaknesses. For example, the number of attack points on all characters is four and they
*** are located on the head, torso, arms, and legs. Each attack points may have certain weaknesses
*** or resistances.
*** ***************************************************************************/
class GlobalAttackPoint
{
public:
    //! \param actor_owner A pointer to the GlobalActor owner of this attack point
    explicit GlobalAttackPoint(GlobalActor* owner);
    ~GlobalAttackPoint() {
        _actor_owner = nullptr;
    }

    /** \brief Reads in the attack point's data from a script file
    *** \param script A reference to the open script file where to retrieve the data from
    *** \return True upon success, false upon failure.
    ***
    *** There are two requirements for using this function. First, the script file must already
    *** be opened for reading permissions. Second, the table which contains the attack point data
    *** must be opened <b>prior</b> to making this function call. This function will not close the
    *** table containing the attack point when it finishes loading the data, so the calling routine
    *** must remember to close the table after this call is made.
    **/
    bool LoadData(vt_script::ReadScriptDescriptor& script);

    /** \brief Determines the total physical and magical defense of the attack point
    *** \param equipped_armor A pointer to the armor equipped on the attack point, or nullptr if no armor is equipped
    ***
    *** This method uses the owning GlobalActor's base defense stats, the attack point's defense modifiers stats,
    *** and the properties of the equipped armor to calculate the attack point's total physical and magical defense.
    *** This method should be called whenever the actor's base defense stats or equipped armor on this point changes.
    **/
    void CalculateTotalDefense(const std::shared_ptr<GlobalArmor>& equipped_armor);

    /** \brief Determines the total evade rating of the attack point
    ***
    *** This method uses the owning GlobalActor's base evade rating and the attack point's evade modifiers stats to
    *** calculate the attack point's total evade rating. This method should be called whenever the actor's base defense
    *** rating changes.
    **/
    void CalculateTotalEvade();

    //! \name Class Member Access Functions
    //@{
    vt_utils::ustring& GetName() {
        return _name;
    }

    GlobalActor* GetActorOwner() const {
        return _actor_owner;
    }

    int16_t GetXPosition() const {
        return _x_position;
    }

    int16_t GetYPosition() const {
        return _y_position;
    }

    float GetPhysDefModifier() const {
        return _phys_def_modifier;
    }

    float GetMagDefModifier() const {
        return _mag_def_modifier;
    }

    float GetEvadeModifier() const {
        return _evade_modifier;
    }

    uint16_t GetTotalPhysicalDefense() const {
        return _total_physical_defense;
    }

    uint16_t GetTotalMagicalDefense(GLOBAL_ELEMENTAL element) const {
        if (element <= GLOBAL_ELEMENTAL_INVALID || element >= GLOBAL_ELEMENTAL_TOTAL)
            element = GLOBAL_ELEMENTAL_NEUTRAL;

        return _total_magical_defense[element];
    }

    float GetTotalEvadeRating() const {
        return _total_evade_rating;
    }

    const std::vector<std::pair<GLOBAL_STATUS, float> >& GetStatusEffects() const {
        return _status_effects;
    }

    //! \note Use this method with extreme caution. It does not update defense/evade totals or any other members
    void SetActorOwner(GlobalActor* new_owner) {
        _actor_owner = new_owner;
    }
    //@}

private:
    /** \brief The name of the attack point as is displayed on the screen
    *** Usually, this is simply the name of a body part such as "head" or "tail". More elaborate names
    *** may be chosen for special foes and bosses, however.
    **/
    vt_utils::ustring _name;

    //! \brief A pointer to the actor which "owns" this attack point (i.e., the attack point is a location on the actor)
    GlobalActor* _actor_owner;

    /** \brief The position of the physical attack point relative to the actor's battle sprite
    *** These members treat the bottom center of the sprite as the origin (0, 0) and increase in the
    *** right and upwards directions. The combination of these two members point to the center pinpoint
    *** location of the attack point. The units of these two members are in number of pixels. The _y_position
    *** member should always be positive, by _x_position may be either positive or negative.
    **/
    int16_t _x_position;
    int16_t _y_position;

    /** \brief The defense and evasion percentage modifiers for this attack point
    ***
    *** These are called "modifiers" because they modify the value of phys_def, mag_def, and evade ratings of the
    *** actor. They represent percentage change from the base stat. So for example, a phys_def modifer that is 0.25f
    *** increases the phys_def of the attack point by 25%. If the base mag_def rating was 10 and the mag_def
    *** modifier was -0.30f, the resulting mag_def for the attack point would be: 10 + (10 * -0.30f) = 7.
    ***
    *** The lower bound for each modifier is -1.0f (-100%), which will result in a value of zero for that stat. No
    *** actor stats can be negative so even if the modifier drops below -1.0f, the resulting value will still be zero.
    *** There is no theoretical upper bound, but it is usually advised to keep it under 1.0f (100%).
    **/
    //@{
    float _phys_def_modifier;
    float _mag_def_modifier;
    float _evade_modifier;
    //@}

    /** \brief The cumulative defense and evade stats for this attack point
    *** These totals include the actor's base stat, the percentage modifier for the attack point, and the stats of any
    *** armor that is equipped on the attack point.
    **/
    //@{
    uint32_t _total_physical_defense;
    //! \brief The magical defense is computed against each elements.
    uint32_t _total_magical_defense[GLOBAL_ELEMENTAL_TOTAL];
    float _total_evade_rating;
    //@}

    /** \brief A vector containing all status effects that may be triggered by attacking the point
    *** This vector contains only the status effects that have a non-zero chance of affecting their target. Therefore,
    *** it is very possible that this vector may be empty, and usually it will contain only a single entry. The first
    *** element of the pair is an identifier for the type of status. The second element is a floating point value from
    *** 0.0 to 100.0, representing a 0-100% probability range for this effect to be triggered upon being hit.
    **/
    std::vector<std::pair<GLOBAL_STATUS, float> > _status_effects;
}; // class GlobalAttackPoint

} // namespace vt_global

#endif // __GLOBAL_ATTACK_POINT_HEADER__
