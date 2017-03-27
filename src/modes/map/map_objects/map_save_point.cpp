///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_objects/map_save_point.h"

#include "modes/map/map_objects/map_particle.h"
#include "modes/map/map_object_supervisor.h"

#include "engine/audio/audio.h"

namespace vt_map
{

namespace private_map
{

const std::string SAVE_POINT_ACTIVATION_SOUND = "data/sounds/save_point_activated_dokashiteru_oga.wav";
const std::string SAVE_POINT_ACTIVE_PARTICLE_EFFECT = "data/visuals/particle_effects/active_save_point.lua";
const std::string SAVE_POINT_INACTIVE_PARTICLE_EFFECT = "data/visuals/particle_effects/inactive_save_point.lua";

SavePoint::SavePoint(float x, float y):
    MapObject(NO_LAYER_OBJECT), // This is a special object
    _animations(nullptr),
    _is_active(false)
{
    _tile_position.x = x;
    _tile_position.y = y;

    _object_type = SAVE_TYPE;
    _collision_mask = NO_COLLISION;

    MapMode* map_mode = MapMode::CurrentInstance();
    _animations = &map_mode->inactive_save_point_animations;

    // Set the collision rectangle according to the dimensions of the first frame
    // Remove a margin to the save point so that the character has to actually
    // enter the save point before colliding with it.
    // Note: We divide by the map zoom ratio because the animation are already rescaled following it.
    SetCollPixelHalfWidth((_animations->at(0).GetWidth() / MAP_ZOOM_RATIO) / 2.0f);
    SetCollPixelHeight((_animations->at(0).GetHeight() / MAP_ZOOM_RATIO) - 0.3f * GRID_LENGTH);

    // Setup the image collision for the display update
    SetImgPixelHalfWidth(_animations->at(0).GetWidth() / MAP_ZOOM_RATIO / 2.0f);
    SetImgPixelHeight(_animations->at(0).GetHeight() / MAP_ZOOM_RATIO);

    // Preload the save active sound
    vt_audio::AudioManager->LoadSound(SAVE_POINT_ACTIVATION_SOUND, map_mode);

    // The save point is going along with two particle objects used to show
    // whether the player is in or out the save point
    _active_particle_object = new ParticleObject(SAVE_POINT_ACTIVE_PARTICLE_EFFECT,
                                                 x, y, GROUND_OBJECT);
    _inactive_particle_object = new ParticleObject(SAVE_POINT_INACTIVE_PARTICLE_EFFECT,
                                                   x, y, GROUND_OBJECT);

    _active_particle_object->Stop();

    // Auto-registers to the Object supervisor for later deletion handling.
    map_mode->GetObjectSupervisor()->AddSavePoint(this);
}

SavePoint* SavePoint::Create(float x, float y)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new SavePoint(x, y);
}

void SavePoint::Update()
{
    if(!_animations || !_updatable)
        return;

    for(uint32_t i = 0; i < _animations->size(); ++i)
        _animations->at(i).Update();
}


void SavePoint::Draw()
{
    if(!_animations || !MapObject::ShouldDraw())
        return;

    for(uint32_t i = 0; i < _animations->size(); ++i)
        _animations->at(i).Draw();
}

void SavePoint::SetActive(bool active)
{
    if(active) {
        _animations = &MapMode::CurrentInstance()->active_save_point_animations;
        _active_particle_object->Start();
        _inactive_particle_object->Stop();

        // Play a sound when the save point become active
        if(!_is_active)
            vt_audio::AudioManager->PlaySound(SAVE_POINT_ACTIVATION_SOUND);
    } else {
        _animations = &MapMode::CurrentInstance()->inactive_save_point_animations;
        _active_particle_object->Stop();
        _inactive_particle_object->Start();
    }
    _is_active = active;
}

} // namespace private_map

} // namespace vt_map
