////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_PASSIVE_EFFECTS_HEADER__
#define __BATTLE_PASSIVE_EFFECTS_HEADER__

#include "common/global/global_effects.h"

#include "engine/video/text.h"

#include "luabind/object.hpp"

namespace vt_battle
{

namespace private_battle
{

/** ****************************************************************************
*** \brief Manages all data related to a single passive status effect in battle
***
*** This class extends the GlobalStatusEffect class, which contains nothing
*** more than two enum members representing the status type and intensity.
***
*** This class represents a passive (from equipment) effect on a single actor.
*** ***************************************************************************/
class PassiveBattleStatusEffect : public vt_global::GlobalStatusEffect
{
public:
    /** \param type The status effect type that this class object should represent
    *** \param intensity The intensity of the status effect
    **/
    PassiveBattleStatusEffect(vt_global::GLOBAL_STATUS type, vt_global::GLOBAL_INTENSITY intensity);

    ~PassiveBattleStatusEffect()
    {}

    //! \brief Class Member Access Functions
    //@{
    const vt_video::TextImage& GetName() const {
        return _name;
    }

    vt_video::StillImage* GetIconImage() const {
        return _icon_image;
    }

    //! \brief Returns the update script function of this passive effect.
    const luabind::object& GetUpdatePassiveFunction() const {
        return _update_passive_function;
    }
    //@}

private:
    //! \brief Holds the translated name of the status effect as an image
    vt_video::TextImage _name;

    //! \brief A pointer to the icon image that represents the status. Will be nullptr if the status is invalid
    vt_video::StillImage* _icon_image;

    //! The UpdatePassive() scripted function of this effect when used as passive one (from equipment)
    luabind::object _update_passive_function;
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_PASSIVE_EFFECTS_HEADER__
