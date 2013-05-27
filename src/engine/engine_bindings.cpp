///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    engine_bindings.cpp
*** \author  Daniel Steuernol, steu@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
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

namespace vt_defs
{

void BindEngineCode()
{
    // ----- Audio Engine Bindings
    {
        using namespace vt_audio;

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_audio")
        [
            luabind::class_<AudioEngine>("GameAudio")
            .def("PlaySound", &AudioEngine::PlaySound)
            .def("PlayMusic", &AudioEngine::PlayMusic)
            .def("LoadMusic", &AudioEngine::LoadMusic)
            .def("PauseAllMusic", &AudioEngine::PauseAllMusic)
            .def("ResumeAllMusic", &AudioEngine::ResumeAllMusic)
            .def("FadeOutAllMusic", &AudioEngine::FadeOutAllMusic)
            .def("FadeInAllMusic", &AudioEngine::FadeInAllMusic)
        ];

    } // End using audio namespaces



    // ----- Input Engine Bindings
    {
        using namespace vt_input;

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_input")
        [
            luabind::class_<InputEngine>("GameInput")
            .def("GetUpKeyName", &InputEngine::GetUpKeyName)
            .def("GetDownKeyName", &InputEngine::GetDownKeyName)
            .def("GetLeftKeyName", &InputEngine::GetLeftKeyName)
            .def("GetRightKeyName", &InputEngine::GetRightKeyName)
            .def("GetConfirmKeyName", &InputEngine::GetConfirmKeyName)
            .def("GetCancelKeyName", &InputEngine::GetCancelKeyName)
            .def("GetMenuKeyName", &InputEngine::GetMenuKeyName)
            .def("GetPauseKeyName", &InputEngine::GetPauseKeyName)
            .def("GetHelpKeyName", &InputEngine::GetHelpKeyName)
            .def("GetQuitKeyName", &InputEngine::GetQuitKeyName)
        ];

    } // End using input namespaces



    // ----- Mode Manager Engine Bindings
    {
        using namespace vt_mode_manager;

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_mode_manager")
        [
            luabind::class_<ScriptSupervisor>("ScriptSupervisor")
            .def("AddScript", &ScriptSupervisor::AddScript)
            .def("AddAnimation", (int32(ScriptSupervisor:: *)(const std::string&))&ScriptSupervisor::AddAnimation)
            .def("AddAnimation", (int32(ScriptSupervisor:: *)(const std::string&, float, float))&ScriptSupervisor::AddAnimation)
            .def("AddImage", &ScriptSupervisor::AddImage)
            .def("DrawImage", (void(ScriptSupervisor:: *)(int32, float, float, const vt_video::Color&))&ScriptSupervisor::DrawImage)
            .def("DrawImage", (void(ScriptSupervisor:: *)(int32, float, float))&ScriptSupervisor::DrawImage)
            .def("DrawRotatedImage", &ScriptSupervisor::DrawRotatedImage)
            .def("DrawAnimation", (void(ScriptSupervisor:: *)(int32, float, float))&ScriptSupervisor::DrawAnimation)
            .def("DrawAnimation", (void(ScriptSupervisor:: *)(int32, float, float, const vt_video::Color&))&ScriptSupervisor::DrawAnimation)
            .def("SetDrawFlag", &ScriptSupervisor::SetDrawFlag)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_mode_manager")
        [
            luabind::class_<EffectSupervisor>("EffectSupervisor")
            .def("EnableLightingOverlay", &EffectSupervisor::EnableLightingOverlay)
            .def("DisableLightingOverlay", &EffectSupervisor::DisableLightingOverlay)
            .def("EnableAmbientOverlay", &EffectSupervisor::EnableAmbientOverlay)
            .def("DisableAmbientOverlay", &EffectSupervisor::DisableAmbientOverlay)
            .def("EnableLightning", &EffectSupervisor::EnableLightning)
            .def("DisableLightning", &EffectSupervisor::DisableLightning)
            .def("DisableEffects", &EffectSupervisor::DisableEffects)
            .def("GetCameraXMovement", &EffectSupervisor::GetCameraXMovement)
            .def("GetCameraYMovement", &EffectSupervisor::GetCameraYMovement)
            .def("ShakeScreen", &EffectSupervisor::ShakeScreen)
            .def("StopShaking", &EffectSupervisor::StopShaking)

            // Namespace constants
            .enum_("constants") [
                // Shake fall off types
                luabind::value("SHAKE_FALLOFF_NONE", SHAKE_FALLOFF_NONE),
                luabind::value("SHAKE_FALLOFF_EASE", SHAKE_FALLOFF_EASE),
                luabind::value("SHAKE_FALLOFF_LINEAR", SHAKE_FALLOFF_LINEAR),
                luabind::value("SHAKE_FALLOFF_GRADUAL", SHAKE_FALLOFF_GRADUAL),
                luabind::value("SHAKE_FALLOFF_SUDDEN", SHAKE_FALLOFF_SUDDEN)
            ]
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_mode_manager")
        [
            luabind::class_<ParticleEffect>("ParticleEffect")
            .def(luabind::constructor<>())
            .def(luabind::constructor<const std::string &>())
            .def("LoadEffect", &ParticleEffect::LoadEffect)
            .def("Update", (bool (ParticleEffect:: *)())&ParticleEffect::Update)
            .def("Draw", &ParticleEffect::Draw)
            .def("IsAlive", &ParticleEffect::IsAlive)
            .def("Move", &ParticleEffect::Move)
            .def("Stop", &ParticleEffect::Stop)
            .def("Start", &ParticleEffect::Start)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_mode_manager")
        [
            luabind::class_<ParticleManager>("ParticleManager")
            .def("AddParticleEffect", &ParticleManager::AddParticleEffect)
            .def("StopAll", &ParticleManager::StopAll)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_mode_manager")
        [
            luabind::class_<GameMode>("GameMode")
            .def("GetScriptSupervisor", &GameMode::GetScriptSupervisor)
            .def("GetEffectSupervisor", &GameMode::GetEffectSupervisor)
            .def("GetParticleManager", &GameMode::GetParticleManager)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_mode_manager")
        [
            luabind::class_<ModeEngine>("GameModeManager")
            // The adopt policy set on the GameMode pointer is permitting to avoid
            // a memory corruption after the call time.
            .def("Push", &ModeEngine::Push, luabind::adopt(_2))
            .def("Pop", &ModeEngine::Pop)
            .def("PopAll", &ModeEngine::PopAll)
            .def("GetTop", &ModeEngine::GetTop)
            .def("Get", &ModeEngine::Get)
            .def("GetGameType", (uint8(ModeEngine:: *)(uint32))&ModeEngine::GetGameType)
            .def("GetGameType", (uint8(ModeEngine:: *)())&ModeEngine::GetGameType)
        ];

    } // End using mode manager namespaces



    // ----- Script Engine Bindings
    {
        using namespace vt_script;

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_script")
        [
            luabind::class_<ScriptEngine>("GameScript")
        ];

    } // End using script namespaces



    // ----- System Engine Bindings
    {
        using namespace vt_system;

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_system")
        [
            luabind::def("Translate", &vt_system::Translate),
            luabind::def("CTranslate", &vt_system::CTranslate),
            luabind::def("UTranslate", &vt_system::UTranslate),
            luabind::def("CUTranslate", &vt_system::CUTranslate),

            // Specializaton of c-formatted translation bounds
            luabind::def("VTranslate", (std::string(*)(const std::string&, uint32)) &vt_system::VTranslate),
            luabind::def("VTranslate", (std::string(*)(const std::string&, int32)) &vt_system::VTranslate),
            luabind::def("VTranslate", (std::string(*)(const std::string&, float)) &vt_system::VTranslate),
            luabind::def("VTranslate", (std::string(*)(const std::string&, const std::string&)) &vt_system::VTranslate),

            luabind::class_<SystemTimer>("SystemTimer")
            .def(luabind::constructor<>())
            .def(luabind::constructor<uint32, int32>())
            .def("Initialize", &SystemTimer::Initialize)
            .def("EnableAutoUpdate", &SystemTimer::EnableAutoUpdate)
            .def("EnableManualUpdate", &SystemTimer::EnableManualUpdate)
            .def("Update", (void(SystemTimer:: *)(void)) &SystemTimer::Update)
            .def("Update", (void(SystemTimer:: *)(uint32)) &SystemTimer::Update)
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
        using namespace vt_video;

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_video")
        [
            luabind::class_<Color>("Color")
            .def(luabind::constructor<float, float, float, float>())
            .def("SetAlpha", &Color::SetAlpha)
            .def("SetColor", (void(Color::*)(float, float, float, float))&Color::SetColor),

            luabind::class_<VideoEngine>("GameVideo")
            .def("FadeScreen", &VideoEngine::FadeScreen)
            .def("IsFading", &VideoEngine::IsFading)
            .def("FadeIn", &VideoEngine::FadeIn)
            .def("DrawText", (void (VideoEngine:: *)(const vt_utils::ustring &, float, float, const Color &)) &VideoEngine::DrawText)
            .def("DrawText", (void (VideoEngine:: *)(const std::string &, float, float, const Color &)) &VideoEngine::DrawText)

            // Draw cursor commands
            .def("Move", &VideoEngine::Move)
            .def("Rotate", &VideoEngine::Rotate)

            // Namespace constants
            .enum_("constants") [
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
    luabind::object global_table = luabind::globals(vt_script::ScriptManager->GetGlobalState());
    global_table["AudioManager"]     = vt_audio::AudioManager;
    global_table["InputManager"]     = vt_input::InputManager;
    global_table["ModeManager"]      = vt_mode_manager::ModeManager;
    global_table["ScriptManager"]    = vt_script::ScriptManager;
    global_table["SystemManager"]    = vt_system::SystemManager;
    global_table["VideoManager"]     = vt_video::VideoManager;
} // void BindEngineCode()

} // namespace vt_defs
