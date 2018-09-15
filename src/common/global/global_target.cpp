////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_target.h"

#include "common/global/status_effects/status_effect_enums.h"

#include "engine/system.h"

#include "utils/utils_common.h"

using namespace vt_system;

namespace vt_global
{

std::string GetTargetText(GLOBAL_TARGET target)
{
    switch(target) {
    case GLOBAL_TARGET_SELF_POINT:
        return Translate("Self — Point");
    case GLOBAL_TARGET_ALLY_POINT:
        return Translate("Ally — Point");
    case GLOBAL_TARGET_FOE_POINT:
        return Translate("Foe — Point");
    case GLOBAL_TARGET_SELF:
        return Translate("Self");
    case GLOBAL_TARGET_ALLY:
        return Translate("Ally");
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
        return Translate("Ally (Even KO)");
    case GLOBAL_TARGET_DEAD_ALLY_ONLY:
        return Translate("Ally (Only KO)");
    case GLOBAL_TARGET_FOE:
        return Translate("Foe");
    case GLOBAL_TARGET_ALL_ALLIES:
        return Translate("All Allies");
    case GLOBAL_TARGET_ALL_FOES:
        return Translate("All Foes");
    default:
        return Translate("Invalid Target");
    }
}

bool IsTargetPoint(GLOBAL_TARGET target)
{
    if((target == GLOBAL_TARGET_SELF_POINT) || (target == GLOBAL_TARGET_ALLY_POINT) || (target == GLOBAL_TARGET_FOE_POINT))
        return true;
    else
        return false;
}

bool IsTargetActor(GLOBAL_TARGET target)
{
    switch(target) {
    case GLOBAL_TARGET_SELF:
    case GLOBAL_TARGET_ALLY:
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
    case GLOBAL_TARGET_DEAD_ALLY_ONLY:
    case GLOBAL_TARGET_FOE:
        return true;
    default:
        break;
    }

    return false;
}

bool IsTargetParty(GLOBAL_TARGET target)
{
    if((target == GLOBAL_TARGET_ALL_ALLIES) || (target == GLOBAL_TARGET_ALL_FOES))
        return true;
    else
        return false;
}

bool IsTargetSelf(GLOBAL_TARGET target)
{
    if((target == GLOBAL_TARGET_SELF_POINT) || (target == GLOBAL_TARGET_SELF))
        return true;
    else
        return false;
}

bool IsTargetAlly(GLOBAL_TARGET target)
{
    switch(target) {
    case GLOBAL_TARGET_SELF:
    case GLOBAL_TARGET_ALLY:
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
    case GLOBAL_TARGET_DEAD_ALLY_ONLY:
    case GLOBAL_TARGET_ALLY_POINT:
    case GLOBAL_TARGET_ALL_ALLIES:
        return true;
    default:
        break;
    }
    return false;
}

bool IsTargetFoe(GLOBAL_TARGET target)
{
    if((target == GLOBAL_TARGET_FOE_POINT) || (target == GLOBAL_TARGET_FOE) || (target == GLOBAL_TARGET_ALL_FOES))
        return true;
    else
        return false;
}

} // namespace vt_global
