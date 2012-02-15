///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software 
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "video.h"

using namespace std;
using namespace hoa_video::private_video;

namespace hoa_video 
{


//-----------------------------------------------------------------------------
// AddParticleEffect: adds a new particle effect to the particle manager. Once you
//                    call this function, you don't have to do any more work. The
//                    effect will display until it's over, and then it will automatically
//                    get destroyed by the particle manager. However, an ID is returned
//                    by this function in case you want to do things like move the
//                    position of the effect around. (for exmaple, if you want to
//                    attach a flame effect to a sword or something)
//-----------------------------------------------------------------------------

ParticleEffectID VideoEngine::AddParticleEffect(const string &filename, float x, float y, bool reload)
{
	ParticleEffectDef *def = NULL;
	
	bool effect_loaded = _particle_effect_defs.find(filename) != _particle_effect_defs.end();
		
	if(effect_loaded && !reload)
	{
		def = _particle_effect_defs[filename];
	}
	else
	{
		def = _particle_manager.LoadEffect(filename);		
		if(effect_loaded)
			delete _particle_effect_defs[filename];			
		_particle_effect_defs[filename] = def;
	}
	
	if(!def)
	{
		if(VIDEO_DEBUG)
			cerr << "VIDEO ERROR: failed to load particle definition file: " << filename << endl;
		return VIDEO_INVALID_EFFECT;
	}
		
	ParticleEffectID id = _particle_manager.AddEffect(def, x, y);
	
	if(id == VIDEO_INVALID_EFFECT)
	{
		if(VIDEO_DEBUG)
			cerr << "VIDEO ERROR: failed to add effect to particle manager in VideoEngine::AddParticleEffect()!" << endl;
	}
	
	return id;
}


//-----------------------------------------------------------------------------
// DrawParticleEffects: call this once per frame. You should call this after
//                      rendering things like tiles, characters, and monsters,
//                      but before rendering the GUI.
//-----------------------------------------------------------------------------

bool VideoEngine::DrawParticleEffects()
{
	return _particle_manager.Draw();
}


//-----------------------------------------------------------------------------
// StopAllParticleEffects: stops all current particle effects. Pass true if
//                         you want them to all stop immediately, or false (Default)
//                         to simply stop emitting new particles
//-----------------------------------------------------------------------------

void VideoEngine::StopAllParticleEffects(bool kill_immediate)
{
	return _particle_manager.StopAll();
}


//-----------------------------------------------------------------------------
// GetParticleEffect: returns the ParticleEffect object for an id. If the
//                    particle system which had that id has expired already,
//                    or an invalid id is passed, then NULL is returned.
//-----------------------------------------------------------------------------

ParticleEffect *VideoEngine::GetParticleEffect(ParticleEffectID id)
{
	return _particle_manager.GetEffect(id);
}


//-----------------------------------------------------------------------------
// GetNumParticles: returns the total number of particles being displayed on
//                  screen
//-----------------------------------------------------------------------------

int32 VideoEngine::GetNumParticles()
{
	return _particle_manager.GetNumParticles();
}




}  // namespace hoa_video
