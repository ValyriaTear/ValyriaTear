////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
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

#include "engine/system.h"

#include "common/global/global.h"

#include "utils/utils_random.h"

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
}; // class BattleAction


/** ****************************************************************************
*** \brief A battle action which involves the execution of an actor's skill
***
*** This class invokes the execution of a GlobalSkill contained by the source
*** actor. When the action is finished, any SP required to use the skill is
*** subtracted from the source actor.
*** ***************************************************************************/
class SkillAction : public BattleAction
{
public:
    SkillAction(BattleActor *actor, BattleTarget target, vt_global::GlobalSkill *skill);

    virtual ~SkillAction();

    bool IsItemAction() const {
        return false;
    }

    // Init the battle action member and possible scripts
    bool Initialize();

    bool Execute();

    //! \brief calls the corresponding skill animation file #Update method, returning it result.
    bool Update();

    void Cancel()
    {}

    vt_utils::ustring GetName() const;

    std::string GetIconFilename() const;

    bool ShouldShowSkillNotice() const;

    uint32_t GetWarmUpTime() const;

    uint32_t GetCoolDownTime() const;

    std::string GetWarmupActionName() const;

    std::string GetActionName() const;

    vt_global::GlobalSkill *GetSkill() {
        return _skill;
    }

private:
    //! \brief Pointer to the skill attached to this script (for skill events only)
    vt_global::GlobalSkill *_skill;

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
    ItemAction(BattleActor *source, BattleTarget target, const std::shared_ptr<BattleItem>& item);

    virtual ~ItemAction();

    bool IsItemAction() const {
        return true;
    }

    bool Initialize();

    bool Update();

    bool Execute();

    ///! \brief Cancel a waiting action, putting back the item in available battle items.
    void Cancel();

    vt_utils::ustring GetName() const;

    std::string GetIconFilename() const;

    bool ShouldShowSkillNotice() const {
        return true;
    }

    uint32_t GetWarmUpTime() const;

    uint32_t GetCoolDownTime() const;

    std::string GetWarmupActionName() const {
        return "idle";
    }

    std::string GetActionName() const {
        return "item";
    }

    std::shared_ptr<BattleItem> GetItem() {
        return _item;
    }

private:
    //! \brief Pointer to the item attached to this script
    const std::shared_ptr<BattleItem> _item;

    //! \brief Tells whether the action has already been canceled.
    bool _action_canceled;

    //! \brief Initialize (Calling #Initialize) a scripted battle animation when one is existing.
    void _InitAnimationScript();
}; // class ItemAction : public BattleAction

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_ACTIONS_HEADER__
