///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_objects.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for map mode objects.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "modes/map/map_objects.h"

#include "modes/map/map_mode.h"
#include "modes/map/map_sprites.h"
#include "modes/map/map_events.h"

#include "common/global/global.h"

#include "engine/video/particle_effect.h"
#include "engine/audio/audio.h"

#include "utils/utils_random.h"

using namespace vt_utils;
using namespace vt_audio;
using namespace vt_script;
using namespace vt_system;
using namespace vt_video;
using namespace vt_global;

namespace vt_map
{

namespace private_map
{

// ----------------------------------------------------------------------------
// ---------- MapObject Class Functions
// ----------------------------------------------------------------------------

MapObject::MapObject() :
    object_id(-1),
    img_half_width(0.0f),
    img_height(0.0f),
    coll_half_width(0.0f),
    coll_height(0.0f),
    updatable(true),
    visible(true),
    collision_mask(ALL_COLLISION),
    sky_object(false),
    draw_on_second_pass(false),
    _object_type(OBJECT_TYPE),
    _emote_animation(0),
    _emote_offset_x(0.0f),
    _emote_offset_y(0.0f),
    _emote_time(0)
{}

bool MapObject::ShouldDraw()
{
    if(!visible)
        return false;

    MapMode *map = MapMode::CurrentInstance();

    // Determine if the sprite is off-screen and if so, don't draw it.
    if(!MapRectangle::CheckIntersection(GetImageRectangle(), map->GetMapFrame().screen_edges))
        return false;

    // Determine the center position coordinates for the camera
    float x_pos, y_pos; // Holds the final X, Y coordinates of the camera
    float x_pixel_length, y_pixel_length; // The X and Y length values that coorespond to a single pixel in the current coodinate system
    float rounded_x_offset, rounded_y_offset; // The X and Y position offsets of the object, rounded to perfectly align on a pixel boundary


    // TODO: the call to GetPixelSize() will return the same result every time so long as the coordinate system did not change. If we never
    // change the coordinate system in map mode, then this should be done only once and the calculated values should be saved for re-use.
    // However, we've discussed the possiblity of adding a zoom feature to maps, in which case we need to continually re-calculate the pixel size
    VideoManager->GetPixelSize(x_pixel_length, y_pixel_length);
    rounded_x_offset = FloorToFloatMultiple(GetFloatFraction(GetXPosition()), x_pixel_length);
    rounded_y_offset = FloorToFloatMultiple(GetFloatFraction(GetYPosition()), y_pixel_length);
    x_pos = static_cast<float>(GetFloatInteger(GetXPosition())) + rounded_x_offset;
    y_pos = static_cast<float>(GetFloatInteger(GetYPosition())) + rounded_y_offset;

    // ---------- Move the drawing cursor to the appropriate coordinates for this sprite
    VideoManager->Move(x_pos - map->GetMapFrame().screen_edges.left,
                       y_pos - map->GetMapFrame().screen_edges.top);
    return true;
} // bool MapObject::ShouldDraw()

MapRectangle MapObject::GetCollisionRectangle() const
{
    MapRectangle rect;
    rect.left = position.x - coll_half_width;
    rect.right = position.x + coll_half_width;
    rect.top = position.y - coll_height;
    rect.bottom = position.y;

    return rect;
}

MapRectangle MapObject::GetCollisionRectangle(float x, float y) const
{
    MapRectangle rect;
    rect.left = x - coll_half_width;
    rect.right = x + coll_half_width;
    rect.top = y - coll_height;
    rect.bottom = y;
    return rect;
}

MapRectangle MapObject::GetImageRectangle() const
{
    MapRectangle rect;
    rect.left = position.x - img_half_width;
    rect.right = position.x + img_half_width;
    rect.top = position.y - img_height;
    rect.bottom = position.y;
    return rect;
}

void MapObject::Emote(const std::string &emote_name, vt_map::private_map::ANIM_DIRECTIONS dir)
{
    _emote_animation = GlobalManager->GetEmoteAnimation(emote_name);

    if(!_emote_animation) {
        PRINT_WARNING << "Invalid emote requested: " << emote_name << " for map object: "
                      << GetObjectID() << std::endl;
        return;
    }

    // Make the offset depend on the sprite direction and emote animation.
    GlobalManager->GetEmoteOffset(_emote_offset_x, _emote_offset_y, emote_name, dir);
    // Scale the offsets for the map mode
    _emote_offset_x = _emote_offset_x / (private_map::GRID_LENGTH / 2);
    _emote_offset_y = _emote_offset_y / (private_map::GRID_LENGTH / 2);

    _emote_animation->ResetAnimation();
    _emote_time = _emote_animation->GetAnimationLength();
}

void MapObject::_UpdateEmote()
{
    if(!_emote_animation)
        return;

    _emote_time -= SystemManager->GetUpdateTime();

    // Once the animation has reached its end, we dereference it
    if(_emote_time <= 0) {
        _emote_animation = 0;
        return;
    }

    // Otherwise, just update it
    _emote_animation->Update();
}

void MapObject::_DrawEmote()
{
    if(!_emote_animation)
        return;

    // Move the emote to the sprite head top, where the offset should applied from.
    VideoManager->MoveRelative(_emote_offset_x, -img_height + _emote_offset_y);
    _emote_animation->Draw();
}

bool MapObject::IsColliding(float x, float y)
{
    ObjectSupervisor* obj_sup = MapMode::CurrentInstance()->GetObjectSupervisor();
    return obj_sup->DetectCollision(this, x, y);
}

bool MapObject::IsCollidingWith(MapObject* other_object)
{
     if (!other_object)
        return false;

     if (collision_mask == NO_COLLISION)
        return false;

     if (other_object->collision_mask == NO_COLLISION)
        return false;

    MapRectangle other_rect = other_object->GetCollisionRectangle();

    if (!MapRectangle::CheckIntersection(GetCollisionRectangle(), other_rect))
        return false;

    return collision_mask & other_object->collision_mask;
}

// ----------------------------------------------------------------------------
// ---------- PhysicalObject Class Functions
// ----------------------------------------------------------------------------

PhysicalObject::PhysicalObject() :
    current_animation(0)
{
    MapObject::_object_type = PHYSICAL_TYPE;
}

PhysicalObject::~PhysicalObject()
{
    animations.clear();
}

void PhysicalObject::Update()
{
    if(!animations.empty() && updatable)
        animations[current_animation].Update();
}

void PhysicalObject::Draw()
{
    if(!animations.empty() && MapObject::ShouldDraw()) {
        animations[current_animation].Draw();

        // Draw collision rectangle if the debug view is on.
        if(VideoManager->DebugInfoOn()) {
            float x, y = 0.0f;
            VideoManager->GetDrawPosition(x, y);
            MapRectangle rect = GetCollisionRectangle(x, y);
            VideoManager->DrawRectangle(rect.right - rect.left, rect.bottom - rect.top, Color(0.0f, 1.0f, 0.0f, 0.6f));
        }
    }
}

int32 PhysicalObject::AddAnimation(const std::string &animation_filename)
{
    AnimatedImage new_animation;
    if(!new_animation.LoadFromAnimationScript(animation_filename)) {
        PRINT_WARNING << "Could not add animation because the animation filename was invalid: "
                      << animation_filename << std::endl;
        return -1;
    }
    new_animation.SetDimensions(img_half_width * 2, img_height);

    animations.push_back(new_animation);
    return (int32)animations.size() - 1;
}

int32 PhysicalObject::AddStillFrame(const std::string &image_filename)
{
    AnimatedImage new_animation;
    if(!new_animation.AddFrame(image_filename, 100000)) {
        PRINT_WARNING << "Could not add a still frame because the image filename was invalid: "
                      << image_filename << std::endl;
        return -1;
    }
    new_animation.SetDimensions(img_half_width * 2, img_height);

    animations.push_back(new_animation);
    return (int32)animations.size() - 1;
}

void PhysicalObject::SetCurrentAnimation(uint32 animation_id)
{
    if(animation_id < animations.size()) {
        animations[current_animation].SetTimeProgress(0);
        current_animation = animation_id;
    }
}

// Particle object
ParticleObject::ParticleObject(const std::string &filename, float x, float y):
    MapObject()
{
    position.x = x;
    position.y = y;

    _object_type = PARTICLE_TYPE;
    collision_mask = NO_COLLISION;

    _particle_effect = new vt_mode_manager::ParticleEffect(filename);
    if(!_particle_effect)
        return;

    SetCollHalfWidth(_particle_effect->GetEffectWidth() / 2.0f / (GRID_LENGTH * 0.5f));
    SetCollHeight(_particle_effect->GetEffectHeight() / (GRID_LENGTH * 0.5f));

    // Setup the image collision for the display update
    SetImgHalfWidth(_particle_effect->GetEffectWidth() / 2.0f / (GRID_LENGTH * 0.5f));
    SetImgHeight(_particle_effect->GetEffectHeight() / (GRID_LENGTH * 0.5f));
}

ParticleObject::~ParticleObject()
{
    // We have to delete the particle effect since we don't register it
    // to the ParticleManager.
    delete _particle_effect;
}

void ParticleObject::Stop()
{
    if(_particle_effect)
        _particle_effect->Stop();
}

bool ParticleObject::Start()
{
    if(_particle_effect)
        return _particle_effect->Start();
    return false;
}

void ParticleObject::Update()
{
    if(!_particle_effect)
        return;

    if(updatable)
        _particle_effect->Update();
}

void ParticleObject::Draw()
{
    if(!_particle_effect)
        return;

    if(MapObject::ShouldDraw()) {
        float standard_pos_x, standard_pos_y;
        VideoManager->GetDrawPosition(standard_pos_x, standard_pos_y);
        VideoManager->SetStandardCoordSys();
        _particle_effect->Move(standard_pos_x / SCREEN_GRID_X_LENGTH * VIDEO_STANDARD_RES_WIDTH,
                               standard_pos_y / SCREEN_GRID_Y_LENGTH * VIDEO_STANDARD_RES_HEIGHT);
        _particle_effect->Draw();
        // Reset the map mode coord sys afterward.
        VideoManager->SetCoordSys(0.0f, SCREEN_GRID_X_LENGTH, SCREEN_GRID_Y_LENGTH, 0.0f);

        // Draw collision rectangle if the debug view is on.
        if(VideoManager->DebugInfoOn()) {
            VideoManager->Move(standard_pos_x, standard_pos_y);
            MapRectangle rect = GetImageRectangle();
            VideoManager->DrawRectangle(rect.right - rect.left, rect.bottom - rect.top, Color(0.0f, 1.0f, 1.0f, 0.6f));
        }
    }
}

// Save points
SavePoint::SavePoint(float x, float y):
    MapObject(),
    _animations(0),
    _save_active(false)
{
    position.x = x;
    position.y = y;

    _object_type = SAVE_TYPE;
    collision_mask = NO_COLLISION;

    _animations = &MapMode::CurrentInstance()->inactive_save_point_animations;

    // Set the collision rectangle according to the dimensions of the first frame
    // Remove a margin to the save point so that the character has to actually
    // enter the save point before colliding with it.
    SetCollHalfWidth((_animations->at(0).GetWidth() - 1.0f) / 2.0f);
    SetCollHeight(_animations->at(0).GetHeight() - 0.3f);

    // Setup the image collision for the display update
    SetImgHalfWidth(_animations->at(0).GetWidth() / 2.0f);
    SetImgHeight(_animations->at(0).GetHeight());

    MapMode *map_mode = MapMode::CurrentInstance();

    // Preload the save active sound
    AudioManager->LoadSound("snd/save_point_activated_dokashiteru_oga.wav", map_mode);

    // The save point is going along with two particle objects used to show
    // whether the player is in or out the save point
    _active_particle_object = new ParticleObject("dat/effects/particles/active_save_point.lua", x, y);
    _inactive_particle_object = new ParticleObject("dat/effects/particles/inactive_save_point.lua", x, y);

    _active_particle_object->Stop();

    _active_particle_object->SetObjectID(map_mode->GetObjectSupervisor()->GenerateObjectID());
    _inactive_particle_object->SetObjectID(map_mode->GetObjectSupervisor()->GenerateObjectID());

    map_mode->AddGroundObject(_active_particle_object);
    map_mode->AddGroundObject(_inactive_particle_object);

}

void SavePoint::Update()
{
    if(!_animations)
        return;

    if(updatable) {
        for(uint32 i = 0; i < _animations->size(); ++i)
            _animations->at(i).Update();
    }
}


void SavePoint::Draw()
{
    if(!_animations)
        return;

    if(MapObject::ShouldDraw()) {
        for(uint32 i = 0; i < _animations->size(); ++i)
            _animations->at(i).Draw();
    }
}

void SavePoint::SetActive(bool active)
{
    if(active) {
        _animations = &MapMode::CurrentInstance()->active_save_point_animations;
        _active_particle_object->Start();
        _inactive_particle_object->Stop();

        // Play a sound when the save point become active
        if(!_save_active)
            AudioManager->PlaySound("snd/save_point_activated_dokashiteru_oga.wav");
    } else {
        _animations = &MapMode::CurrentInstance()->inactive_save_point_animations;
        _active_particle_object->Stop();
        _inactive_particle_object->Start();
    }
    _save_active = active;
}


// Halos
Halo::Halo(const std::string &filename, float x, float y, const Color &color):
    MapObject()
{
    _color = color;
    position.x = x;
    position.y = y;

    _object_type = HALO_TYPE;
    collision_mask = NO_COLLISION;

    if(_animation.LoadFromAnimationScript(filename)) {
        MapMode::ScaleToMapCoords(_animation);

        // Setup the image collision for the display update
        SetImgHalfWidth(_animation.GetWidth() / 2.0f);
        SetImgHeight(_animation.GetHeight());
    }
}

void Halo::Update()
{
    if(updatable)
        _animation.Update();
}


void Halo::Draw()
{
    if(MapObject::ShouldDraw() && _animation.GetCurrentFrame())
        VideoManager->DrawHalo(*_animation.GetCurrentFrame(), _color);
}

// Light objects
Light::Light(const std::string &main_flare_filename,
             const std::string &secondary_flare_filename,
             float x, float y, const Color &main_color, const Color &secondary_color):
    MapObject()
{
    _main_color = main_color;
    _secondary_color = secondary_color;

    position.x = x;
    position.y = y;

    _object_type = LIGHT_TYPE;
    collision_mask = NO_COLLISION;

    _a = _b = 0.0f;
    _distance = 0.0f;

    // For better eye-candy, randomize a bit the secondary flare distances.
    _distance_factor_1 = RandomFloat(8.0f, 12.0f);
    _distance_factor_2 = RandomFloat(17.0f, 23.0f);
    _distance_factor_3 = RandomFloat(12.0f, 18.0f);
    _distance_factor_4 = RandomFloat(5.0f, 9.0f);

    if(_main_animation.LoadFromAnimationScript(main_flare_filename)) {
        MapMode::ScaleToMapCoords(_main_animation);

        // Setup the image collision for the display update
        SetImgHalfWidth(_main_animation.GetWidth() / 3.0f);
        SetImgHeight(_main_animation.GetHeight());
    }
    if(_secondary_animation.LoadFromAnimationScript(secondary_flare_filename)) {
        MapMode::ScaleToMapCoords(_secondary_animation);
    }
}

MapRectangle Light::GetImageRectangle() const
{
    MapRectangle rect;
    rect.left = position.x - img_half_width;
    rect.right = position.x + img_half_width;
    // The y coord is also centered in that case
    rect.top = position.y - (img_height / 2.0f);
    rect.bottom = position.y + (img_height / 2.0f);
    return rect;
}

void Light::_UpdateLightAngle()
{
    MapMode *mm = MapMode::CurrentInstance();
    if(!mm)
        return;
    const MapFrame &frame = mm->GetMapFrame();

    MapPosition center;
    center.x = frame.screen_edges.left + (frame.screen_edges.right - frame.screen_edges.left) / 2.0f;
    center.y = frame.screen_edges.top + (frame.screen_edges.bottom - frame.screen_edges.top) / 2.0f;

    // Don't update the distance and angle data in that case.
    if(center.x == _last_center_pos.x && center.y == _last_center_pos.y)
        return;

    _last_center_pos.x = center.x;
    _last_center_pos.y = center.y;

    _distance = (position.x - center.x) * (position.x - center.x);
    _distance += (position.y - center.y) * (position.y - center.y);
    _distance = sqrtf(_distance);

    if(IsFloatEqual(position.x, center.x, 0.2f))
        _a = 2.5f;
    else
        _a = (position.y - center.y) / (position.x - center.x);

    // Prevent angles rough-edges
    if(_a < 0.0f)
        _a = -_a;
    if(_a > 2.5f)
        _a = 2.5f;

    _b = position.y - _a * position.x;

    // Update the flare alpha depending on the distance
    float distance = _distance / 5.0f;

    if(distance < 0.0f)
        distance = -distance;
    if(distance < 1.0f)
        distance = 1.0f;

    _main_color_alpha = _main_color;
    _main_color_alpha.SetAlpha(_main_color.GetAlpha() / distance);
    _secondary_color_alpha = _secondary_color;
    _secondary_color_alpha.SetAlpha(_secondary_color.GetAlpha() / distance);
}

void Light::Update()
{
    if(updatable) {
        _main_animation.Update();
        _secondary_animation.Update();
        _UpdateLightAngle();
    }
}

void Light::Draw()
{
    if(MapObject::ShouldDraw()
            && _main_animation.GetCurrentFrame()) {
        MapMode *mm = MapMode::CurrentInstance();
        if(!mm)
            return;
        const MapFrame &frame = mm->GetMapFrame();

        VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);

        VideoManager->DrawHalo(*_main_animation.GetCurrentFrame(), _main_color_alpha);

        if(!_secondary_animation.GetCurrentFrame()) {
            VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);
            return;
        }

        float next_pos_x = position.x - _distance / _distance_factor_1;
        float next_pos_y = _a * next_pos_x + _b;
        VideoManager->Move(next_pos_x - frame.screen_edges.left,
                           next_pos_y - frame.screen_edges.top);
        VideoManager->DrawHalo(*_secondary_animation.GetCurrentFrame(), _secondary_color_alpha);

        next_pos_x = position.x - _distance / _distance_factor_2;
        next_pos_y = _a * next_pos_x + _b;
        VideoManager->Move(next_pos_x - frame.screen_edges.left,
                           next_pos_y - frame.screen_edges.top);
        VideoManager->DrawHalo(*_secondary_animation.GetCurrentFrame(), _secondary_color_alpha);

        next_pos_x = position.x + _distance / _distance_factor_3;
        next_pos_y = _a * next_pos_x + _b;
        VideoManager->Move(next_pos_x - frame.screen_edges.left,
                           next_pos_y - frame.screen_edges.top);
        VideoManager->DrawHalo(*_secondary_animation.GetCurrentFrame(), _secondary_color_alpha);

        next_pos_x = position.x + _distance / _distance_factor_4;
        next_pos_y = _a * next_pos_x + _b;
        VideoManager->Move(next_pos_x - frame.screen_edges.left,
                           next_pos_y - frame.screen_edges.top);
        VideoManager->DrawHalo(*_secondary_animation.GetCurrentFrame(), _secondary_color_alpha);

        VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);
    }
}

SoundObject::SoundObject(const std::string &sound_filename, float x, float y, float strength):
    MapObject()
{
    MapObject::_object_type = SOUND_TYPE;

    if (!_sound.LoadAudio(sound_filename)) {
        PRINT_WARNING << "Couldn't load environmental sound file: "
            << sound_filename << std::endl;
    }

    // Tells the engine the sound can be unloaded if no other mode is using
    // when the current map mode will be destructed
    _sound.AddOwner(MapMode::CurrentInstance());
    _sound.SetLooping(true);
    _sound.SetVolume(0.0f);
    _sound.Stop();

    _strength = strength;
    // Invalidates negative or near 0 values.
    if (_strength <= 0.2f)
        _strength = 0.0f;

    _time_remaining = 0.0f;

    position.x = x;
    position.y = y;

    collision_mask = NO_COLLISION;
}

void SoundObject::Update()
{
    if (_strength == 0.0f)
        return;

    // Update the volume only every 100ms
    _time_remaining -= (int32)vt_system::SystemManager->GetUpdateTime();
    if (_time_remaining > 0.0f)
        return;
    _time_remaining = 100;

    // N.B.: The distance between two point formula is:
    // squareroot((x2 - x1)^2+(y2 - y1)^2)
    MapMode *mm = MapMode::CurrentInstance();
    if(!mm)
        return;
    const MapFrame &frame = mm->GetMapFrame();

    MapPosition center;
    center.x = frame.screen_edges.left + (frame.screen_edges.right - frame.screen_edges.left) / 2.0f;
    center.y = frame.screen_edges.top + (frame.screen_edges.bottom - frame.screen_edges.top) / 2.0f;

    float distance = (position.x - center.x) * (position.x - center.x);
    distance += (position.y - center.y) * (position.y - center.y);
    //distance = sqrtf(_distance); <-- We dont actually need it as it is slow.

    if (distance >= (_strength * _strength)) {
        _sound.Stop();
        return;
    }

    float volume = 1.0f - (distance / (_strength * _strength));
    _sound.SetVolume(volume);

    if (_sound.GetState() != AUDIO_STATE_PLAYING)
        _sound.Play();
}

// ----------------------------------------------------------------------------
// ---------- TreasureObject Class Functions
// ----------------------------------------------------------------------------

TreasureObject::TreasureObject(const std::string &treasure_name,
                               const std::string &closed_animation_file,
                               const std::string &opening_animation_file,
                               const std::string &open_animation_file) :
    PhysicalObject()
{
    _object_type = TREASURE_TYPE;
    _events_triggered = false;
    _is_opening = false;

    _treasure_name = treasure_name;
    if(treasure_name.empty())
        PRINT_WARNING << "Empty treasure name found. The treasure won't function normally." << std::endl;

    _treasure = new vt_map::private_map::MapTreasure();

    // Dissect the frames and create the closed, opening, and open animations
    vt_video::AnimatedImage closed_anim, opening_anim, open_anim;

    closed_anim.LoadFromAnimationScript(closed_animation_file);
    MapMode::ScaleToMapCoords(closed_anim);
    if(!opening_animation_file.empty())
        opening_anim.LoadFromAnimationScript(opening_animation_file);
    MapMode::ScaleToMapCoords(opening_anim);
    open_anim.LoadFromAnimationScript(open_animation_file);
    MapMode::ScaleToMapCoords(open_anim);

    // Loop the opening animation only once
    opening_anim.SetNumberLoops(0);

    AddAnimation(closed_anim);
    AddAnimation(opening_anim);
    AddAnimation(open_anim);

    // Set the collision rectangle according to the dimensions of the first frame
    SetCollHalfWidth(closed_anim.GetWidth() / 2.0f);
    SetCollHeight(closed_anim.GetHeight());

    _LoadState();
} // TreasureObject::TreasureObject()

void TreasureObject::_LoadState()
{
    if(!_treasure)
        return;

    // If the event exists, the treasure has already been opened
    if(GlobalManager->DoesEventExist("treasures", _treasure_name)) {
        SetCurrentAnimation(TREASURE_OPEN_ANIM);
        _treasure->SetTaken(true);
    }
}

void TreasureObject::Open()
{
    if(!_treasure) {
        PRINT_ERROR << "Can't open treasure with invalid treasure content." << std::endl;
        return;
    }

    if(_treasure->IsTaken()) {
        IF_PRINT_WARNING(MAP_DEBUG) << "attempted to retrieve an already taken treasure: " << object_id << std::endl;
        return;
    }

    // Test whether events should be triggered
    if (_events.empty())
        _events_triggered = true;

    SetCurrentAnimation(TREASURE_OPENING_ANIM);
    _is_opening = true;
}

void TreasureObject::Update()
{
    PhysicalObject::Update();

    if((current_animation == TREASURE_OPENING_ANIM) && (animations[TREASURE_OPENING_ANIM].IsLoopsFinished()))
        SetCurrentAnimation(TREASURE_OPEN_ANIM);

    if (!_is_opening || current_animation != TREASURE_OPEN_ANIM)
        return;

    // Once opened, we handle potential events and the display of the treasure supervisor
    EventSupervisor *event_manager = MapMode::CurrentInstance()->GetEventSupervisor();

    if (!_events_triggered) {
        // Trigger potential events after opening
        for (uint32 i = 0; i < _events.size(); ++i) {
            if (!event_manager->IsEventActive(_events[i]))
                 MapMode::CurrentInstance()->GetEventSupervisor()->StartEvent(_events[i]);
        }
        _events_triggered = true;
    }
    else if (!_events.empty()) {
        // Test whether the events have finished
        std::vector<std::string>::iterator it = _events.begin();
        for (; it != _events.end();) {
            // Once the event has finished, we forget it
            if (!event_manager->IsEventActive(*it))
                it = _events.erase(it);
            else
                ++it;
        }
    }
    else {
        // Once all events are finished, we can open the treasure supervisor
        MapMode::CurrentInstance()->GetTreasureSupervisor()->Initialize(this);
        // Add an event to the treasures group indicating that the treasure has now been opened
        GlobalManager->SetEventValue("treasures", _treasure_name, 1);
        // End the opening sequence
        _is_opening = false;
    }
}

bool TreasureObject::AddObject(uint32 id, uint32 quantity)
{
    if(!_treasure)
        return false;
    return _treasure->AddObject(id, quantity);
}

void TreasureObject::AddEvent(const std::string& event_id)
{
    if (!event_id.empty())
        _events.push_back(event_id);
}

// ----------------------------------------------------------------------------
// ---------- TriggerObject Class Functions
// ----------------------------------------------------------------------------

TriggerObject::TriggerObject(const std::string &trigger_name,
                             const std::string &off_animation_file,
                             const std::string &on_animation_file,
                             const std::string& off_event_id,
                             const std::string& on_event_id) :
    PhysicalObject(),
    _trigger_state(false)
{
    _object_type = TRIGGER_TYPE;

    _trigger_name = trigger_name;

    _off_event = off_event_id;
    _on_event = on_event_id;

    // By default, the player can step on it to toggle its state.
    _triggerable_by_character = true;

    // Dissect the frames and create the closed, opening, and open animations
    vt_video::AnimatedImage off_anim, on_anim;

    off_anim.LoadFromAnimationScript(off_animation_file);
    MapMode::ScaleToMapCoords(off_anim);
    on_anim.LoadFromAnimationScript(on_animation_file);
    MapMode::ScaleToMapCoords(on_anim);

    AddAnimation(off_anim);
    AddAnimation(on_anim);

    // Set a default collision area making the trigger respond when the character
    // is rather having his/her two feet on it.
    SetCollHalfWidth(off_anim.GetWidth() / 4.0f);
    SetCollHeight(off_anim.GetHeight() * 2.0f / 3.0f);
    SetImgHalfWidth(off_anim.GetWidth() / 2.0f);
    SetImgHeight(off_anim.GetHeight());

    _LoadState();
} // TreasureObject::TreasureObject()

void TriggerObject::Update() {
    PhysicalObject::Update();

    // The trigger can't be toggle by the character, nothing will happen
    if (!_triggerable_by_character)
        return;

    // TODO: Permit other behaviour
    if (_trigger_state)
        return;

    MapMode *map_mode = MapMode::CurrentInstance();
    if (!map_mode->IsCameraOnVirtualFocus()
            && MapRectangle::CheckIntersection(map_mode->GetCamera()->GetCollisionRectangle(), GetCollisionRectangle())) {

        map_mode->GetCamera()->SetMoving(false);
        SetState(true);
    }

}

void TriggerObject::_LoadState()
{
    if(_trigger_name.empty())
        return;

    // If the event value is equal to 1, the trigger has been triggered.
    if(GlobalManager->GetEventValue("triggers", _trigger_name) == 1) {
        SetCurrentAnimation(TRIGGER_ON_ANIM);
        _trigger_state = true;
    }
    else {
        SetCurrentAnimation(TRIGGER_OFF_ANIM);
        _trigger_state = false;
    }
}

void TriggerObject::SetState(bool state)
{
    if (_trigger_state == state)
        return;

    _trigger_state = state;

    // If the event exists, the treasure has already been opened
    if(_trigger_state) {
        SetCurrentAnimation(TRIGGER_ON_ANIM);
        if (!_on_event.empty())
            MapMode::CurrentInstance()->GetEventSupervisor()->StartEvent(_on_event);
        GlobalManager->SetEventValue("triggers", _trigger_name, 1);
    }
    else {
        SetCurrentAnimation(TRIGGER_OFF_ANIM);
        if (!_off_event.empty())
            MapMode::CurrentInstance()->GetEventSupervisor()->StartEvent(_off_event);
        GlobalManager->SetEventValue("triggers", _trigger_name, 0);
    }
}

// ----------------------------------------------------------------------------
// ---------- ObjectSupervisor Class Functions
// ----------------------------------------------------------------------------

ObjectSupervisor::ObjectSupervisor() :
    _num_grid_x_axis(0),
    _num_grid_y_axis(0),
    _last_id(1000),
    _visible_party_member(0)
{
    _virtual_focus = new VirtualSprite();
    _virtual_focus->SetPosition(0.0f, 0.0f);
    _virtual_focus->movement_speed = NORMAL_SPEED;
    _virtual_focus->SetCollisionMask(NO_COLLISION);
    _virtual_focus->SetVisible(false);
}



ObjectSupervisor::~ObjectSupervisor()
{
    // Delete all of the map objects
    for(uint32 i = 0; i < _flat_ground_objects.size(); ++i) {
        delete(_flat_ground_objects[i]);
    }
    for(uint32 i = 0; i < _ground_objects.size(); ++i) {
        delete(_ground_objects[i]);
    }
    for(uint32 i = 0; i < _save_points.size(); ++i) {
        delete(_save_points[i]);
    }
    for(uint32 i = 0; i < _pass_objects.size(); ++i) {
        delete(_pass_objects[i]);
    }
    for(uint32 i = 0; i < _sky_objects.size(); ++i) {
        delete(_sky_objects[i]);
    }
    for(uint32 i = 0; i < _sound_objects.size(); ++i) {
        delete(_sound_objects[i]);
    }
    for(uint32 i = 0; i < _halos.size(); ++i) {
        delete(_halos[i]);
    }
    for(uint32 i = 0; i < _lights.size(); ++i) {
        delete(_lights[i]);
    }
    delete(_virtual_focus);
}



MapObject *ObjectSupervisor::GetObjectByIndex(uint32 index)
{
    if(index >= GetNumberObjects()) {
        return NULL;
    }

    uint32 counter = 0;
    for(std::map<uint16, MapObject *>::iterator it = _all_objects.begin(); it != _all_objects.end(); ++it) {
        if(counter == index)
            return it->second;
        else
            ++counter;
    }

    IF_PRINT_WARNING(MAP_DEBUG) << "object not found after reaching end of set -- this should never happen" << std::endl;
    return NULL;
}



MapObject *ObjectSupervisor::GetObject(uint32 object_id)
{
    std::map<uint16, MapObject *>::iterator it = _all_objects.find(object_id);

    if(it == _all_objects.end())
        return NULL;
    else
        return it->second;
}



VirtualSprite *ObjectSupervisor::GetSprite(uint32 object_id)
{
    MapObject *object = GetObject(object_id);

    if(object == NULL) {
        return NULL;
    }

    VirtualSprite *sprite = dynamic_cast<VirtualSprite *>(object);
    if(sprite == NULL) {
        IF_PRINT_WARNING(MAP_DEBUG) << "could not cast map object to sprite type, object id: " << object_id << std::endl;
        return NULL;
    }

    return sprite;
}



void ObjectSupervisor::SortObjects()
{
    std::sort(_flat_ground_objects.begin(), _flat_ground_objects.end(), MapObject_Ptr_Less());
    std::sort(_ground_objects.begin(), _ground_objects.end(), MapObject_Ptr_Less());
    std::sort(_pass_objects.begin(), _pass_objects.end(), MapObject_Ptr_Less());
    std::sort(_sky_objects.begin(), _sky_objects.end(), MapObject_Ptr_Less());
}



bool ObjectSupervisor::Load(ReadScriptDescriptor &map_file)
{
    if(!map_file.DoesTableExist("map_grid")) {
        PRINT_ERROR << "No map grid found in map file: " << map_file.GetFilename() << std::endl;
        return false;
    }

    // Construct the collision grid
    map_file.OpenTable("map_grid");
    _num_grid_y_axis = map_file.GetTableSize();
    for(uint16 y = 0; y < _num_grid_y_axis; ++y) {
        _collision_grid.push_back(std::vector<uint32>());
        map_file.ReadUIntVector(y, _collision_grid.back());
    }
    map_file.CloseTable();
    _num_grid_x_axis = _collision_grid[0].size();
    return true;
}



void ObjectSupervisor::Update()
{
    for(uint32 i = 0; i < _flat_ground_objects.size(); ++i)
        _flat_ground_objects[i]->Update();
    for(uint32 i = 0; i < _ground_objects.size(); ++i)
        _ground_objects[i]->Update();
    // Update save point animation and activeness.
    _UpdateSavePoints();
    for(uint32 i = 0; i < _pass_objects.size(); ++i)
        _pass_objects[i]->Update();
    for(uint32 i = 0; i < _sky_objects.size(); ++i)
        _sky_objects[i]->Update();
    for(uint32 i = 0; i < _halos.size(); ++i)
        _halos[i]->Update();
    for(uint32 i = 0; i < _lights.size(); ++i)
        _lights[i]->Update();
    for(uint32 i = 0; i < _zones.size(); ++i)
        _zones[i]->Update();

    _UpdateAmbientSounds();
}

void ObjectSupervisor::DrawSavePoints()
{
    for(uint32 i = 0; i < _save_points.size(); ++i) {
        _save_points[i]->Draw();
    }
}

void ObjectSupervisor::DrawFlatGroundObjects()
{
    for(uint32 i = 0; i < _flat_ground_objects.size(); ++i) {
        _flat_ground_objects[i]->Draw();
    }
}

void ObjectSupervisor::DrawGroundObjects(const bool second_pass)
{
    for(uint32 i = 0; i < _ground_objects.size(); i++) {
        if(_ground_objects[i]->draw_on_second_pass == second_pass) {
            _ground_objects[i]->Draw();
        }
    }
}

void ObjectSupervisor::DrawPassObjects()
{
    for(uint32 i = 0; i < _pass_objects.size(); i++) {
        _pass_objects[i]->Draw();
    }
}

void ObjectSupervisor::DrawSkyObjects()
{
    for(uint32 i = 0; i < _sky_objects.size(); i++) {
        _sky_objects[i]->Draw();
    }
}

void ObjectSupervisor::DrawLights()
{
    for(uint32 i = 0; i < _halos.size(); ++i)
        _halos[i]->Draw();
    for(uint32 i = 0; i < _lights.size(); ++i)
        _lights[i]->Draw();
}

void ObjectSupervisor::DrawDialogIcons()
{
    MapSprite *mapSprite;
    for(uint32 i = 0; i < _ground_objects.size(); i++) {
        if(_ground_objects[i]->GetObjectType() == SPRITE_TYPE) {
            mapSprite = static_cast<MapSprite *>(_ground_objects[i]);
            mapSprite->DrawDialog();
        }
    }
}

void ObjectSupervisor::_UpdateSavePoints()
{
    VirtualSprite *sprite = MapMode::CurrentInstance()->GetCamera();

    MapRectangle spr_rect;
    if(sprite)
        spr_rect = sprite->GetCollisionRectangle();

    for(std::vector<SavePoint *>::iterator it = _save_points.begin();
            it != _save_points.end(); ++it) {
        (*it)->SetActive(MapRectangle::CheckIntersection(spr_rect,
                         (*it)->GetCollisionRectangle()));
        (*it)->Update();
    }
}

void ObjectSupervisor::_UpdateAmbientSounds()
{
    for(std::vector<SoundObject *>::iterator it = _sound_objects.begin();
            it != _sound_objects.end(); ++it) {
        (*it)->Update();
    }
}

void ObjectSupervisor::_DrawMapZones()
{
    for(uint32 i = 0; i < _zones.size(); ++i)
        _zones[i]->Draw();
}

MapObject *ObjectSupervisor::_FindNearestSavePoint(const VirtualSprite *sprite)
{
    if(sprite == NULL)
        return NULL;

    for(std::vector<SavePoint *>::iterator it = _save_points.begin();
            it != _save_points.end(); ++it) {

        if(MapRectangle::CheckIntersection(sprite->GetCollisionRectangle(),
                                           (*it)->GetCollisionRectangle())) {
            return (*it);
        }
    }
    return NULL;
}

MapObject *ObjectSupervisor::FindNearestInteractionObject(const VirtualSprite *sprite, float search_distance)
{
    if(!sprite)
        return 0;

    // Using the sprite's direction, determine the boundaries of the search area to check for objects
    MapRectangle search_area = sprite->GetCollisionRectangle();
    if(sprite->direction & FACING_NORTH) {
        search_area.bottom = search_area.top;
        search_area.top = search_area.top - search_distance;
    } else if(sprite->direction & FACING_SOUTH) {
        search_area.top = search_area.bottom;
        search_area.bottom = search_area.bottom + search_distance;
    } else if(sprite->direction & FACING_WEST) {
        search_area.right = search_area.left;
        search_area.left = search_area.left - search_distance;
    } else if(sprite->direction & FACING_EAST) {
        search_area.left = search_area.right;
        search_area.right = search_area.right + search_distance;
    } else {
        IF_PRINT_WARNING(MAP_DEBUG) << "sprite was set to invalid direction: " << sprite->direction << std::endl;
        return NULL;
    }

    // Go through all objects and determine which (if any) lie within the search area

    // A vector to hold objects which are inside the search area (either partially or fully)
    std::vector<MapObject *> valid_objects;
    // A pointer to the vector of objects to search
    std::vector<MapObject *>* search_vector = NULL;

    // Only search the object layer that the sprite resides on.
    // Note that we do not consider searching the pass layer.
    if(sprite->sky_object)
        search_vector = &_sky_objects;
    else
        search_vector = &_ground_objects;

    for(std::vector<MapObject *>::iterator it = (*search_vector).begin(); it != (*search_vector).end(); ++it) {
        if(*it == sprite)  // Don't allow the sprite itself to be considered in the search
            continue;

        // Don't allow scenery object types to get in the way
        // as this is preventing save points from functioning, for instance
        if((*it)->GetObjectType() >= HALO_TYPE)
            continue;

        // If the object is a physical object without any event, we can ignore it
        if((*it)->GetObjectType() == PHYSICAL_TYPE) {
            PhysicalObject *phs = reinterpret_cast<PhysicalObject *>(*it);
            if(phs->GetEventIdWhenTalking().empty())
                continue;
        }

        // If the object is a sprite without any dialogue, we can ignore it
        if((*it)->GetObjectType() == SPRITE_TYPE) {
            MapSprite *sp = reinterpret_cast<MapSprite *>(*it);
            if(!sp->HasAvailableDialogue())
                continue;
        }

        if((*it)->GetType() == TREASURE_TYPE) {
            TreasureObject *treasure_object = reinterpret_cast<TreasureObject *>(*it);
            if(treasure_object->GetTreasure()->IsTaken())
                continue;
        }

        MapRectangle object_rect = (*it)->GetCollisionRectangle();
        if(MapRectangle::CheckIntersection(object_rect, search_area) == true)
            valid_objects.push_back(*it);
    } // for (std::map<MapObject*>::iterator i = _all_objects.begin(); i != _all_objects.end(); i++)

    if(valid_objects.empty()) {
        // If no sprite was here, try searching a save point.
        return _FindNearestSavePoint(sprite);
    } else if(valid_objects.size() == 1) {
        return valid_objects[0];
    }

    // Figure out which of the valid objects is the closest to the sprite
    // NOTE: For simplicity, we use the Manhattan distance to determine which object is the closest
    MapObject *closest_obj = valid_objects[0];

    // Used to hold the full position coordinates of the sprite
    float source_x = sprite->GetXPosition();
    float source_y = sprite->GetYPosition();
    // Holds the minimum distance found between the sprite and a valid object
    float min_distance = fabs(source_x - closest_obj->GetXPosition()) +
                         fabs(source_y - closest_obj->GetYPosition());

    for(uint32 i = 1; i < valid_objects.size(); i++) {
        float dist = fabs(source_x - valid_objects[i]->GetXPosition()) +
                     fabs(source_y - valid_objects[i]->GetYPosition());
        if(dist < min_distance) {
            closest_obj = valid_objects[i];
            min_distance = dist;
        }
    }
    return closest_obj;
} // MapObject* ObjectSupervisor::FindNearestObject(VirtualSprite* sprite, float search_distance)


bool ObjectSupervisor::CheckObjectCollision(const MapRectangle &rect, const private_map::MapObject *const obj)
{
    if(!obj)
        return false;

    MapRectangle obj_rect = obj->GetCollisionRectangle();
    return MapRectangle::CheckIntersection(rect, obj_rect);
}


bool ObjectSupervisor::IsPositionOccupiedByObject(float x, float y, MapObject *object)
{
    if(object == NULL) {
        IF_PRINT_WARNING(MAP_DEBUG) << "NULL pointer passed into function argument" << std::endl;
        return false;
    }

    MapRectangle rect = object->GetCollisionRectangle();

    if(x >= rect.left && x <= rect.right) {
        if(y <= rect.bottom && y >= rect.top) {
            return true;
        }
    }
    return false;
}


COLLISION_TYPE ObjectSupervisor::GetCollisionFromObjectType(MapObject *obj) const
{
    if(!obj)
        return NO_COLLISION;

    switch(obj->GetType()) {
    case VIRTUAL_TYPE:
    case SPRITE_TYPE:
        return CHARACTER_COLLISION;
        break;
    case ENEMY_TYPE:
        return ENEMY_COLLISION;
        break;
    case TREASURE_TYPE:
    case PHYSICAL_TYPE:
        return WALL_COLLISION;
        break;
    default:
        break;
    }
    return NO_COLLISION;
}



COLLISION_TYPE ObjectSupervisor::DetectCollision(MapObject* object,
                                                 float x_pos, float y_pos,
                                                 MapObject **collision_object_ptr)
{
    // If the sprite has this property set it can not collide
    if(!object)
        return NO_COLLISION;

    // Get the collision rectangle at the given position
    MapRectangle sprite_rect = object->GetCollisionRectangle(x_pos, y_pos);

    // Check if any part of the object's collision rectangle is outside of the map boundary
    if(sprite_rect.left < 0.0f || sprite_rect.right >= static_cast<float>(_num_grid_x_axis) ||
            sprite_rect.top < 0.0f || sprite_rect.bottom >= static_cast<float>(_num_grid_y_axis)) {
        return WALL_COLLISION;
    }

    // Check for the absence of collision checking after the map boundaries check,
    // So that no collision beings won't get out of the map.
    if(object->collision_mask == NO_COLLISION)
        return NO_COLLISION;

    // Check if the object's collision rectangel overlaps with any unwalkable elements on the collision grid
    // Grid based collision is not done for objects in the sky layer
    if(!object->sky_object && object->collision_mask & WALL_COLLISION) {
        // Determine if the object's collision rectangle overlaps any unwalkable tiles
        // Note that because the sprite's collision rectangle was previously determined to be within the map bounds,
        // the map grid tile indeces referenced in this loop are all valid entries and do not need to be checked for out-of-bounds conditions
        for(uint32 y = static_cast<uint32>(sprite_rect.top); y <= static_cast<uint32>(sprite_rect.bottom); ++y) {
            for(uint32 x = static_cast<uint32>(sprite_rect.left); x <= static_cast<uint32>(sprite_rect.right); ++x) {
                // Checks the collision grid at the row-column at the object's current context
                if(_collision_grid[y][x] > 0)
                    return WALL_COLLISION;
            }
        }
    }

    std::vector<MapObject *>* objects = object->sky_object ?
                                        &_sky_objects : &_ground_objects;

    std::vector<vt_map::private_map::MapObject *>::const_iterator it, it_end;
    for(it = objects->begin(), it_end = objects->end(); it != it_end; ++it) {
        MapObject *collision_object = *it;
        // Check if the object exists and has the no_collision property enabled
        if(!collision_object || collision_object->collision_mask == NO_COLLISION)
            continue;

        // Object and sprite are the same
        if(collision_object->object_id == object->object_id)
            continue;

        // If the two objects aren't colliding, try next.
        if(!CheckObjectCollision(sprite_rect, collision_object))
            continue;

        // The two objects are colliding, return the potentially asked pointer to it.
        if(collision_object_ptr != NULL)
            *collision_object_ptr = collision_object;

        // When the collision mask is taking in account the collision type
        // we can return it. Otherwise, just ignore the sprite colliding.
        COLLISION_TYPE collision = GetCollisionFromObjectType(collision_object);
        if(object->collision_mask & collision)
            return collision;
        else
            continue;
    }

    return NO_COLLISION;
} // bool ObjectSupervisor::DetectCollision(VirtualSprite* sprite, float x, float y, MapObject** collision_object_ptr)


Path ObjectSupervisor::FindPath(VirtualSprite *sprite, const MapPosition &destination, uint32 max_cost)
{
    // NOTE: Refer to the implementation of the A* algorithm to understand
    // what all these lists and score values are for.
    static const uint32 basic_gcost = 10;

    // NOTE(bis): On the outer scope, we'll use float based positions,
    // but we still use integer positions for path finding.
    Path path;

    if(!MapMode::CurrentInstance()->GetObjectSupervisor()->IsWithinMapBounds(sprite)) {
        IF_PRINT_WARNING(MAP_DEBUG) << "Sprite position is invalid" << std::endl;
        return path;
    }

    // Return when the destination is unreachable
    if(DetectCollision(sprite, destination.x, destination.y) == WALL_COLLISION)
        return path;

    if(!MapMode::CurrentInstance()->GetObjectSupervisor()->IsWithinMapBounds(destination.x, destination.y)) {
        IF_PRINT_WARNING(MAP_DEBUG) << "Invalid destination coordinates" << std::endl;
        return path;
    }

    // The starting node of this path discovery
    PathNode source_node(static_cast<int16>(sprite->GetXPosition()), static_cast<int16>(sprite->GetYPosition()));
    // The ending node.
    PathNode dest(static_cast<int16>(destination.x), static_cast<int16>(destination.y));

    // Check that the source node is not the same as the destination node
    if(source_node == dest) {
        PRINT_ERROR << "source node coordinates are the same as the destination" << std::endl;
        // return an empty path.
        return path;
    }

    std::vector<PathNode> open_list;
    std::vector<PathNode> closed_list;

    // The current "best node"
    PathNode best_node;
    // Used to hold the eight adjacent nodes
    PathNode nodes[8];

    // Temporary delta variables used in calculation of a node's heuristic (h score)
    uint32 x_delta, y_delta;
    // The number to add to a node's g_score, depending on whether it is a lateral or diagonal movement
    int16 g_add;

    open_list.push_back(source_node);

    // We will try to keep the original offset all along.
    float offset_x = GetFloatFraction(destination.x);
    float offset_y = GetFloatFraction(destination.y);

    while(open_list.empty() == false) {
        sort(open_list.begin(), open_list.end());
        best_node = open_list.back();
        open_list.pop_back();
        closed_list.push_back(best_node);

        // Check if destination has been reached, and break out of the loop if so
        if(best_node == dest)
            break;

        // Setup the coordinates of the 8 adjacent nodes to the best node
        nodes[0].tile_x = best_node.tile_x - 1;
        nodes[0].tile_y = best_node.tile_y;
        nodes[1].tile_x = best_node.tile_x + 1;
        nodes[1].tile_y = best_node.tile_y;
        nodes[2].tile_x = best_node.tile_x;
        nodes[2].tile_y = best_node.tile_y - 1;
        nodes[3].tile_x = best_node.tile_x;
        nodes[3].tile_y = best_node.tile_y + 1;
        nodes[4].tile_x = best_node.tile_x - 1;
        nodes[4].tile_y = best_node.tile_y - 1;
        nodes[5].tile_x = best_node.tile_x - 1;
        nodes[5].tile_y = best_node.tile_y + 1;
        nodes[6].tile_x = best_node.tile_x + 1;
        nodes[6].tile_y = best_node.tile_y - 1;
        nodes[7].tile_x = best_node.tile_x + 1;
        nodes[7].tile_y = best_node.tile_y + 1;

        // Check the eight adjacent nodes
        for(uint8 i = 0; i < 8; ++i) {
            // ---------- (A): Check if all tiles are walkable
            // Don't use 0.0f here for both since errors at the border between
            // two positions may occure, especially when running.
            COLLISION_TYPE collision_type = DetectCollision(sprite,
                                            ((float)nodes[i].tile_x) + offset_x,
                                            ((float)nodes[i].tile_y) + offset_y);

            // Can't go through walls.
            if(collision_type == WALL_COLLISION)
                continue;

            // ---------- (B): If this point has been reached, the node is valid for the sprite to move to
            // If this is a lateral adjacent node, g_score is +10, otherwise diagonal adjacent node is +14
            if(i < 4)
                g_add = basic_gcost;
            else
                g_add = basic_gcost + 4;

            // Add some g cost when there is another sprite there,
            // so the NPC try to get around when possible,
            // but will still go through it when there are no other choices.
            if(collision_type == CHARACTER_COLLISION
                    || collision_type == ENEMY_COLLISION)
                g_add += basic_gcost * 2;

            // If the path has reached the maximum length requested, we abort the path
            if (max_cost > 0 && (uint32)(best_node.g_score + g_add) >= max_cost * basic_gcost)
                return path;

            // ---------- (C): Check if the node is already in the closed list
            if(find(closed_list.begin(), closed_list.end(), nodes[i]) != closed_list.end())
                continue;

            // Set the node's parent and calculate its g_score
            nodes[i].parent_x = best_node.tile_x;
            nodes[i].parent_y = best_node.tile_y;
            nodes[i].g_score = best_node.g_score + g_add;

            // ---------- (D): Check to see if the node is already on the open list and update it if necessary
            std::vector<PathNode>::iterator iter = std::find(open_list.begin(), open_list.end(), nodes[i]);
            if(iter != open_list.end()) {
                // If its G is higher, it means that the path we are on is better, so switch the parent
                if(iter->g_score > nodes[i].g_score) {
                    iter->g_score = nodes[i].g_score;
                    iter->f_score = nodes[i].g_score + iter->h_score;
                    iter->parent_x = nodes[i].parent_x;
                    iter->parent_y = nodes[i].parent_y;
                }
            }
            // ---------- (E): Add the new node to the open list
            else {
                // Calculate the H and F score of the new node (the heuristic used is diagonal)
                x_delta = abs(dest.tile_x - nodes[i].tile_x);
                y_delta = abs(dest.tile_y - nodes[i].tile_y);
                if(x_delta > y_delta)
                    nodes[i].h_score = 14 * y_delta + 10 * (x_delta - y_delta);
                else
                    nodes[i].h_score = 14 * x_delta + 10 * (y_delta - x_delta);

                nodes[i].f_score = nodes[i].g_score + nodes[i].h_score;
                open_list.push_back(nodes[i]);
            }
        } // for (uint8 i = 0; i < 8; ++i)
    } // while (open_list.empty() == false)

    if(open_list.empty() == true) {
        IF_PRINT_WARNING(MAP_DEBUG) << "could not find path to destination" << std::endl;
        return path;
    }

    // Add the destination node to the vector.
    path.push_back(destination);

    // Retain the last node parent, and remove it from the closed list
    int16 parent_x = best_node.parent_x;
    int16 parent_y = best_node.parent_y;
    closed_list.pop_back();

    // Go backwards through the closed list following the parent nodes to construct the path
    for(std::vector<PathNode>::iterator iter = closed_list.end() - 1; iter != closed_list.begin(); --iter) {
        if(iter->tile_y == parent_y && iter->tile_x == parent_x) {
            MapPosition next_pos(((float)iter->tile_x) + offset_x, ((float)iter->tile_y) + offset_y);
            path.push_back(next_pos);

            parent_x = iter->parent_x;
            parent_y = iter->parent_y;
        }
    }
    std::reverse(path.begin(), path.end());

    return path;
} // Path ObjectSupervisor::FindPath(const VirtualSprite* sprite, const MapPosition& destination)

void ObjectSupervisor::ReloadVisiblePartyMember()
{
    // Don't do anything when there is no visible party member.
    if(!_visible_party_member)
        return;

    // Get the front party member
    GlobalActor *actor = GlobalManager->GetActiveParty()->GetCharacterAtIndex(0);

    // Update only if the actor has changed
    if(actor && actor->GetMapSpriteName() != _visible_party_member->GetSpriteName())
        _visible_party_member->ReloadSprite(actor->GetMapSpriteName());
}

void ObjectSupervisor::SetAllEnemyStatesToDead()
{
    for(std::map<uint16, MapObject *>::iterator i = _all_objects.begin(); i != _all_objects.end(); ++i) {
        if (i->second->GetObjectType() == ENEMY_TYPE) {
            EnemySprite* enemy = dynamic_cast<EnemySprite*>(i->second);
            enemy->ChangeStateDead();
        }
    }
}

bool ObjectSupervisor::IsWithinMapBounds(float x, float y) const
{
    return (x >= 0.0f && x < static_cast<float>(_num_grid_x_axis)
            && y >= 0.0f && y < static_cast<float>(_num_grid_y_axis));
}

bool ObjectSupervisor::IsWithinMapBounds(VirtualSprite *sprite) const
{
    return sprite ? IsWithinMapBounds(sprite->GetXPosition(), sprite->GetYPosition())
           : false;
}

void ObjectSupervisor::DrawCollisionArea(const MapFrame *frame)
{
    VideoManager->Move(frame->tile_x_offset - 0.5f, frame->tile_y_offset - 1.0f);

    for(uint32 y = static_cast<uint32>(frame->tile_y_start * 2);
            y < static_cast<uint32>((frame->tile_y_start + frame->num_draw_y_axis) * 2); ++y) {
        for(uint32 x = static_cast<uint32>(frame->tile_x_start * 2);
                x < static_cast<uint32>((frame->tile_x_start + frame->num_draw_x_axis) * 2); ++x) {

            // Draw the collision rectangle
            if(_collision_grid[y][x] > 0)
                VideoManager->DrawRectangle(1.0f, 1.0f, Color(1.0f, 0.0f, 0.0f, 0.6f));

            VideoManager->MoveRelative(1.0f, 0.0f);
        } // x
        VideoManager->MoveRelative(-static_cast<float>(frame->num_draw_x_axis * 2), 1.0f);
    } // y
}

bool ObjectSupervisor::IsStaticCollision(uint32 x, uint32 y)
{

    //if the map's collision context is set to 1, we can return since we know there is a collision
    if(IsMapCollision(x, y))
        return true;

    std::vector<vt_map::private_map::MapObject *>::const_iterator it, it_end;
    for(it = _ground_objects.begin(), it_end = _ground_objects.end(); it != it_end; ++it) {
        MapObject *collision_object = *it;
        // Check if the object exists and has the no_collision property enabled
        if(!collision_object || collision_object->collision_mask == NO_COLLISION)
            continue;

        //only check physical objects. we don't care about sprites and enemies, treasure boxes, etc
        if(collision_object->GetObjectType() != PHYSICAL_TYPE)
            continue;

        //get the rect. if the x and y fields are within the rect, we have a collision here
        MapRectangle rect = collision_object->GetCollisionRectangle();
        //we know x and y are inside the map. So, just test then as a box vs point test
        if(rect.top <= y && y <= rect.bottom &&
           rect.left <= x && x <= rect.right)
           return true;
    }

    return false;
}

} // namespace private_map

} // namespace vt_map
