///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
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

MapObject::MapObject(MapObjectDrawLayer layer) :
    _object_id(-1),
    _img_pixel_half_width(0.0f),
    _img_pixel_height(0.0f),
    _img_grid_half_width(0.0f),
    _img_grid_height(0.0f),
    _coll_pixel_half_width(0.0f),
    _coll_pixel_height(0.0f),
    _coll_grid_half_width(0.0f),
    _coll_grid_height(0.0f),
    _updatable(true),
    _visible(true),
    _collision_mask(ALL_COLLISION),
    _draw_on_second_pass(false),
    _object_type(OBJECT_TYPE),
    _emote_animation(nullptr),
    _interaction_icon(nullptr),
    _emote_pixel_offset_x(0.0f),
    _emote_pixel_offset_y(0.0f),
    _emote_time(0),
    _draw_layer(layer),
    _grayscale(false)
{
    // Generate the object Id at creation time.
    ObjectSupervisor* obj_sup = MapMode::CurrentInstance()->GetObjectSupervisor();
    _object_id = obj_sup->GenerateObjectID();
    obj_sup->RegisterObject(this);
}

MapObject::~MapObject()
{
    if (_interaction_icon)
        delete _interaction_icon;
}

void MapObject::Update()
{
    if (_interaction_icon)
        _interaction_icon->Update();
}

bool MapObject::ShouldDraw()
{
    if(!_visible)
        return false;

    MapMode* MM = MapMode::CurrentInstance();

    // Determine if the sprite is off-screen and if so, don't draw it.
    if(!MapRectangle::CheckIntersection(GetGridImageRectangle(), MM->GetMapFrame().screen_edges))
        return false;

    // Move the drawing cursor to the appropriate coordinates for this sprite
    float x_pos = MM->GetScreenXCoordinate(GetXPosition());
    float y_pos = MM->GetScreenYCoordinate(GetYPosition());

    VideoManager->Move(x_pos, y_pos);

    return true;
}

MapRectangle MapObject::GetGridCollisionRectangle() const
{
    MapRectangle rect;
    rect.left = _tile_position.x - _coll_grid_half_width;
    rect.right = _tile_position.x + _coll_grid_half_width;
    rect.top = _tile_position.y - _coll_grid_height;
    rect.bottom = _tile_position.y;
    return rect;
}

MapRectangle MapObject::GetGridCollisionRectangle(float tile_x, float tile_y) const
{
    MapRectangle rect;
    rect.left = tile_x - _coll_grid_half_width;
    rect.right = tile_x + _coll_grid_half_width;
    rect.top = tile_y - _coll_grid_height;
    rect.bottom = tile_y;
    return rect;
}

MapRectangle MapObject::GetScreenCollisionRectangle(float x, float y) const
{
    MapRectangle rect;
    rect.left = x - _coll_pixel_half_width;
    rect.right = x + _coll_pixel_half_width;
    rect.top = y - _coll_pixel_height;
    rect.bottom = y;
    return rect;
}

MapRectangle MapObject::GetScreenCollisionRectangle() const
{
    MapMode* mm = MapMode::CurrentInstance();
    MapRectangle rect;
    float x_screen_pos = mm->GetScreenXCoordinate(_tile_position.x);
    float y_screen_pos = mm->GetScreenYCoordinate(_tile_position.y);
    rect.left = x_screen_pos - _coll_pixel_half_width;
    rect.right = x_screen_pos + _coll_pixel_half_width;
    rect.top = y_screen_pos - _coll_pixel_height;
    rect.bottom = y_screen_pos;
    return rect;
}

MapRectangle MapObject::GetScreenImageRectangle() const
{
    MapMode* mm = MapMode::CurrentInstance();
    MapRectangle rect;
    float x_screen_pos = mm->GetScreenXCoordinate(_tile_position.x);
    float y_screen_pos = mm->GetScreenYCoordinate(_tile_position.y);
    rect.left = x_screen_pos - _img_pixel_half_width;
    rect.right = x_screen_pos + _img_pixel_half_width;
    rect.top = y_screen_pos - _img_pixel_height;
    rect.bottom = y_screen_pos;
    return rect;
}

MapRectangle MapObject::GetGridImageRectangle() const
{
    MapRectangle rect;
    rect.left = _tile_position.x - _img_grid_half_width;
    rect.right = _tile_position.x + _img_grid_half_width;
    rect.top = _tile_position.y - _img_grid_height;
    rect.bottom = _tile_position.y;
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
    GlobalManager->GetEmoteOffset(_emote_pixel_offset_x, _emote_pixel_offset_y, emote_name, dir);

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
    VideoManager->MoveRelative(_emote_pixel_offset_x, -_img_pixel_height + _emote_pixel_offset_y);
    _emote_animation->Draw();
}

void MapObject::SetInteractionIcon(const std::string& animation_filename)
{
    if (_interaction_icon)
        delete _interaction_icon;
    _interaction_icon = new vt_video::AnimatedImage();
    if (!_interaction_icon->LoadFromAnimationScript(animation_filename)) {
        PRINT_WARNING << "Interaction icon animation filename couldn't be loaded: " << animation_filename << std::endl;
    }
}

void MapObject::DrawInteractionIcon()
{
    if (!_interaction_icon)
        return;

    if (!MapObject::ShouldDraw())
        return;

    MapMode* map_mode = MapMode::CurrentInstance();
    Color icon_color(1.0f, 1.0f, 1.0f, 0.0f);
    float icon_alpha = 1.0f - (fabs(GetXPosition() - map_mode->GetCamera()->GetXPosition())
                            + fabs(GetYPosition() - map_mode->GetCamera()->GetYPosition())) / INTERACTION_ICON_VISIBLE_RANGE;
    if (icon_alpha < 0.0f)
        icon_alpha = 0.0f;
    icon_color.SetAlpha(icon_alpha);

    VideoManager->MoveRelative(0, -GetImgPixelHeight());
    _interaction_icon->Draw(icon_color);
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

     if (_collision_mask == NO_COLLISION)
        return false;

     if (other_object->GetCollisionMask() == NO_COLLISION)
        return false;

    MapRectangle other_rect = other_object->GetGridCollisionRectangle();

    if (!MapRectangle::CheckIntersection(GetGridCollisionRectangle(), other_rect))
        return false;

    return _collision_mask & other_object->GetCollisionMask();
}

// ----------------------------------------------------------------------------
// ---------- PhysicalObject Class Functions
// ----------------------------------------------------------------------------

PhysicalObject::PhysicalObject(MapObjectDrawLayer layer) :
    MapObject(layer),
    _current_animation_id(0)
{
    _object_type = PHYSICAL_TYPE;
}

PhysicalObject::~PhysicalObject()
{
    _animations.clear();
}

PhysicalObject* PhysicalObject::Create(MapObjectDrawLayer layer)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new PhysicalObject(layer);
}

void PhysicalObject::Update()
{
    MapObject::Update();
    if(!_animations.empty() && _updatable)
        _animations[_current_animation_id].Update();
}

void PhysicalObject::Draw()
{
    if(_animations.empty() || !MapObject::ShouldDraw())
        return;

    _animations[_current_animation_id].Draw();

    // Draw collision rectangle if the debug view is on.
    if(!VideoManager->DebugInfoOn())
        return;

    float x, y = 0.0f;
    VideoManager->GetDrawPosition(x, y);
    MapRectangle rect = GetScreenCollisionRectangle(x, y);
    VideoManager->DrawRectangle(rect.right - rect.left, rect.bottom - rect.top, Color(0.0f, 1.0f, 0.0f, 0.6f));
}

int32_t PhysicalObject::AddAnimation(const std::string& animation_filename)
{
    AnimatedImage new_animation;
    if(!new_animation.LoadFromAnimationScript(animation_filename)) {
        PRINT_WARNING << "Could not add animation because the animation filename was invalid: "
                      << animation_filename << std::endl;
        return -1;
    }
    new_animation.SetDimensions(_img_pixel_half_width * 2.0f, _img_pixel_height);

    _animations.push_back(new_animation);
    return (int32_t)_animations.size() - 1;
}

int32_t PhysicalObject::AddStillFrame(const std::string& image_filename)
{
    AnimatedImage new_animation;
    // Adds a frame with a zero length: Making it last forever
    if (!new_animation.AddFrame(image_filename, 0)) {
        PRINT_WARNING << "Could not add a still frame because the image filename was invalid: "
                      << image_filename << std::endl;
        return -1;
    }
    new_animation.SetDimensions(_img_pixel_half_width * 2.0f, _img_pixel_height);

    _animations.push_back(new_animation);
    return (int32_t)_animations.size() - 1;
}

void PhysicalObject::SetCurrentAnimation(uint32_t animation_id)
{
    if(animation_id < _animations.size()) {
        _animations[_current_animation_id].SetTimeProgress(0);
        _current_animation_id = animation_id;
    }
}

// Particle object
ParticleObject::ParticleObject(const std::string& filename, float x, float y, MapObjectDrawLayer layer):
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

ParticleObject* ParticleObject::Create(const std::string &filename, float x, float y, MapObjectDrawLayer layer)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new ParticleObject(filename, x, y, layer);
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

bool ParticleObject::IsAlive() const
{
    if (_particle_effect)
        return _particle_effect->IsAlive();
    else
        return false;
}

void ParticleObject::Update()
{
    if(!_particle_effect || !_updatable)
        return;

    _particle_effect->Update();
}

void ParticleObject::Draw()
{
    if(!_particle_effect || !MapObject::ShouldDraw())
        return;

    float standard_pos_x, standard_pos_y;
    VideoManager->GetDrawPosition(standard_pos_x, standard_pos_y);
    VideoManager->SetStandardCoordSys();
    _particle_effect->Move(standard_pos_x, standard_pos_y);
    _particle_effect->Draw();
    // Reset the map mode coord sys afterward.

    // Draw collision rectangle if the debug view is on.
    if(!VideoManager->DebugInfoOn())
        return;

    MapRectangle rect = GetScreenImageRectangle();
    VideoManager->DrawRectangle(rect.right - rect.left, rect.bottom - rect.top, Color(0.0f, 1.0f, 1.0f, 0.6f));
    rect = GetScreenCollisionRectangle();
    VideoManager->DrawRectangle(rect.right - rect.left, rect.bottom - rect.top, Color(0.0f, 0.0f, 1.0f, 0.5f));
}

// Save points
SavePoint::SavePoint(float x, float y):
    MapObject(NO_LAYER_OBJECT), // This is a special object
    _animations(0),
    _save_active(false)
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
    SetCollPixelHalfWidth(_animations->at(0).GetWidth() * 0.5f);
    SetCollPixelHeight(_animations->at(0).GetHeight() - 0.3f * GRID_LENGTH);

    // Setup the image collision for the display update
    SetImgPixelHalfWidth(_animations->at(0).GetWidth() * 0.5f);
    SetImgPixelHeight(_animations->at(0).GetHeight());

    // Preload the save active sound
    AudioManager->LoadSound("data/sounds/save_point_activated_dokashiteru_oga.wav", map_mode);

    // The save point is going along with two particle objects used to show
    // whether the player is in or out the save point
    _active_particle_object = new ParticleObject("data/visuals/particle_effects/active_save_point.lua", x, y, GROUND_OBJECT);
    _inactive_particle_object = new ParticleObject("data/visuals/particle_effects/inactive_save_point.lua", x, y, GROUND_OBJECT);

    _active_particle_object->Stop();

    // Auto-regiters to the Object supervisor for later deletion handling.
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
        if(!_save_active)
            AudioManager->PlaySound("data/sounds/save_point_activated_dokashiteru_oga.wav");
    } else {
        _animations = &MapMode::CurrentInstance()->inactive_save_point_animations;
        _active_particle_object->Stop();
        _inactive_particle_object->Start();
    }
    _save_active = active;
}

Halo::Halo(const std::string& filename, float x, float y, const Color& color):
    MapObject(NO_LAYER_OBJECT) // This is a special object
{
    _color = color;
    _tile_position.x = x;
    _tile_position.y = y;

    _object_type = HALO_TYPE;
    _collision_mask = NO_COLLISION;

    if(!_animation.LoadFromAnimationScript(filename))
        PRINT_WARNING << "Couldn't load the Halo animation " << filename << " properly." << std::endl;

    // Setup the image collision for the display update
    SetImgPixelHalfWidth(_animation.GetWidth() * 0.5f);
    SetImgPixelHeight(_animation.GetHeight());

    // Auto-registers to the object supervisor for later deletion handling
    MapMode::CurrentInstance()->GetObjectSupervisor()->AddHalo(this);
}

Halo* Halo::Create(const std::string& filename, float x, float y, const Color& color)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new Halo(filename, x, y, color);
}

void Halo::Update()
{
    if(_updatable)
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
    MapObject(NO_LAYER_OBJECT)
{
    _main_color = main_color;
    _secondary_color = secondary_color;

    _tile_position.x = x;
    _tile_position.y = y;

    _object_type = LIGHT_TYPE;
    _collision_mask = NO_COLLISION;

    _a = _b = 0.0f;
    _distance = 0.0f;

    // For better eye-candy, randomize a bit the secondary flare distances.
    _distance_factor_1 = RandomFloat(8.0f, 12.0f);
    _distance_factor_2 = RandomFloat(17.0f, 23.0f);
    _distance_factor_3 = RandomFloat(12.0f, 18.0f);
    _distance_factor_4 = RandomFloat(5.0f, 9.0f);

    if(_main_animation.LoadFromAnimationScript(main_flare_filename)) {
        // Setup the image collision for the display update
        SetImgPixelHalfWidth(_main_animation.GetWidth() / 3.0f);
        SetImgPixelHeight(_main_animation.GetHeight());
    }

    _secondary_animation.LoadFromAnimationScript(secondary_flare_filename);

    // Register the object to the light vector
    MapMode::CurrentInstance()->GetObjectSupervisor()->AddLight(this);
}

Light* Light::Create(const std::string &main_flare_filename,
                     const std::string &secondary_flare_filename,
                     float x, float y,
                     const Color &main_color,
                     const Color &secondary_color)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new Light(main_flare_filename, secondary_flare_filename,
                     x, y, main_color, secondary_color);
}

MapRectangle Light::GetGridImageRectangle() const
{
    MapRectangle rect;
    rect.left = _tile_position.x - _img_grid_half_width;
    rect.right = _tile_position.x + _img_grid_half_width;
    // The y coord is also centered in that case
    rect.top = _tile_position.y - (_img_grid_height / 2.0f);
    rect.bottom = _tile_position.y + (_img_grid_height / 2.0f);
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

    _distance = (_tile_position.x - center.x) * (_tile_position.x - center.x);
    _distance += (_tile_position.y - center.y) * (_tile_position.y - center.y);
    _distance = sqrtf(_distance);

    if(IsFloatEqual(_tile_position.x, center.x, 0.2f))
        _a = 2.5f;
    else
        _a = (_tile_position.y - center.y) / (_tile_position.x - center.x);

    // Prevent angles rough-edges
    if(_a < 0.0f)
        _a = -_a;
    if(_a > 2.5f)
        _a = 2.5f;

    _b = _tile_position.y - _a * _tile_position.x;

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
    if(!_updatable)
        return;

    _main_animation.Update();
    _secondary_animation.Update();
    _UpdateLightAngle();
}

void Light::Draw()
{
    if(!MapObject::ShouldDraw() || !_main_animation.GetCurrentFrame())
        return;

    MapMode *mm = MapMode::CurrentInstance();
    if(!mm)
        return;

    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);

    VideoManager->DrawHalo(*_main_animation.GetCurrentFrame(), _main_color_alpha);

    if(!_secondary_animation.GetCurrentFrame()) {
        VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);
        return;
    }

    float next_pos_x = _tile_position.x - _distance / _distance_factor_1;
    float next_pos_y = _a * next_pos_x + _b;

    VideoManager->Move(mm->GetScreenXCoordinate(next_pos_x), mm->GetScreenYCoordinate(next_pos_y));
    VideoManager->DrawHalo(*_secondary_animation.GetCurrentFrame(), _secondary_color_alpha);

    next_pos_x = _tile_position.x - _distance / _distance_factor_2;
    next_pos_y = _a * next_pos_x + _b;
    VideoManager->Move(mm->GetScreenXCoordinate(next_pos_x), mm->GetScreenYCoordinate(next_pos_y));
    VideoManager->DrawHalo(*_secondary_animation.GetCurrentFrame(), _secondary_color_alpha);

    next_pos_x = _tile_position.x + _distance / _distance_factor_3;
    next_pos_y = _a * next_pos_x + _b;
    VideoManager->Move(mm->GetScreenXCoordinate(next_pos_x), mm->GetScreenYCoordinate(next_pos_y));
    VideoManager->DrawHalo(*_secondary_animation.GetCurrentFrame(), _secondary_color_alpha);

    next_pos_x = _tile_position.x + _distance / _distance_factor_4;
    next_pos_y = _a * next_pos_x + _b;
    VideoManager->Move(mm->GetScreenXCoordinate(next_pos_x), mm->GetScreenYCoordinate(next_pos_y));
    VideoManager->DrawHalo(*_secondary_animation.GetCurrentFrame(), _secondary_color_alpha);

    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);
}

SoundObject::SoundObject(const std::string& sound_filename, float x, float y, float strength):
    MapObject(NO_LAYER_OBJECT), // This is a special object
    _max_sound_volume(1.0f),
    _activated(true)
{
    _object_type = SOUND_TYPE;

    if (_sound.LoadAudio(sound_filename)) {
        // Tells the engine the sound can be unloaded if no other mode is using it
        // once the current map mode is destroyed
        _sound.AddGameModeOwner(MapMode::CurrentInstance());
    }
    else {
        PRINT_WARNING << "Couldn't load environmental sound file: "
            << sound_filename << std::endl;
    }

    _sound.SetLooping(true);
    _sound.SetVolume(0.0f);
    _sound.Stop();

    _strength = strength;
    // Invalidates negative or near 0 values.
    if (_strength <= 0.2f)
        _strength = 0.0f;

    _time_remaining = 0.0f;
    _playing = false;

    _tile_position.x = x;
    _tile_position.y = y;

    _collision_mask = NO_COLLISION;

    // Register the object to the sound vector
    MapMode::CurrentInstance()->GetObjectSupervisor()->AddAmbientSound(this);
}

SoundObject* SoundObject::Create(const std::string& sound_filename,
                                 float x, float y, float strength)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new SoundObject(sound_filename, x, y, strength);
}

void SoundObject::SetMaxVolume(float max_volume)
{
    _max_sound_volume = max_volume;

    if (_max_sound_volume < 0.0f)
        _max_sound_volume = 0.0f;
    else if (_max_sound_volume > 1.0f)
        _max_sound_volume = 1.0f;
}

void SoundObject::Update()
{
    // Don't activate a sound which is too weak to be heard anyway.
    if (_strength < 1.0f || _max_sound_volume <= 0.0f)
        return;

    if (!_activated)
        return;

    // Update the volume only every 100ms
    _time_remaining -= (int32_t)vt_system::SystemManager->GetUpdateTime();
    if (_time_remaining > 0)
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

    float distance = (_tile_position.x - center.x) * (_tile_position.x - center.x);
    distance += (_tile_position.y - center.y) * (_tile_position.y - center.y);
    //distance = sqrtf(_distance); <-- We don't actually need it as it is slow.

    float strength2 = _strength * _strength;

    if (distance >= strength2) {
        if (_playing) {
            _sound.FadeOut(1000.0f);
            _playing = false;
        }
        return;
    }

    // We add a one-half-tile neutral margin where nothing happens
    // to avoid the edge case where the sound repeatedly starts/stops
    // because of the camera position rounding.
    if (distance >= (strength2 - 0.5f))
        return;

    float volume = _max_sound_volume - (_max_sound_volume * (distance / strength2));
    _sound.SetVolume(volume);

    if (!_playing) {
        _sound.FadeIn(1000.0f);
        _playing = true;
    }
}

void SoundObject::Stop()
{
    if (!_activated)
        return;

    _sound.FadeOut(1000);
    _activated = false;
}

void SoundObject::Start()
{
    if (_activated)
        return;

    _activated = true;

    // Restores the sound state
    Update();
}

// ----------------------------------------------------------------------------
// ---------- TreasureObject Class Functions
// ----------------------------------------------------------------------------

TreasureObject::TreasureObject(const std::string &treasure_name,
                               MapObjectDrawLayer layer,
                               const std::string &closed_animation_file,
                               const std::string &opening_animation_file,
                               const std::string &open_animation_file) :
    PhysicalObject(layer)
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
    if(!opening_animation_file.empty())
        opening_anim.LoadFromAnimationScript(opening_animation_file);
    open_anim.LoadFromAnimationScript(open_animation_file);

    // Set the collision rectangle according to the dimensions of the first frame
    SetCollPixelHalfWidth(closed_anim.GetWidth() / 2.0f);
    SetCollPixelHeight(closed_anim.GetHeight());

    AddAnimation(closed_anim);
    AddAnimation(opening_anim);
    AddAnimation(open_anim);

    _LoadState();
}

TreasureObject* TreasureObject::Create(const std::string &treasure_name,
                                       MapObjectDrawLayer layer,
                                       const std::string &closed_animation_file,
                                       const std::string &opening_animation_file,
                                       const std::string &open_animation_file)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new TreasureObject(treasure_name, layer,
                              closed_animation_file,
                              opening_animation_file,
                              open_animation_file);
}

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
        IF_PRINT_WARNING(MAP_DEBUG) << "attempted to retrieve an already taken treasure: " << _object_id << std::endl;
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

    if ((GetCurrentAnimationId() == TREASURE_OPENING_ANIM) && (_animations[TREASURE_OPENING_ANIM].IsAnimationFinished()))
        SetCurrentAnimation(TREASURE_OPEN_ANIM);

    if (!_is_opening || GetCurrentAnimationId() != TREASURE_OPEN_ANIM)
        return;

    // Once opened, we handle potential events and the display of the treasure supervisor
    EventSupervisor *event_manager = MapMode::CurrentInstance()->GetEventSupervisor();

    if (!_events_triggered) {
        // Trigger potential events after opening
        for (uint32_t i = 0; i < _events.size(); ++i) {
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

bool TreasureObject::AddItem(uint32_t id, uint32_t quantity)
{
    if(!_treasure)
        return false;
    return _treasure->AddItem(id, quantity);
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
                             MapObjectDrawLayer layer,
                             const std::string &off_animation_file,
                             const std::string &on_animation_file,
                             const std::string& off_event_id,
                             const std::string& on_event_id) :
    PhysicalObject(layer),
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
    on_anim.LoadFromAnimationScript(on_animation_file);

    // Set a default collision area making the trigger respond when the character
    // is rather having his/her two feet on it.
    SetCollPixelHalfWidth(off_anim.GetWidth() * 0.25f);
    SetCollPixelHeight(off_anim.GetHeight() * 2.0f / 3.0f);
    SetImgPixelHalfWidth(off_anim.GetWidth() * 0.5f);
    SetImgPixelHeight(off_anim.GetHeight());

    AddAnimation(off_anim);
    AddAnimation(on_anim);

    _LoadState();
}

TriggerObject* TriggerObject::Create(const std::string &trigger_name,
                                     MapObjectDrawLayer layer,
                                     const std::string &off_animation_file,
                                     const std::string &on_animation_file,
                                     const std::string& off_event_id,
                                     const std::string& on_event_id)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new TriggerObject(trigger_name, layer,
                             off_animation_file, on_animation_file,
                             off_event_id, on_event_id);
}

void TriggerObject::Update()
{
    PhysicalObject::Update();

    // The trigger can't be toggle by the character, nothing will happen
    if (!_triggerable_by_character)
        return;

    // TODO: Permit other behaviour
    if (_trigger_state)
        return;

    MapMode *map_mode = MapMode::CurrentInstance();
    if (!map_mode->IsCameraOnVirtualFocus()
            && MapRectangle::CheckIntersection(map_mode->GetCamera()->GetGridCollisionRectangle(), GetGridCollisionRectangle())) {

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
    _last_id(1), //! Every object Id must be > 0 since 0 is reserved for speakerless dialogues.
    _visible_party_member(nullptr)
{}

ObjectSupervisor::~ObjectSupervisor()
{
    // Delete all the map objects
    for(uint32_t i = 0; i < _all_objects.size(); ++i) {
        delete(_all_objects[i]);
    }

    for(uint32_t i = 0; i < _zones.size(); ++i) {
        delete(_zones[i]);
    }
}

MapObject* ObjectSupervisor::GetObject(uint32_t object_id)
{
    if(object_id >= _all_objects.size())
        return nullptr;
    else
        return _all_objects[object_id];
}

VirtualSprite* ObjectSupervisor::GetSprite(uint32_t object_id)
{
    MapObject* object = GetObject(object_id);

    if(object == nullptr)
        return nullptr;

    VirtualSprite *sprite = dynamic_cast<VirtualSprite *>(object);
    if(sprite == nullptr) {
        IF_PRINT_WARNING(MAP_DEBUG) << "could not cast map object to sprite type, object id: " << object_id << std::endl;
        return nullptr;
    }

    return sprite;
}

void ObjectSupervisor::RegisterObject(MapObject* object)
{
    if (!object || object->GetObjectID() <= 0) {
        PRINT_WARNING << "The object couldn't be registered. It is either nullptr or with an id <= 0." << std::endl;
        return;
    }

    uint32_t obj_id = (uint32_t)object->GetObjectID();
    // Adds the object to the all object collection.
    if (obj_id >= _all_objects.size())
        _all_objects.resize(obj_id + 1, nullptr);
    _all_objects[obj_id] = object;

    switch(object->GetObjectDrawLayer()) {
    case FLATGROUND_OBJECT:
        _flat_ground_objects.push_back(object);
        break;
    case GROUND_OBJECT:
        _ground_objects.push_back(object);
        break;
    case PASS_OBJECT:
        _pass_objects.push_back(object);
        break;
    case SKY_OBJECT:
        _sky_objects.push_back(object);
        break;
    case NO_LAYER_OBJECT:
    default: // Nothing to do. the object is registered in all objects only.
        break;
    }
}

void ObjectSupervisor::AddAmbientSound(SoundObject* object)
{
    if(!object) {
        PRINT_WARNING << "Couldn't add nullptr SoundObject* object." << std::endl;
        return;
    }

    _sound_objects.push_back(object);
}

void ObjectSupervisor::AddLight(Light* light)
{
    if (light == nullptr) {
        PRINT_WARNING << "Couldn't add nullptr Light* object." << std::endl;
        return;
    }

    _lights.push_back(light);
}

void ObjectSupervisor::AddHalo(Halo* halo)
{
    if (halo == nullptr) {
        PRINT_WARNING << "Couldn't add nullptr Halo* object." << std::endl;
        return;
    }

    _halos.push_back(halo);
}

void ObjectSupervisor::AddSavePoint(SavePoint* save_point)
{
    if (save_point == nullptr) {
        PRINT_WARNING << "Couldn't add nullptr SavePoint* object." << std::endl;
        return;
    }

    _save_points.push_back(save_point);
}

void ObjectSupervisor::AddZone(MapZone* zone)
{
    if(!zone) {
        PRINT_WARNING << "Couldn't add nullptr zone." << std::endl;
        return;
    }
    _zones.push_back(zone);
}

void ObjectSupervisor::DeleteObject(MapObject* object)
{
    if (!object)
        return;

    for (uint32_t i = 0; i < _all_objects.size(); ++i) {
        // We only set it to null without removing its place in memory
        // to avoid breaking the vector key used as object id,
        // so that in: _all_objects[key]: key = object_id.
        if (_all_objects[i] == object) {
            _all_objects[i] = nullptr;
            break;
        }
    }

    std::vector<MapObject*>::iterator it;
    std::vector<MapObject*>::iterator it_end;
    std::vector<MapObject*>* to_iterate = nullptr;

    switch(object->GetObjectDrawLayer()) {
    case FLATGROUND_OBJECT:
        it = _flat_ground_objects.begin();
        it_end = _flat_ground_objects.end();
        to_iterate = &_flat_ground_objects;
        break;
    case GROUND_OBJECT:
        it = _ground_objects.begin();
        it_end = _ground_objects.end();
        to_iterate = &_ground_objects;
        break;
    case PASS_OBJECT:
        it = _pass_objects.begin();
        it_end = _pass_objects.end();
        to_iterate = &_pass_objects;
        break;
    case SKY_OBJECT:
        it = _sky_objects.begin();
        it_end = _sky_objects.end();
        to_iterate = &_sky_objects;
        break;
    case NO_LAYER_OBJECT:
    default:
        delete object;
        return;
    }

    for(; it != it_end; ++it) {
        if (*it == object) {
            to_iterate->erase(it);
            break;
        }
    }
    delete object;
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
    for(uint16_t y = 0; y < _num_grid_y_axis; ++y) {
        _collision_grid.push_back(std::vector<uint32_t>());
        map_file.ReadUIntVector(y, _collision_grid.back());
    }
    map_file.CloseTable();
    _num_grid_x_axis = _collision_grid[0].size();
    return true;
}

void ObjectSupervisor::Update()
{
    for(uint32_t i = 0; i < _flat_ground_objects.size(); ++i)
        _flat_ground_objects[i]->Update();
    for(uint32_t i = 0; i < _ground_objects.size(); ++i)
        _ground_objects[i]->Update();
    // Update save point animation and activeness.
    _UpdateSavePoints();
    for(uint32_t i = 0; i < _pass_objects.size(); ++i)
        _pass_objects[i]->Update();
    for(uint32_t i = 0; i < _sky_objects.size(); ++i)
        _sky_objects[i]->Update();
    for(uint32_t i = 0; i < _halos.size(); ++i)
        _halos[i]->Update();
    for(uint32_t i = 0; i < _lights.size(); ++i)
        _lights[i]->Update();
    for(uint32_t i = 0; i < _zones.size(); ++i)
        _zones[i]->Update();

    _UpdateAmbientSounds();
}

void ObjectSupervisor::DrawSavePoints()
{
    for(uint32_t i = 0; i < _save_points.size(); ++i) {
        _save_points[i]->Draw();
    }
}

void ObjectSupervisor::DrawFlatGroundObjects()
{
    for(uint32_t i = 0; i < _flat_ground_objects.size(); ++i) {
        _flat_ground_objects[i]->Draw();
    }
}

void ObjectSupervisor::DrawGroundObjects(const bool second_pass)
{
    for(uint32_t i = 0; i < _ground_objects.size(); i++) {
        if(_ground_objects[i]->IsDrawOnSecondPass() == second_pass) {
            _ground_objects[i]->Draw();
        }
    }
}

void ObjectSupervisor::DrawPassObjects()
{
    for(uint32_t i = 0; i < _pass_objects.size(); i++) {
        _pass_objects[i]->Draw();
    }
}

void ObjectSupervisor::DrawSkyObjects()
{
    for(uint32_t i = 0; i < _sky_objects.size(); i++) {
        _sky_objects[i]->Draw();
    }
}

void ObjectSupervisor::DrawLights()
{
    for(uint32_t i = 0; i < _halos.size(); ++i)
        _halos[i]->Draw();
    for(uint32_t i = 0; i < _lights.size(); ++i)
        _lights[i]->Draw();
}

void ObjectSupervisor::DrawInteractionIcons()
{
    MapMode *map_mode = MapMode::CurrentInstance();
    // Don't show a dialogue bubble when not in exploration mode.
    if (map_mode->CurrentState() != STATE_EXPLORE)
        return;

    // Other logical conditions preventing the bubble from being displayed
    if (!map_mode->IsShowGUI() || map_mode->IsCameraOnVirtualFocus())
        return;

    for(uint32_t i = 0; i < _ground_objects.size(); i++) {
        if (_ground_objects[i]->GetObjectType() == SPRITE_TYPE) {
            MapSprite* mapSprite = static_cast<MapSprite *>(_ground_objects[i]);
            mapSprite->DrawDialogIcon();
        }
        _ground_objects[i]->DrawInteractionIcon();
    }

    for(uint32_t i = 0; i < _zones.size(); i++) {
        _zones[i]->DrawInteractionIcon();
    }
}

void ObjectSupervisor::_UpdateSavePoints()
{
    MapMode* map_mode = MapMode::CurrentInstance();
    VirtualSprite *sprite = map_mode->GetCamera();

    MapRectangle spr_rect;
    if(sprite)
        spr_rect = sprite->GetGridCollisionRectangle();

    for(std::vector<SavePoint *>::iterator it = _save_points.begin();
            it != _save_points.end(); ++it) {
        if (map_mode->AreSavePointsEnabled()) {
            (*it)->SetActive(MapRectangle::CheckIntersection(spr_rect,
                             (*it)->GetGridCollisionRectangle()));
        }
        else {
            (*it)->SetActive(false);
        }

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
    for(uint32_t i = 0; i < _zones.size(); ++i)
        _zones[i]->Draw();
}

MapObject *ObjectSupervisor::_FindNearestSavePoint(const VirtualSprite *sprite)
{
    if(sprite == nullptr)
        return nullptr;

    for(std::vector<SavePoint *>::iterator it = _save_points.begin();
            it != _save_points.end(); ++it) {

        if(MapRectangle::CheckIntersection(sprite->GetGridCollisionRectangle(),
                                           (*it)->GetGridCollisionRectangle())) {
            return (*it);
        }
    }
    return nullptr;
}

std::vector<MapObject*>& ObjectSupervisor::_GetObjectsFromDrawLayer(MapObjectDrawLayer layer)
{
    switch(layer)
    {
    case FLATGROUND_OBJECT:
        return _flat_ground_objects;
    default:
    case GROUND_OBJECT:
        return _ground_objects;
    case PASS_OBJECT:
        return _pass_objects;
    case SKY_OBJECT:
        return _sky_objects;
    }
}

MapObject *ObjectSupervisor::FindNearestInteractionObject(const VirtualSprite *sprite, float search_distance)
{
    if(!sprite)
        return 0;

    // Using the sprite's direction, determine the boundaries of the search area to check for objects
    MapRectangle search_area = sprite->GetGridCollisionRectangle();
    if(sprite->GetDirection() & FACING_NORTH) {
        search_area.bottom = search_area.top;
        search_area.top = search_area.top - search_distance;
    } else if(sprite->GetDirection() & FACING_SOUTH) {
        search_area.top = search_area.bottom;
        search_area.bottom = search_area.bottom + search_distance;
    } else if(sprite->GetDirection() & FACING_WEST) {
        search_area.right = search_area.left;
        search_area.left = search_area.left - search_distance;
    } else if(sprite->GetDirection() & FACING_EAST) {
        search_area.left = search_area.right;
        search_area.right = search_area.right + search_distance;
    } else {
        IF_PRINT_WARNING(MAP_DEBUG) << "sprite was set to invalid direction: " << sprite->GetDirection() << std::endl;
        return nullptr;
    }

    // Go through all objects and determine which (if any) lie within the search area

    // A vector to hold objects which are inside the search area (either partially or fully)
    std::vector<MapObject *> valid_objects;
    // A pointer to the vector of objects to search
    std::vector<MapObject *>* search_vector = &_GetObjectsFromDrawLayer(sprite->GetObjectDrawLayer());

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

        MapRectangle object_rect = (*it)->GetGridCollisionRectangle();
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

    for(uint32_t i = 1; i < valid_objects.size(); i++) {
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

    MapRectangle obj_rect = obj->GetGridCollisionRectangle();
    return MapRectangle::CheckIntersection(rect, obj_rect);
}

bool ObjectSupervisor::IsPositionOccupiedByObject(float x, float y, MapObject *object)
{
    if(object == nullptr) {
        IF_PRINT_WARNING(MAP_DEBUG) << "nullptr pointer passed into function argument" << std::endl;
        return false;
    }

    MapRectangle rect = object->GetGridCollisionRectangle();

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
    MapRectangle sprite_rect = object->GetGridCollisionRectangle(x_pos, y_pos);

    // Check if any part of the object's collision rectangle is outside of the map boundary
    if(sprite_rect.left < 0.0f || sprite_rect.right >= static_cast<float>(_num_grid_x_axis) ||
            sprite_rect.top < 0.0f || sprite_rect.bottom >= static_cast<float>(_num_grid_y_axis)) {
        return WALL_COLLISION;
    }

    // Check for the absence of collision checking after the map boundaries check,
    // So that no collision beings won't get out of the map.
    if(object->GetCollisionMask() == NO_COLLISION)
        return NO_COLLISION;

    // Check if the object's collision rectangle overlaps with any unwalkable elements on the collision grid
    // Grid based collision is not done for objects in the sky layer
    if(object->GetObjectDrawLayer() != vt_map::SKY_OBJECT && object->GetCollisionMask() & WALL_COLLISION) {
        // Determine if the object's collision rectangle overlaps any unwalkable tiles
        // Note that because the sprite's collision rectangle was previously determined to be within the map bounds,
        // the map grid tile indeces referenced in this loop are all valid entries and do not need to be checked for out-of-bounds conditions
        for(uint32_t y = static_cast<uint32_t>(sprite_rect.top); y <= static_cast<uint32_t>(sprite_rect.bottom); ++y) {
            for(uint32_t x = static_cast<uint32_t>(sprite_rect.left); x <= static_cast<uint32_t>(sprite_rect.right); ++x) {
                // Checks the collision grid at the row-column at the object's current context
                if(_collision_grid[y][x] > 0)
                    return WALL_COLLISION;
            }
        }
    }

    std::vector<MapObject *>* objects = &_GetObjectsFromDrawLayer(object->GetObjectDrawLayer());

    std::vector<vt_map::private_map::MapObject *>::const_iterator it, it_end;
    for(it = objects->begin(), it_end = objects->end(); it != it_end; ++it) {
        MapObject *collision_object = *it;
        // Check if the object exists and has the no_collision property enabled
        if(!collision_object || collision_object->GetCollisionMask() == NO_COLLISION)
            continue;

        // Object and sprite are the same
        if(collision_object->GetObjectID() == object->GetObjectID())
            continue;

        // If the two objects aren't colliding, try next.
        if(!CheckObjectCollision(sprite_rect, collision_object))
            continue;

        // The two objects are colliding, return the potentially asked pointer to it.
        if(collision_object_ptr != nullptr)
            *collision_object_ptr = collision_object;

        // When the collision mask is taking in account the collision type
        // we can return it. Otherwise, just ignore the sprite colliding.
        COLLISION_TYPE collision = GetCollisionFromObjectType(collision_object);
        if(object->GetCollisionMask() & collision)
            return collision;
        else
            continue;
    }

    return NO_COLLISION;
} // bool ObjectSupervisor::DetectCollision(VirtualSprite* sprite, float x, float y, MapObject** collision_object_ptr)

Path ObjectSupervisor::FindPath(VirtualSprite *sprite, const MapPosition &destination, uint32_t max_cost)
{
    // NOTE: Refer to the implementation of the A* algorithm to understand
    // what all these lists and score values are for.
    static const uint32_t basic_gcost = 10;

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
    PathNode source_node(static_cast<int16_t>(sprite->GetXPosition()), static_cast<int16_t>(sprite->GetYPosition()));
    // The ending node.
    PathNode dest(static_cast<int16_t>(destination.x), static_cast<int16_t>(destination.y));

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
    uint32_t x_delta, y_delta;
    // The number to add to a node's g_score, depending on whether it is a lateral or diagonal movement
    int16_t g_add;

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
        for(uint8_t i = 0; i < 8; ++i) {
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
            if (max_cost > 0 && (uint32_t)(best_node.g_score + g_add) >= max_cost * basic_gcost)
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
        } // for (uint8_t i = 0; i < 8; ++i)
    } // while (open_list.empty() == false)

    if(open_list.empty() == true) {
        IF_PRINT_WARNING(MAP_DEBUG) << "could not find path to destination" << std::endl;
        return path;
    }

    // Add the destination node to the vector.
    path.push_back(destination);

    // Retain the last node parent, and remove it from the closed list
    int16_t parent_x = best_node.parent_x;
    int16_t parent_y = best_node.parent_y;
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
    for(uint32_t i = 0; i < _all_objects.size(); ++i) {
        if (_all_objects[i] && _all_objects[i]->GetObjectType() == ENEMY_TYPE) {
            EnemySprite* enemy = dynamic_cast<EnemySprite*>(_all_objects[i]);
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
    VideoManager->Move(GRID_LENGTH * (frame->tile_x_offset - 0.5f), GRID_LENGTH * (frame->tile_y_offset - 1.0f));

    for (uint32_t y = static_cast<uint32_t>(frame->tile_y_start * 2);
         y < static_cast<uint32_t>((frame->tile_y_start + frame->num_draw_y_axis) * 2); ++y) {
        for(uint32_t x = static_cast<uint32_t>(frame->tile_x_start * 2);
            x < static_cast<uint32_t>((frame->tile_x_start + frame->num_draw_x_axis) * 2); ++x) {

            // Draw the collision rectangle.
            if (_collision_grid[y][x] > 0)
                VideoManager->DrawRectangle(GRID_LENGTH, GRID_LENGTH, Color(1.0f, 0.0f, 0.0f, 0.6f));

            VideoManager->MoveRelative(GRID_LENGTH, 0.0f);
        } // x
        VideoManager->MoveRelative(-static_cast<float>(frame->num_draw_x_axis * 2) * GRID_LENGTH, GRID_LENGTH);
    } // y
}

bool ObjectSupervisor::IsStaticCollision(float x, float y)
{
    if (!IsWithinMapBounds(x, y))
        return true;

    //if the map's collision context is set to 1, we can return since we know there is a collision
    if (IsMapCollision(static_cast<uint32_t>(x), static_cast<uint32_t>(y)))
        return true;

    std::vector<vt_map::private_map::MapObject *>::const_iterator it, it_end;
    for(it = _ground_objects.begin(), it_end = _ground_objects.end(); it != it_end; ++it) {
        MapObject *collision_object = *it;
        // Check if the object exists and has the no_collision property enabled
        if(!collision_object || collision_object->GetCollisionMask() == NO_COLLISION)
            continue;

        //only check physical objects. we don't care about sprites and enemies, treasure boxes, etc
        if(collision_object->GetObjectType() != PHYSICAL_TYPE)
            continue;

        //get the rect. if the x and y fields are within the rect, we have a collision here
        MapRectangle rect = collision_object->GetGridCollisionRectangle();
        //we know x and y are inside the map. So, just test then as a box vs point test
        if(rect.top < y && y < rect.bottom &&
           rect.left < x && x < rect.right)
           return true;
    }

    return false;
}

void ObjectSupervisor::StopSoundObjects()
{
    _sound_objects_to_restart.clear();
    for (uint32_t i = 0; i < _sound_objects.size(); ++i) {
        vt_audio::SoundDescriptor& sound = _sound_objects[i]->GetSoundDescriptor();
        if (sound.GetState() == vt_audio::AUDIO_STATE_PLAYING
                || sound.GetState() == vt_audio::AUDIO_STATE_FADE_IN) {
            sound.Stop();
            _sound_objects_to_restart.push_back(_sound_objects[i]);
        }
    }
}

void ObjectSupervisor::RestartSoundObjects()
{
    for (uint32_t i = 0; i < _sound_objects_to_restart.size(); ++i) {
        vt_audio::SoundDescriptor& sound = _sound_objects_to_restart[i]->GetSoundDescriptor();
        if (sound.GetState() == vt_audio::AUDIO_STATE_STOPPED)
            sound.Play();
    }
    _sound_objects_to_restart.clear();
}

} // namespace private_map

} // namespace vt_map
