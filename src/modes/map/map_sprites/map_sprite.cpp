///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_sprites/map_sprite.h"

#include "modes/map/map_mode.h"
#include "modes/map/map_dialogue_supervisor.h"
#include "modes/map/map_dialogues/map_sprite_dialogue.h"
#include "modes/map/map_events.h"

#include "common/global/global.h"
#include "common/rectangle_2d.h"

#include "utils/script/script_read.h"
#include "engine/system.h"
#include "engine/video/image.h"

#include "utils/utils_files.h"
#include "utils/utils_numeric.h"
#include "utils/utils_strings.h"

using namespace vt_common;

namespace vt_map
{

namespace private_map
{

MapSprite::MapSprite(MapObjectDrawLayer layer) :
    VirtualSprite(layer),
    _face_portrait(0),
    _has_running_animations(false),
    _current_anim_direction(ANIM_SOUTH),
    _current_custom_animation(0),
    _next_dialogue(-1),
    _has_available_dialogue(false),
    _has_unseen_dialogue(false),
    _dialogue_started(false),
    _custom_animation_on(false),
    _custom_animation_time(0),
    _infinite_custom_animation(false),
    _saved_current_anim_direction(ANIM_SOUTH)
{
    _object_type = SPRITE_TYPE;

    // Points the current animation to the standing animation vector by default
    _animation = &_standing_animations;
}

MapSprite::~MapSprite()
{
    if (_face_portrait)
        delete _face_portrait;
}

MapSprite* MapSprite::Create(MapObjectDrawLayer layer)
{
    // The object auto register to the object supervisor
    // and will later handle deletion.
    return new MapSprite(layer);
}

bool _LoadAnimations(std::vector<vt_video::AnimatedImage>& animations, const std::string &filename)
{
    // Prepare to add the animations for each directions, if needed.

    // In case of reloading
    animations.clear();
    for(uint8_t i = 0; i < NUM_ANIM_DIRECTIONS; ++i)
        animations.push_back(vt_video::AnimatedImage());

    vt_script::ReadScriptDescriptor animations_script;
    if(!animations_script.OpenFile(filename))
        return false;

    if(!animations_script.DoesTableExist("sprite_animation")) {
        PRINT_WARNING << "No 'sprite_animation' table in 4-direction animation script file: " << filename << std::endl;
        animations_script.CloseFile();
        return false;
    }

    animations_script.OpenTable("sprite_animation");

    std::string image_filename = animations_script.ReadString("image_filename");
    if(!vt_utils::DoesFileExist(image_filename)) {
        PRINT_WARNING << "The image file doesn't exist: " << image_filename << std::endl;
        animations_script.CloseTable();
        animations_script.CloseFile();
        return false;
    }

    bool blended_animation = false;
    if (animations_script.DoesBoolExist("blended_animation")) {
        blended_animation = animations_script.ReadBool("blended_animation");
    }

    uint32_t rows = animations_script.ReadUInt("rows");
    uint32_t columns = animations_script.ReadUInt("columns");

    if(!animations_script.DoesTableExist("frames")) {
        animations_script.CloseAllTables();
        animations_script.CloseFile();
        PRINT_WARNING << "No frame table in file: " << filename << std::endl;
        return false;
    }

    std::vector<vt_video::StillImage> image_frames;
    // Load the image data
    if(!vt_video::ImageDescriptor::LoadMultiImageFromElementGrid(image_frames, image_filename, rows, columns)) {
        PRINT_WARNING << "Couldn't load elements from image file: " << image_filename
                      << " (in file: " << filename << ")" << std::endl;
        animations_script.CloseAllTables();
        animations_script.CloseFile();
        return false;
    }

    std::vector <uint32_t> frames_directions_ids;
    animations_script.ReadTableKeys("frames", frames_directions_ids);

    // open the frames table
    animations_script.OpenTable("frames");

    for(uint32_t i = 0; i < frames_directions_ids.size(); ++i) {
        if(frames_directions_ids[i] >= NUM_ANIM_DIRECTIONS) {
            PRINT_WARNING << "Invalid direction id(" << frames_directions_ids[i]
                          << ") in file: " << filename << std::endl;
            continue;
        }

        uint32_t anim_direction = frames_directions_ids[i];

        // Opens frames[ANIM_DIRECTION]
        animations_script.OpenTable(anim_direction);

        // Loads the frames data
        std::vector<uint32_t> frames_ids;
        std::vector<uint32_t> frames_duration;

        uint32_t num_frames = animations_script.GetTableSize();
        for(uint32_t frames_table_id = 0;  frames_table_id < num_frames; ++frames_table_id) {
            // Opens frames[ANIM_DIRECTION][frame_table_id]
            animations_script.OpenTable(frames_table_id);

            int32_t frame_id = animations_script.ReadInt("id");
            int32_t frame_duration = animations_script.ReadInt("duration");

            if(frame_id < 0 || frame_duration < 0 || frame_id >= (int32_t)image_frames.size()) {
                PRINT_WARNING << "Invalid frame (" << frames_table_id << ") in file: "
                              << filename << std::endl;
                PRINT_WARNING << "Request for frame id: " << frame_id << ", duration: "
                              << frame_duration << " is not possible." << std::endl;
                continue;
            }

            frames_ids.push_back((uint32_t)frame_id);
            frames_duration.push_back((uint32_t)frame_duration);

            animations_script.CloseTable(); // frames[ANIM_DIRECTION][frame_table_id] table
        }

        // Actually create the animation data
        animations[anim_direction].Clear();
        animations[anim_direction].ResetAnimation();
        animations[anim_direction].SetAnimationBlended(blended_animation);
        for(uint32_t j = 0; j < frames_ids.size(); ++j) {
            // Set the dimension of the requested frame
            animations[anim_direction].AddFrame(image_frames[frames_ids[j]], frames_duration[j]);
        }

        // Closes frames[ANIM_DIRECTION]
        animations_script.CloseTable();

    } // for each directions

    // Close the 'frames' table and set the dimensions
    animations_script.CloseTable();

    float frame_width = animations_script.ReadFloat("frame_width");
    float frame_height = animations_script.ReadFloat("frame_height");

    // Load requested dimensions
    for(uint8_t i = 0; i < NUM_ANIM_DIRECTIONS; ++i) {
        if(frame_width > 0.0f && frame_height > 0.0f) {
            animations[i].SetDimensions(frame_width, frame_height);
        } else if(vt_utils::IsFloatEqual(animations[i].GetWidth(), 0.0f)
                  && vt_utils::IsFloatEqual(animations[i].GetHeight(), 0.0f)) {
            // If the animation dimensions are not set, we're using the first frame size.
            animations[i].SetDimensions(image_frames.begin()->GetWidth(), image_frames.begin()->GetHeight());
        }

        // Rescale to fit the map mode coordinates system.
        MapMode::ScaleToMapZoomRatio(animations[i]);
    }

    animations_script.CloseTable(); // sprite_animation table
    animations_script.CloseFile();

    return true;
} // bool _LoadAnimations()

void MapSprite::ClearAnimations()
{
    _grayscale = false;
    _standing_animations.clear();
    _walking_animations.clear();
    _running_animations.clear();
    _has_running_animations = false;

    // Disable and clear the custom animations
    _current_custom_animation = 0;
    _custom_animation_on = false;
    _custom_animation_time = 0;
    _infinite_custom_animation = false;
    _custom_animations.clear();
}

bool MapSprite::LoadStandingAnimations(const std::string &filename)
{
    return _LoadAnimations(_standing_animations, filename);
}

bool MapSprite::LoadWalkingAnimations(const std::string &filename)
{
    return _LoadAnimations(_walking_animations, filename);
}

bool MapSprite::LoadRunningAnimations(const std::string &filename)
{
    _has_running_animations = _LoadAnimations(_running_animations, filename);

    return _has_running_animations;
}

bool MapSprite::LoadCustomAnimation(const std::string &animation_name, const std::string &filename)
{
    if(_custom_animations.find(animation_name) != _custom_animations.end()) {
        PRINT_WARNING << "The animation " << animation_name << " is already existing." << std::endl;
        return false;
    }

    vt_video::AnimatedImage animation;
    if(animation.LoadFromAnimationScript(filename)) {
        MapMode::ScaleToMapZoomRatio(animation);
        _custom_animations.insert(std::make_pair(animation_name, animation));
        return true;
    }

    return false;
}

void MapSprite::SetCustomAnimation(const std::string &animation_name, int32_t time)
{
    // If there is no key, there will be no custom animation to display
    if(animation_name.empty()) {
        _custom_animation_on = false;
        return;
    }

    // Same if the key isn't found
    auto it = _custom_animations.find(animation_name);
    if(it == _custom_animations.end()) {
        PRINT_WARNING << "Couldn't find any custom animation '" << animation_name
            << "' for sprite: " << GetSpriteName() << std::endl;
        _custom_animation_on = false;
        return;
    }

    vt_video::AnimatedImage &animation = it->second;
    animation.ResetAnimation();

    _infinite_custom_animation = false;
    if(time == -1)
        time = animation.GetAnimationLength();
    else if (time == 0)
        _infinite_custom_animation = true;


    _custom_animation_time = time;
    _current_custom_animation = &animation;
    _custom_animation_on = true;
}

void MapSprite::ReloadSprite(const std::string& sprite_name)
{
    // Don't reload if it's already the same map sprite
    if (sprite_name == GetSpriteName())
        return;

    vt_script::ReadScriptDescriptor& script = vt_global::GlobalManager->GetMapSpriteScript();

    if (!script.IsFileOpen())
        return;

    if (!script.OpenTable(sprite_name)) {
        PRINT_WARNING << "No map sprite name: " << sprite_name << " found!!"
            << std::endl;
        return;
    }

    SetName(script.ReadString("name"));
    SetSpriteName(sprite_name);
    SetCollPixelHalfWidth(script.ReadFloat("coll_half_width"));
    SetCollPixelHeight(script.ReadFloat("coll_height"));
    SetImgPixelHalfWidth(script.ReadFloat("img_half_width"));
    SetImgPixelHeight(script.ReadFloat("img_height"));
    if (script.DoesStringExist("face_portrait"))
        LoadFacePortrait(script.ReadString("face_portrait"));

    if (!script.OpenTable("standard_animations")) {
        PRINT_WARNING << "No 'standard_animations' table in: "
            << sprite_name << std::endl;
        script.CloseTable(); // sprite_name
        return;
    }

    ClearAnimations();
    LoadStandingAnimations(script.ReadString("idle"));
    LoadWalkingAnimations(script.ReadString("walk"));
    if (script.DoesStringExist("run"))
        LoadRunningAnimations(script.ReadString("run"));

    script.CloseTable(); // standard_animations

    // Load potential custom animations
    if (!script.DoesTableExist("custom_animations")) {
        script.CloseTable(); // sprite_name
        return;
    }

    std::vector<std::string> anim_names;
    script.ReadTableKeys("custom_animations", anim_names);
    script.OpenTable("custom_animations");

    for (uint32_t i = 0; i < anim_names.size(); ++i)
        LoadCustomAnimation(anim_names[i], script.ReadString(anim_names[i]));

    // Put the tabl in the state we found it.
    script.CloseTable(); // custom_animations
    script.CloseTable(); // sprite_name
}

void MapSprite::LoadFacePortrait(const std::string &filename)
{
    if(_face_portrait)
        delete _face_portrait;

    _face_portrait = new vt_video::StillImage();
    if(!_face_portrait->Load(filename)) {
        delete _face_portrait;
        _face_portrait = 0;
        PRINT_ERROR << "failed to load face portrait" << std::endl;
    }
}

void MapSprite::SetGrayscale(bool grayscale) {
    _grayscale = grayscale;
    for (vt_video::AnimatedImage& animation : _standing_animations) {
        animation.SetGrayscale(grayscale);
    }
    for (vt_video::AnimatedImage& animation : _walking_animations) {
        animation.SetGrayscale(grayscale);
    }
    for (vt_video::AnimatedImage& animation : _running_animations) {
        animation.SetGrayscale(grayscale);
    }
    auto it = _custom_animations.begin();
    auto it_end = _custom_animations.end();
    for (; it != it_end; ++it) {
        it->second.SetGrayscale(grayscale);
    }
}

void MapSprite::Update()
{
    // Stores the last value of moved_position to determine when a change in sprite movement between calls to this function occurs
    bool was_moved = _moved_position;

    // This call will update the sprite's position and perform collision detection
    VirtualSprite::Update();

    // if it's a custom animation, just display that and ignore everything else
    if(_custom_animation_on && _current_custom_animation) {
        // Check whether the custom animation can be freed
        if(!_infinite_custom_animation && _custom_animation_time <= 0) {
            _custom_animation_on = false;
            _current_custom_animation = 0;
            _custom_animation_time = 0;
        } else {
            if (!_infinite_custom_animation)
                _custom_animation_time -= vt_system::SystemManager->GetUpdateTime();
            _current_custom_animation->Update();
        }
        return;
    }

    // Save the previous animation state
    uint8_t last_anim_direction = _current_anim_direction;
    std::vector<vt_video::AnimatedImage>* last_animation = _animation;

    // Set the sprite's animation to the standing still position if movement has just stopped
    if(!_moved_position) {
        // Set the current movement animation to zero progress
        if(was_moved)
            _animation->at(_current_anim_direction).ResetAnimation();

        // Determine the correct standing frame to display
        if(!_control_event || _state_saved) {
            _animation = &_standing_animations;

            if(_direction & FACING_NORTH) {
                _current_anim_direction = ANIM_NORTH;
            } else if(_direction & FACING_SOUTH) {
                _current_anim_direction = ANIM_SOUTH;
            } else if(_direction & FACING_WEST) {
                _current_anim_direction = ANIM_WEST;
            } else if(_direction & FACING_EAST) {
                _current_anim_direction = ANIM_EAST;
            } else {
                PRINT_ERROR << "invalid sprite direction, could not find proper standing animation to draw" << std::endl;
            }
        }
    }

    else { // then (moved_position)
        // Determine the correct animation to display
        if(_direction & FACING_NORTH) {
            _current_anim_direction = ANIM_NORTH;
        } else if(_direction & FACING_SOUTH) {
            _current_anim_direction = ANIM_SOUTH;
        } else if(_direction & FACING_WEST) {
            _current_anim_direction = ANIM_WEST;
        } else if(_direction & FACING_EAST) {
            _current_anim_direction = ANIM_EAST;
        } else {
            PRINT_ERROR << "invalid sprite direction, could not find proper standing animation to draw" << std::endl;
        }

        // Increasing the animation index by four from the walking _animations leads to the running _animations
        if(_is_running && _has_running_animations) {
            _animation = &_running_animations;
        } else {
            _animation = &_walking_animations;
        }
    }

    // If the direction of movement changed in mid-flight, update the animation timer on the
    // new animated image to reflect the old, so the walking _animations do not appear to
    // "start and stop" whenever the direction is changed.
    if(last_anim_direction != _current_anim_direction || last_animation != _animation) {
        _animation->at(_current_anim_direction).SetTimeProgress(last_animation->at(last_anim_direction).GetTimeProgress());
        last_animation->at(last_anim_direction).ResetAnimation();
    }

    // Take care of adapting the update time according to the sprite speed when walking or running
    uint32_t elapsed_time = 0;
    if(_animation == &_walking_animations || (_has_running_animations && _animation == &_running_animations)) {
        elapsed_time = (uint32_t)(((float)vt_system::SystemManager->GetUpdateTime()) * NORMAL_SPEED / _movement_speed);
    }

    _animation->at(_current_anim_direction).Update(elapsed_time);
}

void MapSprite::_DrawDebugInfo()
{
    // Draw collision rectangle if the debug view is on.
    Position2D pos = vt_video::VideoManager->GetDrawPosition();
    Rectangle2D rect = GetScreenCollisionRectangle(pos.x, pos.y);
    vt_video::VideoManager->DrawRectangle(rect.right - rect.left,
                                          rect.bottom - rect.top,
                                          vt_video::Color(0.0f, 0.0f, 1.0f, 0.6f));

    // Show a potential active path
    if(_control_event && _control_event->GetEventType() == PATH_MOVE_SPRITE_EVENT) {
        PathMoveSpriteEvent *path_event = (PathMoveSpriteEvent *)_control_event;
        if(path_event) {
            Path path = path_event->GetPath();
            MapMode *map_mode = MapMode::CurrentInstance();
            for(uint32_t i = 0; i < path.size(); ++i) {
                float x_pos = map_mode->GetScreenXCoordinate(path[i].x);
                float y_pos = map_mode->GetScreenYCoordinate(path[i].y);
                vt_video::VideoManager->Move(x_pos, y_pos);

                vt_video::VideoManager->DrawRectangle(GRID_LENGTH / 2,
                                                      GRID_LENGTH / 2,
                                                      vt_video::Color(0.0f, 1.0f, 1.0f, 0.6f));
            }
        }
    }
}

void MapSprite::Draw()
{
    if(!MapObject::ShouldDraw())
        return;

    if(_custom_animation_on && _current_custom_animation)
        _current_custom_animation->Draw();
    else
        _animation->at(_current_anim_direction).Draw();

    MapObject::_DrawEmote();

    if(vt_video::VideoManager->DebugInfoOn())
        _DrawDebugInfo();
}

void MapSprite::DrawDialogIcon()
{
    if(!MapObject::ShouldDraw())
        return;

    // Other map sprite logical conditions preventing the bubble from being displayed
    if (!_has_available_dialogue || !_has_unseen_dialogue || _dialogue_started)
        return;

    MapMode* map_mode = MapMode::CurrentInstance();
    vt_video::Color icon_color(1.0f, 1.0f, 1.0f, 0.0f);
    float icon_alpha = 1.0f - (fabs(GetXPosition() - map_mode->GetCamera()->GetXPosition())
                            + fabs(GetYPosition() - map_mode->GetCamera()->GetYPosition())) / INTERACTION_ICON_VISIBLE_RANGE;
    if (icon_alpha < 0.0f)
        icon_alpha = 0.0f;
    icon_color.SetAlpha(icon_alpha);

    vt_video::VideoManager->MoveRelative(0, -GetImgScreenHeight());
    map_mode->GetDialogueIcon().Draw(icon_color);
}

void MapSprite::AddDialogueReference(SpriteDialogue* dialogue)
{
    _dialogue_references.push_back(dialogue->GetDialogueID());
    UpdateDialogueStatus();
}

void MapSprite::ClearDialogueReferences()
{
    _dialogue_references.clear();
    UpdateDialogueStatus();
}

void MapSprite::RemoveDialogueReference(SpriteDialogue* dialogue)
{
    std::string dialogue_id = dialogue->GetDialogueID();
    // Remove all dialogues with the given reference (for the case, the same dialogue was add several times)
    for(uint32_t i = 0; i < _dialogue_references.size(); i++) {
        if(_dialogue_references[i] == dialogue_id)
            _dialogue_references.erase(_dialogue_references.begin() + i);
    }
    UpdateDialogueStatus();
}

void MapSprite::InitiateDialogue()
{
    if(_dialogue_references.empty()) {
        IF_PRINT_WARNING(MAP_DEBUG) << "sprite: " << _object_id << " has no dialogue referenced" << std::endl;
        return;
    }

    SaveState();
    _moving = false;
    _dialogue_started = true;
    SetDirection(GetOppositeDirection(MapMode::CurrentInstance()->GetCamera()->GetDirection()));
    MapMode::CurrentInstance()->GetDialogueSupervisor()->StartDialogue(_dialogue_references[_next_dialogue]);
    IncrementNextDialogue();
}

void MapSprite::UpdateDialogueStatus()
{
    _has_available_dialogue = false;
    _has_unseen_dialogue = false;

    for(uint32_t i = 0; i < _dialogue_references.size(); i++) {
        SpriteDialogue* dialogue = MapMode::CurrentInstance()->GetDialogueSupervisor()->GetDialogue(_dialogue_references[i]);
        if(!dialogue) {
            PRINT_WARNING << "sprite: " << _object_id << " is referencing unknown dialogue: "
                          << _dialogue_references[i] << std::endl;
            continue;
        }

        // try and not take already seen dialogues.
        // So we take only the last dialogue reference even if already seen.
        if (dialogue->HasAlreadySeen() && i < _dialogue_references.size() - 1)
            continue;

        _has_available_dialogue = true;
        if(_next_dialogue < 0)
            _next_dialogue = i;

        _has_unseen_dialogue = !dialogue->HasAlreadySeen();
    }
}

void MapSprite::IncrementNextDialogue()
{
    // Handle the case where no dialogue is referenced by the sprite
    if(_next_dialogue < 0) {
        IF_PRINT_WARNING(MAP_DEBUG) << "function invoked when no dialogues were referenced by the sprite" << std::endl;
        return;
    }

    int16_t last_dialogue = _next_dialogue;

    while(true) {
        ++_next_dialogue;
        if(static_cast<uint16_t>(_next_dialogue) >= _dialogue_references.size()) {
            --_next_dialogue;
            return;
        }

        SpriteDialogue *dialogue = MapMode::CurrentInstance()->GetDialogueSupervisor()->GetDialogue(_dialogue_references[_next_dialogue]);
        if(dialogue)
            return;

        // If this case occurs, all dialogues are now unavailable
        else if(_next_dialogue == last_dialogue) {
            _has_unseen_dialogue = false;
            _dialogue_started = false;
            return;
        }
    }
}

void MapSprite::SetNextDialogue(uint16_t next)
{
    // If a negative value is passed in, this means the user wants to disable
    if(next >= _dialogue_references.size()) {
        IF_PRINT_WARNING(MAP_DEBUG) << "tried to set _next_dialogue to an value that was invalid (exceeds maximum bounds): " << next << std::endl;
    } else {
        _next_dialogue = static_cast<int16_t>(next);
    }
}

void MapSprite::SaveState()
{
    VirtualSprite::SaveState();

    _saved_current_anim_direction = _current_anim_direction;
}

void MapSprite::RestoreState()
{
    VirtualSprite::RestoreState();

    _current_anim_direction = _saved_current_anim_direction;
}

const std::string& MapSprite::GetNextDialogueID() const
{
    if (_next_dialogue >= 0 && _next_dialogue < static_cast<int16_t>(_dialogue_references.size()))
        return _dialogue_references[_next_dialogue];
    else
        return vt_utils::_empty_string;
}

} // namespace private_map

} // namespace vt_map
