///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_SOUND_HEADER__
#define __MAP_SOUND_HEADER__

#include "modes/map/map_objects/map_object.h"

#include "engine/audio/audio_descriptor.h"

namespace vt_map
{

namespace private_map
{

/** ****************************************************************************
*** \brief Represents a sound source object on the map
*** ***************************************************************************/
class SoundObject : public MapObject
{
public:
    /** \brief An environmental sound object which sound is played looped and with a volume
    *** computed against the distance of the object with the camera.
    *** \param sound_filename The sound filename to play.
    *** \param x, y The sound map location
    *** \param strength The "strength" of the sound, the maximal distance
    in map tiles the sound can be heard within.
    *** The sound volume will be compute according that distance.
    **/
    SoundObject(const std::string& sound_filename, float x, float y, float strength);

    virtual ~SoundObject() override
    {
    }

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static SoundObject* Create(const std::string& sound_filename,
                               float x, float y, float strength);

    //! \brief Updates the object's currently desired volume.
    void UpdateVolume();

    //! \brief Applies the object's currently desired volume.
    void ApplyVolume();

    //! \brief Does nothing
    void Draw()
    {}

    //! \brief Stop the ambient sound
    void Stop();

    //! \brief Start the ambient sound
    void Start();

    //! \brief Tells whether the ambient sound is active
    bool IsActive() const {
        return _activated;
    }

    //! \brief Sets the max sound volume of the ambient sound.
    //! From  0.0f to 1.0f
    void SetMaxVolume(float max_volume);

    //! \brief Gets the sound descriptor of the object.
    //! Used to apply changes directly to the sound object.
    vt_audio::SoundDescriptor* GetSoundDescriptor() const {
        return _sound;
    }

    //! \brief Gets the current desired sound volume.
    //! Used by the object manager to determine the best volume to play the sound object at.
    float GetSoundVolume() const {
        return (_activated && _playing) ? _sound_volume : 0.0f;
    }

private:
    //! \brief The sound object reference. Don't delete it.
    vt_audio::SoundDescriptor* _sound;

    //! \brief The maximal distance in map tiles the sound can be heard within.
    float _strength;

    //! \brief The Volume the sound should currently be played.
    float _sound_volume;

    //! \brief The maximal strength of the sound object. (0.0f - 1.0f)
    float _max_sound_volume;

    //! \brief The time remaining before next update
    int32_t _time_remaining;

    //! \brief Tells whether the sound is activated.
    bool _activated;

    //! \brief Tells whether the sound is currently playing or not
    //! This boolean is here to avoid calling fadeIn()/FadeOut()
    //! repeatedly on sounds.
    bool _playing;
}; // class SoundObject : public MapObject

} // namespace private_map

} // namespace vt_map

#endif // __MAP_SOUND_HEADER__
