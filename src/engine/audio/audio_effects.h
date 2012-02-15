////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software 
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file   audio_effects.h
*** \author Moisés Ferrer Serra, byaku@allacrost.org
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

#ifdef __MACH__
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
#else
	#include "al.h"
	#include "alc.h"
#endif

#include "defs.h"
#include "utils.h"

#include "audio_descriptor.h"

namespace hoa_audio {

namespace private_audio {

/** ****************************************************************************
*** \brief An abstract base class for all audio effects to be derived from
***
*** The purpose of this class is nothing more than to provide a simple common
*** interface for all audio effects, primarily so that they may be stored in
*** the same container. Most effects will only operate upon a single piece of
*** audio, but some effects may operate across multiple audio descriptors.
*** ***************************************************************************/
class AudioEffect {
public:
	AudioEffect() :
		active(true) {}
	
	virtual ~AudioEffect()
		{}

	//! \brief Set to true while an effect is active, and set to false when the effect is finished
	bool active;

	//! \brief Updates the effect and sets the active member to false when the effect is finished
	virtual void Update() = 0;
}; // class AudioEffect


/** ****************************************************************************
*** \brief Gradually fades a playing audio source in from mute to current volume
***
*** This class will set the AudioDescriptor's volume level to 0.0f (mute) upon
*** being created, and will gradually restore the volume to its original level
*** over time.
*** ***************************************************************************/
class FadeInEffect : public AudioEffect {
public:
	/** \brief Constructor for the fade in effect.
	*** \param audio A reference to the AudioDescriptor of the audio to fade
	*** \param time The amount of time that the effect will take, in seconds
	**/
	FadeInEffect(AudioDescriptor& audio, float time);

	//! \brief Gradually increases the volume until the original volume level is restored
	void Update();

private:
	//! \brief The volume of the audio when the effect was registered
	float _original_volume;

	//! \brief The amount of time that the effect lasts for
	float _effect_time;

	//! \brief A reference to the audio to process the effect upon
	AudioDescriptor& _audio;
}; // class FadeInEffect : public AudioEffect {


/** ****************************************************************************
*** \brief Gradually fades a playing audio source from its current volume to silence
***
*** Once this class effectively mutes the audio by setting it to 0.0f, the audio
*** will automatically be set in the stop state and indicate that the effect
*** has finished. The original volume of the audio is restored after it has
*** stopped playing
*** ***************************************************************************/
class FadeOutEffect : public AudioEffect {
public:
	/** \brief Constructor for the fade out effect.
	*** \param audio A reference to the AudioDescriptor of the audio to fade
	*** \param time The amount of time that the effect will take, in seconds
	**/
	FadeOutEffect(AudioDescriptor& audio, float time);

	//! \brief Gradually decreases the volume until it reaches 0.0f
	void Update();

private:
	//! \brief The volume of the audio when the effect was registered
	float _original_volume;

	//! \brief The amount of time that the effect lasts for
	float _effect_time;

	//! \brief A reference to the audio to process the effect upon
	AudioDescriptor& _audio;
}; // class FadeOutEffect : public AudioEffect {

} // namespace private_audio

} // namespace hoa_audio

#endif // __AUDIO_EFFECTS_HEADER__
