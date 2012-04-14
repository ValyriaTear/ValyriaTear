///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include <iostream>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "engine/video/video.h"
#include "engine/script/script_read.h"

#include "engine/video/particle_manager.h"
#include "engine/video/particle_effect.h"
#include "engine/video/particle_system.h"
#include "engine/video/particle_keyframe.h"

using namespace std;
using namespace hoa_script;
using namespace hoa_video;

namespace hoa_mode_manager
{

ParticleEffectID ParticleManager::AddParticleEffect(const string &filename, float x, float y)
{
	ParticleEffectDef *def = _LoadEffect(filename);

	if(!def) {
		PRINT_WARNING << "Failed to load particle definition file: " << filename << endl;
		return VIDEO_INVALID_EFFECT;
	}

	ParticleEffectID id = _AddEffect(def, x, y);
	if(id == VIDEO_INVALID_EFFECT) {
		PRINT_WARNING << "Failed to add effect to particle manager from: " << filename << endl;
	}

	return id;
}


ParticleEffectDef *ParticleManager::_LoadEffect(const std::string &particle_file)
{
	hoa_script::ReadScriptDescriptor particle_script;
	if (!particle_script.OpenFile(particle_file)) {
		PRINT_WARNING << "No script file: '"
			<< particle_file << "' The corresponding particle effect won't work." << endl;
		return NULL;
	}

	if (!particle_script.DoesTableExist("systems")) {
		PRINT_WARNING << "Could not find the 'systems' array in particle effect " << particle_file << endl;
		particle_script.CloseFile();
		return NULL;
	}

	particle_script.OpenTable("systems");
	uint32 system_size = particle_script.GetTableSize();
	if(system_size < 1)
	{
		PRINT_WARNING << "No valid particle systems defined in the particle effect " << particle_file << endl;
		particle_script.CloseTable();
		particle_script.CloseFile();
		return NULL;
	}

	ParticleEffectDef *def = new ParticleEffectDef;

	for(uint32 sys = 0; sys < system_size; ++sys)
	{
		// open the table for the sys'th system
		if (!particle_script.DoesTableExist(sys)) {
			PRINT_WARNING << "Could not find system #" << sys << " in particle effect " << particle_file << endl;
			delete def;
			particle_script.CloseAllTables();
			particle_script.CloseFile();
			return NULL;
		}
		particle_script.OpenTable(sys);

		// open up the emitter table
		if (!particle_script.DoesTableExist("emitter")) {
			PRINT_WARNING << "Could not find 'emitter' the array in system #" << sys << " in particle effect " << particle_file << endl;
			delete def;
			particle_script.CloseAllTables();
			particle_script.CloseFile();
			return NULL;
		}
		particle_script.OpenTable("emitter");

		ParticleSystemDef *sys_def = new ParticleSystemDef;

		sys_def->emitter._x = particle_script.ReadFloat("x");
		sys_def->emitter._y = particle_script.ReadFloat("y");
		sys_def->emitter._x2 = particle_script.ReadFloat("x2");
		sys_def->emitter._y2 = particle_script.ReadFloat("y2");
		sys_def->emitter._center_x = particle_script.ReadFloat("center_x");
		sys_def->emitter._center_y = particle_script.ReadFloat("center_y");
		sys_def->emitter._x_variation = particle_script.ReadFloat("x_variation");
		sys_def->emitter._y_variation = particle_script.ReadFloat("y_variation");
		sys_def->emitter._radius = particle_script.ReadFloat("radius");

		string shape_string = particle_script.ReadString("shape");
		if(!strcasecmp(shape_string.c_str(), "point"))
			sys_def->emitter._shape = EMITTER_SHAPE_POINT;
		else if(!strcasecmp(shape_string.c_str(), "line"))
			sys_def->emitter._shape = EMITTER_SHAPE_LINE;
		else if(!strcasecmp(shape_string.c_str(), "circle outline"))
			sys_def->emitter._shape = EMITTER_SHAPE_CIRCLE;
		else if(!strcasecmp(shape_string.c_str(), "circle"))
			sys_def->emitter._shape = EMITTER_SHAPE_FILLED_CIRCLE;
		else if(!strcasecmp(shape_string.c_str(), "rectangle"))
			sys_def->emitter._shape = EMITTER_SHAPE_FILLED_RECTANGLE;

		sys_def->emitter._omnidirectional = particle_script.ReadBool("omnidirectional");
		sys_def->emitter._orientation = particle_script.ReadFloat("orientation");
		sys_def->emitter._outer_cone = particle_script.ReadFloat("outer_cone");
		sys_def->emitter._inner_cone = particle_script.ReadFloat("inner_cone");
		sys_def->emitter._initial_speed = particle_script.ReadFloat("initial_speed");
		sys_def->emitter._initial_speed_variation = particle_script.ReadFloat("initial_speed_variation");
		sys_def->emitter._emission_rate = particle_script.ReadFloat("emission_rate");
		sys_def->emitter._start_time = particle_script.ReadFloat("start_time");

		string emitter_mode_string = particle_script.ReadString("emitter_mode");
		if(!strcasecmp(emitter_mode_string.c_str(), "looping"))
			sys_def->emitter._emitter_mode = EMITTER_MODE_LOOPING;
		else if(!strcasecmp(emitter_mode_string.c_str(), "one shot"))
			sys_def->emitter._emitter_mode = EMITTER_MODE_ONE_SHOT;
		else if(!strcasecmp(emitter_mode_string.c_str(), "burst"))
			sys_def->emitter._emitter_mode = EMITTER_MODE_BURST;
		else //.. if(!strcasecmp(emitter_mode_string.c_str(), "always"))
			sys_def->emitter._emitter_mode = EMITTER_MODE_ALWAYS;

		string spin_string = particle_script.ReadString("spin");
		if(!strcasecmp(spin_string.c_str(), "random"))
			sys_def->emitter._spin = EMITTER_SPIN_RANDOM;
		else if(!strcasecmp(spin_string.c_str(), "counterclockwise"))
			sys_def->emitter._spin = EMITTER_SPIN_COUNTERCLOCKWISE;
		else //..if(!strcasecmp(spin_string.c_str(), "clockwise"))
			sys_def->emitter._spin = EMITTER_SPIN_CLOCKWISE;

		// Close the emitter table
		particle_script.CloseTable();

		// open up the keyframes table
		if (!particle_script.DoesTableExist("keyframes")) {
			PRINT_WARNING << "Could not find the 'keyframes' array in system #" << sys << " in particle effect " << particle_file << endl;
			delete def;
			particle_script.CloseAllTables();
			particle_script.CloseFile();
			return NULL;
		}
		particle_script.OpenTable("keyframes");

		uint32 num_keyframes = particle_script.GetTableSize();
		sys_def->keyframes.resize(num_keyframes);

		for(uint32 kf = 0; kf < num_keyframes; ++kf)
		{
			sys_def->keyframes[kf] = new ParticleKeyframe;

			// get the kf'th keyframe table
			// unamed tables starts at offset 1 in lua.
			particle_script.OpenTable(kf + 1);

			sys_def->keyframes[kf]->size_x = particle_script.ReadFloat("size_x");
			sys_def->keyframes[kf]->size_y = particle_script.ReadFloat("size_y");
			sys_def->keyframes[kf]->color = _ReadColor(particle_script, "color");
			sys_def->keyframes[kf]->rotation_speed = particle_script.ReadFloat("rotation_speed");
			sys_def->keyframes[kf]->size_variation_x = particle_script.ReadFloat("size_variation_x");
			sys_def->keyframes[kf]->size_variation_y = particle_script.ReadFloat("size_variation_y");
			sys_def->keyframes[kf]->color_variation = _ReadColor(particle_script, "color_variation");
			sys_def->keyframes[kf]->rotation_speed_variation = particle_script.ReadFloat("rotation_speed_variation");
			sys_def->keyframes[kf]->time = particle_script.ReadFloat("time");

			// pop the current keyframe
			particle_script.CloseTable();
		}

		// pop the keyframes table
		particle_script.CloseTable();

		// open up the animation_frames table
		particle_script.ReadStringVector("animation_frames", sys_def->animation_frame_filenames);

		if(sys_def->animation_frame_filenames.size() < 1)
		{
			PRINT_WARNING << "No animation filenames found while opening particle effect " << particle_file << endl;
			delete def;
			particle_script.CloseAllTables();
			particle_script.CloseFile();
			return NULL;
		}

		particle_script.ReadIntVector("animation_frame_times", sys_def->animation_frame_times);


		// Test each file availability
		std::vector<std::string>::const_iterator it, it_end;
		for (it = sys_def->animation_frame_filenames.begin(),
			it_end = sys_def->animation_frame_filenames.end(); it != it_end; ++it) {
			if (!hoa_utils::DoesFileExist(*it)) {
				PRINT_WARNING << "Could not find file: "
					<< *it << " in system #" << sys << " in particle effect "
					<< particle_file << endl;
				delete def;
				particle_script.CloseAllTables();
				particle_script.CloseFile();
				return NULL;
			}
		}

		if(sys_def->animation_frame_times.size() < 1)
		{
			PRINT_WARNING << "No animation frame times found while opening particle effect " << particle_file << endl;
			delete def;
			particle_script.CloseAllTables();
			particle_script.CloseFile();
			return NULL;
		}

		sys_def->enabled = particle_script.ReadBool("enabled");
		sys_def->blend_mode = particle_script.ReadInt("blend_mode");
		sys_def->system_lifetime = particle_script.ReadFloat("system_lifetime");

		sys_def->particle_lifetime = particle_script.ReadFloat("particle_lifetime");
		sys_def->particle_lifetime_variation = particle_script.ReadFloat("particle_lifetime_variation");
		sys_def->max_particles = particle_script.ReadInt("max_particles");

		sys_def->damping = particle_script.ReadFloat("damping");
		sys_def->damping_variation = particle_script.ReadFloat("damping_variation");

		sys_def->acceleration_x = particle_script.ReadFloat("acceleration_x");
		sys_def->acceleration_y = particle_script.ReadFloat("acceleration_y");
		sys_def->acceleration_variation_x = particle_script.ReadFloat("acceleration_variation_x");
		sys_def->acceleration_variation_y = particle_script.ReadFloat("acceleration_variation_y");

		sys_def->wind_velocity_x = particle_script.ReadFloat("wind_velocity_x");
		sys_def->wind_velocity_y = particle_script.ReadFloat("wind_velocity_y");
		sys_def->wind_velocity_variation_x = particle_script.ReadFloat("wind_velocity_variation_x");
		sys_def->wind_velocity_variation_y = particle_script.ReadFloat("wind_velocity_variation_y");

		sys_def->wave_motion_used = particle_script.ReadBool("wave_motion_used");
		sys_def->wave_length = particle_script.ReadFloat("wave_length");
		sys_def->wave_length_variation = particle_script.ReadFloat("wave_length_variation");
		sys_def->wave_amplitude = particle_script.ReadFloat("wave_amplitude");
		sys_def->wave_amplitude_variation = particle_script.ReadFloat("wave_amplitude_variation");

		sys_def->tangential_acceleration = particle_script.ReadFloat("tangential_acceleration");
		sys_def->tangential_acceleration_variation = particle_script.ReadFloat("tangential_acceleration_variation");

		sys_def->radial_acceleration = particle_script.ReadFloat("radial_acceleration");
		sys_def->radial_acceleration_variation = particle_script.ReadFloat("radial_acceleration_variation");

		sys_def->user_defined_attractor = particle_script.ReadBool("user_defined_attractor");
		sys_def->attractor_falloff = particle_script.ReadFloat("attractor_falloff");

		sys_def->rotation_used = particle_script.ReadBool("rotation_used");
		sys_def->rotate_to_velocity = particle_script.ReadBool("rotate_to_velocity");

		sys_def->speed_scale_used = particle_script.ReadBool("speed_scale_used");
		sys_def->speed_scale = particle_script.ReadFloat("speed_scale");
		sys_def->min_speed_scale = particle_script.ReadFloat("min_speed_scale");
		sys_def->max_speed_scale = particle_script.ReadFloat("max_speed_scale");

		sys_def->smooth_animation = particle_script.ReadBool("smooth_animation");
		sys_def->modify_stencil = particle_script.ReadBool("modify_stencil");

		string stencil_op_string = particle_script.ReadString("stencil_op");

		if(!strcasecmp(stencil_op_string.c_str(), "incr"))
			sys_def->stencil_op = VIDEO_STENCIL_OP_INCREASE;
		else if(!strcasecmp(stencil_op_string.c_str(), "decr"))
			sys_def->stencil_op = VIDEO_STENCIL_OP_DECREASE;
		else if(!strcasecmp(stencil_op_string.c_str(), "zero"))
			sys_def->stencil_op = VIDEO_STENCIL_OP_ZERO;
		else //..if(!strcasecmp(stencil_op_string.c_str(), "one"))
			sys_def->stencil_op = VIDEO_STENCIL_OP_ONE;

		sys_def->use_stencil = particle_script.ReadBool("use_stencil");
		sys_def->random_initial_angle = particle_script.ReadBool("random_initial_angle");

		// pop the system table
		particle_script.CloseTable();

		def->_systems.push_back(sys_def);
	}

	return def;
}


ParticleEffectID ParticleManager::_AddEffect(const ParticleEffectDef *def, float x, float y)
{
	if(!def)
	{
		IF_PRINT_WARNING(VIDEO_DEBUG)
			<< "AddEffect() failed because def was NULL!" << endl;
		return VIDEO_INVALID_EFFECT;
	}

	if(def->_systems.empty())
	{
		IF_PRINT_WARNING(VIDEO_DEBUG)
			<< "AddEffect() failed because def->_systems was empty!" << endl;
		return VIDEO_INVALID_EFFECT;
	}

	ParticleEffect *effect = _CreateEffect(def);
	if(!effect)
	{
		IF_PRINT_WARNING(VIDEO_DEBUG)
			<< "Could not create particle effect!" << endl;
		return VIDEO_INVALID_EFFECT;
	}

	effect->Move(x, y);
	_effects.push_back(effect);

	return _effects.size() - 1;
};


void ParticleManager::_DEBUG_ShowParticleStats() {
	char text[50];
	sprintf(text, "Particles: %d", GetNumParticles());

	VideoManager->Move(896.0f, 690.0f);
	TextManager->Draw(text);
}


bool ParticleManager::Draw()
{
	VideoManager->PushState();
	VideoManager->SetStandardCoordSys();
	VideoManager->DisableScissoring();

	std::vector<ParticleEffect *>::iterator iEffect = _effects.begin();

	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);

	bool success = true;

	while(iEffect != _effects.end())
	{
		if(!(*iEffect)->_Draw())
		{
			success = false;
			IF_PRINT_WARNING(VIDEO_DEBUG)
				<< "Effect failed to draw!" << endl;
		}
		++iEffect;
	}

	VideoManager->PopState();
	return success;
}


bool ParticleManager::Update(int32 frame_time)
{
	float frame_time_seconds = static_cast<float>(frame_time) / 1000.0f;

	std::vector<ParticleEffect *>::iterator iEffect = _effects.begin();

	bool success = true;

	_num_particles = 0;

	while(iEffect != _effects.end())
	{
		if(!(*iEffect)->IsAlive())
		{
			_effects.erase(iEffect++);
		}
		else
		{
			if(!(*iEffect)->_Update(frame_time_seconds))
			{
				success = false;
				IF_PRINT_WARNING(VIDEO_DEBUG)
					<< "Effect failed to update!" << endl;
			}

			_num_particles += (*iEffect)->GetNumParticles();
			++iEffect;
		}
	}

	return success;
}


void ParticleManager::StopAll(bool kill_immediate)
{
	std::vector<ParticleEffect *>::iterator iEffect = _effects.begin();

	while(iEffect != _effects.end())
	{
		(*iEffect)->Stop(kill_immediate);
		++iEffect;
	}
}


void ParticleManager::_Destroy() {
	std::vector<ParticleEffect *>::iterator iEffect = _effects.begin();

	while(iEffect != _effects.end())
	{
		(*iEffect)->_Destroy();
		delete (*iEffect);
		++iEffect;
	}

	_effects.clear();
}


ParticleEffect *ParticleManager::_GetEffect(ParticleEffectID id) {
	if (id > -1 && id < (int32)_effects.size())
		return _effects[id];

	return NULL;
}


int32 ParticleManager::GetNumParticles()
{
	return _num_particles;
}


// A helper function reading a lua subtable of 4 float values.
Color ParticleManager::_ReadColor(hoa_script::ReadScriptDescriptor& particle_script,
						   std::string param_name) {
	std::vector<float> float_vec;
	particle_script.ReadFloatVector(param_name, float_vec);
	if (float_vec.size() < 4) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "Invalid color read" << endl;
		return Color();
	}
	Color new_color(float_vec[0], float_vec[1], float_vec[2], float_vec[3]);

	return new_color;
}


ParticleEffect *ParticleManager::_CreateEffect(const ParticleEffectDef *def)
{
	std::list<ParticleSystemDef *>::const_iterator iSystem = def->_systems.begin();
	std::list<ParticleSystemDef *>::const_iterator iEnd = def->_systems.end();

	ParticleEffect *effect = new ParticleEffect;

	// store pointer of effect properties
	effect->_effect_def = def;

	// initialize systems
	while(iSystem != iEnd)
	{
		if((*iSystem)->enabled)
		{
			ParticleSystem *sys = new ParticleSystem;
			if(!sys->Create(*iSystem))
			{
				// if a system could not be created then we bail out
				sys->Destroy();
				delete sys;

				list<ParticleSystem *>::iterator iEffectSystem = effect->_systems.begin();
				list<ParticleSystem *>::iterator iEffectEnd = effect->_systems.end();

				while(iEffectSystem != iEffectEnd)
				{
					(*iEffectSystem)->Destroy();
					delete (*iEffectSystem);
					++iEffectSystem;
				}

				IF_PRINT_WARNING(VIDEO_DEBUG)
					<< "sys->Create() returned false while trying to create effect!" << endl;
				return NULL;

			}
			effect->_systems.push_back(sys);
		}
		++iSystem;
	}

	effect->_alive = true;
	effect->_age = 0.0f;
	return effect;
}

}  // namespace hoa_video
