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

#include "video.h"
#include "script.h"

#include "particle_manager.h"
#include "particle_effect.h"
#include "particle_system.h"
#include "particle_keyframe.h"

using namespace std;
using namespace hoa_script;
using namespace hoa_video;


#define LOAD_INT(str, var)  \
{ \
	lua_pushstring(L, str); \
	lua_gettable(L, -2); \
	if(!lua_isnumber(L, -1))  \
	{\
		if(VIDEO_DEBUG) \
			cerr << "VIDEO ERROR: could not load parameter " << str << " in ParticleManager::LoadEffect()!" << endl; \
		return false; \
	}\
	(var) = (int32)lua_tonumber(L, -1);\
	lua_pop(L, 1); \
}


#define LOAD_FLOAT(str, var)  \
{ \
	lua_pushstring(L, str); \
	lua_gettable(L, -2); \
	if(!lua_isnumber(L, -1))  \
	{\
		if(VIDEO_DEBUG) \
			cerr << "VIDEO ERROR: could not load parameter " << str << " in ParticleManager::LoadEffect()!" << endl; \
		return false; \
	}\
	(var) = (float)lua_tonumber(L, -1);\
	lua_pop(L, 1); \
}


#define LOAD_BOOL(str, var)  \
{ \
	lua_pushstring(L, str); \
	lua_gettable(L, -2); \
	if(!lua_isnumber(L, -1))  \
	{\
		if(VIDEO_DEBUG) \
			cerr << "VIDEO ERROR: could not load parameter " << str << " in ParticleManager::LoadEffect()!" << endl; \
		return false; \
	}\
	(var) = (bool)lua_tonumber(L, -1);\
	lua_pop(L, 1); \
}


#define LOAD_STRING(str, var)  \
{ \
	lua_pushstring(L, str); \
	lua_gettable(L, -2); \
	if(!lua_isstring(L, -1))  \
	{\
		if(VIDEO_DEBUG) \
			cerr << "VIDEO ERROR: could not load parameter " << str << " in ParticleManager::LoadEffect()!" << endl; \
		return false; \
	}\
	var = string(lua_tostring(L, -1));\
	lua_pop(L, 1); \
}


#define LOAD_COLOR(str, var)  \
{ \
	lua_pushstring(L, str); \
	lua_gettable(L, -2); \
	if(!lua_istable(L, -1))  \
	{\
		if(VIDEO_DEBUG) \
			cerr << "VIDEO ERROR: could not load parameter " << str << " in ParticleManager::LoadEffect()!" << endl; \
		return false; \
	}\
	int32 num_color_components = lua_objlen(L, -1); \
	if(num_color_components != 4)\
	{\
		if(VIDEO_DEBUG) \
			cerr << "VIDEO ERROR: wrong number of components while loading color " << str << " in ParticleManager::LoadEffect()!" << endl; \
		return false; \
	}\
	for(int32 cmp = 1; cmp <= 4; ++cmp) \
	{\
		lua_rawgeti(L, -1, cmp);\
		if(!lua_isnumber(L, -1)) \
		{\
			if(VIDEO_DEBUG) \
				cerr << "VIDEO ERROR: lua_isnumber() returned false while trying to load " << str << " in ParticleManager::LoadEffect()!" << endl; \
			return false; \
		}\
		var[cmp-1] = (float)lua_tonumber(L, -1);\
		lua_pop(L, 1);\
	}\
	lua_pop(L, 1); \
}



namespace hoa_video
{

namespace private_video
{


//-----------------------------------------------------------------------------
// Loads the effect, returns false on failure. This is a temporary function, until
// the ScriptEngine code is finished
//-----------------------------------------------------------------------------

bool TEMP_LoadEffectHelper(const string &filename, lua_State *L, ParticleEffectDef *def)
{
	if(!L)
	{
		if(VIDEO_DEBUG)
			cerr << "VIDEO ERROR: lua_open() failed in ParticleManager::LoadEffect()!" << endl;
		return false;
	}

	luaopen_base(L);
	luaopen_io(L);
	luaopen_string(L);
	luaopen_math(L);

	if(luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0))
	{
		cerr << "VIDEO ERROR: could not load particle effect " << filename << " :: " << lua_tostring(L, -1) << endl;
		return false;
	}


	lua_getglobal(L, "systems");
	if(!lua_istable(L, -1))
	{
		if(VIDEO_DEBUG)
			cerr << "VIDEO ERROR: could not find 'systems' in particle effect " << filename << endl;
		return false;
	}


	int32 num_systems = lua_objlen(L, -1);

	if(num_systems < 1)
	{
		if(VIDEO_DEBUG)
			cerr << "VIDEO ERROR: num_systems less than 1 while opening particle effect " << filename << endl;
		return false;
	}

	for(int32 sys = 1; sys <= num_systems; ++sys)
	{
		// open the table for the sys'th system
		lua_rawgeti(L, -1, sys);
		if(!lua_istable(L, -1))
		{
			if(VIDEO_DEBUG)
				cerr << "VIDEO ERROR: could not find system #" << sys << " in particle effect " << filename << endl;
			return false;
		}

		// open up the emitter table
		lua_pushstring(L, "emitter");
		lua_gettable(L, -2);
		if(!lua_istable(L, -1))
		{
			if(VIDEO_DEBUG)
				cerr << "VIDEO ERROR: could not find emitter in system #" << sys << " in particle effect " << filename << endl;
			return false;
		}

		ParticleSystemDef *sys_def = new ParticleSystemDef;

		LOAD_FLOAT("x", sys_def->emitter._x);
		LOAD_FLOAT("y", sys_def->emitter._y);
		LOAD_FLOAT("x2", sys_def->emitter._x2);
		LOAD_FLOAT("y2", sys_def->emitter._y2);
		LOAD_FLOAT("center_x", sys_def->emitter._center_x);
		LOAD_FLOAT("center_y", sys_def->emitter._center_y);
		LOAD_FLOAT("x_variation", sys_def->emitter._x_variation);
		LOAD_FLOAT("y_variation", sys_def->emitter._y_variation);
		LOAD_FLOAT("radius", sys_def->emitter._radius);

		string shape_string;
		LOAD_STRING("shape", shape_string);

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

		LOAD_BOOL("omnidirectional", sys_def->emitter._omnidirectional);
		LOAD_FLOAT("orientation", sys_def->emitter._orientation);
		LOAD_FLOAT("outer_cone", sys_def->emitter._outer_cone);
		LOAD_FLOAT("inner_cone", sys_def->emitter._inner_cone);
		LOAD_FLOAT("initial_speed", sys_def->emitter._initial_speed);
		LOAD_FLOAT("initial_speed_variation", sys_def->emitter._initial_speed_variation);
		LOAD_FLOAT("emission_rate", sys_def->emitter._emission_rate);
		LOAD_FLOAT("start_time", sys_def->emitter._start_time);

		string emitter_mode_string;
		LOAD_STRING("emitter_mode", emitter_mode_string);

		if(!strcasecmp(emitter_mode_string.c_str(), "looping"))
			sys_def->emitter._emitter_mode = EMITTER_MODE_LOOPING;
		else if(!strcasecmp(emitter_mode_string.c_str(), "one shot"))
			sys_def->emitter._emitter_mode = EMITTER_MODE_ONE_SHOT;
		else if(!strcasecmp(emitter_mode_string.c_str(), "burst"))
			sys_def->emitter._emitter_mode = EMITTER_MODE_BURST;
		else //.. if(!strcasecmp(emitter_mode_string.c_str(), "always"))
			sys_def->emitter._emitter_mode = EMITTER_MODE_ALWAYS;

		string spin_string;
		LOAD_STRING("spin", spin_string);

		if(!strcasecmp(spin_string.c_str(), "random"))
			sys_def->emitter._spin = EMITTER_SPIN_RANDOM;
		else if(!strcasecmp(spin_string.c_str(), "counterclockwise"))
			sys_def->emitter._spin = EMITTER_SPIN_COUNTERCLOCKWISE;
		else //..if(!strcasecmp(spin_string.c_str(), "clockwise"))
			sys_def->emitter._spin = EMITTER_SPIN_CLOCKWISE;

		// OK! done loading the emitter data. Now, load the keyframes

		lua_pop(L, 1);   // pop the emitter table
		lua_pushstring(L, "keyframes");
		lua_gettable(L, -2);

		if(!lua_istable(L, -1))
		{
			if(VIDEO_DEBUG)
				cerr << "VIDEO ERROR: could not locate keyframes while loading particle effect " << filename << endl;
			return false;
		}

		int32 num_keyframes = lua_objlen(L, -1);

		sys_def->keyframes.resize(num_keyframes);


		for(int32 kf = 0; kf < num_keyframes; ++kf)
		{
			sys_def->keyframes[kf] = new ParticleKeyframe;

			// get the kf'th keyframe table
			lua_rawgeti(L, -1, kf+1);

			LOAD_FLOAT("size_x", sys_def->keyframes[kf]->size_x);
			LOAD_FLOAT("size_y", sys_def->keyframes[kf]->size_y);
			LOAD_COLOR("color", sys_def->keyframes[kf]->color);
			LOAD_FLOAT("rotation_speed", sys_def->keyframes[kf]->rotation_speed);
			LOAD_FLOAT("size_variation_x", sys_def->keyframes[kf]->size_variation_x);
			LOAD_FLOAT("size_variation_y", sys_def->keyframes[kf]->size_variation_y);
			LOAD_COLOR("color_variation", sys_def->keyframes[kf]->color_variation);
			LOAD_FLOAT("rotation_speed_variation", sys_def->keyframes[kf]->rotation_speed_variation);
			LOAD_FLOAT("time", sys_def->keyframes[kf]->time);

			// pop the current keyframe
			lua_pop(L, 1);
		}

		// pop the keyframes table
		lua_pop(L, 1);

		lua_pushstring(L, "animation_frames");
		lua_gettable(L, -2);

		if(!lua_istable(L, -1))
		{
			if(VIDEO_DEBUG)
				cerr << "VIDEO ERROR: could not locate 'animation_frames' in the effect file " << filename << endl;
			return false;
		}

		int32 num_animation_frames = lua_objlen(L, -1);

		if(num_animation_frames < 1)
		{
			if(VIDEO_DEBUG)
				cerr << "VIDEO ERROR: the 'animation_frames' table was empty in effect file " << filename << endl;
			return false;
		}

		for(int32 n_frame = 0; n_frame < num_animation_frames; ++n_frame)
		{
			lua_rawgeti(L, -1, n_frame+1);

			if(!lua_isstring(L, -1))
			{
				if(VIDEO_DEBUG)
					cerr << "VIDEO ERROR: encountered a non-string element in animation frames array, in effect file " << filename << endl;
				return false;
			}

			string anim_filename = lua_tostring(L, -1);
			sys_def->animation_frame_filenames.push_back(anim_filename);

			// pop the current animation frame filename
			lua_pop(L, 1);
		}

		// pop the animation_frames table
		lua_pop(L, 1);

		lua_pushstring(L, "animation_frame_times");
		lua_gettable(L, -2);

		if(!lua_istable(L, -1))
		{
			if(VIDEO_DEBUG)
				cerr << "VIDEO ERROR: could not locate 'animation_frame_times' in the effect file " << filename << endl;
			return false;
		}

		int32 num_animation_frame_times = lua_objlen(L, -1);

		if(num_animation_frame_times < 1)
		{
			if(VIDEO_DEBUG)
				cerr << "VIDEO ERROR: the 'animation_frame_times' table was empty in effect file " << filename << endl;
			return false;
		}

		for(int32 n_frame_time = 0; n_frame_time < num_animation_frame_times; ++n_frame_time)
		{
			lua_rawgeti(L, -1, n_frame_time+1);

			if(!lua_isnumber(L, -1))
			{
				if(VIDEO_DEBUG)
					cerr << "VIDEO ERROR: encountered a non-numeric element in animation frame times array, in effect file " << filename << endl;
				return false;
			}

			int32 anim_frame_time = (int32)lua_tonumber(L, -1);
			sys_def->animation_frame_times.push_back(anim_frame_time);

			// pop the current animation frame time
			lua_pop(L, 1);
		}

		// pop the animation_frame times table
		lua_pop(L, 1);

		LOAD_BOOL("enabled", sys_def->enabled);
		LOAD_INT ("blend_mode", sys_def->blend_mode);
		LOAD_FLOAT("system_lifetime", sys_def->system_lifetime);

		LOAD_FLOAT("particle_lifetime", sys_def->particle_lifetime);
		LOAD_FLOAT("particle_lifetime_variation", sys_def->particle_lifetime_variation);
		LOAD_INT  ("max_particles", sys_def->max_particles);
		LOAD_FLOAT("damping", sys_def->damping);
		LOAD_FLOAT("damping_variation", sys_def->damping_variation);
		LOAD_FLOAT("acceleration_x", sys_def->acceleration_x);
		LOAD_FLOAT("acceleration_y", sys_def->acceleration_y);
		LOAD_FLOAT("acceleration_variation_x", sys_def->acceleration_variation_x);
		LOAD_FLOAT("acceleration_variation_y", sys_def->acceleration_variation_y);
		LOAD_FLOAT("wind_velocity_x", sys_def->wind_velocity_x);
		LOAD_FLOAT("wind_velocity_y", sys_def->wind_velocity_y);
		LOAD_FLOAT("wind_velocity_variation_x", sys_def->wind_velocity_variation_x);
		LOAD_FLOAT("wind_velocity_variation_y", sys_def->wind_velocity_variation_y);
		LOAD_BOOL ("wave_motion_used", sys_def->wave_motion_used);
		LOAD_FLOAT("wave_length", sys_def->wave_length);
		LOAD_FLOAT("wave_length_variation", sys_def->wave_length_variation);
		LOAD_FLOAT("wave_amplitude", sys_def->wave_amplitude);
		LOAD_FLOAT("wave_amplitude_variation", sys_def->wave_amplitude_variation);
		LOAD_FLOAT("tangential_acceleration", sys_def->tangential_acceleration);
		LOAD_FLOAT("tangential_acceleration_variation", sys_def->tangential_acceleration_variation);
		LOAD_FLOAT("radial_acceleration", sys_def->radial_acceleration);
		LOAD_FLOAT("radial_acceleration_variation", sys_def->radial_acceleration_variation);
		LOAD_BOOL ("user_defined_attractor", sys_def->user_defined_attractor);
		LOAD_FLOAT("attractor_falloff", sys_def->attractor_falloff);
		LOAD_BOOL ("rotation_used", sys_def->rotation_used);
		LOAD_BOOL ("rotate_to_velocity", sys_def->rotate_to_velocity);
		LOAD_BOOL ("speed_scale_used", sys_def->speed_scale_used);
		LOAD_FLOAT("speed_scale", sys_def->speed_scale);
		LOAD_FLOAT("min_speed_scale", sys_def->min_speed_scale);
		LOAD_FLOAT("max_speed_scale", sys_def->max_speed_scale);
		LOAD_BOOL ("smooth_animation", sys_def->smooth_animation);
		LOAD_BOOL ("modify_stencil", sys_def->modify_stencil);

		string stencil_op_string;
		LOAD_STRING("stencil_op", stencil_op_string);

		if(!strcasecmp(stencil_op_string.c_str(), "incr"))
			sys_def->stencil_op = VIDEO_STENCIL_OP_INCREASE;
		else if(!strcasecmp(stencil_op_string.c_str(), "decr"))
			sys_def->stencil_op = VIDEO_STENCIL_OP_DECREASE;
		else if(!strcasecmp(stencil_op_string.c_str(), "zero"))
			sys_def->stencil_op = VIDEO_STENCIL_OP_ZERO;
		else //..if(!strcasecmp(stencil_op_string.c_str(), "one"))
			sys_def->stencil_op = VIDEO_STENCIL_OP_ONE;

		LOAD_BOOL("use_stencil", sys_def->use_stencil);
		LOAD_FLOAT("scene_lighting", sys_def->scene_lighting);
		LOAD_BOOL ("random_initial_angle", sys_def->random_initial_angle);

		// pop the system table
		lua_pop(L, 1);

		def->_systems.push_back(sys_def);
	}

	return true;
}


//-----------------------------------------------------------------------------
// LoadEffect: loads an effect definition from disk given the filename
//             Returns NULL if there is a problem with loading
//-----------------------------------------------------------------------------

ParticleEffectDef *ParticleManager::LoadEffect(const std::string &filename)
{
	ParticleEffectDef *def = new ParticleEffectDef;
	lua_State *L = lua_open();

	bool could_load = TEMP_LoadEffectHelper(filename, L, def);

	if(!could_load)
	{
		lua_close(L);
		delete def;
		return NULL;
	}

	return def;
}


//-----------------------------------------------------------------------------
// AddEffect: adds a new particle effect to the list
//-----------------------------------------------------------------------------

ParticleEffectID ParticleManager::AddEffect(const ParticleEffectDef *def, float x, float y)
{
	if(!def)
	{
		if(VIDEO_DEBUG)
			cerr << "VIDEO ERROR: ParticleManager::AddEffect() failed because def was NULL!" << endl;
		return VIDEO_INVALID_EFFECT;
	}

	if(def->_systems.empty())
	{
		if(VIDEO_DEBUG)
			cerr << "VIDEO ERROR: ParticleManager::AddEffect() failed because def->_systems was empty!" << endl;
		return VIDEO_INVALID_EFFECT;
	}

	ParticleEffect *effect = _CreateEffect(def);
	if(!effect)
	{
		if(VIDEO_DEBUG)
			cerr << "VIDEO ERROR: could not create particle effect in ParticleManager::AddEffect()!" << endl;
		return VIDEO_INVALID_EFFECT;
	}

	effect->Move(x, y);
	_effects[_current_id] = effect;

	++_current_id;

	return _current_id - 1;
};


//-----------------------------------------------------------------------------
// Draw: draws all particle effects
//-----------------------------------------------------------------------------

bool ParticleManager::Draw()
{
	VideoManager->PushState();
	VideoManager->SetCoordSys(CoordSys(0.0f, 1024.0f, 768.0f, 0.0f));
	VideoManager->DisableScissoring();

	map<ParticleEffectID, ParticleEffect *>::iterator iEffect = _effects.begin();

	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);

	bool success = true;

	while(iEffect != _effects.end())
	{
		if(!(iEffect->second)->_Draw())
		{
			success = false;
			if(VIDEO_DEBUG)
				cerr << "VIDEO ERROR: effect failed to draw in ParticleManager::Draw()!" << endl;
		}
		++iEffect;
	}

	VideoManager->PopState();
	return success;
}


//-----------------------------------------------------------------------------
// Update: updates all particle effects
//-----------------------------------------------------------------------------

bool ParticleManager::Update(int32 frame_time)
{
	float frame_time_seconds = static_cast<float>(frame_time) / 1000.0f;

	map<ParticleEffectID, ParticleEffect *>::iterator iEffect = _effects.begin();

	bool success = true;

	_num_particles = 0;

	while(iEffect != _effects.end())
	{
		if(!(iEffect->second)->IsAlive())
		{
			_effects.erase(iEffect++);
		}
		else
		{
			if(!(iEffect->second)->_Update(frame_time_seconds))
			{
				success = false;
				if(VIDEO_DEBUG)
					cerr << "VIDEO ERROR: effect failed to update in ParticleManager::Update()!" << endl;
			}

			_num_particles += iEffect->second->GetNumParticles();
			++iEffect;
		}
	}

	return success;
}


//-----------------------------------------------------------------------------
// StopAll: stops all particle effects
//-----------------------------------------------------------------------------

void ParticleManager::StopAll(bool kill_immediate)
{
	map<ParticleEffectID, ParticleEffect *>::iterator iEffect = _effects.begin();

	while(iEffect != _effects.end())
	{
		(iEffect->second)->Stop(kill_immediate);
	}
}


//-----------------------------------------------------------------------------
// Destroy: destroys all particle effects
//-----------------------------------------------------------------------------

void ParticleManager::Destroy()
{
	map<ParticleEffectID, ParticleEffect *>::iterator iEffect = _effects.begin();

	while(iEffect != _effects.end())
	{
		(iEffect->second)->_Destroy();
		delete (iEffect->second);
		++iEffect;
	}

	_effects.clear();
}


//-----------------------------------------------------------------------------
// GetEffect: returns a pointer to the effect with the given ID. Only valid
//            up until the next call to ParticleManager::Update(). In other words,
//            don't store any pointers yourself, store the ID and call GetEffect()
//
//            Returns NULL if the effect cannot be found (perhaps it died)
//-----------------------------------------------------------------------------

ParticleEffect *ParticleManager::GetEffect(ParticleEffectID id)
{
	map<ParticleEffectID, ParticleEffect *>::iterator iEffect = _effects.find(id);
	if(iEffect == _effects.end())
		return NULL;
	else
		return iEffect->second;
}


//-----------------------------------------------------------------------------
// GetNumParticles: returns the entire number of live particles between all
//                  currently displaying effects
//-----------------------------------------------------------------------------

int32 ParticleManager::GetNumParticles()
{
	return _num_particles;
}


//-----------------------------------------------------------------------------
// _CreateEffect: helper to AddEffect(), does the job of initializing a new effect
//                given its definition. Returns NULL on failure
//
//                NOTE: assumes that def isn't NULL and def->_systems is non-empty
//-----------------------------------------------------------------------------

ParticleEffect *ParticleManager::_CreateEffect(const ParticleEffectDef *def)
{
	list<ParticleSystemDef *>::const_iterator iSystem = def->_systems.begin();
	list<ParticleSystemDef *>::const_iterator iEnd = def->_systems.end();

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

				if(VIDEO_DEBUG)
					cerr << "VIDEO ERROR: sys->Create() returned false while trying to create effect!" << endl;
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


}  // namespace private_video
}  // namespace hoa_video
