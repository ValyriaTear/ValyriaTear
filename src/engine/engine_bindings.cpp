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
*** \brief   Lua bindings for the engine code
***
*** All bindings for the engine code is contained within this file.
*** Therefore, everything that you see bound within this file will be made
*** available in Lua.
***
*** \note To most C++ programmers, the syntax of the binding code found in this
*** file may be very unfamiliar and obtuse. Refer to the Luabind documentation
*** as necessary to gain an understanding of this code style.
*** **************************************************************************/

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/mode_manager.h"
#include "engine/script/script.h"
#include "engine/system.h"
#include "engine/video/video.h"
#include "engine/video/particle_effect.h"

#include "common/global/global.h"

namespace hoa_defs {

void BindEngineCode() {
	// ----- Audio Engine Bindings
	{
	using namespace hoa_audio;

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_audio")
	[
		luabind::class_<AudioEngine>("GameAudio")
			.def("PlaySound", &AudioEngine::PlaySound)
			.def("PlayMusic", &AudioEngine::PlayMusic)
			.def("PauseAllMusic", &AudioEngine::PauseAllMusic)
			.def("ResumeAllMusic", &AudioEngine::ResumeAllMusic)
			.def("FadeOutAllMusic", &AudioEngine::FadeOutAllMusic)
			.def("FadeInAllMusic", &AudioEngine::FadeInAllMusic)
	];

	} // End using audio namespaces



	// ----- Input Engine Bindings
	{
	using namespace hoa_input;

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_input")
	[
		luabind::class_<InputEngine>("GameInput")
	];

	} // End using input namespaces



	// ----- Mode Manager Engine Bindings
	{
	using namespace hoa_mode_manager;

 	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_mode_manager")
 	[
		luabind::class_<ScriptSupervisor>("ScriptSupervisor")
			.def("AddScript", &ScriptSupervisor::AddScript)
			.def("AddAnimation", &ScriptSupervisor::AddAnimation)
			.def("AddImage", &ScriptSupervisor::AddImage)
			.def("DrawImage", &ScriptSupervisor::DrawImage)
			.def("DrawRotatedImage", &ScriptSupervisor::DrawRotatedImage)
			.def("DrawAnimation", &ScriptSupervisor::DrawAnimation)
			.def("SetDrawFlag", &ScriptSupervisor::SetDrawFlag)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_mode_manager")
	[
		luabind::class_<EffectSupervisor>("EffectSupervisor")
			.def("EnableLightingOverlay", &EffectSupervisor::EnableLightingOverlay)
			.def("DisableLightingOverlay", &EffectSupervisor::DisableLightingOverlay)
			.def("EnableAmbientOverlay", &EffectSupervisor::EnableAmbientOverlay)
			.def("DisableAmbientOverlay", &EffectSupervisor::DisableAmbientOverlay)
			.def("EnableLightning", &EffectSupervisor::EnableLightning)
			.def("DisableLightning", &EffectSupervisor::DisableLightning)
			.def("DisableEffects", &EffectSupervisor::DisableEffects)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_mode_manager")
	[
		luabind::class_<ParticleEffect>("ParticleEffect")
			.def("IsAlive", &ParticleEffect::IsAlive)
			.def("Move", &ParticleEffect::Move)
			.def("Stop", &ParticleEffect::Stop)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_mode_manager")
	[
		luabind::class_<ParticleManager>("ParticleManager")
			.def("AddParticleEffect", &ParticleManager::AddParticleEffect)
			.def("RestartParticleEffect", &ParticleManager::RestartParticleEffect)
			.def("StopAll", &ParticleManager::StopAll)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_mode_manager")
	[
		luabind::class_<GameMode>("GameMode")
			.def("GetScriptSupervisor", &GameMode::GetScriptSupervisor)
			.def("GetEffectSupervisor", &GameMode::GetEffectSupervisor)
			.def("GetParticleManager", &GameMode::GetParticleManager)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_mode_manager")
	[
		luabind::class_<ModeEngine>("GameModeManager")
			// The adopt policy set on the GameMode pointer is permitting to avoid
			// a memory corruption after the call time.
			.def("Push", &ModeEngine::Push, luabind::adopt(_2))
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

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_script")
	[
		luabind::class_<ScriptEngine>("GameScript")
	];

	} // End using script namespaces



	// ----- System Engine Bindings
	{
	using namespace hoa_system;

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_system")
	[
		luabind::def("Translate", &hoa_system::Translate),

		luabind::class_<SystemTimer>("SystemTimer")
			.def(luabind::constructor<>())
			.def(luabind::constructor<uint32, int32>())
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

		luabind::class_<SystemEngine>("GameSystem")
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

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_video")
	[
		luabind::class_<Color>("Color")
			.def(luabind::constructor<float, float, float, float>()),

		luabind::class_<VideoEngine>("GameVideo")
			.def("FadeScreen", &VideoEngine::FadeScreen)
			.def("IsFading", &VideoEngine::IsFading)
			.def("FadeIn", &VideoEngine::FadeIn)
			.def("ShakeScreen", &VideoEngine::ShakeScreen)
			.def("DrawText", (void (VideoEngine::*)(const hoa_utils::ustring&, float, float, const Color&)) &VideoEngine::DrawText)
			.def("DrawText", (void (VideoEngine::*)(const std::string&, float, float, const Color&)) &VideoEngine::DrawText)

			// Draw cursor commands
			.def("Move", &VideoEngine::Move)
			.def("Rotate", &VideoEngine::Rotate)

			// Namespace constants
			.enum_("constants") [
				// Shake fall off types
				luabind::value("VIDEO_FALLOFF_NONE", VIDEO_FALLOFF_NONE),
				luabind::value("VIDEO_FALLOFF_EASE", VIDEO_FALLOFF_EASE),
				luabind::value("VIDEO_FALLOFF_LINEAR", VIDEO_FALLOFF_LINEAR),
				luabind::value("VIDEO_FALLOFF_GRADUAL", VIDEO_FALLOFF_GRADUAL),
				luabind::value("VIDEO_FALLOFF_SUDDEN", VIDEO_FALLOFF_SUDDEN),

				// Video context drawing constants
				luabind::value("VIDEO_X_LEFT", VIDEO_X_LEFT),
				luabind::value("VIDEO_X_CENTER", VIDEO_X_CENTER),
				luabind::value("VIDEO_X_RIGHT", VIDEO_X_RIGHT),
				luabind::value("VIDEO_Y_TOP", VIDEO_Y_TOP),
				luabind::value("VIDEO_Y_CENTER", VIDEO_Y_CENTER),
				luabind::value("VIDEO_Y_BOTTOM", VIDEO_Y_BOTTOM),
				luabind::value("VIDEO_X_FLIP", VIDEO_X_FLIP),
				luabind::value("VIDEO_X_NOFLIP", VIDEO_X_NOFLIP),
				luabind::value("VIDEO_Y_FLIP", VIDEO_Y_FLIP),
				luabind::value("VIDEO_Y_NOFLIP", VIDEO_Y_NOFLIP),
				luabind::value("VIDEO_NO_BLEND", VIDEO_NO_BLEND),
				luabind::value("VIDEO_BLEND", VIDEO_BLEND),
				luabind::value("VIDEO_BLEND_ADD", VIDEO_BLEND_ADD)
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
