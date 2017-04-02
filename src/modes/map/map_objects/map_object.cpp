///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_objects/map_object.h"

#include "modes/map/map_mode.h"
#include "modes/map/map_object_supervisor.h"
#include "modes/map/map_sprites/map_virtual_sprite.h"

#include "engine/system.h"
#include "engine/video/video.h"
#include "common/global/global.h"

namespace vt_map
{

namespace private_map
{

MapObject::MapObject(MapObjectDrawLayer layer) :
    _object_id(-1),
    _img_pixel_half_width(0.0f),
    _img_pixel_height(0.0f),
    _img_screen_half_width(0.0f),
    _img_screen_height(0.0f),
    _img_grid_half_width(0.0f),
    _img_grid_height(0.0f),
    _coll_pixel_half_width(0.0f),
    _coll_pixel_height(0.0f),
    _coll_screen_half_width(0.0f),
    _coll_screen_height(0.0f),
    _coll_grid_half_width(0.0f),
    _coll_grid_height(0.0f),
    _updatable(true),
    _visible(true),
    _collision_mask(ALL_COLLISION),
    _draw_on_second_pass(false),
    _object_type(OBJECT_TYPE),
    _emote_animation(nullptr),
    _interaction_icon(nullptr),
    _emote_screen_offset_x(0.0f),
    _emote_screen_offset_y(0.0f),
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

    vt_video::VideoManager->Move(x_pos, y_pos);

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
    rect.left = x - _coll_screen_half_width;
    rect.right = x + _coll_screen_half_width;
    rect.top = y - _coll_screen_height;
    rect.bottom = y;
    return rect;
}

MapRectangle MapObject::GetScreenCollisionRectangle() const
{
    MapMode* mm = MapMode::CurrentInstance();
    MapRectangle rect;
    float x_screen_pos = mm->GetScreenXCoordinate(_tile_position.x);
    float y_screen_pos = mm->GetScreenYCoordinate(_tile_position.y);
    rect.left = x_screen_pos - _coll_screen_half_width;
    rect.right = x_screen_pos + _coll_screen_half_width;
    rect.top = y_screen_pos - _coll_screen_height;
    rect.bottom = y_screen_pos;
    return rect;
}

MapRectangle MapObject::GetScreenImageRectangle() const
{
    MapMode* mm = MapMode::CurrentInstance();
    MapRectangle rect;
    float x_screen_pos = mm->GetScreenXCoordinate(_tile_position.x);
    float y_screen_pos = mm->GetScreenYCoordinate(_tile_position.y);
    rect.left = x_screen_pos - _img_screen_half_width;
    rect.right = x_screen_pos + _img_screen_half_width;
    rect.top = y_screen_pos - _img_screen_height;
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
    _emote_animation = vt_global::GlobalManager->GetEmoteAnimation(emote_name);

    if(!_emote_animation) {
        PRINT_WARNING << "Invalid emote requested: " << emote_name << " for map object: "
                      << GetObjectID() << std::endl;
        return;
    }

    // Make the offset depend on the sprite direction and emote animation.
    vt_global::GlobalManager->GetEmoteOffset(_emote_screen_offset_x,
                                             _emote_screen_offset_y,
                                             emote_name,
                                             dir);
    // Scale the offsets for the map mode
    _emote_screen_offset_x = _emote_screen_offset_x * MAP_ZOOM_RATIO;
    _emote_screen_offset_y = _emote_screen_offset_y * MAP_ZOOM_RATIO;

    _emote_animation->ResetAnimation();
    _emote_time = _emote_animation->GetAnimationLength();
}

void MapObject::_UpdateEmote()
{
    if(!_emote_animation)
        return;

    _emote_time -= vt_system::SystemManager->GetUpdateTime();

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
    vt_video::VideoManager->MoveRelative(_emote_screen_offset_x,
                                         -_img_screen_height + _emote_screen_offset_y);
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
    vt_video::Color icon_color(1.0f, 1.0f, 1.0f, 0.0f);
    float icon_alpha = 1.0f - (fabs(GetXPosition() - map_mode->GetCamera()->GetXPosition())
                            + fabs(GetYPosition() - map_mode->GetCamera()->GetYPosition())) / INTERACTION_ICON_VISIBLE_RANGE;
    if (icon_alpha < 0.0f)
        icon_alpha = 0.0f;
    icon_color.SetAlpha(icon_alpha);

    vt_video::VideoManager->MoveRelative(0, -GetImgScreenHeight());
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

} // namespace private_map

} // namespace vt_map
