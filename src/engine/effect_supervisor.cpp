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
*** \file   effect_supervisor.cpp
*** \author Yohann Ferreira, yohann ferreira orange fre
*** \brief  Source file for managing visual ambient effects
*** **************************************************************************/

#include "effect_supervisor.h"

#include "engine/script/script_read.h"
#include "engine/audio/audio.h"
#include "engine/video/video.h"

#include "utils/utils_random.h"

using namespace vt_video;

namespace vt_mode_manager
{

EffectSupervisor::EffectSupervisor()
{
    // Initialize the overlays
    // Light
    _info.light.active = false;
    _light_overlay_img.Load("", 1.0f, 1.0f);

    // Texture overlay
    _info.overlay.active = false;
    _info.overlay.x_speed = 0;
    _info.overlay.y_speed = 0;
    _info.overlay.x_shift = 0;
    _info.overlay.y_shift = 0;

    // lightning
    _info.lightning.loop = false;
    _info.lightning.active = false;
    _info.lightning.active_id = -1;

    _lightning_inner_info._lightning_current_time = 0;
    _lightning_inner_info._lightning_overlay_img.Load("", 1.0f, 1.0f);

    // Load the lightning effect
    _LoadLightnings("dat/effects/lightning.lua");
}


void EffectSupervisor::EnableAmbientOverlay(const std::string &filename,
        float x_speed, float y_speed,
        bool parallax)
{
    // Note: The StillImage class handles clearing an image
    // when loading another one.
    _ambient_overlay_img.Clear();

    if(_info.overlay.filename != filename
            && _ambient_overlay_img.Load(filename)) {
        _info.overlay.filename = filename;
        _info.overlay.x_speed = x_speed;
        _info.overlay.y_speed = y_speed;
        _info.overlay.active = true;
        _info.overlay.x_parallax = 0.0f;
        _info.overlay.y_parallax = 0.0f;
        _info.overlay.is_parallax = parallax;
    }
}

void EffectSupervisor::DisableAmbientOverlay()
{
    _info.overlay.active = false;
}


void EffectSupervisor::EnableLightingOverlay(const Color &color)
{
    _info.light.color = color;
    _light_overlay_img.SetColor(color);
    _info.light.active = true;
}


void EffectSupervisor::DisableLightingOverlay()
{
    _info.light.active = false;
}

bool EffectSupervisor::_LoadLightnings(const std::string &lightning_file)
{
    _lightning_inner_info._lightning_data.clear();

    vt_script::ReadScriptDescriptor lightning_script;
    if(lightning_script.OpenFile(lightning_file) == false) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "No script file: '"
                                      << lightning_file << "' The lightning effects won't work." << std::endl;
        return false;
    }

    int16 lightnings_size = lightning_script.ReadInt("num_of_lightnings");
    _lightning_inner_info._lightning_end_times.resize(lightnings_size);
    _lightning_inner_info._lightning_data.resize(lightnings_size);
    _lightning_inner_info._lightning_sound_events.resize(lightnings_size);

    // Read a list of alpha intensities (0.0f - 1.0f)
    // the lightning_intensity lua table.
    lightning_script.OpenTable("lightning_intensities");
    for(int i = 0; i < lightnings_size; ++i) {
        lightning_script.ReadFloatVector(i, _lightning_inner_info._lightning_data[i]);
    }
    lightning_script.CloseTable();

    // Load the lightning sounds events
    lightning_script.OpenTable("lightning_sounds_filenames");
    std::vector <std::string> sound_filenames;
    for(int i = 0; i < lightnings_size; ++i) {
        sound_filenames.clear();
        lightning_script.ReadStringVector(i, sound_filenames);
        _lightning_inner_info._lightning_sound_events[i].resize(sound_filenames.size());
        for(size_t j = 0; j < sound_filenames.size(); ++j) {
            _lightning_inner_info._lightning_sound_events[i][j].sound_filename = sound_filenames.at(j);
        }
    }
    lightning_script.CloseTable();
    lightning_script.OpenTable("lightning_sounds_times");

    std::vector <int32> times;
    for(int i = 0; i < lightnings_size; ++i) {
        times.clear();
        lightning_script.ReadIntVector(i, times);
        for(size_t j = 0; j < times.size(); ++j) {
            _lightning_inner_info._lightning_sound_events[i][j].time = times.at(j);
        }
        // Add a sound event queue terminator
        LightningVideoManagerInfo::lightning_sound_event terminator_event;
        terminator_event.time = -1;
        _lightning_inner_info._lightning_sound_events[i].push_back(terminator_event);
    }
    lightning_script.CloseTable();
    lightning_script.CloseFile();

    if(_lightning_inner_info._lightning_data.empty()) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "No lightning intensities read from: '"
                                      << lightning_file << "'. The effects won't work." << std::endl;
        return false;
    }

    // Check the table of float intensities for sane values
    std::vector<float>::iterator it, it_end;
    for(int i = 0; i < lightnings_size; ++i) {
        for(it = _lightning_inner_info._lightning_data[i].begin(),
                it_end = _lightning_inner_info._lightning_data[i].end();
                it != it_end; ++it) {
            if(*it > 1.0f)
                *it = 1.0f;
            else if(*it < 0.0f)
                *it = 0.0f;
        }
        // Set the timer's end (one piece of data each 100ms)
        _lightning_inner_info._lightning_end_times[i] = _lightning_inner_info._lightning_data.at(i).size() * 1000 / 100;
    }

    // reset the effect timer
    _lightning_inner_info._lightning_current_time = 0;
    return true;
}

void EffectSupervisor::EnableLightning(int16 id, bool loop)
{
    if(id > -1 && id < (int16)_lightning_inner_info._lightning_data.size()) {
        _info.lightning.active_id = id;
        _info.lightning.active = true;
        _info.lightning.loop = loop;
        _lightning_inner_info._lightning_current_time = 0;

        // Load the current sound events
        _lightning_inner_info._current_lightning_sound_events.clear();
        std::vector<LightningVideoManagerInfo::lightning_sound_event>::iterator it, it_end;
        for(it = _lightning_inner_info._lightning_sound_events.at(id).begin(),
                it_end = _lightning_inner_info._lightning_sound_events.at(id).end(); it != it_end; ++it) {
            _lightning_inner_info._current_lightning_sound_events.push_back(*it);
            // Preload the files for efficiency
            vt_audio::AudioManager->LoadSound(it->sound_filename);
        }
    } else {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "Invalid lightning effect requested: "
                                      << id << ", the effect won't be displayed." << std::endl;
        DisableLightning();
    }
}

void EffectSupervisor::Update(uint32 frame_time)
{
    _UpdateAmbientOverlay(frame_time);
    _UpdateLightning(frame_time);
    _UpdateShake(frame_time);
}

void EffectSupervisor::_UpdateAmbientOverlay(uint32 frame_time)
{
    if(!_info.overlay.active)
        return;

    // Update the shifting
    float elapsed_ms = static_cast<float>(frame_time);
    //static_cast<float>(vt_system::SystemManager->GetUpdateTime());
    _info.overlay.x_shift += elapsed_ms / 1000 * _info.overlay.x_speed;
    _info.overlay.y_shift += elapsed_ms / 1000 * _info.overlay.y_speed;

    // Add the parallax values to the shifting and reset them for next update.
    if(_info.overlay.is_parallax) {
        _info.overlay.x_shift += _info.overlay.x_parallax;
        _info.overlay.y_shift += _info.overlay.y_parallax;
    }
    _info.overlay.x_parallax = 0.0f;
    _info.overlay.y_parallax = 0.0f;

    float width = _ambient_overlay_img.GetWidth();
    float height = _ambient_overlay_img.GetHeight();

    // Make them negative to draw on the entire screen
    while(_info.overlay.x_shift > 0.0f) {
        _info.overlay.x_shift -= width;
    }
    // handle negative shifting
    if(_info.overlay.x_shift < 2 * -width)
        _info.overlay.x_shift += width;

    while(_info.overlay.y_shift > 0.0f) {
        _info.overlay.y_shift -= height;
    }
    // handle negative shifting
    if(_info.overlay.y_shift < 2 * -height)
        _info.overlay.y_shift += height;
}

void EffectSupervisor::_UpdateLightning(uint32 frame_time)
{
    if(!_info.lightning.active)
        return;

    // Update lightning timer
    _lightning_inner_info._lightning_current_time += frame_time;

    // Play potential lightning effect sounds based on their timers
    std::deque<LightningVideoManagerInfo::lightning_sound_event>::const_iterator it = _lightning_inner_info._current_lightning_sound_events.begin();
    if(it != _lightning_inner_info._current_lightning_sound_events.end()
            && (*it).time > -1 && (*it).time <= _lightning_inner_info._lightning_current_time) {
        // Play the sound
        vt_audio::AudioManager->PlaySound((*it).sound_filename);
        // And put the data at bottom for next potential lightning loop
        LightningVideoManagerInfo::lightning_sound_event next_event;
        next_event.sound_filename = (*it).sound_filename;
        next_event.time = (*it).time;
        _lightning_inner_info._current_lightning_sound_events.push_back(next_event);
        _lightning_inner_info._current_lightning_sound_events.pop_front();
    }

    if(_info.lightning.active_id > -1
            && _lightning_inner_info._lightning_current_time >
            _lightning_inner_info._lightning_end_times.at(_info.lightning.active_id)) {
        if(_info.lightning.loop) {
            _lightning_inner_info._lightning_current_time = 0;
            // Remove the sound terminator event when the queue has got sufficient events.
            // One event + the terminator event.
            if(_lightning_inner_info._current_lightning_sound_events.size() > 1) {
                _lightning_inner_info._current_lightning_sound_events.pop_front();
                LightningVideoManagerInfo::lightning_sound_event terminator_event;
                terminator_event.time = -1;
                _lightning_inner_info._current_lightning_sound_events.push_back(terminator_event);
            }
        } else {
            _info.lightning.active = false;
        }
    }
}

void EffectSupervisor::_DrawLightning()
{
    if(_info.lightning.active_id < 0 || !_info.lightning.active)
        return;

    // convert milliseconds elapsed into data points elapsed
    float t = _lightning_inner_info._lightning_current_time * 100.0f / 1000.0f;

    int32 rounded_t = static_cast<int32>(t);
    t -= rounded_t;

    // Safety check
    if(rounded_t + 1 >= (int32)_lightning_inner_info._lightning_data[_info.lightning.active_id].size())
        return;

    // get 2 separate data points and blend together (linear interpolation)
    float data1 = _lightning_inner_info._lightning_data.at(_info.lightning.active_id)[rounded_t];
    float data2 = _lightning_inner_info._lightning_data.at(_info.lightning.active_id)[rounded_t + 1];

    float intensity = data1 * (1 - t) + data2 * t;

    VideoManager->PushState();
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->SetCoordSys(0.1f, 0.9f, 0.1f, 0.9f);
    VideoManager->Move(0.0f, 0.0f);
    _lightning_inner_info._lightning_overlay_img.Draw(Color(1.0f, 1.0f, 1.0f, intensity));
    VideoManager->PopState();
}

void EffectSupervisor::DrawEffects()
{
    // Draw the textured ambient overlay
    if(_info.overlay.active) {
        VideoManager->PushState();
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
        VideoManager->SetCoordSys(0.0f, VIDEO_STANDARD_RES_WIDTH, 0.0f, VIDEO_STANDARD_RES_HEIGHT);
        float width = _ambient_overlay_img.GetWidth();
        float height = _ambient_overlay_img.GetHeight();
        for(float x = _info.overlay.x_shift; x <= VIDEO_STANDARD_RES_WIDTH; x = x + width) {
            for(float y = _info.overlay.y_shift; y <= VIDEO_STANDARD_RES_HEIGHT; y = y + height) {
                VideoManager->Move(x, y);
                _ambient_overlay_img.Draw();
            }
        }
        VideoManager->PopState();
    }

    // Draw the light overlay
    if(_info.light.active) {
        VideoManager->PushState();
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
        // We use a margin to avoid making the shake effects show unlit parts
        VideoManager->SetCoordSys(0.1f, 0.9f, 0.1f, 0.9f);
        VideoManager->Move(0.0f, 0.0f);
        _light_overlay_img.Draw();
        VideoManager->PopState();
    }

    if(_info.lightning.active)
        _DrawLightning();
}

void EffectSupervisor::DisableEffects()
{
    DisableAmbientOverlay();
    DisableLightingOverlay();
    DisableLightning();
    StopShaking();
}

void EffectSupervisor::ShakeScreen(float force, uint32 falloff_time, ShakeFalloff falloff_method)
{
    if(force < 0.0f) {
        IF_PRINT_WARNING(VIDEO_DEBUG)
                << " was passed a negative force value" << std::endl;
        return;
    }

    if(falloff_method <= SHAKE_FALLOFF_INVALID || falloff_method >= SHAKE_FALLOFF_TOTAL) {
        IF_PRINT_WARNING(VIDEO_DEBUG)
                << " was passed an invalid falloff method" << std::endl;
        return;
    }

    if(falloff_time == 0 && falloff_method != SHAKE_FALLOFF_NONE) {
        IF_PRINT_WARNING(VIDEO_DEBUG)
                << " was called with infinite falloff_time (0), but falloff_method was not VIDEO_FALLOFF_NONE" << std::endl;
        return;
    }

    // Create a new ShakeForce object to represent the shake
    ShakeForce shake;
    shake.current_time  = 0;
    shake.end_time = falloff_time;
    shake.initial_force = force;

    // set up the interpolation
    switch(falloff_method) {
    case SHAKE_FALLOFF_NONE:
        shake.interpolator.SetMethod(INTERPOLATE_SRCA);
        shake.interpolator.Start(force, 0.0f, falloff_time);
        break;

    case SHAKE_FALLOFF_EASE:
        shake.interpolator.SetMethod(INTERPOLATE_EASE);
        shake.interpolator.Start(0.0f, force, falloff_time);
        break;

    case SHAKE_FALLOFF_LINEAR:
        shake.interpolator.SetMethod(INTERPOLATE_LINEAR);
        shake.interpolator.Start(force, 0.0f, falloff_time);
        break;

    case SHAKE_FALLOFF_GRADUAL:
        shake.interpolator.SetMethod(INTERPOLATE_SLOW);
        shake.interpolator.Start(force, 0.0f, falloff_time);
        break;

    case SHAKE_FALLOFF_SUDDEN:
        shake.interpolator.SetMethod(INTERPOLATE_FAST);
        shake.interpolator.Start(force, 0.0f, falloff_time);
        break;

    default:
        IF_PRINT_WARNING(VIDEO_DEBUG)
                << "VIDEO ERROR: falloff method passed to ShakeScreen() was not supported!" << std::endl;
        return;
    };

    // Add the shake force to the list of shakes
    _shake_forces.push_front(shake);
}

/** \brief Rounds a force value to the nearest integer based on probability.
*** \param force  The force to round
*** \return the rounded force value
*** \note For example, a force value of 2.85 has an 85% chance of rounding to 3 and a 15% chance of rounding to 2. This rounding
*** methodology is necessary because for force values less than 1 (e.g. 0.5f), the shake force would always round down to zero
*** even though there is positive force.
**/
static float _RoundForce(float force)
{
    int32 fraction_percent = static_cast<int32>(force * 100.0f) - (static_cast<int32>(force) * 100);

    int32 random_percent = rand() % 100;
    if(fraction_percent > random_percent)
        force = ceilf(force);
    else
        force = floorf(force);

    return force;
}

void EffectSupervisor::_UpdateShake(uint32 frame_time)
{
    const uint32 TIME_BETWEEN_SHAKE_UPDATES = 50;

    if(_shake_forces.empty()) {
        _x_shake = 0;
        _y_shake = 0;
        return;
    }

    static uint32 time_til_next_update = 0; // Used to cap the maximum update frequency
    time_til_next_update += frame_time;

    // Return if not enough time has expired to do a shake update
    if(time_til_next_update < TIME_BETWEEN_SHAKE_UPDATES)
        return;

    // First, update all the shake effects based on the time expired.
    // Then calculate the net force, i.e. the sum of forces for all the shakes
    float net_force = 0.0f;

    // NOTE: time_til_next_update now holds the total update time for ShakeForce and Interpolator to use

    for(std::deque<ShakeForce>::iterator it = _shake_forces.begin(); it != _shake_forces.end();) {
        ShakeForce &shake = *it;
        shake.current_time += time_til_next_update;

        if(shake.end_time != 0 && shake.current_time >= shake.end_time) {
            it = _shake_forces.erase(it);
        } else {
            shake.interpolator.Update(time_til_next_update);
            net_force += shake.interpolator.GetValue();
            ++it;
        }
    }

    time_til_next_update -= TIME_BETWEEN_SHAKE_UPDATES;

    // Calculate random shake offsets using the negative and positive net force values
    // Note that this doesn't produce a radially symmetric distribution of offsets
    _x_shake = _RoundForce(vt_utils::RandomFloat(-net_force, net_force));
    _y_shake = _RoundForce(vt_utils::RandomFloat(-net_force, net_force));
}

} // namespace vt_mode_manager
