////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software 
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file   audio_effects.cpp
*** \author Moisés Ferrer Serra, byaku@allacrost.org
*** \brief  Source file for audio effects
*** ***************************************************************************/

#include "audio_effects.h"
#include "audio_descriptor.h"

using namespace std;

namespace hoa_audio {

namespace private_audio {

////////////////////////////////////////////////////////////////////////////////
// FadeInEffect class methods
////////////////////////////////////////////////////////////////////////////////

FadeInEffect::FadeInEffect(AudioDescriptor& audio, float time) :
	AudioEffect(),
	_original_volume(audio.GetVolume()),
	_effect_time(time),
	_audio(audio)
{
	_audio.SetVolume(0.0f);
}



void FadeInEffect::Update() {
	// If the sound is not playing, there's nothing to be done
	if (_audio.GetState() != AUDIO_STATE_PLAYING) {
		return;
	}

	float new_volume = _audio.GetVolume() + (1.0f / _effect_time) * 0.00025f;

	// If the volume is over the original audio volume, mark the effect as over
	if (new_volume >= _original_volume) {
		_audio.SetVolume(_original_volume);
		active = false;
	}
	// Otherwise, update the volume for the audio
	else {
		_audio.SetVolume(new_volume);
	}
}

////////////////////////////////////////////////////////////////////////////////
// FadeOutEffect class methods
////////////////////////////////////////////////////////////////////////////////

FadeOutEffect::FadeOutEffect(AudioDescriptor& audio, float time) :
	AudioEffect(),
	_original_volume(audio.GetVolume()),
	_effect_time(time),
	_audio(audio)
{}



void FadeOutEffect::Update () {
	// If the sound is not playing, there's nothing to be done
	if (_audio.GetState() != AUDIO_STATE_PLAYING) {
		return;
	}

	float new_volume = _audio.GetVolume() - (1.0f / _effect_time) * 0.00025f;

	// Stop the audio, reset the original volume, and terminate the effect if the volume drops to 0.0f or below
	if (new_volume <= 0.0f) {
		_audio.Stop();
		_audio.SetVolume(_original_volume);
		active = false;
	}
	// Otherwise, update the volume for the audio
	else {
		_audio.SetVolume(new_volume);
	}
}

} // namespace private_audio

} // namespace hoa_audio
