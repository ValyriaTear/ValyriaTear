////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "battle_action.h"

using namespace vt_global;

namespace vt_battle
{

namespace private_battle
{

BattleAction::BattleAction(BattleActor* actor, BattleTarget target) :
    _actor(actor),
    _target(target),
    _is_scripted(false)
{
    if(actor == nullptr)
        PRINT_WARNING << "constructor received nullptr actor" << std::endl;
    if(target.GetType() == GLOBAL_TARGET_INVALID)
        PRINT_WARNING << "constructor received invalid target" << std::endl;
}

} // namespace private_battle

} // namespace vt_battle
