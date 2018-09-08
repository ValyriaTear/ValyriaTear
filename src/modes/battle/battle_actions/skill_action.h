////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ACTION_HEADER__
#define __SKILL_ACTION_HEADER__

#include "battle_action.h"

namespace vt_battle
{

namespace private_battle
{

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

    vt_global::GlobalSkill* GetSkill() {
        return _skill;
    }

private:
    //! \brief Pointer to the skill attached to this script (for skill events only)
    vt_global::GlobalSkill* _skill;

    //! \brief Initialize (Calling #Initialize) a scripted battle animation when one is existing.
    void _InitAnimationScript();
};

} // namespace private_battle

} // namespace vt_battle

#endif // __SKILL_ACTION_HEADER__
