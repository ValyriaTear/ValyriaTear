///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    audio.cpp
*** \author  Tyler Olsen - roots@allacrost.org
*** \author  Mois�s Ferrer Serra - byaku@allacrost.org
*** \author  Aaron Smith - etherstar@allacrost.org
*** \brief   Implementation of the audio engine singleton.
***
*** The code included here implements the interface of the audio singleton.
***
*** \note This code uses the OpenAL audio library. See http://www.openal.com/
*** ***************************************************************************/
#include <iostream>

#include "audio.h"
#include "system.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_system;
using namespace hoa_audio::private_audio;


template<> hoa_audio::AudioEngine* Singleton<hoa_audio::AudioEngine>::_singleton_reference = 0;


namespace hoa_audio {

AudioEngine* AudioManager = NULL;
bool AUDIO_DEBUG = false;
bool AUDIO_ENABLE = true;



AudioEngine::AudioEngine () :
	_sound_volume(1.0f),
	_music_volume(1.0f),
	_device(0),
	_context(0),
	_max_sources(MAX_DEFAULT_AUDIO_SOURCES),
	_active_music(NULL),
	_max_cache_size(MAX_DEFAULT_AUDIO_SOURCES / 4)
{}




bool AudioEngine::SingletonInitialize() {
	if (!AUDIO_ENABLE)
		return true;

	const ALCchar* best_device = 0; // Will store the name of the 'best' device for audio playback
	ALCint highest_version = 0; // The highest version number found
	CheckALError(); // Clears errors
	CheckALCError(); // Clears errors

	// Find the highest-version device available, if the extension for device enumeration is present
	if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE) {
		const ALCchar* device_list = 0;
		device_list = alcGetString(0, ALC_DEVICE_SPECIFIER); // Get list of all devices (terminated with two '0')
		if (CheckALCError() == true) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "failed to retrieve the list of available audio devices: " << CreateALCErrorString() << endl;
		}


		while (*device_list != 0) { // Check all the detected devices
			ALCint major_v = 0, minor_v = 0;

			// Open a temporary device for reading in its version number
			ALCdevice* temp_device = alcOpenDevice(device_list);
			if (CheckALCError() || temp_device == NULL) { // If we couldn't open the device, just move on to the next
				IF_PRINT_WARNING(AUDIO_DEBUG) << "couldn't open device for version checking: " << device_list << endl;
				device_list += strlen(device_list) + 1;
				continue;
			}

			// Create a temporary context for the device
			ALCcontext *temp_context = alcCreateContext(temp_device, 0);
			if (CheckALCError() || temp_context == NULL) { // If we couldn't create the context, move on to the next device
				IF_PRINT_WARNING(AUDIO_DEBUG) << "couldn't create a temporary context for device: " << device_list << endl;
				alcCloseDevice(temp_device);
				device_list += strlen(device_list) + 1;
				continue;
			}

			// Retrieve the version number for the device
			alcMakeContextCurrent(temp_context);

			alcGetIntegerv(temp_device, ALC_MAJOR_VERSION, sizeof(ALCint), &major_v);
			alcGetIntegerv(temp_device, ALC_MINOR_VERSION, sizeof(ALCint), &minor_v);
			alcMakeContextCurrent(0); // Disable the temporary context
			alcDestroyContext(temp_context); // Destroy the temporary context
			alcCloseDevice(temp_device); // Close the temporary device

			// Check if a higher version device was found
			if (highest_version < (major_v * 10 + minor_v)) {
				highest_version = (major_v * 10 + minor_v);
				best_device = device_list;
			}
			device_list += strlen(device_list) + 1; // Go to the next device name in the list
		} // while (*device_name != 0)
	} // if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE)

	// Open the 'best' device we found above. If no devices were previously found,
	// it will try opening the default device (= 0)
	_device = alcOpenDevice(best_device);
	if (CheckALCError() || _device == NULL) {
		PRINT_ERROR << "failed to open an OpenAL audio device: " << CreateALCErrorString() << endl;
		return false;
	}

	// Create an OpenAL context
	_context = alcCreateContext(_device, NULL);
	if (CheckALCError() || _context == NULL) {
		PRINT_ERROR << "failed to create an OpenAL context: " << CreateALCErrorString()<< endl;
		alcCloseDevice(_device);
		return false;
	}

	alcMakeContextCurrent(_context);
	CheckALError(); // Clear errors
	CheckALCError(); // Clear errors

	// Create as many sources as possible (we fix an upper bound of MAX_DEFAULT_AUDIO_SOURCES)
	ALuint source;
	for (uint16 i = 0; i < _max_sources; i++) {
		alGenSources(1, &source);
		if (CheckALError() == true) {
			_max_sources = i;
			_max_cache_size = i / 4;
			break;
		}
		_audio_sources.push_back(new private_audio::AudioSource(source));
	}

	if (_max_sources == 0) {
		PRINT_ERROR << "failed to create at least one OpenAL audio source" << endl;
		return false;
	}

	return true;
} // bool AudioEngine::SingletonInitialize()



AudioEngine::~AudioEngine() {
	if (!AUDIO_ENABLE)
		return;

	// Delete any active audio effects
	for (list<AudioEffect*>::iterator i = _audio_effects.begin(); i != _audio_effects.end(); i++) {
		delete (*i);
	}
	_audio_effects.clear();

	// Delete all entries in the sound cache
	for (map<std::string, private_audio::AudioCacheElement>::iterator i = _audio_cache.begin(); i != _audio_cache.end(); i++) {
		delete i->second.audio;
	}
	_audio_cache.clear();

	// Delete all audio sources
	for (vector<AudioSource*>::iterator i = _audio_sources.begin(); i != _audio_sources.end(); i++) {
		delete (*i);
	}
	_audio_sources.clear();

	// We shouldn't have any descriptors registered now -- check that this is true
	if (_registered_sounds.empty() == false) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << _registered_sounds.size() << " SoundDescriptor objects were still "
			"registered when destructor was invoked" << endl;
	}
	if (_registered_music.empty() == false) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << _registered_music.size() << " MusicDescriptor objects were still "
			"registered when destructor was invoked" << endl;
	}

	alcMakeContextCurrent(0);
	alcDestroyContext(_context);
	alcCloseDevice(_device);
}



void AudioEngine::Update() {
	if (!AUDIO_ENABLE)
		return;

	for (vector<AudioSource*>::iterator i = _audio_sources.begin(); i != _audio_sources.end(); i++) {
		if ((*i)->owner != NULL) {
			(*i)->owner->_Update();
		}
	}

	// Update all registered audio effects
	for (list<AudioEffect*>::iterator i = _audio_effects.begin(); i != _audio_effects.end();) {
		(*i)->Update();

		// If the effect is finished, delete it
		if ((*i)->active == false) {
			delete (*i);
			i = _audio_effects.erase(i);
		}
		else {
			i++;
		}
	}
}



void AudioEngine::SetSoundVolume(float volume) {
	if (volume < 0.0f) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to set sound volume less than 0.0f" << endl;
		_sound_volume = 0.0f;
	}
	else if (volume > 1.0f) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to set sound volume greater than 1.0f" << endl;
		_sound_volume = 1.0f;
	}
	else {
		_sound_volume = volume;
	}

	for (list<SoundDescriptor*>::iterator i = _registered_sounds.begin(); i != _registered_sounds.end(); i++) {
		if ((*i)->_source != NULL) {
			alSourcef((*i)->_source->source, AL_GAIN, _sound_volume * (*i)->GetVolume());
		}
	}
}



void AudioEngine::SetMusicVolume(float volume) {
	if (volume < 0.0f) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to set music volume less than 0.0f: " << volume << endl;
		_music_volume = 0.0f;
	}
	else if (volume > 1.0f) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to set music volume greater than 1.0f: " << volume << endl;
		_music_volume = 1.0f;
	}
	else {
		_music_volume = volume;
	}

	for (list<MusicDescriptor*>::iterator i = _registered_music.begin(); i != _registered_music.end(); i++) {
		if ((*i)->_source != NULL) {
			alSourcef((*i)->_source->source, AL_GAIN, _music_volume * (*i)->GetVolume());
		}
	}
}



void AudioEngine::PauseAllSounds() {
	for (list<SoundDescriptor*>::iterator i = _registered_sounds.begin();
			i != _registered_sounds.end(); i++) {
		(*i)->Pause();
	}
}



void AudioEngine::ResumeAllSounds() {
	for (list<SoundDescriptor*>::iterator i = _registered_sounds.begin();
			i != _registered_sounds.end(); i++) {
		(*i)->Resume();
	}
}



void AudioEngine::StopAllSounds() {
	for (list<SoundDescriptor*>::iterator i =_registered_sounds.begin();
			i != _registered_sounds.end(); i++) {
		(*i)->Stop();
	}
}



void AudioEngine::RewindAllSounds() {
	for (list<SoundDescriptor*>::iterator i = _registered_sounds.begin();
			i != _registered_sounds.end(); i++) {
		(*i)->Rewind();
	}
}



void AudioEngine::PauseAllMusic() {
	for (list<MusicDescriptor*>::iterator i = _registered_music.begin();
			i != _registered_music.end(); i++) {
		(*i)->Pause();
	}
}



void AudioEngine::ResumeAllMusic() {
	for (list<MusicDescriptor*>::iterator i = _registered_music.begin();
			i != _registered_music.end(); i++) {
		(*i)->Resume();
	}
}



void AudioEngine::StopAllMusic() {
	for (list<MusicDescriptor*>::iterator i = _registered_music.begin();
			i != _registered_music.end(); i++) {
		(*i)->Stop();
	}
}



void AudioEngine::RewindAllMusic() {
	for (list<MusicDescriptor*>::iterator i = _registered_music.begin();
			i != _registered_music.end(); i++) {
		(*i)->Rewind();
	}
}



void AudioEngine::SetListenerPosition(const float position[3]) {
	alListenerfv(AL_POSITION, position);
	memcpy(_listener_position, position, sizeof(float) * 3);
}



void AudioEngine::SetListenerVelocity(const float velocity[3]) {
	alListenerfv(AL_VELOCITY, velocity);
	memcpy(_listener_velocity, velocity, sizeof(float) * 3);
}



void AudioEngine::SetListenerOrientation(const float orientation[3]) {
	alListenerfv(AL_ORIENTATION, orientation);
	memcpy(_listener_orientation, orientation, sizeof(float) * 3);
}



bool AudioEngine::LoadSound(const std::string& filename) {
	SoundDescriptor* new_sound = new SoundDescriptor();

	if (_LoadAudio(new_sound, filename) == false) {
		delete new_sound;
		return false;
	}

	return true;
}



bool AudioEngine::LoadMusic(const std::string& filename) {
	MusicDescriptor* new_music = new MusicDescriptor();

	if (_LoadAudio(new_music, filename) == false) {
		delete new_music;
		return false;
	}

	return true;
}



void AudioEngine::PlaySound(const std::string& filename) {
	map<std::string, AudioCacheElement>::iterator element = _audio_cache.find(filename);

	if (element == _audio_cache.end()) {
		if (LoadSound(filename) == false) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "could not play sound from cache because the sound could not be loaded" << endl;
			return;
		}
		else {
			element = _audio_cache.find(filename);
		}
	}

	element->second.audio->Play();
	element->second.last_update_time = SDL_GetTicks();
}



void AudioEngine::PlayMusic(const std::string& filename) {
	map<std::string, AudioCacheElement>::iterator element = _audio_cache.find(filename);

	if (element == _audio_cache.end()) {
		if (LoadMusic(filename) == false) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "could not play music from cache because the music could not be loaded" << endl;
			return;
		}
		else {
			element = _audio_cache.find(filename);
		}
	}

	element->second.audio->Play();
	element->second.last_update_time = SDL_GetTicks();
}



void AudioEngine::StopSound(const std::string& filename) {
	map<std::string, AudioCacheElement>::iterator element = _audio_cache.find(filename);

	if (element == _audio_cache.end()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "could not stop audio because it was not contained in the cache: " << filename << endl;
		return;
	}

	element->second.audio->Stop();
	element->second.last_update_time = SDL_GetTicks();
}



void AudioEngine::PauseSound(const std::string& filename) {
	map<std::string, AudioCacheElement>::iterator element = _audio_cache.find(filename);

	if (element == _audio_cache.end()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "could not pause audio because it was not contained in the cache: " << filename << endl;
		return;
	}

	element->second.audio->Pause();
	element->second.last_update_time = SDL_GetTicks();
}



void AudioEngine::ResumeSound(const std::string& filename) {
	map<std::string, AudioCacheElement>::iterator element = _audio_cache.find(filename);

	if (element == _audio_cache.end()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "could not resume audio because it was not contained in the cache: " << filename << endl;
		return;
	}

	element->second.audio->Resume();
	element->second.last_update_time = SDL_GetTicks();
}



SoundDescriptor* AudioEngine::RetrieveSound(const std::string& filename) {
	map<std::string, AudioCacheElement>::iterator element = _audio_cache.find(filename);

	if (element == _audio_cache.end()) {
		return NULL;
	}
	else if (element->second.audio->IsSound() == false) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "incorrectly requested to retrieve a sound for a music filename: " << filename << endl;
		return NULL;
	}
	else {
		return dynamic_cast<SoundDescriptor*>(element->second.audio);
	}
}



MusicDescriptor* AudioEngine::RetrieveMusic(const std::string& filename) {
	map<std::string, AudioCacheElement>::iterator element = _audio_cache.find(filename);

	if (element == _audio_cache.end()) {
		return NULL;
	}
	else if (element->second.audio->IsSound() == true) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "incorrectly requested to retrieve music for a sound filename: " << filename << endl;
		return NULL;
	}
	else {
		return dynamic_cast<MusicDescriptor*>(element->second.audio);
	}
}



const std::string AudioEngine::CreateALErrorString() {
	switch (_al_error_code) {
		case AL_NO_ERROR:
			return "AL_NO_ERROR";
		case AL_INVALID_NAME:
			return "AL_INVALID_NAME";
		case AL_INVALID_ENUM:
			return "AL_INVALID_ENUM";
		case AL_INVALID_VALUE:
			return "AL_INVALID_VALUE";
		case AL_INVALID_OPERATION:
			return "AL_INVALID_OPERATION";
		case AL_OUT_OF_MEMORY:
			return "AL_OUT_OF_MEMORY";
		default:
			return ("Unknown AL error code: " + NumberToString(_al_error_code));
	}
}



const std::string AudioEngine::CreateALCErrorString() {
	switch (_alc_error_code) {
		case ALC_NO_ERROR:
			return "ALC_NO_ERROR";
		case ALC_INVALID_DEVICE:
			return "ALC_INVALID_DEVICE";
		case ALC_INVALID_CONTEXT:
			return "ALC_INVALID_CONTEXT";
		case ALC_INVALID_ENUM:
			return "ALC_INVALID_ENUM";
		case ALC_INVALID_VALUE:
			return "ALC_INVALID_VALUE";
		case ALC_OUT_OF_MEMORY:
			return "ALC_OUT_OF_MEMORY";
		default:
			return ("Unknown ALC error code: " + NumberToString(_alc_error_code));
	}
}



void AudioEngine::DEBUG_PrintInfo() {
	const ALCchar* c;

	cout << "*** Audio Information ***" << endl;

	cout << "Maximum number of sources:   " << _max_sources << endl;
	cout << "Maximum audio cache size:    " << _max_cache_size << endl;
	cout << "Default audio device:        " << alcGetString(_device, ALC_DEFAULT_DEVICE_SPECIFIER) << endl;
	cout << "OpenAL Version:              " << alGetString(AL_VERSION) << endl;
	cout << "OpenAL Renderer:             " << alGetString(AL_RENDERER) << endl;
	cout << "OpenAL Vendor:               " << alGetString(AL_VENDOR) << endl;

	CheckALError();

	cout << "Available OpenAL Extensions:" << endl;
	c = alGetString(AL_EXTENSIONS);
	bool new_extension = true;
	while (c[0]) {
		if (new_extension) {
			cout << " - ";
			new_extension = false;
			continue;
		}
		else if (c[0] == ' ') {
			cout << endl;
			new_extension = true;
			c++;
			continue;
		}

		cout << c[0];
		c++;
	}
}



private_audio::AudioSource* AudioEngine::_AcquireAudioSource() {
	// (1) Find and return the first source that does not have an owner
	for (vector<AudioSource*>::iterator i = _audio_sources.begin(); i != _audio_sources.end(); i++) {
		if ((*i)->owner == NULL) {
			return *i;
		}
	}

	// (2) If all sources are owned, find one that is in the initial or stopped state and change its ownership
	for (vector<AudioSource*>::iterator i = _audio_sources.begin(); i != _audio_sources.end(); i++) {
		ALint state;
		alGetSourcei((*i)->source, AL_SOURCE_STATE, &state);
		if (state == AL_INITIAL || state == AL_STOPPED) {
			(*i)->owner->_source = NULL;
			(*i)->Reset(); // this call sets the source owner pointer to NULL
			return *i;
		}
	}

	// (3) Return NULL in the (extremely rare) case that all sources are owned and actively playing or paused
	return NULL;
}



bool AudioEngine::_LoadAudio(AudioDescriptor* audio, const std::string& filename) {
	if (_audio_cache.find(filename) != _audio_cache.end()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "audio was already contained within the cache: " << filename << endl;
		return false;
	}

	// (1) If the cache is not full, try loading the audio and adding it in
	if (_audio_cache.size() < _max_cache_size) {
		if (audio->LoadAudio(filename) == false) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "could not add new audio file into cache because load operation failed: " << filename << endl;
			return false;
		}

		_audio_cache.insert(make_pair(filename, AudioCacheElement(SDL_GetTicks(), audio)));
		return true;
	}

	// (2) The cache is full, so find an element to remove. First make sure that at least one piece of audio is stopped
	map<std::string, AudioCacheElement>::iterator lru_element = _audio_cache.end();
	for (map<std::string, AudioCacheElement>::iterator i = _audio_cache.begin(); i != _audio_cache.end(); i++) {
		if (i->second.audio->GetState() == AUDIO_STATE_STOPPED) {
			lru_element = i;
			break;
		}
	}

	if (lru_element == _audio_cache.end()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed to remove element from cache because no piece of audio was in the stopped state" << endl;
		return false;
	}

	for (map<std::string, AudioCacheElement>::iterator i = _audio_cache.begin(); i != _audio_cache.end(); i++) {
		if (i->second.audio->GetState() == AUDIO_STATE_STOPPED && i->second.last_update_time < lru_element->second.last_update_time) {
			lru_element = i;
		}
	}

	delete lru_element->second.audio;
	_audio_cache.erase(lru_element);

	if (audio->LoadAudio(filename) == false) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "could not add new audio file into cache because load operation failed: " << filename << endl;
		return false;
	}

	_audio_cache.insert(make_pair(filename, AudioCacheElement(SDL_GetTicks(), audio)));
	return true;
} // bool AudioEngine::_LoadAudio(AudioDescriptor* audio, const std::string& filename)

} // namespace hoa_audio
