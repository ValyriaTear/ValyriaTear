///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    script_supervisor.cpp
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for managing scriptable animated effects for any game mode.
*** ***************************************************************************/

#include "common/include_pch.h"
#include "engine/script_supervisor.h"

#include "engine/mode_manager.h"

using namespace vt_video;
using namespace vt_script;

ScriptSupervisor::~ScriptSupervisor()
{
    // Free the created images before destruction.
    for(uint32_t i = 0; i < _text_images.size(); ++i)
        delete _text_images[i];
    for(uint32_t i = 0; i < _still_images.size(); ++i)
        delete _still_images[i];
    for(uint32_t i = 0; i < _animated_images.size(); ++i)
        delete _animated_images[i];

    // Free every luabind object pointers before freeing the scripts,
    // so that no object may trigger a segmentation fault
    // when the lua coroutine is deleted.
    _reset_functions.clear();
    _restart_functions.clear();
    _update_functions.clear();
    _draw_background_functions.clear();
    _draw_foreground_functions.clear();
    _draw_post_effects_functions.clear();

    // Close every loaded scripts to free their threads
    for(uint32_t i = 0; i < _scene_scripts.size(); ++i)
        delete _scene_scripts[i];
}

void ScriptSupervisor::Initialize(vt_mode_manager::GameMode *gm)
{
    // Open every possible scene script files registered and process them.
    for(uint32_t i = 0; i < _script_filenames.size(); ++i) {

        // Clears out old script data
        std::string tablespace = ScriptEngine::GetTableSpace(_script_filenames[i]);
        ScriptManager->DropGlobalTable(tablespace);

        ReadScriptDescriptor* scene_script = new ReadScriptDescriptor();
        if(!scene_script->OpenFile(_script_filenames[i])) {
            delete scene_script;
            continue;
        }

        if(scene_script->OpenTablespace().empty()) {
            PRINT_ERROR << "The scene script file: " << _script_filenames[i]
                        << "has not set a correct namespace" << std::endl;
            scene_script->CloseFile();
            delete scene_script;
            continue;
        }

        _reset_functions.push_back(scene_script->ReadFunctionPointer("Reset"));
        _restart_functions.push_back(scene_script->ReadFunctionPointer("Restart"));
        _update_functions.push_back(scene_script->ReadFunctionPointer("Update"));
        _draw_background_functions.push_back(scene_script->ReadFunctionPointer("DrawBackground"));
        _draw_foreground_functions.push_back(scene_script->ReadFunctionPointer("DrawForeground"));
        _draw_post_effects_functions.push_back(scene_script->ReadFunctionPointer("DrawPostEffects"));

        // Add the script to the list now it is valid.
        _scene_scripts.push_back(scene_script);

        // Trigger the Initialize functions in the loading order.
        luabind::object init_function = scene_script->ReadFunctionPointer("Initialize");
        if(init_function.is_valid() && gm)
            luabind::call_function<void>(init_function, gm);
        else
            PRINT_ERROR << "Couldn't initialize the scene component" << std::endl; // Should never happen
    }
}

void ScriptSupervisor::SetDrawFlag(vt_video::VIDEO_DRAW_FLAGS draw_flag)
{
    VideoManager->SetDrawFlags(draw_flag, 0);
}

void ScriptSupervisor::Reset()
{
    // Updates custom scripts
    for(uint32_t i = 0; i < _reset_functions.size(); ++i)
        ReadScriptDescriptor::RunScriptObject(_reset_functions[i]);
}

void ScriptSupervisor::Restart()
{
    // Updates custom scripts
    for(uint32_t i = 0; i < _restart_functions.size(); ++i)
        ReadScriptDescriptor::RunScriptObject(_restart_functions[i]);
}

void ScriptSupervisor::Update()
{
    // Updates custom scripts
    for(uint32_t i = 0; i < _update_functions.size(); ++i)
        ReadScriptDescriptor::RunScriptObject(_update_functions[i]);
}

void ScriptSupervisor::DrawBackground()
{
    // Handles custom scripted draw before sprites
    for(uint32_t i = 0; i < _draw_background_functions.size(); ++i)
        ReadScriptDescriptor::RunScriptObject(_draw_background_functions[i]);
}

void ScriptSupervisor::DrawForeground()
{
    for(uint32_t i = 0; i < _draw_foreground_functions.size(); ++i)
        ReadScriptDescriptor::RunScriptObject(_draw_foreground_functions[i]);
}

void ScriptSupervisor::DrawPostEffects()
{
    for(uint32_t i = 0; i < _draw_post_effects_functions.size(); ++i)
        ReadScriptDescriptor::RunScriptObject(_draw_post_effects_functions[i]);
}

// Images loading
TextImage* ScriptSupervisor::CreateText(const vt_utils::ustring& text, const vt_video::TextStyle& style)
{
    vt_video::TextImage* text_image = new vt_video::TextImage(text, style);
    _text_images.push_back(text_image);
    return text_image;
}

AnimatedImage* ScriptSupervisor::CreateAnimation(const std::string& filename)
{
    AnimatedImage* anim = new AnimatedImage();
    if(!filename.empty() && !anim->LoadFromAnimationScript(filename))
        PRINT_WARNING << "The animation file could not be loaded: " << filename << std::endl;

    _animated_images.push_back(anim);
    return anim;
}

StillImage* ScriptSupervisor::CreateImage(const std::string& filename)
{
    StillImage* img = new StillImage();
    if(!filename.empty() && !img->Load(filename))
        PRINT_WARNING << "The image file could not be loaded: " << filename << std::endl;
    _still_images.push_back(img);
    return img;
}
