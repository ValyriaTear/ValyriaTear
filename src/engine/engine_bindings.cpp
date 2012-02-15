///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    engine_bindings.cpp
*** \author  Daniel Steuernol, steu@allacrost.org
*** \brief   Lua bindings for Allacrost engine code
***
*** All bindings for the engine code is contained within this file.
*** Therefore, everything that you see bound within this file will be made
*** available in Lua.
***
*** \note To most C++ programmers, the syntax of the binding code found in this
*** file may be very unfamiliar and obtuse. Refer to the Luabind documentation
*** as necessary to gain an understanding of this code style.
*** **************************************************************************/

#include "defs.h"

#include "audio.h"
#include "input.h"
#include "mode_manager.h"
#include "script.h"
#include "system.h"
#include "video.h"

#include "global.h"

using namespace luabind;

namespace hoa_defs {

void BindEngineCode() {
	// ----- Audio Engine Bindings
	{
	using namespace hoa_audio;

	module(hoa_script::ScriptManager->GetGlobalState(), "hoa_audio")
	[
		class_<AudioEngine>("GameAudio")
			.def("PlaySound", &AudioEngine::PlaySound)
	];

	} // End using audio namespaces



	// ----- Input Engine Bindings
	{
	using namespace hoa_input;

	module(hoa_script::ScriptManager->GetGlobalState(), "hoa_input")
	[
		class_<InputEngine>("GameInput")
	];

	} // End using input namespaces



	// ----- Mode Manager Engine Bindings
	{
	using namespace hoa_mode_manager;

	module(hoa_script::ScriptManager->GetGlobalState(), "hoa_mode_manager")
	[
		class_<GameMode>("GameMode")
	];

	module(hoa_script::ScriptManager->GetGlobalState(), "hoa_mode_manager")
	[
		class_<ModeEngine>("GameModeManager")
			.def("Push", &ModeEngine::Push, adopt(_2))
			.def("Pop", &ModeEngine::Pop)
			.def("PopAll", &ModeEngine::PopAll)
			.def("GetTop", &ModeEngine::GetTop)
			.def("Get", &ModeEngine::Get)
			.def("GetGameType", (uint8 (ModeEngine::*)(uint32))&ModeEngine::GetGameType)
			.def("GetGameType", (uint8 (ModeEngine::*)())&ModeEngine::GetGameType)
	];

	} // End using mode manager namespaces



	// ----- Script Engine Bindings
	{
	using namespace hoa_script;

	module(hoa_script::ScriptManager->GetGlobalState(), "hoa_script")
	[
		class_<ScriptEngine>("GameScript")
	];

	} // End using script namespaces



	// ----- System Engine Bindings
	{
	using namespace hoa_system;

	module(hoa_script::ScriptManager->GetGlobalState(), "hoa_system")
	[
		def("Translate", &hoa_system::Translate),

		class_<SystemTimer>("SystemTimer")
			.def(constructor<>())
			.def(constructor<uint32, int32>())
			.def("Initialize", &SystemTimer::Initialize)
			.def("EnableAutoUpdate", &SystemTimer::EnableAutoUpdate)
			.def("EnableManualUpdate", &SystemTimer::EnableManualUpdate)
			.def("Update", (void(SystemTimer::*)(void)) &SystemTimer::Update)
			.def("Update", (void(SystemTimer::*)(uint32)) &SystemTimer::Update)
			.def("Reset", &SystemTimer::Reset)
			.def("Run", &SystemTimer::Run)
			.def("Pause", &SystemTimer::Pause)
			.def("Finish", &SystemTimer::Finish)
			.def("IsInitial", &SystemTimer::IsInitial)
			.def("IsRunning", &SystemTimer::IsRunning)
			.def("IsPaused", &SystemTimer::IsPaused)
			.def("IsFinished", &SystemTimer::IsFinished)
			.def("CurrentLoop", &SystemTimer::CurrentLoop)
			.def("TimeLeft", &SystemTimer::TimeLeft)
			.def("PercentComplete", &SystemTimer::PercentComplete)
			.def("SetDuration", &SystemTimer::SetDuration)
			.def("SetNumberLoops", &SystemTimer::SetNumberLoops)
			.def("SetModeOwner", &SystemTimer::SetModeOwner)
			.def("GetState", &SystemTimer::GetState)
			.def("GetDuration", &SystemTimer::GetDuration)
			.def("GetNumberLoops", &SystemTimer::GetNumberLoops)
			.def("IsAutoUpdate", &SystemTimer::IsAutoUpdate)
			.def("GetModeOwner", &SystemTimer::GetModeOwner)
			.def("GetTimeExpired", &SystemTimer::GetTimeExpired)
			.def("GetTimesCompleted", &SystemTimer::GetTimesCompleted),

		class_<SystemEngine>("GameSystem")
			.def("GetUpdateTime", &SystemEngine::GetUpdateTime)
			.def("SetPlayTime", &SystemEngine::SetPlayTime)
			.def("GetPlayHours", &SystemEngine::GetPlayHours)
			.def("GetPlayMinutes", &SystemEngine::GetPlayMinutes)
			.def("GetPlaySeconds", &SystemEngine::GetPlaySeconds)
			.def("GetLanguage", &SystemEngine::GetLanguage)
			.def("SetLanguage", &SystemEngine::SetLanguage)
			.def("NotDone", &SystemEngine::NotDone)
			.def("ExitGame", &SystemEngine::ExitGame)
	];

	} // End using system namespaces



	// ----- Video Engine Bindings
	{
	using namespace hoa_video;

	module(hoa_script::ScriptManager->GetGlobalState(), "hoa_video")
	[
		class_<Color>("Color")
			.def(constructor<float, float, float, float>()),

		class_<VideoEngine>("GameVideo")
			.def("FadeScreen", &VideoEngine::FadeScreen)
			.def("IsFading", &VideoEngine::IsFading)
			.def("ShakeScreen", &VideoEngine::ShakeScreen)
			.def("EnableSceneLighting", &VideoEngine::EnableSceneLighting)
			.def("DisableSceneLighting", &VideoEngine::DisableSceneLighting)
			.def("ApplyLightingOverlay", &VideoEngine::ApplyLightingOverlay)

			// Namespace constants
			.enum_("constants") [
				// Shake fall off types
				value("VIDEO_FALLOFF_NONE", VIDEO_FALLOFF_NONE),
				value("VIDEO_FALLOFF_EASE", VIDEO_FALLOFF_EASE),
				value("VIDEO_FALLOFF_LINEAR", VIDEO_FALLOFF_LINEAR),
				value("VIDEO_FALLOFF_GRADUAL", VIDEO_FALLOFF_GRADUAL),
				value("VIDEO_FALLOFF_SUDDEN", VIDEO_FALLOFF_SUDDEN)
			]
	];

	} // End using video namespaces

	// ---------- Bind engine class objects
	luabind::object global_table = luabind::globals(hoa_script::ScriptManager->GetGlobalState());
	global_table["AudioManager"]     = hoa_audio::AudioManager;
	global_table["InputManager"]     = hoa_input::InputManager;
	global_table["ModeManager"]      = hoa_mode_manager::ModeManager;
	global_table["ScriptManager"]    = hoa_script::ScriptManager;
	global_table["SystemManager"]    = hoa_system::SystemManager;
	global_table["VideoManager"]     = hoa_video::VideoManager;
} // void BindEngineCode()

} // namespace hoa_defs
