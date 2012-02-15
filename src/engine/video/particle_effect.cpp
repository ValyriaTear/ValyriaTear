///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software 
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "video.h"
#include "script.h"

#include "particle_effect.h"
#include "particle_system.h"

using namespace std;
using namespace hoa_script;
using namespace hoa_video::private_video;

namespace hoa_video
{


//-----------------------------------------------------------------------------
// ParticleEffect
//-----------------------------------------------------------------------------

ParticleEffect::ParticleEffect()
{
	_alive = false;
	_x = _y = 0.0f;
	_attractor_x = _attractor_y = 0.0f;
	_age = 0.0f;
	_effect_def = NULL;
	_orientation = 0.0f;
}


//-----------------------------------------------------------------------------
// _Draw: draws the effect. This is called by ParticleManager, not by API user
//-----------------------------------------------------------------------------

bool ParticleEffect::_Draw()
{
	bool success = true;

	// move to the effect's location
	VideoManager->Move(_x, _y);

	list<ParticleSystem *>::iterator iSystem = _systems.begin();
	
	while(iSystem != _systems.end())
	{
		VideoManager->PushMatrix();
		if(!(*iSystem)->Draw())
		{
			VideoManager->PopMatrix();
			success = false;			
			if(VIDEO_DEBUG)
				cerr << "VIDEO ERROR: failed to draw system in ParticleEffect::_Update()" << endl;
		}		
		
		VideoManager->PopMatrix();
		++iSystem;
	}
	
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_ALPHA_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	
	return success;
}


//-----------------------------------------------------------------------------
// Update: updates particle effect. Called by ParticleManager, not by user.
//-----------------------------------------------------------------------------

bool ParticleEffect::_Update(float frame_time)
{
	_age += frame_time;
	_num_particles = 0;
	
	if(!_alive)
		return true;
	
	bool success = true;

	private_video::EffectParameters effect_parameters;
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
				if(VIDEO_DEBUG)
					cerr << "VIDEO ERROR: failed to update system in ParticleEffect::_Update()" << endl;				
			}
			
			_num_particles += (*iSystem)->GetNumParticles();			
			++iSystem;
		}
	}

	return success;
}


//-----------------------------------------------------------------------------
// Destroy: destroys the effect. Called by ParticleManager during _Update(), if
//          this effect is not alive (i.e. IsAlive() returns false)
//-----------------------------------------------------------------------------

void ParticleEffect::_Destroy()
{
	list<ParticleSystem *>::iterator iSystem = _systems.begin();
	
	while(iSystem != _systems.end())
	{
		(*iSystem)->Destroy();
		++iSystem;
	}
}


//-----------------------------------------------------------------------------
// Move: function for the API user to move the effect around to different
//       locations
//-----------------------------------------------------------------------------

void ParticleEffect::Move(float x, float y)
{
	_x = x;
	_y = y;
}


//-----------------------------------------------------------------------------
// MoveRelative: function for API user to move the effect relative to its current
//            position
//-----------------------------------------------------------------------------

void ParticleEffect::MoveRelative(float dx, float dy)
{
	_x += dx;
	_y += dy;
}


//-----------------------------------------------------------------------------
// SetOrientation: sets the orientation of the effect, which is added to the
//                 orientation for each of the emitters
//-----------------------------------------------------------------------------

void ParticleEffect::SetOrientation(float angle)
{
	_orientation = angle;
}


//-----------------------------------------------------------------------------
// SetAttractorPoint: set the location of an attractor point. Any systems with
//                    radial acceleration and user-defined attractor points
//                    enabled will have particles gravitate toward this point
//-----------------------------------------------------------------------------

void ParticleEffect::SetAttractorPoint(float x, float y)
{
	_attractor_x = x;
	_attractor_y = y;
}


//-----------------------------------------------------------------------------
// IsAlive: returns true if effect is still alive. Used by ParticleManager
//          so it knows when to destroy an effect.
//-----------------------------------------------------------------------------

bool ParticleEffect::IsAlive()
{
	return _alive;
}


//-----------------------------------------------------------------------------
// Stop: stops the effect. If kill_immediate is true, then the effect is stopped
//       immediately. Otherwise, the particles which are still alive are allowed
//       to keep drawing, but no new particles are emitted.
//-----------------------------------------------------------------------------

void ParticleEffect::Stop(bool kill_immediate)
{
	if(kill_immediate)
	{
		// just set _alive to false, and the particle manager will remove it for us
		_alive = false;
	}
	else
	{
		// if we're not killing immediately, then calling Stop() just means to stop emitting NEW
		// particles, so go through each system and turn off its emitter
		
		list<ParticleSystem *>::iterator iSystem = _systems.begin();
		
		while(iSystem != _systems.end())
		{
			(*iSystem)->Stop();
			++iSystem;
		}		
	}
}


//-----------------------------------------------------------------------------
// GetNumParticles: returns the number of live particles for the entire effect
//-----------------------------------------------------------------------------

int32 ParticleEffect::GetNumParticles() const
{
	return _num_particles;	
}


//-----------------------------------------------------------------------------
// GetPosition: returns the current position of the effect
//-----------------------------------------------------------------------------

void ParticleEffect::GetPosition(float &x, float &y) const
{
	x = _x;
	y = _y;
}


//-----------------------------------------------------------------------------
// GetAge: returns the current age of the effect, i.e. how many seconds it has
//         been since the effect was created
//-----------------------------------------------------------------------------

float ParticleEffect::GetAge() const
{
	return _age;
}


} // namespace hoa_video
