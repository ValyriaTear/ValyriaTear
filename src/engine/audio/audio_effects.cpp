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

#include "engine/system.h"

namespace hoa_audio {

namespace private_audio {

////////////////////////////////////////////////////////////////////////////////
// FadeInEffect class methods
////////////////////////////////////////////////////////////////////////////////

FadeInEffect::FadeInEffect(AudioDescriptor& audio, float time) :
	AudioEffect(),
	_effect_time(time),
	_audio(audio)
{
	_audio.SetVolume(0.0f);
	// If the sound is not playing, then start it.
	// Note: Only audio descriptors being played are updated.
	if (_audio.GetState() != AUDIO_STATE_PLAYING)
		_audio.Play();
}



void FadeInEffect::Update() {
	if (!active)
		return;

	// Stop right away when the effect is less than a usual cpu cycle
	if (_effect_time <= 10.0f) {
		_audio.SetVolume(1.0f);
		active = false;
		return;
	}

	float time_elapsed = (float)hoa_system::SystemManager->GetUpdateTime();
	float new_volume = _audio.GetVolume() + (time_elapsed / _effect_time);


	// If the volume has reached the maximum, mark the effect as over
	if (new_volume >= 1.0f) {
		_audio.SetVolume(1.0f);
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
	if (!active)
		return;

	// If the sound is not playing, there's nothing to be done
	if (_audio.GetState() != AUDIO_STATE_PLAYING) {
		// Disable the effect in that case.
		_audio.SetVolume(0.0f);
		active = false;
		return;
	}

	// Stop right away when the effect is less than a usual cpu cycle
	if (_effect_time <= 10.0f) {
		_audio.Stop();
		_audio.SetVolume(0.0f);
		active = false;
		return;
	}

	float time_elapsed = (float)hoa_system::SystemManager->GetUpdateTime();
	float new_volume = _audio.GetVolume() - (_original_volume - (_original_volume - (time_elapsed / _effect_time)));

	// Stop the audio, and terminate the effect if the volume drops to 0.0f or below
	if (new_volume <= 0.0f) {
		_audio.Stop();
		_audio.SetVolume(0.0f);
		active = false;
	}
	// Otherwise, update the volume for the audio
	else {
		_audio.SetVolume(new_volume);
	}
}

} // namespace private_audio

} // namespace hoa_audio
