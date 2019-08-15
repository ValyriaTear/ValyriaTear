////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_ACTION_HEADER__
#define __BATTLE_ACTION_HEADER__

#include "modes/battle/battle_target.h"

namespace vt_battle
{

namespace private_battle
{

/** ****************************************************************************
*** \brief Representation of a single action to be executed in battle
***
*** This is an abstract base class for all action classes to inherit from. Actions are what
*** actors perform in battle whenever they move to attack an opponent, protect a comrade, use
*** an item, etc. There is no distinguishment between characters and enemies as far as the action
*** classes are concerned. All actions are implemented via Lua script functions that perform the
*** necessary synchronization of visual and audio media presented to the user as well as modifying
*** any change to the stats of the actor or target. Actions (and by proxy the actors executing them)
*** may be either processed individually one at a time, or multiple skills may be executed
*** simultaneously.
***
*** Each action used determines the amount of time that the actor using the action
*** must wait in the warm up and cool down states. The warm up state is when the
*** actor has chosen to use the action but has not yet used it. The cool down state
*** occurs immediately after the actor finishes the action an
*** ***************************************************************************/
class BattleAction
{
public:
    BattleAction(BattleActor *user, BattleTarget target);

    virtual ~BattleAction()
    {}

    //! \brief Returns true if this action consumes an item
    virtual bool IsItemAction() const = 0;

    //! \brief Init the battle action member and possible scripts
    virtual bool Initialize() {
        return true;
    }

    /** \brief Executes the warmup action.
    *** \return True if the action executed fine, or false otherwise.
    **/
    virtual void Warmup() = 0;

    /** \brief Executes the action.
    *** \return True if the action executed fine, or false otherwise.
    **/
    virtual bool Execute() = 0;

    ///! \brief Cancel a waiting action, and restore potential involved objects if necessary.
    virtual void Cancel() = 0;

    //! \brief Updates a skill process. Returns true when when the skill has finished
    virtual bool Update() {
        return true;
    };

    //! \brief Tells whether the battle action is handled through a script
    virtual bool IsScripted() const {
        return _is_scripted;
    }

    //! \brief Returns the name of the action that the player would read
    virtual vt_utils::ustring GetName() const = 0;

    //! \brief Returns the icon filename of the action that the player would need
    virtual std::string GetIconFilename() const = 0;

    //! \brief Returns whether a short notice should be shown just before triggering the action.
    virtual bool ShouldShowSkillNotice() const = 0;

    //! \brief Returns the number of milliseconds that the owner actor must wait in the warm up state
    virtual uint32_t GetWarmUpTime() const = 0;

    //! \brief Returns the number of milliseconds that the owner actor must wait in the cool down state
    virtual uint32_t GetCoolDownTime() const = 0;

    //! \brief Returns the character action name played before at warmup time.
    virtual std::string GetWarmupActionName() const = 0;

    //! \brief Returns the character action name played before executing the scripted function.
    virtual std::string GetActionName() const = 0;

    //! \name Class member access functions
    //@{
    BattleActor *GetActor() {
        return _actor;
    }

    BattleTarget &GetTarget() {
        return _target;
    }
    //@}

protected:
    //! \brief The actor who will be executing the action
    BattleActor *_actor;

    //! \brief The target of the action which may be an attack point, actor, or entire party
    BattleTarget _target;

    /** The functions of the possible animation.
    *** When valid, the Update function should be called until the function returns true.
    **/
    luabind::object _init_function;
    luabind::object _update_function;

    //! \brief The Animation script, used when the skill is animated.
    vt_script::ReadScriptDescriptor _anim_script;

    //! \brief Tells whether the battle action animation is scripted.
    bool _is_scripted;

    //! \brief Initialize (Calling #Initialize) a scripted battle animation when one is existing.
    virtual void _InitAnimationScript()
    {}
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_ACTION_HEADER__
