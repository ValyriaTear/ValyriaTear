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
*** \file    particle.h
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for particle effects
*** **************************************************************************/

#include "engine/video/particle_effect.h"
#include "engine/video/particle_system.h"

#include "engine/system.h"
#include "engine/video/video.h"
#include "engine/script/script_read.h"

#include "utils/utils_files.h"

// Case-insensitive string compare is called stricmp in Windows and strcasecmp everywhere else
#ifdef _WIN32
#ifndef strcasecmp
#define strcasecmp stricmp
#endif
#endif

using namespace vt_script;
using namespace vt_video;

namespace vt_mode_manager
{

bool ParticleEffect::_LoadEffectDef(const std::string &particle_file)
{
    _effect_def.Clear();
    _loaded = false;

    // Make sure the corresponding tables are empty
    ScriptManager->DropGlobalTable("systems");
    ScriptManager->DropGlobalTable("map_effect_collision");

    vt_script::ReadScriptDescriptor particle_script;
    if(!particle_script.OpenFile(particle_file)) {
        PRINT_WARNING << "No script file: '"
                      << particle_file << "' The corresponding particle effect won't work."
                      << std::endl;
        return false;
    }

    // Read the particle image rectangle when existing
    if (particle_script.OpenTable("map_effect_collision")) {
        _effect_def.effect_collision_width = particle_script.ReadFloat("effect_collision_width");
        _effect_def.effect_collision_height = particle_script.ReadFloat("effect_collision_height");
        particle_script.CloseTable(); // map_effect_collision
    }

    if(!particle_script.DoesTableExist("systems")) {
        PRINT_WARNING << "Could not find the 'systems' array in particle effect "
                      << particle_file << std::endl;
        particle_script.CloseFile();
        _effect_def.Clear();
        _loaded = false;
        return false;
    }

    particle_script.OpenTable("systems");
    uint32 system_size = particle_script.GetTableSize();
    if(system_size < 1) {
        PRINT_WARNING << "No valid particle systems defined in the particle effect "
                      << particle_file << std::endl;
        particle_script.CloseTable();
        particle_script.CloseFile();
        _effect_def.Clear();
        _loaded = false;
        return false;
    }

    for(uint32 sys = 0; sys < system_size; ++sys) {
        // open the table for the sys'th system
        if(!particle_script.DoesTableExist(sys)) {
            PRINT_WARNING << "Could not find system #" << sys
                          << " in particle effect " << particle_file << std::endl;
            particle_script.CloseAllTables();
            particle_script.CloseFile();
            _effect_def.Clear();
            _loaded = false;
            return false;
        }
        particle_script.OpenTable(sys);

        // open up the emitter table
        if(!particle_script.DoesTableExist("emitter")) {
            PRINT_WARNING << "Could not find 'emitter' the array in system #"
                          << sys << " in particle effect " << particle_file << std::endl;
            particle_script.CloseAllTables();
            particle_script.CloseFile();
            _effect_def.Clear();
            _loaded = false;
            return false;
        }
        particle_script.OpenTable("emitter");

        ParticleSystemDef sys_def;

        sys_def.emitter._x = particle_script.ReadFloat("x");
        sys_def.emitter._y = particle_script.ReadFloat("y");
        sys_def.emitter._x2 = particle_script.ReadFloat("x2");
        sys_def.emitter._y2 = particle_script.ReadFloat("y2");
        sys_def.emitter._center_x = particle_script.ReadFloat("center_x");
        sys_def.emitter._center_y = particle_script.ReadFloat("center_y");
        sys_def.emitter._x_variation = particle_script.ReadFloat("x_variation");
        sys_def.emitter._y_variation = particle_script.ReadFloat("y_variation");
        sys_def.emitter._radius = particle_script.ReadFloat("radius");

        std::string shape_string = particle_script.ReadString("shape");
        if(!strcasecmp(shape_string.c_str(), "point"))
            sys_def.emitter._shape = EMITTER_SHAPE_POINT;
        else if(!strcasecmp(shape_string.c_str(), "line"))
            sys_def.emitter._shape = EMITTER_SHAPE_LINE;
        else if(!strcasecmp(shape_string.c_str(), "ellipse outline"))
            sys_def.emitter._shape = EMITTER_SHAPE_ELLIPSE;
        else if(!strcasecmp(shape_string.c_str(), "circle outline"))
            sys_def.emitter._shape = EMITTER_SHAPE_CIRCLE;
        else if(!strcasecmp(shape_string.c_str(), "circle"))
            sys_def.emitter._shape = EMITTER_SHAPE_FILLED_CIRCLE;
        else if(!strcasecmp(shape_string.c_str(), "rectangle"))
            sys_def.emitter._shape = EMITTER_SHAPE_FILLED_RECTANGLE;

        sys_def.emitter._omnidirectional = particle_script.ReadBool("omnidirectional");
        sys_def.emitter._orientation = particle_script.ReadFloat("orientation");
        sys_def.emitter._outer_cone = particle_script.ReadFloat("outer_cone");
        sys_def.emitter._inner_cone = particle_script.ReadFloat("inner_cone");
        sys_def.emitter._initial_speed = particle_script.ReadFloat("initial_speed");
        sys_def.emitter._initial_speed_variation = particle_script.ReadFloat("initial_speed_variation");
        sys_def.emitter._emission_rate = particle_script.ReadFloat("emission_rate");
        sys_def.emitter._start_time = particle_script.ReadFloat("start_time");

        std::string emitter_mode_string = particle_script.ReadString("emitter_mode");
        if(!strcasecmp(emitter_mode_string.c_str(), "looping"))
            sys_def.emitter._emitter_mode = EMITTER_MODE_LOOPING;
        else if(!strcasecmp(emitter_mode_string.c_str(), "one shot"))
            sys_def.emitter._emitter_mode = EMITTER_MODE_ONE_SHOT;
        else if(!strcasecmp(emitter_mode_string.c_str(), "burst"))
            sys_def.emitter._emitter_mode = EMITTER_MODE_BURST;
        else //.. if(!strcasecmp(emitter_mode_string.c_str(), "always"))
            sys_def.emitter._emitter_mode = EMITTER_MODE_ALWAYS;

        std::string spin_string = particle_script.ReadString("spin");
        if(!strcasecmp(spin_string.c_str(), "random"))
            sys_def.emitter._spin = EMITTER_SPIN_RANDOM;
        else if(!strcasecmp(spin_string.c_str(), "counterclockwise"))
            sys_def.emitter._spin = EMITTER_SPIN_COUNTERCLOCKWISE;
        else //..if(!strcasecmp(spin_string.c_str(), "clockwise"))
            sys_def.emitter._spin = EMITTER_SPIN_CLOCKWISE;

        // Close the emitter table
        particle_script.CloseTable();

        // open up the keyframes table
        if(!particle_script.DoesTableExist("keyframes")) {
            PRINT_WARNING << "Could not find the 'keyframes' array in system #"
                          << sys << " in particle effect " << particle_file << std::endl;
            particle_script.CloseAllTables();
            particle_script.CloseFile();
            _effect_def.Clear();
            _loaded = false;
            return false;
        }
        particle_script.OpenTable("keyframes");

        uint32 num_keyframes = particle_script.GetTableSize();
        sys_def.keyframes.resize(num_keyframes);

        for(uint32 kf = 0; kf < num_keyframes; ++kf) {
            // get the kf'th keyframe table
            // unamed tables starts at offset 1 in lua.
            particle_script.OpenTable(kf + 1);

            sys_def.keyframes[kf].size_x = particle_script.ReadFloat("size_x");
            sys_def.keyframes[kf].size_y = particle_script.ReadFloat("size_y");
            sys_def.keyframes[kf].color = _ReadColor(particle_script, "color");
            sys_def.keyframes[kf].rotation_speed = particle_script.ReadFloat("rotation_speed");
            sys_def.keyframes[kf].size_variation_x = particle_script.ReadFloat("size_variation_x");
            sys_def.keyframes[kf].size_variation_y = particle_script.ReadFloat("size_variation_y");
            sys_def.keyframes[kf].color_variation = _ReadColor(particle_script, "color_variation");
            sys_def.keyframes[kf].rotation_speed_variation = particle_script.ReadFloat("rotation_speed_variation");
            sys_def.keyframes[kf].time = particle_script.ReadFloat("time");

            // pop the current keyframe
            particle_script.CloseTable();
        }

        // pop the keyframes table
        particle_script.CloseTable();

        // open up the animation_frames table
        particle_script.ReadStringVector("animation_frames", sys_def.animation_frame_filenames);

        if(sys_def.animation_frame_filenames.size() < 1) {
            PRINT_WARNING << "No animation filenames found while opening particle effect "
                          << particle_file << std::endl;
            particle_script.CloseAllTables();
            particle_script.CloseFile();
            _effect_def.Clear();
            _loaded = false;
            return false;
        }

        particle_script.ReadIntVector("animation_frame_times", sys_def.animation_frame_times);

        // Test each file availability
        std::vector<std::string>::const_iterator it, it_end;
        for(it = sys_def.animation_frame_filenames.begin(),
                it_end = sys_def.animation_frame_filenames.end(); it != it_end; ++it) {
            if(!vt_utils::DoesFileExist(*it)) {
                PRINT_WARNING << "Could not find file: "
                              << *it << " in system #" << sys << " in particle effect "
                              << particle_file << std::endl;
                particle_script.CloseAllTables();
                particle_script.CloseFile();
                _effect_def.Clear();
                _loaded = false;
                return false;
            }
        }

        if(sys_def.animation_frame_times.size() < 1) {
            PRINT_WARNING << "No animation frame times found while opening particle effect "
                          << particle_file << std::endl;
            particle_script.CloseAllTables();
            particle_script.CloseFile();
            _effect_def.Clear();
            _loaded = false;
            return false;
        }

        sys_def.enabled = particle_script.ReadBool("enabled");
        sys_def.blend_mode = particle_script.ReadInt("blend_mode");
        sys_def.system_lifetime = particle_script.ReadFloat("system_lifetime");

        sys_def.particle_lifetime = particle_script.ReadFloat("particle_lifetime");
        sys_def.particle_lifetime_variation = particle_script.ReadFloat("particle_lifetime_variation");
        sys_def.max_particles = particle_script.ReadInt("max_particles");

        sys_def.damping = particle_script.ReadFloat("damping");
        sys_def.damping_variation = particle_script.ReadFloat("damping_variation");

        sys_def.acceleration_x = particle_script.ReadFloat("acceleration_x");
        sys_def.acceleration_y = particle_script.ReadFloat("acceleration_y");
        sys_def.acceleration_variation_x = particle_script.ReadFloat("acceleration_variation_x");
        sys_def.acceleration_variation_y = particle_script.ReadFloat("acceleration_variation_y");

        sys_def.wind_velocity_x = particle_script.ReadFloat("wind_velocity_x");
        sys_def.wind_velocity_y = particle_script.ReadFloat("wind_velocity_y");
        sys_def.wind_velocity_variation_x = particle_script.ReadFloat("wind_velocity_variation_x");
        sys_def.wind_velocity_variation_y = particle_script.ReadFloat("wind_velocity_variation_y");

        if (particle_script.OpenTable("wave_motion")) {

            sys_def.wave_motion_used = true;
            sys_def.wave_length = particle_script.ReadFloat("wave_length");
            sys_def.wave_length_variation = particle_script.ReadFloat("wave_length_variation");
            sys_def.wave_amplitude = particle_script.ReadFloat("wave_amplitude");
            sys_def.wave_amplitude_variation = particle_script.ReadFloat("wave_amplitude_variation");

            particle_script.CloseTable();
        }
        else {
            sys_def.wave_motion_used = false;
        }

        sys_def.tangential_acceleration = particle_script.ReadFloat("tangential_acceleration");
        sys_def.tangential_acceleration_variation = particle_script.ReadFloat("tangential_acceleration_variation");

        sys_def.radial_acceleration = particle_script.ReadFloat("radial_acceleration");
        sys_def.radial_acceleration_variation = particle_script.ReadFloat("radial_acceleration_variation");

        sys_def.user_defined_attractor = particle_script.ReadBool("user_defined_attractor");
        sys_def.attractor_falloff = particle_script.ReadFloat("attractor_falloff");

        if (particle_script.OpenTable("rotation")) {
            sys_def.rotation_used = true;

            if (particle_script.OpenTable("rotate_to_velocity")) {

                sys_def.rotate_to_velocity = true;

                if (particle_script.DoesFloatExist("speed_scale")) {
                    sys_def.speed_scale_used = true;

                    sys_def.speed_scale = particle_script.ReadFloat("speed_scale");
                    sys_def.min_speed_scale = particle_script.ReadFloat("min_speed_scale");
                    sys_def.max_speed_scale = particle_script.ReadFloat("max_speed_scale");

                }
                else {
                    sys_def.speed_scale_used = false;
                }

                particle_script.CloseTable(); // rotate_to_velocity
            }
            else {
                sys_def.rotate_to_velocity = false;
            }

            particle_script.CloseTable(); // rotation
        }
        else {
            sys_def.rotation_used = false;
        }

        sys_def.smooth_animation = particle_script.ReadBool("smooth_animation");
        sys_def.modify_stencil = particle_script.ReadBool("modify_stencil");

        std::string stencil_op_string = particle_script.ReadString("stencil_op");

        if(!strcasecmp(stencil_op_string.c_str(), "incr"))
            sys_def.stencil_op = VIDEO_STENCIL_OP_INCREASE;
        else if(!strcasecmp(stencil_op_string.c_str(), "decr"))
            sys_def.stencil_op = VIDEO_STENCIL_OP_DECREASE;
        else if(!strcasecmp(stencil_op_string.c_str(), "zero"))
            sys_def.stencil_op = VIDEO_STENCIL_OP_ZERO;
        else //..if(!strcasecmp(stencil_op_string.c_str(), "one"))
            sys_def.stencil_op = VIDEO_STENCIL_OP_ONE;

        sys_def.use_stencil = particle_script.ReadBool("use_stencil");
        sys_def.random_initial_angle = particle_script.ReadBool("random_initial_angle");

        // pop the system table
        particle_script.CloseTable();

        _effect_def._systems.push_back(sys_def);
    }

    _loaded = true;
    return true;
}

// A helper function reading a lua subtable of 4 float values.
Color ParticleEffect::_ReadColor(vt_script::ReadScriptDescriptor &particle_script,
                                 const std::string &param_name)
{
    std::vector<float> float_vec;
    particle_script.ReadFloatVector(param_name, float_vec);
    if(float_vec.size() < 4) {
        PRINT_WARNING << "Invalid color read in parameter: " << param_name
                      << " for file: " << particle_script.GetFilename() << std::endl;
        return Color();
    }
    Color new_color(float_vec[0], float_vec[1], float_vec[2], float_vec[3]);

    return new_color;
}

bool ParticleEffect::_CreateEffect()
{
    // The effect isn't loaded, so we can't create the effect.
    if(!IsLoaded())
        return false;

    // Initialize systems
    _systems.clear();
    std::vector<ParticleSystemDef>::iterator it = _effect_def._systems.begin();
    for(; it != _effect_def._systems.end(); ++it) {
        if((*it).enabled) {
            ParticleSystem sys(&(*it));
            if(!sys.IsAlive()) {
                // If a system could not be created then we bail out
                _systems.clear();

                IF_PRINT_WARNING(VIDEO_DEBUG)
                        << "sys->Create() returned false while trying to create effect!" << std::endl;
                return false;

            }
            _systems.push_back(sys);
        }
    }

    _alive = true;
    _age = 0.0f;
    return true;
}

bool ParticleEffect::LoadEffect(const std::string &filename)
{
    if(!_LoadEffectDef(filename)) {
        PRINT_WARNING << "Failed to load particle definition file: "
                      << filename << std::endl;
        return false;
    }

    if(!_CreateEffect()) {
        PRINT_WARNING << "Failed to create particle effect from file: "
                      << filename << std::endl;
        return false;
    }

    return true;
}

bool ParticleEffect::Draw()
{
    bool success = true;

    // move to the effect's location
    VideoManager->Move(_x, _y);

    std::vector<ParticleSystem>::iterator iSystem = _systems.begin();

    while(iSystem != _systems.end()) {
        VideoManager->PushMatrix();
        if(!(*iSystem).Draw()) {
            success = false;
            IF_PRINT_WARNING(VIDEO_DEBUG)
                    << "Failed to draw system!" << std::endl;
        }

        VideoManager->PopMatrix();
        ++iSystem;
    }

    VideoManager->DisableAlphaTest();
    VideoManager->DisableStencilTest();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    return success;
}

bool ParticleEffect::Update()
{
    return Update(static_cast<float>(vt_system::SystemManager->GetUpdateTime()) / 1000.0f);
}

bool ParticleEffect::Update(float frame_time)
{
    _age += frame_time;
    _num_particles = 0;

    if(!_alive)
        return true;

    bool success = true;

    vt_mode_manager::EffectParameters effect_parameters;
    effect_parameters.orientation = _orientation;

    // note we subtract the effect position to put the attractor point in effect
    // space instead of screen space
    effect_parameters.attractor_x = _attractor_x - _x;
    effect_parameters.attractor_y = _attractor_y - _y;

    std::vector<ParticleSystem>::iterator iSystem = _systems.begin();

    while(iSystem != _systems.end()) {
        if(!(*iSystem).IsAlive()) {
            iSystem = _systems.erase(iSystem);

            if(_systems.empty())
                _alive = false;
        } else {
            if(!(*iSystem).Update(frame_time, effect_parameters)) {
                success = false;
                IF_PRINT_WARNING(VIDEO_DEBUG)
                        << "Failed to update system!" << std::endl;
            }

            _num_particles += (*iSystem).GetNumParticles();
            ++iSystem;
        }
    }

    return success;
}


void ParticleEffect::_Destroy()
{
    _alive = false;
    _x = 0.0f;
    _y = 0.0f;
    _attractor_x = 0.0f;
    _attractor_y = 0.0f;
    _age = 0.0f;
    _orientation = 0.0f;

    _systems.clear();

    _loaded = false;
}


void ParticleEffect::Move(float x, float y)
{
    _x = x;
    _y = y;
}

void ParticleEffect::MoveRelative(float dx, float dy)
{
    _x += dx;
    _y += dy;
}

void ParticleEffect::SetAttractorPoint(float x, float y)
{
    _attractor_x = x;
    _attractor_y = y;
}

void ParticleEffect::Stop(bool kill_immediate)
{
    if(kill_immediate) {
        _alive = false;
        _systems.clear();
    } else {
        // if we're not killing immediately, then calling Stop() just means to stop emitting NEW
        // particles, so go through each system and turn off its emitter
        std::vector<ParticleSystem>::iterator iSystem = _systems.begin();

        while(iSystem != _systems.end()) {
            (*iSystem).Stop();
            ++iSystem;
        }
    }
}

bool ParticleEffect::Start()
{
    if(IsAlive())
        return true;

    return _CreateEffect();
}

void ParticleEffect::GetPosition(float &x, float &y) const
{
    x = _x;
    y = _y;
}

} // namespace vt_mode_manager
