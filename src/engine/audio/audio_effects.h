////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file   audio_effects.h
*** \author Mois�s Ferrer Serra, byaku@allacrost.org
*** \author Yohann Ferreira, yohann ferreira orange fr
*** \brief  Header file for audio effects
***
*** This code provides the interface for effects, as well as the private classes
*** used for build all the audio fx system
***
*** \todo Add an effect class that modifies an audio's position over time,
*** which will allow support for panning effects
*** ***************************************************************************/

#ifndef __AUDIO_EFFECTS_HEADER__
#define __AUDIO_EFFECTS_HEADER__

#include "audio_descriptor.h"

namespace vt_audio
{

class AudioDescriptor;

enum AUDIO_EFFECT {
    AUDIO_EFFECT_NONE = 0
};

namespace private_audio
{

/** ****************************************************************************
*** \brief An abstract base class for all audio effects to be derived from
***
*** The purpose of this class is nothing more than to provide a simple common
*** interface for all audio effects, primarily so that they may be stored in
*** the same container. Most effects will only operate upon a single piece of
*** audio, but some effects may operate across multiple audio descriptors.
*** ***************************************************************************/
class AudioEffect
{
public:
    AudioEffect() :
        active(true),
        effect_type(AUDIO_EFFECT_NONE)
        {}

    virtual ~AudioEffect()
    {}

    //! \brief Set to true while an effect is active, and set to false when the effect is finished
    bool active;

    //! \brief Updates the effect and sets the active member to false when the effect is finished
    virtual void Update() = 0;

    //! \brief Get the audio descriptor concerned by the effect.
    virtual AudioDescriptor &GetAudioDescriptor() const = 0;

    //! The audio effect type
    AUDIO_EFFECT effect_type;
}; // class AudioEffect

// NOTE: No actual effect is existing for now.

} // namespace private_audio

} // namespace vt_audio

#endif // __AUDIO_EFFECTS_HEADER__
