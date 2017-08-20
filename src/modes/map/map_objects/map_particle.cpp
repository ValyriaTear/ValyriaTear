///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_objects/map_particle.h"

#include "engine/video/particle_effect.h"

#include "engine/video/video.h"

using namespace vt_common;

namespace vt_map
{

namespace private_map
{

ParticleObject::ParticleObject(const std::string& filename,
                               float x,
                               float y,
                               MapObjectDrawLayer layer):
    MapObject(layer)
{
    _tile_position.x = x;
    _tile_position.y = y;

    _object_type = PARTICLE_TYPE;
    _collision_mask = NO_COLLISION;

    _particle_effect = new vt_mode_manager::ParticleEffect(filename);
    if(!_particle_effect)
        return;

    SetCollPixelHalfWidth(_particle_effect->GetEffectCollisionWidth() / 2.0f);
    SetCollPixelHeight(_particle_effect->GetEffectCollisionHeight());

    // Setup the image collision for the display update
    SetImgPixelHalfWidth(_particle_effect->GetEffectWidth() / 2.0f);
    SetImgPixelHeight(_particle_effect->GetEffectHeight());
}

ParticleObject::~ParticleObject()
{
    // We have to delete the particle effect since we don't register it
    // to the ParticleManager.
    delete _particle_effect;
}

ParticleObject* ParticleObject::Create(const std::string &filename,
                                       float x,
                                       float y,
                                       MapObjectDrawLayer layer)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new ParticleObject(filename, x, y, layer);
}

bool ParticleObject::Stop()
{
    if(_particle_effect)
    {
        _particle_effect->Stop();
        return true;
    }
    return false;
}

bool ParticleObject::Start()
{
    if(_particle_effect)
        return _particle_effect->Start();
    return false;
}

bool ParticleObject::IsAlive() const
{
    if (_particle_effect)
        return _particle_effect->IsAlive();
    return false;
}

bool ParticleObject::Update()
{
    if(!_particle_effect || !_updatable)
        return false;

    _particle_effect->Update();
    return true;
}

bool ParticleObject::Draw()
{
    if(!_particle_effect || !MapObject::ShouldDraw())
        return false;

    Position2D position = vt_video::VideoManager->GetDrawPosition();
    vt_video::VideoManager->SetStandardCoordSys();
    _particle_effect->Move(position.x, position.y);
    _particle_effect->Draw();
    // Reset the map mode coord sys afterward.

    // Draw collision rectangle if the debug view is on.
    if(!vt_video::VideoManager->DebugInfoOn())
        return true;

    Rectangle2D rect = GetScreenImageRectangle();
    vt_video::VideoManager->DrawRectangle(rect.right - rect.left,
                                          rect.bottom - rect.top,
                                          vt_video::Color(0.0f, 1.0f, 1.0f, 0.6f));
    rect = GetScreenCollisionRectangle();
    vt_video::VideoManager->DrawRectangle(rect.right - rect.left,
                                          rect.bottom - rect.top,
                                          vt_video::Color(0.0f, 0.0f, 1.0f, 0.5f));
    return true;
}

} // namespace private_map

} // namespace vt_map
