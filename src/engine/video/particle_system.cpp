///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "video.h"

#include "particle_system.h"
#include "particle_keyframe.h"

using namespace std;
using namespace hoa_utils;

namespace hoa_video
{

namespace private_video
{


//-----------------------------------------------------------------------------
// ParticleSystem
//-----------------------------------------------------------------------------

ParticleSystem::ParticleSystem()
{
	_system_def = NULL;
	_max_particles = 0;
	_num_particles = 0;
	_age = 0.0f;
	_last_update_time = 0.0f;

	_alive = true;
	_stopped = false;
}


//-----------------------------------------------------------------------------
// Create: initializes the particle system from the definition
//-----------------------------------------------------------------------------

bool ParticleSystem::Create(const ParticleSystemDef *sys_def)
{
	_system_def = sys_def;
	_max_particles = sys_def->max_particles;
	_num_particles = 0;

	_particles.resize(_max_particles);
	_particle_vertices.resize(_max_particles * 4);
	_particle_texcoords.resize(_max_particles * 4);
	_particle_colors.resize(_max_particles * 4);

	_alive = true;
	_stopped = false;
	_age = 0.0f;

	size_t num_frames = sys_def->animation_frame_filenames.size();

	for(size_t j = 0; j < num_frames; ++j)
	{
		int32 frame_time;
		if(j < sys_def->animation_frame_times.size())
			frame_time = sys_def->animation_frame_times[j];
		else if(sys_def->animation_frame_times.empty())
			frame_time = 0;
		else
			frame_time = sys_def->animation_frame_times.back();

		_animation.AddFrame(sys_def->animation_frame_filenames[j], frame_time);
	}

// 	VideoManager->LoadImage(_animation);
	return true;
}



//-----------------------------------------------------------------------------
// Draw: draws the particle system
//-----------------------------------------------------------------------------

bool ParticleSystem::Draw()
{
	if(!_system_def->enabled || _age < _system_def->emitter._start_time)
		return true;

	// set blending parameters
	if(_system_def->blend_mode == VIDEO_NO_BLEND)
	{
		glDisable(GL_BLEND);
	}
	else
	{
		glEnable(GL_BLEND);

		if(_system_def->blend_mode == VIDEO_BLEND)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		else
			glBlendFunc(GL_SRC_ALPHA, GL_ONE); // additive
	}


	if(_system_def->use_stencil)
	{
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
	else if(_system_def->modify_stencil)
	{
		glEnable(GL_STENCIL_TEST);

		if(_system_def->stencil_op == VIDEO_STENCIL_OP_INCREASE)
			glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);
		else if(_system_def->stencil_op == VIDEO_STENCIL_OP_DECREASE)
			glStencilOp(GL_DECR, GL_KEEP, GL_KEEP);
		else if(_system_def->stencil_op == VIDEO_STENCIL_OP_ZERO)
			glStencilOp(GL_ZERO, GL_KEEP, GL_KEEP);
		else
			glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);

		glStencilFunc(GL_NEVER, 1, 0xFFFFFFFF);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.00f);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	}
	else
	{
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_ALPHA_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}

	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	StillImage *id = _animation.GetFrame(_animation.GetCurrentFrameIndex());
	ImageTexture *img = id->_image_texture;
	TextureManager->_BindTexture(img->texture_sheet->tex_id);


	float frame_progress = _animation.GetPercentProgress();

	float u1 = img->u1;
	float u2 = img->u2;
	float v1 = img->v1;
	float v2 = img->v2;

	float img_width  = static_cast<float>(img->width);
	float img_height = static_cast<float>(img->height);

	float img_width_half = img_width * 0.5f;
	float img_height_half = img_height * 0.5f;

	Color scene_light_modifier;

	bool use_scene_lighting = false;

	if(_system_def->scene_lighting != 0.0f)
	{
		scene_light_modifier = VideoManager->GetSceneLightingColor();

		if(scene_light_modifier[0] != 1.0f ||
		   scene_light_modifier[1] != 1.0f ||
		   scene_light_modifier[2] != 1.0f ||
		   scene_light_modifier[3] != 1.0f )
		{
			use_scene_lighting = true;

			if(_system_def->scene_lighting != 1.0f)
				scene_light_modifier = Color::white * (1.0f - _system_def->scene_lighting) + scene_light_modifier * (_system_def->scene_lighting);
		}
	}


	// fill the vertex array

	if(_system_def->rotation_used)
	{
		int32 v = 0;

		for(int32 j = 0; j < _num_particles; ++j)
		{
			float scaled_width_half  = img_width_half * _particles[j].size_x;
			float scaled_height_half = img_height_half * _particles[j].size_y;

			float rotation_angle = _particles[j].rotation_angle;

			if(_system_def->rotate_to_velocity)
			{
				// calculate the angle based on the velocity
				rotation_angle += UTILS_HALF_PI + atan2f(_particles[j].combined_velocity_y, _particles[j].combined_velocity_x);

				// calculate the scaling due to speed
				if(_system_def->speed_scale_used)
				{
					// speed is magnitude of velocity
					float speed = sqrtf(_particles[j].combined_velocity_x * _particles[j].combined_velocity_x + _particles[j].combined_velocity_y * _particles[j].combined_velocity_y);
					float scale_factor = _system_def->speed_scale * speed;

					if(scale_factor < _system_def->min_speed_scale)
						scale_factor = _system_def->min_speed_scale;
					if(scale_factor > _system_def->max_speed_scale)
						scale_factor = _system_def->max_speed_scale;

					scaled_height_half *= scale_factor;
				}
			}

			// upper-left vertex
			_particle_vertices[v]._x = -scaled_width_half;
			_particle_vertices[v]._y = -scaled_height_half;
			RotatePoint(_particle_vertices[v]._x, _particle_vertices[v]._y, rotation_angle);
			_particle_vertices[v]._x += _particles[j].x;
			_particle_vertices[v]._y += _particles[j].y;
			++v;

			// upper-right vertex
			_particle_vertices[v]._x = scaled_width_half;
			_particle_vertices[v]._y = -scaled_height_half;
			RotatePoint(_particle_vertices[v]._x, _particle_vertices[v]._y, rotation_angle);
			_particle_vertices[v]._x += _particles[j].x;
			_particle_vertices[v]._y += _particles[j].y;
			++v;

			// lower-right vertex
			_particle_vertices[v]._x = scaled_width_half;
			_particle_vertices[v]._y = scaled_height_half;
			RotatePoint(_particle_vertices[v]._x, _particle_vertices[v]._y, rotation_angle);
			_particle_vertices[v]._x += _particles[j].x;
			_particle_vertices[v]._y += _particles[j].y;
			++v;

			// lower-left vertex
			_particle_vertices[v]._x = -scaled_width_half;
			_particle_vertices[v]._y = scaled_height_half;
			RotatePoint(_particle_vertices[v]._x, _particle_vertices[v]._y, rotation_angle);
			_particle_vertices[v]._x += _particles[j].x;
			_particle_vertices[v]._y += _particles[j].y;
			++v;


		}
	}
	else
	{
		int32 v = 0;

		for(int32 j = 0; j < _num_particles; ++j)
		{
			float scaled_width_half  = img_width_half * _particles[j].size_x;
			float scaled_height_half = img_height_half * _particles[j].size_y;

			// upper-left vertex
			_particle_vertices[v]._x = _particles[j].x - scaled_width_half;
			_particle_vertices[v]._y = _particles[j].y - scaled_height_half;
			++v;

			// upper-right vertex
			_particle_vertices[v]._x = _particles[j].x + scaled_width_half;
			_particle_vertices[v]._y = _particles[j].y - scaled_height_half;
			++v;

			// lower-right vertex
			_particle_vertices[v]._x = _particles[j].x + scaled_width_half;
			_particle_vertices[v]._y = _particles[j].y + scaled_height_half;
			++v;

			// lower-left vertex
			_particle_vertices[v]._x = _particles[j].x - scaled_width_half;
			_particle_vertices[v]._y = _particles[j].y + scaled_height_half;
			++v;
		}
	}

	// fill the color array

	int32 c = 0;
	for(int32 j = 0; j < _num_particles; ++j)
	{
		Color color = _particles[j].color;

		if(_system_def->smooth_animation)
			color = color * (1.0f - frame_progress);

		if(use_scene_lighting)
			color = color * scene_light_modifier;

		_particle_colors[c] = color;
		++c;
		_particle_colors[c] = color;
		++c;
		_particle_colors[c] = color;
		++c;
		_particle_colors[c] = color;
		++c;
	}

	// fill the texcoord array

	int32 t = 0;
	for(int32 j = 0; j < _num_particles; ++j)
	{
		// upper-left
		_particle_texcoords[t]._t0 = u1;
		_particle_texcoords[t]._t1 = v1;
		++t;

		// upper-right
		_particle_texcoords[t]._t0 = u2;
		_particle_texcoords[t]._t1 = v1;
		++t;

		// lower-right
		_particle_texcoords[t]._t0 = u2;
		_particle_texcoords[t]._t1 = v2;
		++t;

		// lower-left
		_particle_texcoords[t]._t0 = u1;
		_particle_texcoords[t]._t1 = v2;
		++t;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer   (2, GL_FLOAT, 0, &_particle_vertices[0]);
	glColorPointer    (4, GL_FLOAT, 0, &_particle_colors[0]);
	glTexCoordPointer (2, GL_FLOAT, 0, &_particle_texcoords[0]);

	glDrawArrays(GL_QUADS, 0, _num_particles * 4);

	glDisableClientState(GL_VERTEX_ARRAY);

	if(_system_def->smooth_animation)
	{
		glEnableClientState(GL_VERTEX_ARRAY);

		int findex = _animation.GetCurrentFrameIndex();
		findex = (findex + 1) % _animation.GetNumFrames();

		StillImage *id2 = _animation.GetFrame(findex);
		ImageTexture *img2 = id2->_image_texture;
		TextureManager->_BindTexture(img2->texture_sheet->tex_id);


		u1 = img2->u1;
		u2 = img2->u2;
		v1 = img2->v1;
		v2 = img2->v2;


		t = 0;
		for(int32 j = 0; j < _num_particles; ++j)
		{
			// upper-left
			_particle_texcoords[t]._t0 = u1;
			_particle_texcoords[t]._t1 = v1;
			++t;

			// upper-right
			_particle_texcoords[t]._t0 = u2;
			_particle_texcoords[t]._t1 = v1;
			++t;

			// lower-right
			_particle_texcoords[t]._t0 = u2;
			_particle_texcoords[t]._t1 = v2;
			++t;

			// lower-left
			_particle_texcoords[t]._t0 = u1;
			_particle_texcoords[t]._t1 = v2;
			++t;
		}



		c = 0;
		for(int32 j = 0; j < _num_particles; ++j)
		{
			Color color = _particles[j].color;
			color = color * frame_progress;
			if(use_scene_lighting)
				color = color * scene_light_modifier;

			_particle_colors[c] = color;
			++c;
			_particle_colors[c] = color;
			++c;
			_particle_colors[c] = color;
			++c;
			_particle_colors[c] = color;
			++c;
		}

		glVertexPointer   (2, GL_FLOAT, 0, &_particle_vertices[0]);
		glColorPointer    (4, GL_FLOAT, 0, &_particle_colors[0]);
		glTexCoordPointer (2, GL_FLOAT, 0, &_particle_texcoords[0]);

		glDrawArrays(GL_QUADS, 0, _num_particles * 4);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	return true;
}


//-----------------------------------------------------------------------------
// IsAlive: returns whether the particle system has active particles or not
//-----------------------------------------------------------------------------

bool ParticleSystem::IsAlive() const
{
	return _alive && _system_def->enabled;
}


//-----------------------------------------------------------------------------
// IsStopped: returns whether the system has been stopped due to a call to Stop(),
//            meaning that it cannot emit any more particles
//-----------------------------------------------------------------------------

bool ParticleSystem::IsStopped() const
{
	return _stopped;
}


//-----------------------------------------------------------------------------
// Update: updates particle positions and properties, and emits/kills particles
//-----------------------------------------------------------------------------

bool ParticleSystem::Update(float frame_time, const EffectParameters &params)
{
	if(!_system_def->enabled)
		return true;

	_age += frame_time;

	if(_age < _system_def->emitter._start_time)
	{
		_last_update_time = _age;
		return true;
	}

	_animation.Update();

	// update properties of existing particles
	_UpdateParticles(frame_time, params);

	// figure out how many particles need to be emitted this frame
	int32 num_particles_to_emit = 0;
	if(!_stopped)
	{
		if(_system_def->emitter._emitter_mode == EMITTER_MODE_ALWAYS)
		{
			num_particles_to_emit = _system_def->max_particles - _num_particles;
		}
		else if(_system_def->emitter._emitter_mode != EMITTER_MODE_BURST)
		{
			float time_low  = _last_update_time * _system_def->emitter._emission_rate;
			float time_high = _age * _system_def->emitter._emission_rate;

			time_low  = floorf(time_low);
			time_high = ceilf(time_high);

			num_particles_to_emit = static_cast<int32>(time_high - time_low) - 1;

			if(num_particles_to_emit + _num_particles > _max_particles)
				num_particles_to_emit = _max_particles - _num_particles;
		}
		else
		{
			num_particles_to_emit = _system_def->max_particles;
		}
	}

	// kill expired particles. If there are particles waiting to be emitted, then instead of
	// killing, just respawn the expired particle since this is much more efficient
	_KillParticles(num_particles_to_emit, params);

	// if there are still any particles waiting to be emitted, emit them
	_EmitParticles(num_particles_to_emit, params);

	// stop the particle system immediately if burst is used
	if(_system_def->emitter._emitter_mode == EMITTER_MODE_BURST)
		Stop();

	// stop the system if it's past its lifetime. Note that the only mode in which
	// the system lifetime is applicable is ONE_SHOT mode
	if(_system_def->emitter._emitter_mode == EMITTER_MODE_ONE_SHOT)
	{
		if(_age > _system_def->system_lifetime)
			_stopped = true;
	}

	// check if the system is dead
	if(_num_particles == 0 && _stopped)
	{
		_alive = false;
	}

	_last_update_time = _age;
	return true;
}


//-----------------------------------------------------------------------------
// GetNumParticles: return the number of active particles
//-----------------------------------------------------------------------------

int32 ParticleSystem::GetNumParticles() const
{
	return _num_particles;
}


//-----------------------------------------------------------------------------
// Destroy: destroys the system (when the effect is destroyed)
//-----------------------------------------------------------------------------

void ParticleSystem::Destroy()
{
	_particles.clear();
	_particle_vertices.clear();
}


//-----------------------------------------------------------------------------
// Stop: ceases particle emission
//-----------------------------------------------------------------------------

void ParticleSystem::Stop()
{
	_stopped = true;
}


//-----------------------------------------------------------------------------
// _UpdateParticles: helper function to update the positions and properties
//-----------------------------------------------------------------------------

void ParticleSystem::_UpdateParticles(float t, const EffectParameters &params)
{
	for(int j = 0; j < _num_particles; ++j)
	{
		// calculate a time for the particle from 0 to 1 since this is what
		// the keyframes are based on
		float scaled_time = _particles[j].time / _particles[j].lifetime;

		// figure out which keyframe we're on
		if(_particles[j].next_keyframe)
		{
			ParticleKeyframe *old_next = _particles[j].next_keyframe;

			// check if we need to advance the keyframe
			if(scaled_time >= _particles[j].next_keyframe->time)
			{
				// figure out what keyframe we're on
				size_t num_keyframes = _system_def->keyframes.size();

				size_t k;
				for(k = 0; k < num_keyframes; ++k)
				{
					if(_system_def->keyframes[k]->time > scaled_time)
					{
						_particles[j].current_keyframe = _system_def->keyframes[k - 1];
						_particles[j].next_keyframe    = _system_def->keyframes[k];
						break;
					}
				}

				// if we didn't find any keyframe whose time is larger than this
				// particle's time, then we are on the last one
				if(k == num_keyframes)
				{
					_particles[j].current_keyframe = _system_def->keyframes[k - 1];
					_particles[j].next_keyframe = NULL;

					// set all of the keyframed properties to the value stored in the last
					// keyframe
					_particles[j].color          = _particles[j].current_keyframe->color;
					_particles[j].rotation_speed = _particles[j].current_keyframe->rotation_speed;
					_particles[j].size_x         = _particles[j].current_keyframe->size_x;
					_particles[j].size_y         = _particles[j].current_keyframe->size_y;
				}

				// if we skipped ahead only 1 keyframe, then inherit the current variations
				// from the next ones
				if(_particles[j].current_keyframe == old_next)
				{
					_particles[j].current_color_variation = _particles[j].next_color_variation;
					_particles[j].current_rotation_speed_variation = _particles[j].current_rotation_speed_variation;
					_particles[j].current_size_variation_x = _particles[j].next_size_variation_x;
					_particles[j].current_size_variation_y = _particles[j].next_size_variation_y;
				}
				else
				{
					_particles[j].current_rotation_speed_variation = RandomFloat(-_particles[j].current_keyframe->rotation_speed_variation, _particles[j].current_keyframe->rotation_speed_variation);
					for(int32 c = 0; c < 4; ++c)
						_particles[j].current_color_variation[c] = RandomFloat(-_particles[j].current_keyframe->color_variation[c], _particles[j].current_keyframe->color_variation[c]);
					_particles[j].current_size_variation_x = RandomFloat(-_particles[j].current_keyframe->size_variation_x, _particles[j].current_keyframe->size_variation_x);
					_particles[j].current_size_variation_y = RandomFloat(-_particles[j].current_keyframe->size_variation_y, _particles[j].current_keyframe->size_variation_y);
				}

				// if there is a next keyframe, generate variations for it
				if(_particles[j].next_keyframe)
				{
					_particles[j].next_rotation_speed_variation = RandomFloat(-_particles[j].next_keyframe->rotation_speed_variation, _particles[j].next_keyframe->rotation_speed_variation);
					for(int32 c = 0; c < 4; ++c)
						_particles[j].next_color_variation[c] = RandomFloat(-_particles[j].next_keyframe->color_variation[c], _particles[j].next_keyframe->color_variation[c]);
					_particles[j].next_size_variation_x = RandomFloat(-_particles[j].next_keyframe->size_variation_x, _particles[j].next_keyframe->size_variation_x);
					_particles[j].next_size_variation_y = RandomFloat(-_particles[j].next_keyframe->size_variation_y, _particles[j].next_keyframe->size_variation_y);
				}
			}
		}


		// if we aren't already at the last keyframe, interpolate to figure out the
		// current keyframed properties
		if(_particles[j].next_keyframe)
		{
			// figure out how far we are from the current to the next (0.0 to 1.0)
			float a = (scaled_time - _particles[j].current_keyframe->time) / (_particles[j].next_keyframe->time - _particles[j].current_keyframe->time);

			_particles[j].rotation_speed = Lerp(a, _particles[j].current_keyframe->rotation_speed + _particles[j].current_rotation_speed_variation, _particles[j].next_keyframe->rotation_speed + _particles[j].next_rotation_speed_variation);
			_particles[j].size_x         = Lerp(a, _particles[j].current_keyframe->size_x + _particles[j].current_size_variation_x, _particles[j].next_keyframe->size_x + _particles[j].next_size_variation_x);
			_particles[j].size_y         = Lerp(a, _particles[j].current_keyframe->size_y + _particles[j].current_size_variation_y, _particles[j].next_keyframe->size_y + _particles[j].next_size_variation_y);
			_particles[j].color[0]       = Lerp(a, _particles[j].current_keyframe->color[0] + _particles[j].current_color_variation[0], _particles[j].next_keyframe->color[0] + _particles[j].next_color_variation[0]);
			_particles[j].color[1]       = Lerp(a, _particles[j].current_keyframe->color[1] + _particles[j].current_color_variation[1], _particles[j].next_keyframe->color[1] + _particles[j].next_color_variation[1]);
			_particles[j].color[2]       = Lerp(a, _particles[j].current_keyframe->color[2] + _particles[j].current_color_variation[2], _particles[j].next_keyframe->color[2] + _particles[j].next_color_variation[2]);
			_particles[j].color[3]       = Lerp(a, _particles[j].current_keyframe->color[3] + _particles[j].current_color_variation[3], _particles[j].next_keyframe->color[3] + _particles[j].next_color_variation[3]);
		}


		_particles[j].rotation_angle += _particles[j].rotation_speed * _particles[j].rotation_direction * t;

		float wind_velocity_x = _particles[j].wind_velocity_x;
		float wind_velocity_y = _particles[j].wind_velocity_y;

		_particles[j].combined_velocity_x = _particles[j].velocity_x + wind_velocity_x;
		_particles[j].combined_velocity_y = _particles[j].velocity_y + wind_velocity_y;

		if(_system_def->wave_motion_used && _particles[j].wave_half_amplitude > 0.0f)
		{
			// find the magnitude of the wave velocity
// 			float half_amp = _particles[j].wave_half_amplitude; UNUSED VARIABLE
// 			float wcoef = _particles[j].wave_length_coefficient; UNUSED VARIABLE
			float wave_speed = _particles[j].wave_half_amplitude * sinf(_particles[j].wave_length_coefficient * _particles[j].time);

			// now the wave velocity is just that wave speed times the particle's tangential vector
			float tangent_x = -_particles[j].combined_velocity_y;
			float tangent_y = _particles[j].combined_velocity_x;
			float speed = sqrtf(tangent_x * tangent_x + tangent_y * tangent_y);
			tangent_x /= speed;
			tangent_y /= speed;

			float wave_velocity_x = tangent_x * wave_speed;
			float wave_velocity_y = tangent_y * wave_speed;

			_particles[j].combined_velocity_x += wave_velocity_x;
			_particles[j].combined_velocity_y += wave_velocity_y;
		}

		_particles[j].x += (_particles[j].combined_velocity_x) * t;
		_particles[j].y += (_particles[j].combined_velocity_y) * t;


		// client-specified acceleration (dv = a * t)
		_particles[j].velocity_x += _particles[j].acceleration_x * t;
		_particles[j].velocity_y += _particles[j].acceleration_y * t;

		// radial acceleration: calculate unit vector from emitter center to this particle,
		// and scale by the radial acceleration, if there is any


		bool use_radial     = (_particles[j].radial_acceleration != 0.0f);
		bool use_tangential = (_particles[j].tangential_acceleration != 0.0f);


		if(use_radial || use_tangential)
		{
			// unit vector from attractor to particle
			float attractor_to_particle_x;
			float attractor_to_particle_y;

			if(_system_def->user_defined_attractor)
			{
				attractor_to_particle_x = _particles[j].x - params.attractor_x;
				attractor_to_particle_y = _particles[j].y - params.attractor_y;
			}
			else
			{
				attractor_to_particle_x = _particles[j].x - _system_def->emitter._center_x;
				attractor_to_particle_y = _particles[j].y - _system_def->emitter._center_y;
			}

			float distance = sqrtf(attractor_to_particle_x * attractor_to_particle_x + attractor_to_particle_y * attractor_to_particle_y);

			if(distance != 0.0f)
			{
				attractor_to_particle_x /= distance;
				attractor_to_particle_y /= distance;
			}

			// radial acceleration
			if(use_radial)
			{
				if(_system_def->attractor_falloff != 0.0f)
				{
					float attraction = 1.0f - _system_def->attractor_falloff * distance;
					if(attraction > 0.0f)
					{
						_particles[j].velocity_x += attractor_to_particle_x * _particles[j].radial_acceleration * t * attraction;
						_particles[j].velocity_y += attractor_to_particle_y * _particles[j].radial_acceleration * t * attraction;
					}
				}
				else
				{
					_particles[j].velocity_x += attractor_to_particle_x * _particles[j].radial_acceleration * t;
					_particles[j].velocity_y += attractor_to_particle_y * _particles[j].radial_acceleration * t;
				}
			}

			// tangential acceleration
			if(use_tangential)
			{
				// tangent vector is simply perpendicular vector
				float tangent_x = -attractor_to_particle_y;
				float tangent_y = attractor_to_particle_x;

				_particles[j].velocity_x += tangent_x * _particles[j].tangential_acceleration * t;
				_particles[j].velocity_y += tangent_y * _particles[j].tangential_acceleration * t;
			}
		}


		// damp the velocity

		if(_particles[j].damping != 1.0f)
		{
			_particles[j].velocity_x *= pow(_particles[j].damping, t);
			_particles[j].velocity_y *= pow(_particles[j].damping, t);
		}

		_particles[j].time += t;
	}
}


//-----------------------------------------------------------------------------
// _KillParticles: helper function to kill expired particles. The num parameter
//                 tells how many particles need to be emitted this frame.
//                 If possible, we try to respawn particles instead of killing
//                 and then emitting, because it is much more efficient.
//-----------------------------------------------------------------------------

void ParticleSystem::_KillParticles(int32 &num, const EffectParameters &params)
{
	// check each active particle to see if it is expired
	for(int j = 0; j < _num_particles; ++j)
	{
		if(_particles[j].time > _particles[j].lifetime)
		{
			if(num > 0)
			{
				// if we still have particles to emit, then instead of killing the particle,
				// respawn it as a new one
				_RespawnParticle(j, params);
				--num;
			}
			else
			{
				// kill the particle, i.e. move the particle at the end of the array to this
				// particle's spot, and decrement _num_particles

				if(j != _num_particles - 1)
					_MoveParticle(_num_particles - 1, j);
				--_num_particles;
			}
		}
	}
}


//-----------------------------------------------------------------------------
// _EmitParticles: helper function, emits new particles
//-----------------------------------------------------------------------------

void ParticleSystem::_EmitParticles(int32 num, const EffectParameters &params)
{
	// respawn 'num' new particles at the end of the array
	for(int32 j = 0; j < num; ++j)
	{
		_RespawnParticle(_num_particles, params);
		++_num_particles;
	}
}


//-----------------------------------------------------------------------------
// _MoveParticle: helper function, moves the data for a particle from src
//                to dest index in the array
//-----------------------------------------------------------------------------

void ParticleSystem::_MoveParticle(int32 src, int32 dest)
{
	_particles[dest] = _particles[src];
}


//-----------------------------------------------------------------------------
// _RespawnParticle: helper function to Update(), does the work of setting up
//                   the properties for a newly spawned particle
//-----------------------------------------------------------------------------

void ParticleSystem::_RespawnParticle(int32 i, const EffectParameters &params)
{
	const ParticleEmitter &emitter = _system_def->emitter;

	switch(emitter._shape)
	{
		case EMITTER_SHAPE_POINT:
		{
			_particles[i].x = emitter._x;
			_particles[i].y = emitter._y;
			break;
		}
		case EMITTER_SHAPE_LINE:
		{
			_particles[i].x = RandomFloat(emitter._x, emitter._x2);
			_particles[i].y = RandomFloat(emitter._y, emitter._y2);
			break;
		}
		case EMITTER_SHAPE_CIRCLE:
		{
			float angle = RandomFloat(0.0f, UTILS_2PI);
			_particles[i].x = emitter._radius * cosf(angle);
			_particles[i].y = emitter._radius * sinf(angle);
			break;
		}
		case EMITTER_SHAPE_FILLED_CIRCLE:
		{
			float radius_squared = emitter._radius;
			radius_squared *= radius_squared;

			// use rejection sampling to choose a point within the circle
			// this may need to be replaced by a speedier algorithm later on
			do
			{
				float half_radius = emitter._radius * 0.5f;
				_particles[i].x = RandomFloat(-half_radius, half_radius);
				_particles[i].y = RandomFloat(-half_radius, half_radius);
			} while(_particles[i].x * _particles[i].x +
			        _particles[i].y * _particles[i].y > radius_squared);


			break;
		}
		case EMITTER_SHAPE_FILLED_RECTANGLE:
		{
			_particles[i].x = RandomFloat(emitter._x, emitter._x2);
			_particles[i].y = RandomFloat(emitter._y, emitter._y2);
			break;
		}
		default:
			break;
	};


	_particles[i].x += RandomFloat(-emitter._x_variation, emitter._x_variation);
	_particles[i].y += RandomFloat(-emitter._y_variation, emitter._y_variation);

	if(params.orientation != 0.0f)
		RotatePoint(_particles[i].x, _particles[i].y, params.orientation);

	_particles[i].color = _system_def->keyframes[0]->color;

	_particles[i].rotation_speed  = _system_def->keyframes[0]->rotation_speed;
	_particles[i].time            = 0.0f;
	_particles[i].size_x            = _system_def->keyframes[0]->size_x;
	_particles[i].size_y            = _system_def->keyframes[0]->size_y;

	if(_system_def->random_initial_angle)
		_particles[i].rotation_angle = RandomFloat(0.0f, UTILS_2PI);
	else
		_particles[i].rotation_angle = 0.0f;

	_particles[i].current_keyframe = _system_def->keyframes[0];

	if(_system_def->keyframes.size() > 1)
		_particles[i].next_keyframe = _system_def->keyframes[1];
	else
		_particles[i].next_keyframe = NULL;

	float speed = _system_def->emitter._initial_speed;
	speed += RandomFloat(-emitter._initial_speed_variation, emitter._initial_speed_variation);


	if(_system_def->emitter._spin == EMITTER_SPIN_CLOCKWISE)
	{
		_particles[i].rotation_direction = 1.0f;
	}
	else if(_system_def->emitter._spin == EMITTER_SPIN_COUNTERCLOCKWISE)
	{
		_particles[i].rotation_direction = -1.0f;
	}
	else
	{
		_particles[i].rotation_direction = static_cast<float>(2 * (rand()%2)) - 1.0f;
	}

	// figure out the orientation

	float angle = 0.0f;

	if(emitter._omnidirectional)
	{
		angle = RandomFloat(0.0f, UTILS_2PI);
	}
	else if(emitter._inner_cone == 0.0f && emitter._outer_cone == 0.0f)
	{
		angle = emitter._orientation + params.orientation;
	}

	_particles[i].velocity_x = speed * cosf(angle);
	_particles[i].velocity_y = speed * sinf(angle);

	// figure out property variations

	_particles[i].current_size_variation_x  = RandomFloat(-_system_def->keyframes[0]->size_variation_x, _system_def->keyframes[0]->size_variation_x);
	_particles[i].current_size_variation_y  = RandomFloat(-_system_def->keyframes[0]->size_variation_y, _system_def->keyframes[0]->size_variation_y);

	for(int32 j = 0; j < 4; ++j)
		_particles[i].current_color_variation[j] = RandomFloat(-_system_def->keyframes[0]->color_variation[j], _system_def->keyframes[0]->color_variation[j]);

	_particles[i].current_rotation_speed_variation = RandomFloat(-_system_def->keyframes[0]->rotation_speed_variation, _system_def->keyframes[0]->rotation_speed_variation);

	if(_system_def->keyframes.size() > 1)
	{
		// figure out the next keyframe's variations
		_particles[i].next_size_variation_x  = RandomFloat(-_system_def->keyframes[1]->size_variation_x, _system_def->keyframes[1]->size_variation_x);
		_particles[i].next_size_variation_y  = RandomFloat(-_system_def->keyframes[1]->size_variation_y, _system_def->keyframes[1]->size_variation_y);

		for(int32 j = 0; j < 4; ++j)
			_particles[i].next_color_variation[j] = RandomFloat(-_system_def->keyframes[1]->color_variation[j], _system_def->keyframes[1]->color_variation[j]);

		_particles[i].next_rotation_speed_variation = RandomFloat(-_system_def->keyframes[1]->rotation_speed_variation, _system_def->keyframes[1]->rotation_speed_variation);
	}
	else
	{
		// if there's only 1 keyframe, then apply the variations now
		for(int32 j = 0; j < 4; ++j)
			_particles[i].color[j] += RandomFloat(-_particles[i].current_color_variation[j], _particles[i].current_color_variation[j]);

		_particles[i].size_x += RandomFloat(-_particles[i].current_size_variation_x, _particles[i].current_size_variation_x);
		_particles[i].size_y += RandomFloat(-_particles[i].current_size_variation_y, _particles[i].current_size_variation_y);

		_particles[i].rotation_speed += RandomFloat(-_particles[i].current_rotation_speed_variation, _particles[i].current_rotation_speed_variation);
	}

	_particles[i].tangential_acceleration = _system_def->tangential_acceleration;
	if(_system_def->tangential_acceleration_variation != 0.0f)
		_particles[i].tangential_acceleration += RandomFloat(-_system_def->tangential_acceleration_variation, _system_def->tangential_acceleration_variation);

	_particles[i].radial_acceleration = _system_def->radial_acceleration;
	if(_system_def->radial_acceleration_variation != 0.0f)
		_particles[i].radial_acceleration += RandomFloat(-_system_def->radial_acceleration_variation, _system_def->radial_acceleration_variation);

	_particles[i].acceleration_x = _system_def->acceleration_x;
	if(_system_def->acceleration_variation_x != 0.0f)
		_particles[i].acceleration_x += RandomFloat(-_system_def->acceleration_variation_x, _system_def->acceleration_variation_x);

	_particles[i].acceleration_y = _system_def->acceleration_y;
	if(_system_def->acceleration_variation_y != 0.0f)
		_particles[i].acceleration_y += RandomFloat(-_system_def->acceleration_variation_y, _system_def->acceleration_variation_y);

	_particles[i].wind_velocity_x = _system_def->wind_velocity_x;
	if(_system_def->wind_velocity_variation_x != 0.0f)
		_particles[i].wind_velocity_x += RandomFloat(-_system_def->wind_velocity_variation_x, _system_def->wind_velocity_variation_x);

	_particles[i].wind_velocity_y = _system_def->wind_velocity_y;
	if(_system_def->wind_velocity_variation_y != 0.0f)
		_particles[i].wind_velocity_y += RandomFloat(-_system_def->wind_velocity_variation_y, _system_def->wind_velocity_variation_y);

	_particles[i].damping = _system_def->damping;
	if(_system_def->damping_variation != 0.0f)
		_particles[i].damping += RandomFloat(-_system_def->damping_variation, _system_def->damping_variation);

	if(_system_def->wave_motion_used)
	{
		_particles[i].wave_length_coefficient = _system_def->wave_length;
		if(_system_def->wave_length_variation != 0.0f)
			_particles[i].wave_length_coefficient += RandomFloat(-_system_def->wave_length_variation, _system_def->wave_length_variation);

		_particles[i].wave_length_coefficient = UTILS_2PI / _particles[i].wave_length_coefficient;

		_particles[i].wave_half_amplitude = _system_def->wave_amplitude;
		if(_system_def->wave_amplitude != 0.0f)
			_particles[i].wave_half_amplitude += RandomFloat(-_system_def->wave_amplitude_variation, _system_def->wave_amplitude_variation);
		_particles[i].wave_half_amplitude *= 0.5f;
	}

	_particles[i].lifetime = _system_def->particle_lifetime + RandomFloat(-_system_def->particle_lifetime_variation, _system_def->particle_lifetime_variation);
}


//-----------------------------------------------------------------------------
// GetAge: return the number of seconds since this system was created
//-----------------------------------------------------------------------------

float ParticleSystem::GetAge() const
{
	return _age;
}



}  // namespace private_video
}  // namespace hoa_video
