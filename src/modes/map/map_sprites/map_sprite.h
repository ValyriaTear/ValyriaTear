///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_SPRITE_HEADER__
#define __MAP_SPRITE_HEADER__

#include "modes/map/map_sprites/map_virtual_sprite.h"

#include "utils/ustring.h"

namespace vt_map
{

namespace private_map
{

class SpriteDialogue;

/** ****************************************************************************
*** \brief A mobile map object with which the player can interact with.
***
*** Map sprites are animate, mobile, living map objects. Although there is
*** but this single class to represent all the map sprites in the game, they can
*** divided into types such as NPCs, friendly creatures, and enemies. The fact
*** that there is only one class for representing several sprite types is the
*** reason why many of the class members are pointers. For example, we don't
*** need dialogue for a dog sprite.
*** ***************************************************************************/
class MapSprite : public VirtualSprite
{
public:
    explicit MapSprite(MapObjectDrawLayer layer);
    virtual ~MapSprite() override;

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static MapSprite* Create(MapObjectDrawLayer layer);

    // ---------- Public methods

    /** \brief Loads the standing animations of the sprite for the four directions.
    *** \param filename The name of the script animation file holding the standing animations
    *** \return False if there was a problem loading the sprites.
    **/
    bool LoadStandingAnimations(const std::string &filename);

    /** \brief Loads the walking animations of the sprite for the four directions.
    *** \param filename The name of the script animation file holding the walking animations
    *** \return False if there was a problem loading the sprites.
    **/
    bool LoadWalkingAnimations(const std::string &filename);

    /** \brief Loads the running animations of the sprite for the four directions.
    *** \param filename The name of the image file holding the walking animation
    *** \return False if there was a problem loading the sprites.
    **/
    bool LoadRunningAnimations(const std::string &filename);

    /** \brief Loads the script containing the one-sided custom animation of the sprite.
    *** \param animation_name The animation name of the custom animation.
    *** \param filename The name of the image file holding the given custom animation (one direction only)
    *** \return False if there was a problem loading the animation.
    **/
    bool LoadCustomAnimation(const std::string &animation_name, const std::string& filename);

    //! \brief Clear out all the sprite animation. Useful in case of reloading.
    void ClearAnimations();

    void LoadFacePortrait(const std::string& filename);

    //! \brief Updates the sprite's position and state.
    virtual void Update();

    //! \brief Draws the sprite frame in the appropriate position on the screen, if it is visible.
    virtual void Draw();

    //! \brief Draws the dialogue icon at the top of the sprite
    virtual void DrawDialogIcon();

    /** \brief Adds a new reference to a dialogue that the sprite uses
    *** \param dialogue The SpriteDialogue used.
    *** \note It is valid for a dialogue to be referenced more than once by a sprite
    **/
    void AddDialogueReference(SpriteDialogue* dialogue);

    /** \brief Removes all dialogue references from a sprite
    **/
    void ClearDialogueReferences();

    /** \brief Removes a specific dialogue reference from a sprite
    *** \param dialogue The SpriteDialogue used.
    **/
    void RemoveDialogueReference(SpriteDialogue* dialogue);

    /** \brief Begins a new dialogue with this sprite using its next referenced dialogue
    ***
    *** If the sprite has no dialogues referenced or has dialogues that are referenced but are unavailable,
    *** a warning will be printed and no dialogue will take place. It is the caller's responsibility to first
    *** check that the sprite has dialogue available.
    **/
    void InitiateDialogue();

    //! \brief Updates all dialogue status members based on the status of all referenced dialogues
    void UpdateDialogueStatus();

    /** \brief Increments the next_dialogue member to index the proceeding dialogue
    *** \note Change from Allacrost: Stay at the last dialogue when at this point.
    *** (Don't go back to the first one as done previously).
    **/
    void IncrementNextDialogue();

    /** \brief Sets the next dialogue member for the sprite
    *** \param next The index value of the dialogue_references vector to set the next_dialogue member to
    *** \note You can not set the next_dialogue member to a negative number. This could cause run-time errors if it was supported here.
    **/
    void SetNextDialogue(uint16_t next);

    /** \brief This method will save the state of a sprite.
    *** Attributes saved: direction, speed, moving state, name
    *** current animation.
    **/
    virtual void SaveState();

    /** \brief This method will load the saved state of a sprite.
    *** Attributes loaded: direction, speed, moving state, name
    *** current animation.
    *** \return false if there was no saved state, true otherwise.
    **/
    virtual void RestoreState();

    /** \name Lua Access Functions
    *** These functions are specifically written to enable Lua to access the members of this class.
    **/
    //@{
    void SetName(const vt_utils::ustring &name) {
        _name = name;
    }

    void SetName(const std::string &name) {
        _name = vt_utils::MakeUnicodeString(name);
    }

    void SetCurrentAnimationDirection(uint8_t anim_direction) {
        _current_anim_direction = anim_direction;
    }

    uint8_t GetCurrentAnimationDirection() const {
        return _current_anim_direction;
    }

    bool HasAvailableDialogue() const {
        return _has_available_dialogue;
    }

    bool HasUnseenDialogue() const {
        return _has_unseen_dialogue;
    }

    vt_utils::ustring &GetName() {
        return _name;
    }

    vt_video::StillImage *GetFacePortrait() const {
        return _face_portrait;
    }

    //! \brief Returns the next dialogue to reference (negative value returned if no dialogues are referenced)
    int16_t GetNextDialogue() const {
        return _next_dialogue;
    }

    //! \brief Gets the ID value of the dialogue that will be the next to be referenced by the sprite
    const std::string& GetNextDialogueID() const;

    //! \brief Returns the number of dialogues referenced by the sprite (including duplicates)
    uint16_t GetNumberDialogueReferences() const {
        return _dialogue_references.size();
    }

    /** \brief Tells the sprite to use a custom animation
    *** \param The animation name used as a key to find the custom animation declared in map_sprites.lua
    *** You can set the animation key to empty to disable the custom animation.
    *** \param The time to display the given animation, -1 for the default time and 0 for an infinite amount of time.
    **/
    void SetCustomAnimation(const std::string &animaton_name, int32_t time);

    bool IsAnimationCustom() const {
        return _custom_animation_on;
    }

    /** \brief Disable a posible running custom animation.
    *** Useful after setting an inifinite running animation, for instance.
    **/
    void DisableCustomAnimation() {
        _custom_animation_on = false;
        _infinite_custom_animation = false;
    }

    void SetSpriteName(const std::string &map_sprite_name) {
        _sprite_name = map_sprite_name;
    }

    const std::string &GetSpriteName() const {
        return _sprite_name;
    }

    /** \brief Used to reload (or change) the graphic animations
    *** of an existing sprite.
    *** \param sprite_name The sprite name entry found in the map_sprites.lua
    *** sprite table.
    **/
    void ReloadSprite(const std::string& sprite_name);

    //! Will change the sprite type to SCENERY,
    //! making other sprites unable to collide with it. It is usually used to setup
    //! harmless and little animals seen on maps.
    void SetSpriteAsScenery(bool is_scenery) {
        MapObject::_object_type = is_scenery ? SCENERY_TYPE : SPRITE_TYPE;
    }

    //! \brief Sets/unsets the sprite animations as grayscale.
    virtual void SetGrayscale(bool grayscale = true);
    //@}

protected:
    //! \brief The name of the sprite, as seen by the player in the game.
    vt_utils::ustring _name;

    /** \brief A pointer to the face portrait of the sprite, as seen in dialogues and menus.
    *** \note Not all sprites have portraits, in which case this member will be nullptr
    **/
    vt_video::StillImage *_face_portrait;

    /** Keeps the map sprite reference name permitting, used to know whether a map sprite needs reloading
    *** when the map sprite name has actually changed.
    **/
    std::string _sprite_name;

    //! \brief Set to true if the sprite has running animations loaded
    bool _has_running_animations;

    //! \brief The current sprite direction. (for animation)
    uint8_t _current_anim_direction;

    //! \brief A map containing all four directions of the sprite's various animations.
    std::vector<vt_video::AnimatedImage> _standing_animations;
    std::vector<vt_video::AnimatedImage> _walking_animations;
    std::vector<vt_video::AnimatedImage> _running_animations;

    //! \brief A pointer to the current standard animation vector
    std::vector<vt_video::AnimatedImage>* _animation;

    //! \brief A map containing all the custom animations, indexed by their name.
    std::map<std::string, vt_video::AnimatedImage> _custom_animations;

    //! \brief The currently used custom animation.
    vt_video::AnimatedImage *_current_custom_animation;

    //! \brief Contains the id values of all dialogues referenced by the sprite
    std::vector<std::string> _dialogue_references;

    /** \brief An index to the dialogue_references vector, representing the next dialogue the sprite should reference
    *** A negative value indicates that the sprite has no dialogue.
    **/
    int16_t _next_dialogue;

    //! \brief True if the sprite references at least one dialogue
    bool _has_available_dialogue;

    //! \brief True if at least one dialogue referenced by this sprite has not yet been viewed -and- is available to be viewed
    bool _has_unseen_dialogue;

    //! \brief true if the sprite is talking with the camera.
    bool _dialogue_started;

    //! \brief True if a custom animation is in use
    bool _custom_animation_on;

    //! \brief Tells how much time left the custom animation will have to be drawn
    int32_t _custom_animation_time;

    //! Tells whether the animation has got an infinite duration
    bool _infinite_custom_animation;

    /** \name Saved state attributes
    *** These attributes are used to save and load the state of a VirtualSprite
    **/
    //@{
    uint8_t _saved_current_anim_direction;
    //@}

    //! \brief Draws debug information, used for pathfinding mostly.
    void _DrawDebugInfo();
};

} // namespace private_map

} // namespace vt_map

#endif // __MAP_SPRITE_HEADER__
