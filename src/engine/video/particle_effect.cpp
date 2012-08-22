///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "engine/video/video.h"
#include "engine/script/script.h"

#include "engine/video/particle_effect.h"
#include "engine/video/particle_system.h"

using namespace std;
using namespace hoa_script;
using namespace hoa_video;

namespace hoa_mode_manager
{

ParticleEffect::ParticleEffect()
{
	_alive = false;
	_x = _y = 0.0f;
	_attractor_x = _attractor_y = 0.0f;
	_age = 0.0f;
	_effect_def = NULL;
	_orientation = 0.0f;
}

bool ParticleEffect::_Draw()
{
	bool success = true;

	// move to the effect's location
	VideoManager->Move(_x, _y);

	std::list<ParticleSystem *>::iterator iSystem = _systems.begin();

	while(iSystem != _systems.end())
	{
		VideoManager->PushMatrix();
		if(!(*iSystem)->Draw())
		{
			success = false;
			IF_PRINT_WARNING(VIDEO_DEBUG)
				<< "Failed to draw system!" << endl;
		}

		VideoManager->PopMatrix();
		++iSystem;
	}

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_ALPHA_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	return success;
}


bool ParticleEffect::_Update(float frame_time)
{
	_age += frame_time;
	_num_particles = 0;

	if(!_alive)
		return true;

	bool success = true;

	hoa_mode_manager::EffectParameters effect_parameters;
	effect_parameters.orientation = _orientation;

	// note we subtract the effect position to put the attractor point in effect
	// space instead of screen space
	effect_parameters.attractor_x = _attractor_x - _x;
	effect_parameters.attractor_y = _attractor_y - _y;

	list<ParticleSystem *>::iterator iSystem = _systems.begin();

	while(iSystem != _systems.end())
	{
		if(!(*iSystem)->IsAlive())
		{
			(*iSystem)->Destroy();
			iSystem = _systems.erase(iSystem);

			if(_systems.empty())
				_alive = false;
		}
		else
		{
			if(!(*iSystem)->Update(frame_time, effect_parameters))
			{
				success = false;
				IF_PRINT_WARNING(VIDEO_DEBUG)
					<< "Failed to update system!" << endl;
			}

			_num_particles += (*iSystem)->GetNumParticles();
			++iSystem;
		}
	}

	return success;
}


void ParticleEffect::_Destroy()
{
	list<ParticleSystem *>::iterator iSystem = _systems.begin();

	while(iSystem != _systems.end())
	{
		(*iSystem)->Destroy();
		++iSystem;
	}
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


void ParticleEffect::SetOrientation(float angle)
{
	_orientation = angle;
}


void ParticleEffect::SetAttractorPoint(float x, float y)
{
	_attractor_x = x;
	_attractor_y = y;
}


bool ParticleEffect::IsAlive()
{
	return _alive;
}

void ParticleEffect::Stop(bool kill_immediate)
{
	if(kill_immediate)
	{
		// just set _alive to false, and the particle manager will remove it for us
		_alive = false;
		// TODO: Permit immediate kill without the particle manager
	}
	else
	{
		// if we're not killing immediately, then calling Stop() just means to stop emitting NEW
		// particles, so go through each system and turn off its emitter

		std::list<ParticleSystem *>::iterator iSystem = _systems.begin();

		while(iSystem != _systems.end())
		{
			(*iSystem)->Stop();
			++iSystem;
		}
	}
}

int32 ParticleEffect::GetNumParticles() const
{
	return _num_particles;
}

void ParticleEffect::GetPosition(float &x, float &y) const
{
	x = _x;
	y = _y;
}

float ParticleEffect::GetAge() const
{
	return _age;
}

} // namespace hoa_video
