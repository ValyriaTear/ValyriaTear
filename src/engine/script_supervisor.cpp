///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012 by Bertram (Yohann Ferreira)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    script_supervisor.cpp
*** \author Yohann Ferreira, yohann ferreira orange fre
*** \brief  Source file for managing scriptable animated effects for any game mode.
*** ***************************************************************************/

#include "engine/script_supervisor.h"

#include "engine/mode_manager.h"

using namespace hoa_video;
using namespace hoa_script;

void ScriptSupervisor::Initialize(hoa_mode_manager::GameMode *gm)
{
    // Open every possible scene script files registered and process them.
    for(uint32 i = 0; i < _script_filenames.size(); ++i) {

        // Clears out old script data
        std::string tablespace = ScriptEngine::GetTableSpace(_script_filenames[i]);
        ScriptManager->DropGlobalTable(tablespace);

        ReadScriptDescriptor scene_script;
        if(!scene_script.OpenFile(_script_filenames[i]))
            continue;

        if(scene_script.OpenTablespace().empty()) {
            PRINT_ERROR << "The scene script file: " << _script_filenames[i]
                        << "has not set a correct namespace" << std::endl;
            scene_script.CloseFile();
            continue;
        }

        _reset_functions.push_back(scene_script.ReadFunctionPointer("Reset"));
        _update_functions.push_back(scene_script.ReadFunctionPointer("Update"));
        _draw_background_functions.push_back(scene_script.ReadFunctionPointer("DrawBackground"));
        _draw_foreground_functions.push_back(scene_script.ReadFunctionPointer("DrawForeground"));
        _draw_post_effects_functions.push_back(scene_script.ReadFunctionPointer("DrawPostEffects"));

        // Trigger the Initialize functions in the loading order.
        ScriptObject init_function = scene_script.ReadFunctionPointer("Initialize");
        if(init_function.is_valid() && gm)
            ScriptCallFunction<void>(init_function, gm);
        else
            PRINT_ERROR << "Couldn't initialize the scene component" << std::endl; // Should never happen

        scene_script.CloseTable(); // The tablespace
        scene_script.CloseFile();
    }
}

void ScriptSupervisor::SetDrawFlag(hoa_video::VIDEO_DRAW_FLAGS draw_flag)
{
    VideoManager->SetDrawFlags(draw_flag, 0);
}

void ScriptSupervisor::Reset()
{
    // Updates custom scripts
    for(uint32 i = 0; i < _reset_functions.size(); ++i)
        ReadScriptDescriptor::RunScriptObject(_reset_functions[i]);
}

void ScriptSupervisor::Update()
{
    // Update custom animations
    for(uint32 i = 0; i < _script_animations.size(); ++i)
        _script_animations[i].Update();

    // Updates custom scripts
    for(uint32 i = 0; i < _update_functions.size(); ++i)
        ReadScriptDescriptor::RunScriptObject(_update_functions[i]);
}


void ScriptSupervisor::DrawBackground()
{
    // Handles custom scripted draw before sprites
    for(uint32 i = 0; i < _draw_background_functions.size(); ++i)
        ReadScriptDescriptor::RunScriptObject(_draw_background_functions[i]);
}


void ScriptSupervisor::DrawForeground()
{
    for(uint32 i = 0; i < _draw_foreground_functions.size(); ++i)
        ReadScriptDescriptor::RunScriptObject(_draw_foreground_functions[i]);
}

void ScriptSupervisor::DrawPostEffects()
{
    for(uint32 i = 0; i < _draw_post_effects_functions.size(); ++i)
        ReadScriptDescriptor::RunScriptObject(_draw_post_effects_functions[i]);
}

int32 ScriptSupervisor::AddAnimation(const std::string &filename)
{
    AnimatedImage anim;
    if(!anim.LoadFromAnimationScript(filename)) {
        PRINT_WARNING << "The animation file could not be loaded: " << filename << std::endl;
        return -1;
    }

    _script_animations.push_back(anim);

    int32 id = _script_animations.size() - 1;
    return id;
}


void ScriptSupervisor::DrawAnimation(int32 id, float x, float y)
{
    if(id < 0 || id > static_cast<int32>(_script_animations.size()) - 1)
        return;

    VideoManager->Move(x, y);
    _script_animations[id].Draw();
}


int32 ScriptSupervisor::AddImage(const std::string &filename, float width,
                                 float height)
{
    StillImage img;
    if(!img.Load(filename, width, height)) {
        PRINT_WARNING << "The image file could not be loaded: " << filename << std::endl;
        return -1;
    }

    _script_images.push_back(img);

    int32 id = _script_images.size() - 1;
    return id;
}

void ScriptSupervisor::DrawImage(int32 id, float x, float y, const Color &color)
{
    if(id < 0 || id > static_cast<int32>(_script_images.size()) - 1)
        return;

    VideoManager->Move(x, y);
    _script_images[id].Draw(color);
}

void ScriptSupervisor::DrawRotatedImage(int32 id, float x, float y, const Color &color, float angle)
{
    if(id < 0 || id > static_cast<int32>(_script_images.size()) - 1)
        return;

    VideoManager->Move(x, y);
    VideoManager->Rotate(angle);
    _script_images[id].Draw(color);
}
