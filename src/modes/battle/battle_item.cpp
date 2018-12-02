///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/battle/battle_item.h"

namespace vt_battle
{

namespace private_battle
{

BattleItem::BattleItem(const vt_global::GlobalItem& item) :
    _item(item),
    _battle_count(item.GetCount())
{
    if(item.GetID() == 0)
        PRINT_WARNING << "constructor received invalid item argument" << std::endl;
}

void BattleItem::IncrementBattleCount()
{
    ++_battle_count;
    if(_battle_count > _item.GetCount()) {
        PRINT_WARNING << "attempted to increment available count above actual count: " << _battle_count << std::endl;
        --_battle_count;
    }
}

void BattleItem::DecrementBattleCount()
{
    if(_battle_count == 0) {
        PRINT_WARNING << "attempted to decrement available count below zero" << std::endl;
        return;
    }
    --_battle_count;
}

bool BattleItem::ExecuteBattleFunction(BattleActor* battle_actor,
                                       BattleTarget target)
{
    if (!battle_actor) {
        PRINT_WARNING << "Can't execute battle script function with invalid battle actor." << std::endl;
        return false;
    }
    const luabind::object& battle_use_function = _item.GetBattleUseFunction();
    if(!battle_use_function.is_valid()) {
        PRINT_WARNING << "Can't execute invalid battle script function." << std::endl;
        return false;
    }

    bool return_value = true;
    try {
        return_value = luabind::call_function<bool>(battle_use_function, battle_actor, target);
    } catch(const luabind::error& err) {
        vt_script::ScriptManager->HandleLuaError(err);
        // Give the item back when failing
        IncrementBattleCount();
        return false;
    } catch(const luabind::cast_failed& e) {
        vt_script::ScriptManager->HandleCastError(e);
        // Give the item back when failing
        IncrementBattleCount();
        return false;
    }

    if (!return_value) {
        // Give the item back when failing
        IncrementBattleCount();
    }
    return return_value;
}

} // namespace private_battle

} // namespace vt_battle
