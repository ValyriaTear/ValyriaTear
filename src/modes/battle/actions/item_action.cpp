////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "item_action.h"

#include "common/global/objects/global_item.h"
#include "engine/system.h"

#include "utils/ustring.h"

using namespace vt_global;
using namespace vt_script;
using namespace vt_system;
using namespace vt_utils;

namespace vt_battle
{

namespace private_battle
{

ItemAction::ItemAction(BattleActor* source, BattleTarget target, const std::shared_ptr<BattleItem>& item) :
    BattleAction(source, target),
    _battle_item(item),
    _action_canceled(false)
{
    if(item == nullptr) {
        PRINT_WARNING << "Item action without valid item!!" << std::endl;
        return;
    }

    const GlobalItem& global_item = item->GetGlobalItem();

    if(global_item.GetTargetType() == GLOBAL_TARGET_INVALID)
        PRINT_WARNING << "constructor received invalid item" << std::endl;
    if(global_item.GetTargetType() != target.GetType())
        PRINT_WARNING << "item and target reference different target types" << std::endl;
    if(global_item.IsUsableInBattle() == false)
        PRINT_WARNING << "item is not usable in battle" << std::endl;

    // Check for a custom item animation script for the given character
    _is_scripted = false;
    std::string animation_script_file = global_item.GetAnimationScript(_actor->GetID());

    if(animation_script_file.empty())
        return;

    // Clears out old script data
    std::string tablespace = ScriptEngine::GetTableSpace(animation_script_file);
    ScriptManager->DropGlobalTable(tablespace);

    if(!_anim_script.OpenFile(animation_script_file)) {
        _anim_script.CloseFile();
        return;
    }

    if(_anim_script.OpenTablespace().empty()) {
        PRINT_ERROR << "No namespace found in file: " << animation_script_file << std::endl;
        _anim_script.CloseFile();
        return;
    }

    _init_function = _anim_script.ReadFunctionPointer("Initialize");

    if(!_init_function.is_valid()) {
        _anim_script.CloseFile();
        return;
    }

    // Attempt to load a possible update function.
    _update_function = _anim_script.ReadFunctionPointer("Update");
    _is_scripted = true;
}

ItemAction::~ItemAction()
{
    // Remove reference from the init and update function
    // to permit their deletion when freeing the lua coroutine.
    _init_function = luabind::object();
    _update_function = luabind::object();

    _anim_script.CloseFile();
}

bool ItemAction::Initialize()
{
    if(IsScripted())
        _InitAnimationScript();
    return true;
}

bool ItemAction::Update()
{
    // When there is no update function, the animation is done.
    if(!_update_function.is_valid())
        return true;

    try {
        return luabind::call_function<bool>(_update_function);
    } catch(const luabind::error& err) {
        ScriptManager->HandleLuaError(err);
        // Give the item back in case of error
        _battle_item->IncrementBattleCount();
        return true;
    } catch(const luabind::cast_failed& e) {
        ScriptManager->HandleCastError(e);
        // Give the item back in case of error
        _battle_item->IncrementBattleCount();
        return true;
    }

    // Should never happen
    return true;
}

void ItemAction::Warmup()
{
    const GlobalItem& global_item = _battle_item->GetGlobalItem();

    const luabind::object& script_function = global_item.GetBattleWarmupFunction();
    if(!script_function.is_valid()) {
        return;
    }

    try {
        luabind::call_function<void>(script_function, _actor, _target);
    } catch(const luabind::error &err) {
        ScriptManager->HandleLuaError(err);
    } catch(const luabind::cast_failed &e) {
        ScriptManager->HandleCastError(e);
    }
}

bool ItemAction::Execute()
{
    if(_battle_item == nullptr) {
        PRINT_WARNING << "Item action Execute() without valid item!!" << std::endl;
        return false;
    }

    const GlobalItem& global_item = _battle_item->GetGlobalItem();

    // Note that the battle item is already removed from the item list at that step.
    const luabind::object& script_function = global_item.GetBattleUseFunction();
    if(!script_function.is_valid()) {
        PRINT_WARNING << "item did not have a battle use function" << std::endl;

        Cancel();
        return false;
    }

    bool ret = false;
    try {
        ret = luabind::call_function<bool>(script_function, _actor, _target);
    } catch(const luabind::error &err) {
        ScriptManager->HandleLuaError(err);
        ret = false;
    } catch(const luabind::cast_failed &e) {
        ScriptManager->HandleCastError(e);
        ret = false;
    }

    // Cancel item action when failed.
    if(!ret)
        Cancel();

    return ret;
}

void ItemAction::Cancel()
{
    if(_action_canceled)
        return;

    // Give the item back in that case
    if (_battle_item)
        _battle_item->IncrementBattleCount();

    // Permit to cancel only once.
    _action_canceled = true;
}

ustring ItemAction::GetName() const
{
    if(_battle_item)
        return _battle_item->GetGlobalItem().GetName();
    return UTranslate("[error]");
}

std::string ItemAction::GetIconFilename() const
{
    if(_battle_item)
        return _battle_item->GetGlobalItem().GetIconImage().GetFilename();
    return std::string();
}

uint32_t ItemAction::GetWarmUpTime() const
{
    if(_battle_item == nullptr)
        return 0;
    else
        return _battle_item->GetWarmUpTime();
}

uint32_t ItemAction::GetCoolDownTime() const
{
    if(_battle_item == nullptr)
        return 0;
    else
        return _battle_item->GetCoolDownTime();
}

void ItemAction::_InitAnimationScript()
{
    try {
        // N.B: _battle_item is a shared_ptr, but we need the actual pointer for luabind.
        luabind::call_function<void>(_init_function, _actor, _target, _battle_item.get());
    } catch(const luabind::error& err) {
        ScriptManager->HandleLuaError(err);
        // Fall back to hard-coded mode
        _is_scripted = false;
    } catch(const luabind::cast_failed& e) {
        ScriptManager->HandleCastError(e);
        // Fall back to hard-coded mode
        _is_scripted = false;
    }
}

} // namespace private_battle

} // namespace vt_battle
