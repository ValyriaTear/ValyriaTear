////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_actions.h
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \author  Andy Gardner, chopperdave@allacrost.org
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for actions that occur in battles.
***
*** Actions are events that are carried out by actors and include the execution
*** of skills or the use of items.
*** ***************************************************************************/

#ifndef __BATTLE_ACTIONS_HEADER__
#define __BATTLE_ACTIONS_HEADER__

#include "utils.h"

#include "engine/system.h"
#include "engine/video/video.h"

#include "common/global/global.h"

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
*** simulatenously.
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

    // Init the battle action member and possible scripts
    virtual bool Initialize() {
        return true;
    }

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
        return false;
    }

    //! \brief Returns the name of the action that the player would read
    virtual vt_utils::ustring GetName() const = 0;

    //! \brief Returns the number of milliseconds that the owner actor must wait in the warm up state
    virtual uint32 GetWarmUpTime() const = 0;

    //! \brief Returns the number of milliseconds that the owner actor must wait in the cool down state
    virtual uint32 GetCoolDownTime() const = 0;

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
    //! \brief The rendered text for the name of the action
// 	vt_video::TextImage _action_name;

    //! \brief The actor who will be executing the action
    BattleActor *_actor;

    //! \brief The target of the action which may be an attack point, actor, or entire party
    BattleTarget _target;

    /** \brief Makes sure that the target is valid and selects a new target if it is not
    *** This method is necessary because there is a period of time between when the desired target
    *** is selected and when the action actually gets executed by the owning actor. Within that time
    *** period the target may have become invalid due death or some other reason. This method will
    *** search for the next available target of the same type and modify the _target member so that
    *** it points to a valid target.
    ***
    *** \note Certain skills may have different criteria for determining target validity. For example,
    *** a revive skill or item would be useless if no allies were in the dead state. For this reason,
    *** inheriting classes may wish to expand upon this function to check for these types of specific
    *** conditions.
    **/
// 	virtual void _VerifyValidTarget();
}; // class BattleAction


/** ****************************************************************************
*** \brief A battle action which involves the exectuion of an actor's skill
***
*** This class invokes the execution of a GlobalSkill contained by the source
*** actor. When the action is finished, any SP required to use the skill is
*** subtracted from the source actor.
*** ***************************************************************************/
class SkillAction : public BattleAction
{
public:
    SkillAction(BattleActor *actor, BattleTarget target, vt_global::GlobalSkill *skill);

    bool IsItemAction() const {
        return false;
    }

    bool IsScripted() const {
        return _is_scripted;
    }

    // Init the battle action member and possible scripts
    bool Initialize();

    bool Execute();

    //! \brief calls the corresponding skill animation file #Update method, returning it result.
    bool Update();

    void Cancel()
    {}

    vt_utils::ustring GetName() const;

    uint32 GetWarmUpTime() const;

    uint32 GetCoolDownTime() const;

    std::string GetWarmupActionName() const;

    std::string GetActionName() const;

    vt_global::GlobalSkill *GetSkill() {
        return _skill;
    }

private:
    //! \brief Pointer to the skill attached to this script (for skill events only)
    vt_global::GlobalSkill *_skill;

    /** The functions of the possible attack animation skill.
    *** When valid, the Update function should be called until the function returns true.
    **/
    ScriptObject _init_function;
    ScriptObject _update_function;

    //! \brief Tells whether the battle action animation is scripted.
    bool _is_scripted;

    //! \brief Initialize (Calling #Initialize) a scripted battle animation when one is existing.
    void _InitAnimationScript();
}; // class SkillAction : public BattleAction


/** ****************************************************************************
*** \brief A battle action which involves the use of an item
***
*** This class invokes the usage of a GlobalItem. The item's count is decremented
*** as soon as the action goes into the FIFO queue. After the action is executed,
*** the item is removed if its count has become zero. If the action is removed
*** from the queue before it is executed (because the source actor perished, or
*** the battle ended, or other circumstances), then the item's count is
*** incremented back to its original value since it was not used.
*** ***************************************************************************/
class ItemAction : public BattleAction
{
public:
    ItemAction(BattleActor *source, BattleTarget target, BattleItem *item);

    bool IsItemAction() const {
        return true;
    }

    bool Execute();

    ///! \brief Cancel a waiting action, putting back the item in available battle items.
    void Cancel();

    vt_utils::ustring GetName() const;

    uint32 GetWarmUpTime() const;

    uint32 GetCoolDownTime() const;

    std::string GetWarmupActionName() const {
        return "idle";
    }

    std::string GetActionName() const {
        return "item";
    }

    BattleItem *GetItem() {
        return _item;
    }

private:
    //! \brief Pointer to the item attached to this script
    BattleItem *_item;

    //! \brief Tells whether the action has already been canceled.
    bool _action_canceled;
}; // class ItemAction : public BattleAction

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_ACTIONS_HEADER__


