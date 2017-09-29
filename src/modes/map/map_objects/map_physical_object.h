///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __PHYSICAL_OBJECT_HEADER__
#define __PHYSICAL_OBJECT_HEADER__

#include "modes/map/map_objects/map_object.h"

namespace vt_map
{

namespace private_map
{

/** ****************************************************************************
*** \brief Represents visible objects on the map that have no motion.
***
*** This class represents both still image and animated objects. These objects
*** are usually fixed in place and do not change their position.
***
*** \note If the object does not have any animated images, set the 'updatable'
*** member of the base class to false. Forgetting to do this will do no harm, but
*** it will make it heavier.
*** ***************************************************************************/
class PhysicalObject : public MapObject
{
public:
    explicit PhysicalObject(MapObjectDrawLayer layer);
    virtual ~PhysicalObject() override;

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static PhysicalObject* Create(MapObjectDrawLayer layer);

    //! \brief Updates the object's current animation.
    virtual void Update();

    //! \brief Draws the object to the screen, if it is visible.
    virtual void Draw();

    /** \brief Sets a new animation using the animation filename provided
    *** \param animation_filename The name of the animation file to use for the animation
    *** \return The animation id that can later be used with SetCurrentAnimation() or -1 if invalid
    **/
    int32_t AddAnimation(const std::string& animation_filename);

    /** \brief Sets a new still animation using the image filename provided
    *** \param image_filename The name of the image file to use for the animation
    *** \return The animation id that can later be used with SetCurrentAnimation() or -1 if invalid
    **/
    int32_t AddStillFrame(const std::string& image_filename);

    void AddAnimation(const vt_video::AnimatedImage& new_img) {
        _animations.push_back(new_img);
    }

    void SetCurrentAnimation(uint32_t animation_id);

    void SetAnimationProgress(uint32_t progress) {
        _animations[_current_animation_id].SetTimeProgress(progress);
    }

    uint32_t GetCurrentAnimationId() const {
        return _current_animation_id;
    }

    void RandomizeCurrentAnimationFrame() {
        _animations[_current_animation_id].RandomizeAnimationFrame();
    }

    /** \brief Adds an event triggered when talking to a physical object
    *** \param event_id The event string id
    **/
    void SetEventWhenTalking(const std::string& event_id) {
        _event_when_talking = event_id;
    }

    /** \brief Removes the event linked to a physical object
    **/
    void ClearEventWhenTalking() {
        _event_when_talking.clear();
    }

    //! \brief Returns the event id triggered when talking to the physical object.
    const std::string& GetEventIdWhenTalking() const {
        return _event_when_talking;
    }

    virtual void SetGrayscale(bool grayscale = true) {
        _grayscale = grayscale;
        for (vt_video::AnimatedImage& animation : _animations) {
            animation.SetGrayscale(grayscale);
        }
    }
    //@}

protected:
    /** \brief A vector containing all the object's animations.
    *** These need not be actual animations. If you just want a still image, add only a single
    *** frame to the animation. Usually only need a single still image or animation will be
    *** needed, but a vector is used here in case others are needed.
    **/
    std::vector<vt_video::AnimatedImage> _animations;

private:
    /** \brief The index to the animations vector that contains the current image to display
    *** When modifying this member, take care not to exceed the bounds of the animations vector
    **/
    uint32_t _current_animation_id;

    //! \brief The event id triggered when talking to the sprite.
    std::string _event_when_talking;
};

} // namespace private_map

} // namespace vt_map

#endif // __PHYSICAL_OBJECT_HEADER__
