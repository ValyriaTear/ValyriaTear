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
*** \file    particle_manager.cpp
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for particle manager
*** **************************************************************************/

#include "utils/utils_pch.h"
#include "engine/video/particle_manager.h"

#include "engine/video/video.h"
#include "engine/video/transform2d.h"
#include "engine/video/particle_effect.h"

using namespace vt_script;
using namespace vt_video;

namespace vt_mode_manager
{

bool ParticleManager::AddParticleEffect(const std::string &effect_filename, float x, float y)
{

    ParticleEffect *effect = new ParticleEffect(effect_filename);
    if(!effect->IsLoaded()) {
        PRINT_WARNING << "Failed to add effect to particle manager" <<
                      " for file: " << effect_filename << std::endl;
        delete effect;
        return false;
    }

    effect->Move(x, y);
    _all_effects.push_back(effect);
    _active_effects.push_back(effect);

    return true;
}

void ParticleManager::_DEBUG_ShowParticleStats()
{
    char text[50];
    sprintf(text, "Particles: %d", GetNumParticles());

    TextManager->Draw(text, Transform2D(896.0f, 690.0f));
}

bool ParticleManager::Draw()
{
    VideoManager->PushState();
    VideoManager->SetStandardCoordSys();
    VideoManager->DisableScissoring();

    std::vector<ParticleEffect *>::iterator it = _active_effects.begin();

    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);

    bool success = true;

    while(it != _active_effects.end()) {
        if(!(*it)->Draw()) {
            success = false;
            IF_PRINT_WARNING(VIDEO_DEBUG)
                    << "Effect failed to draw!" << std::endl;
        }
        ++it;
    }

    VideoManager->PopState();
    return success;
}

bool ParticleManager::Update(int32 frame_time)
{
    float frame_time_seconds = static_cast<float>(frame_time) / 1000.0f;

    std::vector<ParticleEffect *>::iterator it = _active_effects.begin();

    bool success = true;

    _num_particles = 0;

    while(it != _active_effects.end()) {
        if(!(*it)->IsAlive()) {
            it = _active_effects.erase(it);
        } else {
            if(!(*it)->Update(frame_time_seconds)) {
                success = false;
                IF_PRINT_WARNING(VIDEO_DEBUG)
                        << "Effect failed to update!" << std::endl;
            }

            _num_particles += (*it)->GetNumParticles();
            ++it;
        }
    }

    return success;
}

void ParticleManager::StopAll(bool kill_immediate)
{
    std::vector<ParticleEffect *>::iterator it = _active_effects.begin();

    while(it != _active_effects.end()) {
        (*it)->Stop(kill_immediate);
        ++it;
    }
}

void ParticleManager::_Destroy()
{
    // Clear out every effects.
    std::vector<ParticleEffect *>::iterator it = _all_effects.begin();
    for(; it != _all_effects.end(); ++it) {
        delete(*it);
    }
    _all_effects.clear();
    // Clear the active effect pointer references
    _active_effects.clear();
}

}  // namespace vt_mode_manager
