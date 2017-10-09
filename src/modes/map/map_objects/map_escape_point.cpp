///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_objects/map_escape_point.h"

#include "modes/map/map_objects/map_particle.h"
#include "modes/map/map_mode.h"
#include "modes/map/map_object_supervisor.h"

#include "engine/audio/audio.h"

namespace vt_map
{

namespace private_map
{

const std::string ESCAPE_POINT_ACTIVATION_SOUND = "data/sounds/escape_activate.ogg";
const std::string ESCAPE_POINT_ACTIVE_PARTICLE_EFFECT = "data/visuals/particle_effects/active_escape_point.lua";
const std::string ESCAPE_POINT_INACTIVE_PARTICLE_EFFECT = "data/visuals/particle_effects/inactive_escape_point.lua";

EscapePoint::EscapePoint(float x, float y):
    MapObject(NO_LAYER_OBJECT), // This is a special object
    _animation(nullptr),
    _is_active(false)
{
    _tile_position.x = x;
    _tile_position.y = y;

    _object_type = ESCAPE_TYPE;
    _collision_mask = NO_COLLISION;

    MapMode* map_mode = MapMode::CurrentInstance();
    _animation = &map_mode->inactive_escape_point_anim;

    // Set the collision rectangle according to the dimensions of the first frame
    // Remove a margin to the save point so that the character has to actually
    // enter the save point before colliding with it.
    // Note: We divide by the map zoom ratio because the animation are already rescaled following it.
    SetCollPixelHalfWidth((_animation->GetWidth() / MAP_ZOOM_RATIO) / 2.0f);
    SetCollPixelHeight((_animation->GetHeight() / MAP_ZOOM_RATIO)
                       - 0.3f * GRID_LENGTH);

    // Setup the image collision for the display update
    SetImgPixelHalfWidth(_animation->GetWidth() / MAP_ZOOM_RATIO / 2.0f);
    SetImgPixelHeight(_animation->GetHeight() / MAP_ZOOM_RATIO);

    // Preload the escape active sound
    vt_audio::AudioManager->LoadSound(ESCAPE_POINT_ACTIVATION_SOUND, map_mode);

    // The escape point is going along with two particle objects used to show
    // whether the player is in or out the save point
    _active_particle_object =
        new ParticleObject(ESCAPE_POINT_ACTIVE_PARTICLE_EFFECT,
                           x, y, GROUND_OBJECT);
    _inactive_particle_object =
        new ParticleObject(ESCAPE_POINT_INACTIVE_PARTICLE_EFFECT,
                           x, y, GROUND_OBJECT);

    _active_particle_object->Stop();

    // Auto-registers to the Object supervisor for later deletion handling.
    map_mode->GetObjectSupervisor()->AddEscapePoint(this);
}

EscapePoint* EscapePoint::Create(float x, float y)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new EscapePoint(x, y);
}

void EscapePoint::Update()
{
    if(!_animation || !_updatable)
        return;

    _animation->Update();
}

void EscapePoint::Draw()
{
    if(!_animation || !MapObject::ShouldDraw())
        return;

    _animation->Draw();
}

void EscapePoint::SetActive(bool active)
{
    if(active) {
        _animation = &MapMode::CurrentInstance()->active_escape_point_anim;
        _active_particle_object->Start();
        _inactive_particle_object->Stop();

        // Play a sound when the save point become active
        if(!_is_active)
            vt_audio::AudioManager->PlaySound(ESCAPE_POINT_ACTIVATION_SOUND);
    } else {
        _animation = &MapMode::CurrentInstance()->inactive_escape_point_anim;
        _active_particle_object->Stop();
        _inactive_particle_object->Start();
    }
    _is_active = active;
}

} // namespace private_map

} // namespace vt_map
