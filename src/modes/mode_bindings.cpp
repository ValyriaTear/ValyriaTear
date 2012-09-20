///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    mode_bindings.cpp
*** \author  Daniel Steuernol, steu@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Lua bindings for game mode code
***
*** All bindings for the game mode code is contained within this file.
*** Therefore, everything that you see bound within this file will be made
*** available in Lua.
***
*** \note To most C++ programmers, the syntax of the binding code found in this
*** file may be very unfamiliar and obtuse. Refer to the Luabind documentation
*** as necessary to gain an understanding of this code style.
*** **************************************************************************/

// Common code headers
#include "common/dialogue.h"
#include "common/global/global_actors.h"
#include "common/global/global_effects.h"

#include "modes/boot/boot.h"
#include "modes/battle/battle.h"
#include "modes/battle/battle_actors.h"
#include "modes/battle/battle_command.h"
#include "modes/battle/battle_dialogue.h"
#include "modes/battle/battle_effects.h"
#include "modes/battle/battle_utils.h"
#include "modes/map/map.h"
#include "modes/map/map_dialogue.h"
#include "modes/map/map_events.h"
#include "modes/map/map_objects.h"
#include "modes/map/map_sprites.h"
#include "modes/map/map_treasure.h"
#include "modes/map/map_utils.h"
#include "modes/map/map_zones.h"
#include "modes/menu/menu.h"
#include "modes/shop/shop.h"

namespace hoa_defs {

void BindModeCode() {
	// ----- Boot Mode Bindings
	{
	using namespace hoa_boot;
	using namespace hoa_boot::private_boot;

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_boot")
	[
		luabind::class_<BootMode, hoa_mode_manager::GameMode>("BootMode")
			.def(luabind::constructor<>())
			.def("GetState", &BootMode::GetState)
			.def("ChangeState", &BootMode::ChangeState)

			// Namespace constants
			.enum_("constants") [
				// Battle states
				luabind::value("BOOT_STATE_INTRO", BOOT_STATE_INTRO),
				luabind::value("BOOT_STATE_MENU", BOOT_STATE_MENU)
			]
	];
	}

	// ----- Map Mode Bindings
	{
	using namespace hoa_map;
	using namespace hoa_map::private_map;

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<MapMode, hoa_mode_manager::GameMode>("MapMode")
			.def(luabind::constructor<const std::string&>())
			.def_readonly("object_supervisor", &MapMode::_object_supervisor)
			.def_readonly("event_supervisor", &MapMode::_event_supervisor)
			.def_readonly("dialogue_supervisor", &MapMode::_dialogue_supervisor)
			.def_readonly("treasure_supervisor", &MapMode::_treasure_supervisor)

			.def_readwrite("camera", &MapMode::_camera)
			.def_readwrite("unlimited_stamina", &MapMode::_unlimited_stamina)
			.def_readwrite("running_disabled", &MapMode::_running_disabled)
			.def_readwrite("run_stamina", &MapMode::_run_stamina)

			.def("AddGroundObject", &MapMode::AddGroundObject, luabind::adopt(_2))
			.def("AddPassObject", &MapMode::AddPassObject, luabind::adopt(_2))
			.def("AddSkyObject", &MapMode::AddSkyObject, luabind::adopt(_2))
			.def("AddZone", &MapMode::AddZone, luabind::adopt(_2))
			.def("AddSavePoint", &MapMode::AddSavePoint)
			.def("AddHalo", &MapMode::AddHalo)
			.def("AddLight", &MapMode::AddLight)
			.def("SetCamera", (void(MapMode::*)(private_map::VirtualSprite*))&MapMode::SetCamera)
			.def("SetCamera", (void(MapMode::*)(private_map::VirtualSprite*, uint32))&MapMode::SetCamera)
			.def("MoveVirtualFocus", (void(MapMode::*)(float, float))&MapMode::MoveVirtualFocus)
			.def("MoveVirtualFocus", (void(MapMode::*)(float, float, uint32))&MapMode::MoveVirtualFocus)
			.def("IsCameraOnVirtualFocus", &MapMode::IsCameraOnVirtualFocus)
			.def("SetShowGUI", &MapMode::SetShowGUI)
			.def("IsShowGUI", &MapMode::IsShowGUI)
			.def("PushState", &MapMode::PushState)
			.def("PopState", &MapMode::PopState)
			.def("DrawMapLayers", &MapMode::_DrawMapLayers)

			// Namespace constants
			.enum_("constants") [
				// Map states
				luabind::value("STATE_EXPLORE", STATE_EXPLORE),
				luabind::value("STATE_SCENE", STATE_SCENE),
				luabind::value("STATE_DIALOGUE", STATE_DIALOGUE),
				luabind::value("STATE_TREASURE", STATE_TREASURE),
				// Map Collision types
				luabind::value("NO_COLLISION", NO_COLLISION),
				luabind::value("CHARACTER_COLLISION", CHARACTER_COLLISION),
				luabind::value("ENEMY_COLLISION", ENEMY_COLLISION),
				luabind::value("WALL_COLLISION", WALL_COLLISION),
				luabind::value("ALL_COLLISION", ALL_COLLISION),
				// Map contexts
				luabind::value("CONTEXT_NONE", MAP_CONTEXT_NONE),
				luabind::value("CONTEXT_01", MAP_CONTEXT_01),
				luabind::value("CONTEXT_02", MAP_CONTEXT_02),
				luabind::value("CONTEXT_03", MAP_CONTEXT_03),
				luabind::value("CONTEXT_04", MAP_CONTEXT_04),
				luabind::value("CONTEXT_05", MAP_CONTEXT_05),
				luabind::value("CONTEXT_06", MAP_CONTEXT_06),
				luabind::value("CONTEXT_07", MAP_CONTEXT_07),
				luabind::value("CONTEXT_08", MAP_CONTEXT_08),
				luabind::value("CONTEXT_09", MAP_CONTEXT_09),
				luabind::value("CONTEXT_10", MAP_CONTEXT_10),
				luabind::value("CONTEXT_11", MAP_CONTEXT_11),
				luabind::value("CONTEXT_12", MAP_CONTEXT_12),
				luabind::value("CONTEXT_13", MAP_CONTEXT_13),
				luabind::value("CONTEXT_14", MAP_CONTEXT_14),
				luabind::value("CONTEXT_15", MAP_CONTEXT_15),
				luabind::value("CONTEXT_16", MAP_CONTEXT_16),
				luabind::value("CONTEXT_17", MAP_CONTEXT_17),
				luabind::value("CONTEXT_18", MAP_CONTEXT_18),
				luabind::value("CONTEXT_19", MAP_CONTEXT_19),
				luabind::value("CONTEXT_20", MAP_CONTEXT_20),
				luabind::value("CONTEXT_21", MAP_CONTEXT_21),
				luabind::value("CONTEXT_22", MAP_CONTEXT_22),
				luabind::value("CONTEXT_23", MAP_CONTEXT_23),
				luabind::value("CONTEXT_24", MAP_CONTEXT_24),
				luabind::value("CONTEXT_25", MAP_CONTEXT_25),
				luabind::value("CONTEXT_26", MAP_CONTEXT_26),
				luabind::value("CONTEXT_27", MAP_CONTEXT_27),
				luabind::value("CONTEXT_28", MAP_CONTEXT_28),
				luabind::value("CONTEXT_29", MAP_CONTEXT_29),
				luabind::value("CONTEXT_30", MAP_CONTEXT_30),
				luabind::value("CONTEXT_31", MAP_CONTEXT_31),
				luabind::value("CONTEXT_32", MAP_CONTEXT_32),
				luabind::value("CONTEXT_ALL", MAP_CONTEXT_ALL),
				// Object types
				luabind::value("PHYSICAL_TYPE", PHYSICAL_TYPE),
				luabind::value("VIRTUAL_TYPE", VIRTUAL_TYPE),
				luabind::value("SPRITE_TYPE", SPRITE_TYPE),
				// Sprite directions
				luabind::value("NORTH", NORTH),
				luabind::value("SOUTH", SOUTH),
				luabind::value("EAST", EAST),
				luabind::value("WEST", WEST),
				luabind::value("NW_NORTH", NW_NORTH),
				luabind::value("NW_WEST", NW_WEST),
				luabind::value("NE_NORTH", NE_NORTH),
				luabind::value("NE_EAST", NE_EAST),
				luabind::value("SW_SOUTH", SW_SOUTH),
				luabind::value("SW_WEST", SW_WEST),
				luabind::value("SE_SOUTH", SE_SOUTH),
				luabind::value("SE_EAST", SE_EAST),
				// Sprite animations
				luabind::value("ANIM_SOUTH", ANIM_SOUTH),
				luabind::value("ANIM_NORTH", ANIM_NORTH),
				luabind::value("ANIM_WEST", ANIM_WEST),
				luabind::value("ANIM_EAST", ANIM_EAST),
				// Sprite speeds
				luabind::value("VERY_SLOW_SPEED", static_cast<uint32>(VERY_SLOW_SPEED)),
				luabind::value("SLOW_SPEED", static_cast<uint32>(SLOW_SPEED)),
				luabind::value("NORMAL_SPEED", static_cast<uint32>(NORMAL_SPEED)),
				luabind::value("FAST_SPEED", static_cast<uint32>(FAST_SPEED)),
				luabind::value("VERY_FAST_SPEED", static_cast<uint32>(VERY_FAST_SPEED))
			]
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<ObjectSupervisor>("ObjectSupervisor")
			.def_readonly("virtual_focus", &ObjectSupervisor::_virtual_focus)

			.def("GenerateObjectID", &ObjectSupervisor::GenerateObjectID)
			.def("GetNumberObjects", &ObjectSupervisor::GetNumberObjects)
			.def("GetObjectByIndex", &ObjectSupervisor::GetObjectByIndex)
			.def("GetObject", &ObjectSupervisor::GetObject)
			.def("SetPartyMemberVisibleSprite", &ObjectSupervisor::SetPartyMemberVisibleSprite)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<MapObject>("MapObject")
			.def("SetObjectID", &MapObject::SetObjectID)
			.def("SetContext", &MapObject::SetContext)
			.def("SetXPosition", &MapObject::SetXPosition)
			.def("SetYPosition", &MapObject::SetYPosition)
			.def("SetPosition", &MapObject::SetPosition)
			.def("SetImgHalfWidth", &MapObject::SetImgHalfWidth)
			.def("SetImgHeight", &MapObject::SetImgHeight)
			.def("SetCollHalfWidth", &MapObject::SetCollHalfWidth)
			.def("SetCollHeight", &MapObject::SetCollHeight)
			.def("SetUpdatable", &MapObject::SetUpdatable)
			.def("SetVisible", &MapObject::SetVisible)
			.def("SetCollisionMask", &MapObject::SetCollisionMask)
			.def("SetDrawOnSecondPass", &MapObject::SetDrawOnSecondPass)
			.def("GetObjectID", &MapObject::GetObjectID)
			.def("GetContext", &MapObject::GetContext)
			.def("GetXPosition", &MapObject::GetXPosition)
			.def("GetYPosition", &MapObject::GetYPosition)
			.def("GetImgHalfWidth", &MapObject::GetImgHalfWidth)
			.def("GetImgHeight", &MapObject::GetImgHeight)
			.def("GetCollHalfWidth", &MapObject::GetCollHalfWidth)
			.def("GetCollHeight", &MapObject::GetCollHeight)
			.def("IsUpdatable", &MapObject::IsUpdatable)
			.def("IsVisible", &MapObject::IsVisible)
			.def("GetCollisionMask", &MapObject::GetCollisionMask)
			.def("IsDrawOnSecondPass", &MapObject::IsDrawOnSecondPass)
			.def("Emote", &MapObject::Emote)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<ParticleObject, MapObject>("ParticleObject")
			.def(luabind::constructor<const std::string&, float, float, MAP_CONTEXT>())
			.def("Stop", &ParticleObject::Stop)
			.def("Start", &ParticleObject::Start)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<PhysicalObject, MapObject>("PhysicalObject")
			.def(luabind::constructor<>())
			.def("AddAnimation", (int32(PhysicalObject::*)(const std::string&))&PhysicalObject::AddAnimation)
			.def("AddStillFrame", &PhysicalObject::AddStillFrame)
			.def("SetCurrentAnimation", &PhysicalObject::SetCurrentAnimation)
			.def("SetAnimationProgress", &PhysicalObject::SetAnimationProgress)
			.def("GetCurrentAnimation", &PhysicalObject::GetCurrentAnimation)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<TreasureObject, PhysicalObject>("TreasureObject")
			.def(luabind::constructor<const std::string&, const std::string&, const std::string&, const std::string&>())
			.def("SetDrunes", &TreasureObject::SetDrunes)
			.def("AddObject", &TreasureObject::AddObject)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<VirtualSprite, MapObject>("VirtualSprite")
			.def(luabind::constructor<>())
			.def("SetMoving", &VirtualSprite::SetMoving)
			.def("SetDirection", &VirtualSprite::SetDirection)
			.def("SetMovementSpeed", &VirtualSprite::SetMovementSpeed)
			.def("GetMoving", &VirtualSprite::GetMoving)
			.def("GetDirection", &VirtualSprite::GetDirection)
			.def("GetMovementSpeed", &VirtualSprite::GetMovementSpeed)
			.def("LookAt", (void(VirtualSprite::*)(float, float))&VirtualSprite::LookAt)
			.def("LookAt", (void(VirtualSprite::*)(MapObject*))&VirtualSprite::LookAt)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<MapSprite, VirtualSprite>("MapSprite")
			.def(luabind::constructor<>())
			.def("SetName", (void(MapSprite::*)(const std::string&))&MapSprite::SetName)
			.def("SetCurrentAnimationDirection", &MapSprite::SetCurrentAnimationDirection)
			.def("GetCurrentAnimationDirection", &MapSprite::GetCurrentAnimationDirection)
			.def("LoadFacePortrait", &MapSprite::LoadFacePortrait)
			.def("LoadStandingAnimations", &MapSprite::LoadStandingAnimations)
			.def("LoadWalkingAnimations", &MapSprite::LoadWalkingAnimations)
			.def("LoadRunningAnimations", &MapSprite::LoadRunningAnimations)
			.def("LoadCustomAnimation", &MapSprite::LoadCustomAnimation)
			.def("ClearAnimations", &MapSprite::ClearAnimations)
			.def("AddDialogueReference", (void(MapSprite::*)(uint32))&MapSprite::AddDialogueReference)
			.def("AddDialogueReference", (void(MapSprite::*)(SpriteDialogue*))&MapSprite::AddDialogueReference)
			.def("ClearDialogueReferences", &MapSprite::ClearDialogueReferences)
			.def("RemoveDialogueReference", (void(MapSprite::*)(uint32))&MapSprite::RemoveDialogueReference)
			.def("RemoveDialogueReference", (void(MapSprite::*)(SpriteDialogue*))&MapSprite::RemoveDialogueReference)
			.def("SetSpriteName", &MapSprite::SetSpriteName)
			.def("GetSpriteName", &MapSprite::GetSpriteName)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<EnemySprite, MapSprite>("EnemySprite")
			.def(luabind::constructor<>())
			.def(luabind::constructor<std::string>())
			.def("Reset", &EnemySprite::Reset)
			.def("NewEnemyParty", &EnemySprite::NewEnemyParty)
			.def("AddEnemy", (void(EnemySprite::*)(uint32, float, float))&EnemySprite::AddEnemy)
			.def("AddEnemy", (void(EnemySprite::*)(uint32))&EnemySprite::AddEnemy)
			.def("GetAggroRange", &EnemySprite::GetAggroRange)
			.def("GetTimeToChange", &EnemySprite::GetTimeToChange)
			.def("GetTimeToSpawn", &EnemySprite::GetTimeToSpawn)
			.def("GetBattleMusicTheme", &EnemySprite::GetBattleMusicTheme)
			.def("IsDead", &EnemySprite::IsDead)
			.def("IsSpawning", &EnemySprite::IsSpawning)
			.def("IsHostile", &EnemySprite::IsHostile)
			.def("SetZone", &EnemySprite::SetZone)
			.def("SetAggroRange", &EnemySprite::SetAggroRange)
			.def("SetTimeToChange", &EnemySprite::SetTimeToChange)
			.def("SetTimeToSpawn", &EnemySprite::SetTimeToSpawn)
			.def("SetBattleMusicTheme", &EnemySprite::SetBattleMusicTheme)
			.def("SetBattleBackground", &EnemySprite::SetBattleBackground)
			.def("AddBattleScript", &EnemySprite::AddBattleScript)
			.def("ChangeStateDead", &EnemySprite::ChangeStateDead)
			.def("ChangeStateSpawning", &EnemySprite::ChangeStateSpawning)
			.def("ChangeStateHostile", &EnemySprite::ChangeStateHostile)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<MapZone>("MapZone")
			.def(luabind::constructor<>())
			.def(luabind::constructor<uint16, uint16, uint16, uint16>())
			.def(luabind::constructor<uint16, uint16, uint16, uint16, MAP_CONTEXT>())
			.def("AddSection", &MapZone::AddSection)
			.def("IsInsideZone", &MapZone::IsInsideZone)
			.def("GetActiveContexts", &MapZone::GetActiveContexts)
			.def("SetActiveContexts", &MapZone::SetActiveContexts)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<CameraZone, MapZone>("CameraZone")
			.def(luabind::constructor<>())
			.def(luabind::constructor<uint16, uint16, uint16, uint16>())
			.def(luabind::constructor<uint16, uint16, uint16, uint16, MAP_CONTEXT>())
			.def("IsCameraInside", &CameraZone::IsCameraInside)
			.def("IsCameraEntering", &CameraZone::IsCameraEntering)
			.def("IsCameraExiting", &CameraZone::IsCameraExiting)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<ResidentZone, MapZone>("ResidentZone")
			.def(luabind::constructor<>())
			.def(luabind::constructor<uint16, uint16, uint16, uint16>())
			.def(luabind::constructor<uint16, uint16, uint16, uint16, MAP_CONTEXT>())
			.def("IsResidentEntering", &ResidentZone::IsResidentEntering)
			.def("IsResidentExiting", &ResidentZone::IsResidentExiting)
			.def("IsSpriteResident", (bool(ResidentZone::*)(uint32)const)&ResidentZone::IsSpriteResident)
			.def("IsSpriteResident", (bool(ResidentZone::*)(VirtualSprite*)const)&ResidentZone::IsSpriteResident)
			.def("IsCameraResident", &ResidentZone::IsCameraResident)
			.def("IsSpriteEntering", (bool(ResidentZone::*)(uint32)const)&ResidentZone::IsSpriteEntering)
			.def("IsSpriteEntering", (bool(ResidentZone::*)(VirtualSprite*)const)&ResidentZone::IsSpriteEntering)
			.def("IsCameraEntering", &ResidentZone::IsCameraEntering)
			.def("IsSpriteExiting", (bool(ResidentZone::*)(uint32)const)&ResidentZone::IsSpriteExiting)
			.def("IsSpriteExiting", (bool(ResidentZone::*)(VirtualSprite*)const)&ResidentZone::IsSpriteExiting)
			.def("IsCameraExiting", &ResidentZone::IsCameraExiting)
			.def("GetResident", &ResidentZone::GetResident)
			.def("GetEnteringResident", &ResidentZone::GetEnteringResident)
			.def("GetExitingResident", &ResidentZone::GetExitingResident)
			.def("GetNumberResidents", &ResidentZone::GetNumberResidents)
			.def("GetNumberEnteringResidents", &ResidentZone::GetNumberEnteringResidents)
			.def("GetNumberExitingResidents", &ResidentZone::GetNumberExitingResidents)

	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<EnemyZone, MapZone>("EnemyZone")
			.def(luabind::constructor<>())
			.def(luabind::constructor<uint16, uint16, uint16, uint16, MAP_CONTEXT>())
			.def("AddEnemy", &EnemyZone::AddEnemy, luabind::adopt(_2))
			.def("AddSpawnSection", &EnemyZone::AddSpawnSection)
			.def("IsRoamingRestrained", &EnemyZone::IsRoamingRestrained)
			.def("GetSpawnTime", &EnemyZone::GetSpawnTime)
			.def("SetRoamingRestrained", &EnemyZone::SetRoamingRestrained)
			.def("SetSpawnTime", &EnemyZone::SetSpawnTime)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<ContextZone, MapZone>("ContextZone")
			.def(luabind::constructor<MAP_CONTEXT, MAP_CONTEXT>())
			.def("AddSection", (void(ContextZone::*)(uint16, uint16, uint16, uint16, bool))&ContextZone::AddSection)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<DialogueSupervisor>("DialogueSupervisor")
			.def("AddDialogue", &DialogueSupervisor::AddDialogue, luabind::adopt(_2))
			.def("BeginDialogue", &DialogueSupervisor::BeginDialogue)
			.def("EndDialogue", &DialogueSupervisor::EndDialogue)
			.def("GetDialogue", &DialogueSupervisor::GetDialogue)
			.def("GetCurrentDialogue", &DialogueSupervisor::GetCurrentDialogue)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<SpriteDialogue, hoa_common::CommonDialogue>("SpriteDialogue")
			.def(luabind::constructor<uint32>())
			.def(luabind::constructor<>())
			.def("AddLine", (void(SpriteDialogue::*)(const std::string&, uint32))&SpriteDialogue::AddLine)
			.def("AddLine", (void(SpriteDialogue::*)(const std::string&, VirtualSprite*))&SpriteDialogue::AddLine)
			.def("AddLineEmote", (void(SpriteDialogue::*)(const std::string&, VirtualSprite*, const std::string&))&SpriteDialogue::AddLineEmote)

			.def("AddLine", (void(SpriteDialogue::*)(const std::string&, uint32, int32))&SpriteDialogue::AddLine)
			.def("AddLine", (void(SpriteDialogue::*)(const std::string&, VirtualSprite*, int32))&SpriteDialogue::AddLine)

			.def("AddLineTimed", (void(SpriteDialogue::*)(const std::string&, uint32, uint32))&SpriteDialogue::AddLineTimed)
			.def("AddLineTimed", (void(SpriteDialogue::*)(const std::string&, VirtualSprite*, uint32))&SpriteDialogue::AddLineTimed)

			.def("AddLineTimed", (void(SpriteDialogue::*)(const std::string&, uint32, int32, uint32))&SpriteDialogue::AddLineTimed)
			.def("AddLineTimed", (void(SpriteDialogue::*)(const std::string&, VirtualSprite*, int32, uint32))&SpriteDialogue::AddLineTimed)

			.def("AddLineEvent", (void(SpriteDialogue::*)(const std::string&, uint32, const std::string&, const std::string&))&SpriteDialogue::AddLineEvent)
			.def("AddLineEvent", (void(SpriteDialogue::*)(const std::string&, VirtualSprite*, const std::string&, const std::string&))&SpriteDialogue::AddLineEvent)
			.def("AddLineEventEmote", (void(SpriteDialogue::*)(const std::string&, VirtualSprite*, const std::string&, const std::string&, const std::string&))&SpriteDialogue::AddLineEventEmote)

			.def("AddLineEvent", (void(SpriteDialogue::*)(const std::string&, uint32, int32, const std::string&, const std::string&))&SpriteDialogue::AddLineEvent)
			.def("AddLineEvent", (void(SpriteDialogue::*)(const std::string&, VirtualSprite*, int32, const std::string&, const std::string&))&SpriteDialogue::AddLineEvent)

			.def("AddLineTimedEvent", (void(SpriteDialogue::*)(const std::string&, uint32, uint32, const std::string&, const std::string&))&SpriteDialogue::AddLineTimedEvent)
			.def("AddLineTimedEvent", (void(SpriteDialogue::*)(const std::string&, VirtualSprite*, uint32, const std::string&, const std::string&))&SpriteDialogue::AddLineTimedEvent)

			.def("AddLineTimedEvent", (void(SpriteDialogue::*)(const std::string&, uint32, int32, uint32, const std::string&, const std::string&, const std::string&))&SpriteDialogue::AddLineTimedEvent)
			.def("AddLineTimedEvent", (void(SpriteDialogue::*)(const std::string&, VirtualSprite*, int32, uint32, const std::string&, const std::string&))&SpriteDialogue::AddLineTimedEvent)

			.def("AddOption", (void(SpriteDialogue::*)(const std::string&))&SpriteDialogue::AddOption)
			.def("AddOption", (void(SpriteDialogue::*)(const std::string&, int32))&SpriteDialogue::AddOption)

			.def("AddOptionEvent", (void(SpriteDialogue::*)(const std::string&, const std::string&))&SpriteDialogue::AddOptionEvent)
			.def("AddOptionEvent", (void(SpriteDialogue::*)(const std::string&, int32, const std::string&))&SpriteDialogue::AddOptionEvent)

			.def("Validate", &SpriteDialogue::Validate)
			.def("SetInputBlocked", &SpriteDialogue::SetInputBlocked)
			.def("SetRestoreState", &SpriteDialogue::SetRestoreState)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<EventSupervisor>("EventSupervisor")
			.def("RegisterEvent", &EventSupervisor::RegisterEvent, luabind::adopt(_2))
			.def("StartEvent", (void(EventSupervisor::*)(const std::string&))&EventSupervisor::StartEvent)
			.def("StartEvent", (void(EventSupervisor::*)(const std::string&, uint32))&EventSupervisor::StartEvent)
			.def("StartEvent", (void(EventSupervisor::*)(MapEvent*))&EventSupervisor::StartEvent)
			.def("StartEvent", (void(EventSupervisor::*)(MapEvent*, uint32))&EventSupervisor::StartEvent)
			.def("TerminateEvents", (void(EventSupervisor::*)(const std::string&, bool))&EventSupervisor::TerminateEvents)
			.def("TerminateEvents", (void(EventSupervisor::*)(MapEvent*, bool))&EventSupervisor::TerminateEvents)
			.def("TerminateAllEvents", &EventSupervisor::TerminateAllEvents)
			.def("IsEventActive", &EventSupervisor::IsEventActive)
			.def("HasActiveEvent", &EventSupervisor::HasActiveEvent)
			.def("HasActiveDelayedEvent", &EventSupervisor::HasActiveDelayedEvent)
			.def("GetEvent", &EventSupervisor::GetEvent)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<MapEvent>("MapEvent")
			.def("GetEventID", &MapEvent::GetEventID)
			.def("AddEventLinkAtStart", (void(MapEvent::*)(const std::string&))&MapEvent::AddEventLinkAtStart)
			.def("AddEventLinkAtStart", (void(MapEvent::*)(const std::string&, uint32))&MapEvent::AddEventLinkAtStart)
			.def("AddEventLinkAtEnd", (void(MapEvent::*)(const std::string&))&MapEvent::AddEventLinkAtEnd)
			.def("AddEventLinkAtEnd", (void(MapEvent::*)(const std::string&, uint32))&MapEvent::AddEventLinkAtEnd)
	];


	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<DialogueEvent, MapEvent>("DialogueEvent")
			.def(luabind::constructor<std::string, uint32>())
			.def(luabind::constructor<std::string, SpriteDialogue*>())
			.def("SetStopCameraMovement", &DialogueEvent::SetStopCameraMovement)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<SoundEvent, MapEvent>("SoundEvent")
			.def(luabind::constructor<std::string, std::string>())
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<MapTransitionEvent, MapEvent>("MapTransitionEvent")
			.def(luabind::constructor<std::string, std::string, std::string>())
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<ScriptedEvent, MapEvent>("ScriptedEvent")
			.def(luabind::constructor<std::string, std::string, std::string>())
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<SpriteEvent, MapEvent>("SpriteEvent")
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<ScriptedSpriteEvent, SpriteEvent>("ScriptedSpriteEvent")
			.def(luabind::constructor<std::string, uint16, std::string, std::string>())
			.def(luabind::constructor<std::string, VirtualSprite*, std::string, std::string>())
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<ChangeDirectionSpriteEvent, SpriteEvent>("ChangeDirectionSpriteEvent")
			.def(luabind::constructor<std::string, uint16, uint16>())
			.def(luabind::constructor<std::string, VirtualSprite*, uint16>())
	];

 	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
 	[
		luabind::class_<LookAtSpriteEvent, SpriteEvent>("LookAtSpriteEvent")
			.def(luabind::constructor<std::string, uint16, uint16>())
			.def(luabind::constructor<std::string, VirtualSprite*, VirtualSprite*>())
			.def(luabind::constructor<std::string, VirtualSprite*, float, float>())
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<PathMoveSpriteEvent, SpriteEvent>("PathMoveSpriteEvent")
			.def(luabind::constructor<std::string, uint32, float, float, bool>())
			.def(luabind::constructor<std::string, VirtualSprite*, float, float, bool>())
			.def(luabind::constructor<std::string, VirtualSprite*, VirtualSprite*, bool>())
			.def("SetDestination", (void(PathMoveSpriteEvent::*)(float, float, bool))&PathMoveSpriteEvent::SetDestination)
			.def("SetDestination", (void(PathMoveSpriteEvent::*)(VirtualSprite*, bool))&PathMoveSpriteEvent::SetDestination)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<RandomMoveSpriteEvent, SpriteEvent>("RandomMoveSpriteEvent")
			.def(luabind::constructor<std::string, VirtualSprite*, uint32, uint32>())
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<AnimateSpriteEvent, MapEvent>("AnimateSpriteEvent")
			.def(luabind::constructor<const std::string&, VirtualSprite*, const std::string&, uint32>())
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<BattleEncounterEvent, MapEvent>("BattleEncounterEvent")
			.def(luabind::constructor<std::string>())
			.def("SetMusic", &BattleEncounterEvent::SetMusic)
			.def("SetBackground", &BattleEncounterEvent::SetBackground)
			.def("AddScript", &BattleEncounterEvent::AddScript)
			.def("AddEnemy", &BattleEncounterEvent::AddEnemy)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<ShopEvent, MapEvent>("ShopEvent")
			.def(luabind::constructor<std::string>())
			.def("AddWare", &ShopEvent::AddWare)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_map")
	[
		luabind::class_<TreasureEvent, MapEvent>("TreasureEvent")
			.def(luabind::constructor<std::string>())
			.def("SetDrunes", &TreasureEvent::SetDrunes)
			.def("AddObject", &TreasureEvent::AddObject)
	];

	} // End using map mode namespaces



	// ----- Battle Mode bindings
	{
	using namespace hoa_battle;
	using namespace hoa_battle::private_battle;

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_battle")
	[
		luabind::def("CalculateStandardEvasion", (bool(*)(BattleTarget*)) &CalculateStandardEvasion),
		luabind::def("CalculateStandardEvasionAdder", (bool(*)(BattleTarget*, float)) &CalculateStandardEvasion),
		luabind::def("CalculateStandardEvasionMultiplier", (bool(*)(BattleTarget*, float)) &CalculateStandardEvasionMultiplier),
		luabind::def("CalculatePhysicalDamage", (uint32(*)(BattleActor*, BattleTarget*)) &CalculatePhysicalDamage),
		luabind::def("CalculatePhysicalDamage", (uint32(*)(BattleActor*, BattleTarget*, float)) &CalculatePhysicalDamage),
		luabind::def("CalculatePhysicalDamageAdder", (uint32(*)(BattleActor*, BattleTarget*, int32)) &CalculatePhysicalDamageAdder),
		luabind::def("CalculatePhysicalDamageAdder", (uint32(*)(BattleActor*, BattleTarget*, int32, float)) &CalculatePhysicalDamageAdder),
		luabind::def("CalculatePhysicalDamageMultiplier", (uint32(*)(BattleActor*, BattleTarget*, float)) &CalculatePhysicalDamageMultiplier),
		luabind::def("CalculatePhysicalDamageMultiplier", (uint32(*)(BattleActor*, BattleTarget*, float, float)) &CalculatePhysicalDamageMultiplier),
		luabind::def("CalculateMetaphysicalDamage", (uint32(*)(BattleActor*, BattleTarget*)) &CalculateMetaphysicalDamage),
		luabind::def("CalculateMetaphysicalDamage", (uint32(*)(BattleActor*, BattleTarget*, float)) &CalculateMetaphysicalDamage),
		luabind::def("CalculateMetaphysicalDamageAdder", (uint32(*)(BattleActor*, BattleTarget*, int32)) &CalculateMetaphysicalDamageAdder),
		luabind::def("CalculateMetaphysicalDamageAdder", (uint32(*)(BattleActor*, BattleTarget*, int32, float)) &CalculateMetaphysicalDamageAdder),
		luabind::def("CalculateMetaphysicalDamageMultiplier", (uint32(*)(BattleActor*, BattleTarget*, float)) &CalculateMetaphysicalDamageMultiplier),
		luabind::def("CalculateMetaphysicalDamageMultiplier", (uint32(*)(BattleActor*, BattleTarget*, float, float)) &CalculateMetaphysicalDamageMultiplier)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_battle")
	[
		luabind::class_<BattleMode, hoa_mode_manager::GameMode>("BattleMode")
			.def(luabind::constructor<>())
			.def("AddEnemy", (void(BattleMode::*)(uint32, float, float))&BattleMode::AddEnemy)
			.def("RestartBattle", &BattleMode::RestartBattle)
			.def("AreActorStatesPaused", &BattleMode::AreActorStatesPaused)
			.def("GetState", &BattleMode::GetState)
			.def("ChangeState", &BattleMode::ChangeState)
			.def("OpenCommandMenu", &BattleMode::OpenCommandMenu)
			.def("IsBattleFinished", &BattleMode::IsBattleFinished)
			.def("GetNumberOfCharacters", &BattleMode::GetNumberOfCharacters)
			.def("GetNumberOfEnemies", &BattleMode::GetNumberOfEnemies)
			.def("GetMedia", &BattleMode::GetMedia)
			.def("GetDialogueSupervisor", &BattleMode::GetDialogueSupervisor)
			.def("GetCommandSupervisor", &BattleMode::GetCommandSupervisor)
			.def("GetBattleType", &BattleMode::GetBattleType)
			.def("SetBattleType", &BattleMode::SetBattleType)

			// Namespace constants
			.enum_("constants") [
				// Battle states
				luabind::value("BATTLE_STATE_INITIAL", BATTLE_STATE_INITIAL),
				luabind::value("BATTLE_STATE_NORMAL", BATTLE_STATE_NORMAL),
				luabind::value("BATTLE_STATE_COMMAND", BATTLE_STATE_COMMAND),
				luabind::value("BATTLE_STATE_VICTORY", BATTLE_STATE_VICTORY),
				luabind::value("BATTLE_STATE_DEFEAT", BATTLE_STATE_DEFEAT),
				luabind::value("BATTLE_STATE_EXITING", BATTLE_STATE_EXITING),

				// Battle settings
				luabind::value("BATTLE_TYPE_INVALID", BATTLE_TYPE_INVALID),
				luabind::value("BATTLE_TYPE_WAIT", BATTLE_TYPE_WAIT),
				luabind::value("BATTLE_TYPE_SEMI_ACTIVE", BATTLE_TYPE_SEMI_ACTIVE),
				luabind::value("BATTLE_TYPE_ACTIVE", BATTLE_TYPE_ACTIVE),
				luabind::value("BATTLE_TYPE_TOTAL", BATTLE_TYPE_TOTAL)
			]
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_battle")
	[
		luabind::class_<BattleMedia>("BattleMedia")
			.def("SetBackgroundImage", &BattleMedia::SetBackgroundImage)
			.def("SetBattleMusic", &BattleMedia::SetBattleMusic)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_battle")
	[
		luabind::class_<BattleActor, hoa_global::GlobalActor>("BattleActor")
			.def("ChangeSpriteAnimation", &BattleActor::ChangeSpriteAnimation)
			.def("RegisterDamage", (void(BattleActor::*)(uint32)) &BattleActor::RegisterDamage)
			.def("RegisterDamage", (void(BattleActor::*)(uint32, BattleTarget*)) &BattleActor::RegisterDamage)
			.def("RegisterHealing", &BattleActor::RegisterHealing)
			.def("RegisterRevive", &BattleActor::RegisterRevive)
			.def("RegisterMiss", &BattleActor::RegisterMiss)
			.def("RegisterStatusChange", &BattleActor::RegisterStatusChange)
			.def("ResetHitPoints", &BattleActor::ResetHitPoints)
			.def("ResetMaxHitPoints", &BattleActor::ResetMaxHitPoints)
			.def("ResetSkillPoints", &BattleActor::ResetSkillPoints)
			.def("ResetMaxSkillPoints", &BattleActor::ResetMaxSkillPoints)
			.def("ResetStrength", &BattleActor::ResetStrength)
			.def("ResetVigor", &BattleActor::ResetVigor)
			.def("ResetFortitude", &BattleActor::ResetFortitude)
			.def("ResetProtection", &BattleActor::ResetProtection)
			.def("ResetAgility", &BattleActor::ResetAgility)
			.def("ResetEvade", &BattleActor::ResetEvade)
			.def("TotalPhysicalDefense", &BattleActor::TotalPhysicalDefense)
			.def("TotalMetaphysicalDefense", &BattleActor::TotalMetaphysicalDefense)
			.def("TotalEvadeRating", &BattleActor::TotalEvadeRating)
			.def("GetXOrigin", &BattleActor::GetXOrigin)
			.def("GetYOrigin", &BattleActor::GetYOrigin)
			.def("GetXLocation", &BattleActor::GetXLocation)
			.def("GetYLocation", &BattleActor::GetYLocation)
			.def("SetXLocation", &BattleActor::SetXLocation)
			.def("SetYLocation", &BattleActor::SetYLocation)
			.def("GetSpriteWidth", &BattleActor::GetSpriteWidth)
			.def("GetSpriteHeight", &BattleActor::GetSpriteHeight)
			.def("SetShowAmmo", &BattleActor::SetShowAmmo)
			.def("SetAmmoPosition", &BattleActor::SetAmmoPosition)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_battle")
	[
		luabind::class_<BattleCharacter, BattleActor>("BattleCharacter")
			.def("ChangeSpriteAnimation", &BattleCharacter::ChangeSpriteAnimation)
			.def("GetSpriteWidth", &BattleCharacter::GetSpriteWidth)
			.def("GetSpriteHeight", &BattleCharacter::GetSpriteHeight)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_battle")
	[
		luabind::class_<BattleEnemy, BattleActor>("BattleEnemy")
			.def("ChangeSpriteAnimation", &BattleEnemy::ChangeSpriteAnimation)
			.def("GetSpriteWidth", &BattleEnemy::GetSpriteWidth)
			.def("GetSpriteHeight", &BattleEnemy::GetSpriteHeight)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_battle")
	[
		luabind::class_<CommandSupervisor>("CommandSupervisor")
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_battle")
	[
		luabind::class_<BattleDialogue, hoa_common::CommonDialogue>("BattleDialogue")
			.def(luabind::constructor<uint32>())
			.def("AddLine", (void(BattleDialogue::*)(const std::string&, uint32))&BattleDialogue::AddLine)
			.def("AddLine", (void(BattleDialogue::*)(const std::string&, uint32, int32))&BattleDialogue::AddLine)
			.def("AddLineTimed", (void(BattleDialogue::*)(const std::string&, uint32, uint32))&BattleDialogue::AddLineTimed)
			.def("AddLineTimed", (void(BattleDialogue::*)(const std::string&, uint32, int32, uint32))&BattleDialogue::AddLineTimed)
			.def("AddOption", (void(BattleDialogue::*)(const std::string&))&BattleDialogue::AddOption)
			.def("AddOption", (void(BattleDialogue::*)(const std::string&, int32))&BattleDialogue::AddOption)
			.def("Validate", &BattleDialogue::Validate)
			.def("SetHaltBattleAction", &BattleDialogue::SetHaltBattleAction)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_battle")
	[
		luabind::class_<DialogueSupervisor>("DialogueSupervisor")
			.def("AddDialogue", &DialogueSupervisor::AddDialogue, luabind::adopt(_2))
			.def("AddCharacterSpeaker", &DialogueSupervisor::AddCharacterSpeaker)
			.def("AddEnemySpeaker", &DialogueSupervisor::AddEnemySpeaker)
			.def("AddCustomSpeaker", &DialogueSupervisor::AddCustomSpeaker)
			.def("ChangeSpeakerName", &DialogueSupervisor::ChangeSpeakerName)
			.def("ChangeSpeakerPortrait", &DialogueSupervisor::ChangeSpeakerPortrait)
			.def("BeginDialogue", &DialogueSupervisor::BeginDialogue)
			.def("EndDialogue", &DialogueSupervisor::EndDialogue)
			.def("ForceNextLine", &DialogueSupervisor::ForceNextLine)
			.def("IsDialogueActive", &DialogueSupervisor::IsDialogueActive)
			.def("GetCurrentDialogue", &DialogueSupervisor::GetCurrentDialogue)
			.def("GetLineCounter", &DialogueSupervisor::GetLineCounter)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_battle")
	[
		luabind::class_<BattleTarget>("BattleTarget")
			.def("SetPointTarget", &BattleTarget::SetPointTarget)
			.def("SetActorTarget", &BattleTarget::SetActorTarget)
			.def("SetPartyTarget", &BattleTarget::SetPartyTarget)
			.def("IsValid", &BattleTarget::IsValid)
			.def("SelectNextPoint", &BattleTarget::SelectNextPoint)
			.def("SelectNextActor", &BattleTarget::SelectNextActor)
			.def("GetType", &BattleTarget::GetType)
			.def("GetPoint", &BattleTarget::GetPoint)
			.def("GetActor", &BattleTarget::GetActor)
			.def("GetPartyActor", &BattleTarget::GetPartyActor)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_battle")
	[
		luabind::class_<BattleStatusEffect, hoa_global::GlobalStatusEffect>("BattleStatusEffect")
			.def("GetAffectedActor", &BattleStatusEffect::GetAffectedActor)
			.def("GetTimer", &BattleStatusEffect::GetTimer)
			.def("IsIntensityChanged", &BattleStatusEffect::IsIntensityChanged)
	];

	} // End using battle mode namespaces

	// ----- Menu Mode Bindings
	{
	using namespace hoa_menu;

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_menu")
	[
		luabind::class_<MenuMode, hoa_mode_manager::GameMode>("MenuMode")
			.def(luabind::constructor<hoa_utils::ustring, std::string>())
	];

	} // End using menu mode namespaces


	// ----- Shop Mode Bindings
	{
	using namespace hoa_shop;

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_shop")
	[
		luabind::class_<ShopMode, hoa_mode_manager::GameMode>("ShopMode")
			.def(luabind::constructor<>())
			.def("AddObject", &ShopMode::AddObject)
	];

	} // End using shop mode namespaces
} // void BindModeCode()

} // namespace hoa_defs
