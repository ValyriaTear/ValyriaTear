///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
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
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Implementation of the audio engine singleton.
***
*** The code included here implements the interface of the audio singleton.
***
*** \note This code uses the OpenAL audio library. See http://www.openal.com/
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "engine/audio/audio.h"

#include "engine/system.h"
#include "engine/mode_manager.h"

#include "utils/utils_strings.h"
#include "utils/utils_files.h"

using namespace vt_utils;
using namespace vt_system;
using namespace vt_audio::private_audio;

namespace vt_audio
{

AudioEngine *AudioManager = nullptr;
bool AUDIO_DEBUG = false;
bool AUDIO_ENABLE = true;

AudioEngine::AudioEngine() :
    _sound_volume(1.0f),
    _music_volume(1.0f),
    _device(0),
    _context(0),
    _max_sources(MAX_DEFAULT_AUDIO_SOURCES),
    _active_music(nullptr),
    _max_cache_size(MAX_DEFAULT_AUDIO_SOURCES / 4)
{}

bool AudioEngine::SingletonInitialize()
{
    if(!AUDIO_ENABLE)
        return true;

    const ALCchar *best_device = 0; // Will store the name of the 'best' device for audio playback
    ALCint highest_version = 0; // The highest version number found
    CheckALError(); // Clears errors
    CheckALCError(); // Clears errors

    // Find the highest-version device available, if the extension for device enumeration is present
    if(alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT") == AL_TRUE) {
        const ALCchar *device_list = 0;
        device_list = alcGetString(0, ALC_DEVICE_SPECIFIER); // Get list of all devices (terminated with two '0')
        if(CheckALCError() == true) {
            IF_PRINT_WARNING(AUDIO_DEBUG) << "failed to retrieve the list of available audio devices: " << CreateALCErrorString() << std::endl;
        }


        while(*device_list != 0) {  // Check all the detected devices
            ALCint major_v = 0, minor_v = 0;

            // Open a temporary device for reading in its version number
            ALCdevice *temp_device = alcOpenDevice(device_list);
            if(CheckALCError() || temp_device == nullptr) {  // If we couldn't open the device, just move on to the next
                IF_PRINT_WARNING(AUDIO_DEBUG) << "couldn't open device for version checking: " << device_list << std::endl;
                device_list += strlen(device_list) + 1;
                continue;
            }

            // Create a temporary context for the device
            ALCcontext *temp_context = alcCreateContext(temp_device, 0);
            if(CheckALCError() || temp_context == nullptr) {  // If we couldn't create the context, move on to the next device
                IF_PRINT_WARNING(AUDIO_DEBUG) << "couldn't create a temporary context for device: " << device_list << std::endl;
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
            if(highest_version < (major_v * 10 + minor_v)) {
                highest_version = (major_v * 10 + minor_v);
                best_device = device_list;
            }
            device_list += strlen(device_list) + 1; // Go to the next device name in the list
        } // while (*device_name != 0)
    } // if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT") == AL_TRUE)

    // Open the 'best' device we found above. If no devices were previously found,
    // it will try opening the default device (= 0)
    _device = alcOpenDevice(best_device);
    if(CheckALCError() || _device == nullptr) {
        PRINT_ERROR << "failed to open an OpenAL audio device: " << CreateALCErrorString() << std::endl;
        return false;
    }

    // Create an OpenAL context
    _context = alcCreateContext(_device, nullptr);
    if(CheckALCError() || _context == nullptr) {
        PRINT_ERROR << "failed to create an OpenAL context: " << CreateALCErrorString() << std::endl;
        alcCloseDevice(_device);
        return false;
    }

    alcMakeContextCurrent(_context);
    CheckALError(); // Clear errors
    CheckALCError(); // Clear errors

    // Create as many sources as possible (we fix an upper bound of MAX_DEFAULT_AUDIO_SOURCES)
    ALuint source;
    for(uint16 i = 0; i < _max_sources; ++i) {
        alGenSources(1, &source);
        if(CheckALError() == true) {
            _max_sources = i;
            _max_cache_size = i / 4;
            break;
        }
        _audio_sources.push_back(new private_audio::AudioSource(source));
    }

    if(_max_sources == 0) {
        PRINT_ERROR << "failed to create at least one OpenAL audio source" << std::endl;
        return false;
    }

    return true;
} // bool AudioEngine::SingletonInitialize()

AudioEngine::~AudioEngine()
{
    if(!AUDIO_ENABLE)
        return;

    // Delete all entries in the sound cache
    for(std::map<std::string, private_audio::AudioCacheElement>::iterator i = _audio_cache.begin(); i != _audio_cache.end(); ++i) {
        delete i->second.audio;
    }
    _audio_cache.clear();

    // Delete all audio sources
    for(std::vector<AudioSource *>::iterator i = _audio_sources.begin(); i != _audio_sources.end(); ++i) {
        delete(*i);
    }
    _audio_sources.clear();

    // We shouldn't have any descriptors registered left,
    // except when some scripts have created its own descriptors and didn't free them.
    // So, let's do that now.
    if(!_registered_sounds.empty()) {
        PRINT_WARNING << _registered_sounds.size() << " SoundDescriptor objects were still "
                      "registered when the destructor was invoked, "
                      "the objects will be freed now." << std::endl;

        for(std::vector<SoundDescriptor *>::iterator it = _registered_sounds.begin();
                it != _registered_sounds.end();) {
            std::string filename = (*it)->GetFilename();
            if(!filename.empty()) {
                PRINT_WARNING << "This sound file was never unloaded: "
                              << filename << std::endl;
            }

            delete *it;
            it = _registered_sounds.erase(it);
        }
    }
    if(!_registered_music.empty()) {
        PRINT_WARNING << _registered_music.size() << " MusicDescriptor objects were still "
                      "registered when the destructor was invoked, "
                      "the objects will be freed now." << std::endl;
        for(std::vector<MusicDescriptor *>::iterator it = _registered_music.begin();
                it != _registered_music.end();) {
            std::string filename = (*it)->GetFilename();
            if(!filename.empty()) {
                PRINT_WARNING << "This music file was never unloaded: "
                              << filename << std::endl;
            }

            delete *it;
            it = _registered_music.erase(it);
        }
    }

    alcMakeContextCurrent(0);
    alcDestroyContext(_context);
    alcCloseDevice(_device);
}

void AudioEngine::Update()
{
    if(!AUDIO_ENABLE)
        return;

    for(std::vector<AudioSource *>::iterator i = _audio_sources.begin(); i != _audio_sources.end(); ++i) {
        if((*i)->owner) {
            (*i)->owner->_Update();
        }
    }
}

void AudioEngine::SetSoundVolume(float volume)
{
    if(volume < 0.0f) {
        IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to set sound volume less than 0.0f" << std::endl;
        _sound_volume = 0.0f;
    } else if(volume > 1.0f) {
        IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to set sound volume greater than 1.0f" << std::endl;
        _sound_volume = 1.0f;
    } else {
        _sound_volume = volume;
    }

    for(std::vector<SoundDescriptor *>::iterator i = _registered_sounds.begin(); i != _registered_sounds.end(); ++i) {
        if((*i)->_source != nullptr) {
            alSourcef((*i)->_source->source, AL_GAIN, _sound_volume * (*i)->GetVolume());
        }
    }
}

void AudioEngine::SetMusicVolume(float volume)
{
    if(volume < 0.0f) {
        IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to set music volume less than 0.0f: " << volume << std::endl;
        _music_volume = 0.0f;
    } else if(volume > 1.0f) {
        IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to set music volume greater than 1.0f: " << volume << std::endl;
        _music_volume = 1.0f;
    } else {
        _music_volume = volume;
    }

    for(std::vector<MusicDescriptor *>::iterator i = _registered_music.begin(); i != _registered_music.end(); ++i) {
        if((*i)->_source != nullptr) {
            alSourcef((*i)->_source->source, AL_GAIN, _music_volume * (*i)->GetVolume());
        }
    }
}

void AudioEngine::PauseAllSounds()
{
    for(std::vector<SoundDescriptor *>::iterator i = _registered_sounds.begin();
            i != _registered_sounds.end(); ++i) {
        (*i)->Pause();
    }
}

void AudioEngine::ResumeAllSounds()
{
    for(std::vector<SoundDescriptor *>::iterator i = _registered_sounds.begin();
            i != _registered_sounds.end(); ++i) {
        (*i)->Resume();
    }
}

void AudioEngine::StopAllSounds()
{
    for(std::vector<SoundDescriptor *>::iterator i = _registered_sounds.begin();
            i != _registered_sounds.end(); ++i) {
        (*i)->Stop();
    }
}

void AudioEngine::RewindAllSounds()
{
    for(std::vector<SoundDescriptor *>::iterator i = _registered_sounds.begin();
            i != _registered_sounds.end(); ++i) {
        (*i)->Rewind();
    }
}

void AudioEngine::PauseActiveMusic()
{
    MusicDescriptor* music = GetActiveMusic();
    if (music)
        music->Pause();
}

void AudioEngine::ResumeActiveMusic()
{
    MusicDescriptor* music = GetActiveMusic();
    if (music)
        music->Resume();
}

void AudioEngine::StopActiveMusic()
{
    MusicDescriptor* music = GetActiveMusic();
    if (music)
        music->Stop();
}

void AudioEngine::RewindActiveMusic()
{
    MusicDescriptor* music = GetActiveMusic();
    if (music)
        music->Rewind();
}

void AudioEngine::FadeOutActiveMusic(float time)
{
    MusicDescriptor* music = GetActiveMusic();
    if (music)
        music->FadeOut(time);
}

void AudioEngine::FadeInActiveMusic(float time)
{
    MusicDescriptor* music = GetActiveMusic();
    if (music)
        music->FadeIn(time);
}

void AudioEngine::FadeOutAllSounds(float time)
{
    for(std::vector<SoundDescriptor *>::iterator it = _registered_sounds.begin();
            it != _registered_sounds.end(); ++it) {
        if(*it)
            (*it)->FadeOut(time);
    }
}

void AudioEngine::SetListenerPosition(const float position[3])
{
    alListenerfv(AL_POSITION, position);
    memcpy(_listener_position, position, sizeof(float) * 3);
}

void AudioEngine::SetListenerVelocity(const float velocity[3])
{
    alListenerfv(AL_VELOCITY, velocity);
    memcpy(_listener_velocity, velocity, sizeof(float) * 3);
}

void AudioEngine::SetListenerOrientation(const float orientation[3])
{
    alListenerfv(AL_ORIENTATION, orientation);
    memcpy(_listener_orientation, orientation, sizeof(float) * 3);
}

bool AudioEngine::LoadSound(const std::string &filename, vt_mode_manager::GameMode *gm)
{
    return _LoadAudio(filename, false, gm);
}

bool AudioEngine::LoadMusic(const std::string &filename, vt_mode_manager::GameMode *gm)
{
    return _LoadAudio(filename, true, gm);
}

void AudioEngine::PlaySound(const std::string &filename)
{
    std::map<std::string, AudioCacheElement>::iterator element = _audio_cache.find(filename);

    if(element == _audio_cache.end()) {
        // Don't check the current game mode to prevent the sound unloading in certain cases.
        // We'll let the audio cache handle it all atm.
        if(!LoadSound(filename)) {
            IF_PRINT_WARNING(AUDIO_DEBUG)
                    << "could not play sound from cache because "
                    "the sound could not be loaded" << std::endl;
            return;
        } else {
            element = _audio_cache.find(filename);
        }
    }

    element->second.audio->Play();
    element->second.last_update_time = SDL_GetTicks();
}

void AudioEngine::PlayMusic(const std::string &filename)
{
    std::map<std::string, AudioCacheElement>::iterator element = _audio_cache.find(filename);

    if(element == _audio_cache.end()) {
        // Get the current game mode, so that the loading/freeing micro management
        // is handled the most possible.
        vt_mode_manager::GameMode *gm = vt_mode_manager::ModeManager->GetTop();
        if(!LoadMusic(filename, gm)) {
            IF_PRINT_WARNING(AUDIO_DEBUG)
                    << "could not play music from cache because "
                    "the music could not be loaded" << std::endl;
            return;
        } else {
            element = _audio_cache.find(filename);
        }
    }

    // Special case: the music descriptor object must be taken back:
    MusicDescriptor *music_audio = reinterpret_cast<MusicDescriptor *>(element->second.audio);
    if(music_audio) {
        music_audio->Play();
        element->second.last_update_time = SDL_GetTicks();
    }
}

void AudioEngine::StopSound(const std::string &filename)
{
    std::map<std::string, AudioCacheElement>::iterator element = _audio_cache.find(filename);

    if(element == _audio_cache.end()) {
        IF_PRINT_WARNING(AUDIO_DEBUG) << "could not stop audio because it was not contained in the cache: " << filename << std::endl;
        return;
    }

    element->second.audio->Stop();
    element->second.last_update_time = SDL_GetTicks();
}

void AudioEngine::PauseSound(const std::string &filename)
{
    std::map<std::string, AudioCacheElement>::iterator element = _audio_cache.find(filename);

    if(element == _audio_cache.end()) {
        IF_PRINT_WARNING(AUDIO_DEBUG) << "could not pause audio because it was not contained in the cache: " << filename << std::endl;
        return;
    }

    element->second.audio->Pause();
    element->second.last_update_time = SDL_GetTicks();
}

void AudioEngine::ResumeSound(const std::string &filename)
{
    std::map<std::string, AudioCacheElement>::iterator element = _audio_cache.find(filename);

    if(element == _audio_cache.end()) {
        IF_PRINT_WARNING(AUDIO_DEBUG) << "could not resume audio because it was not contained in the cache: " << filename << std::endl;
        return;
    }

    element->second.audio->Resume();
    element->second.last_update_time = SDL_GetTicks();
}

SoundDescriptor *AudioEngine::RetrieveSound(const std::string &filename)
{
    std::map<std::string, AudioCacheElement>::iterator element = _audio_cache.find(filename);

    if(element == _audio_cache.end()) {
        return nullptr;
    } else if(element->second.audio->IsSound() == false) {
        IF_PRINT_WARNING(AUDIO_DEBUG) << "incorrectly requested to retrieve a sound for a music filename: " << filename << std::endl;
        return nullptr;
    } else {
        return dynamic_cast<SoundDescriptor *>(element->second.audio);
    }
}

MusicDescriptor *AudioEngine::RetrieveMusic(const std::string &filename)
{
    std::map<std::string, AudioCacheElement>::iterator element = _audio_cache.find(filename);

    if(element == _audio_cache.end()) {
        return nullptr;
    } else if(element->second.audio->IsSound() == true) {
        IF_PRINT_WARNING(AUDIO_DEBUG) << "incorrectly requested to retrieve music for a sound filename: " << filename << std::endl;
        return nullptr;
    } else {
        return dynamic_cast<MusicDescriptor *>(element->second.audio);
    }
}

void AudioEngine::RemoveGameModeOwner(vt_mode_manager::GameMode* gm)
{
    if(!gm)
        return;

    // Tells all audio descriptor the owner can be removed.
    std::map<std::string, AudioCacheElement>::iterator it = _audio_cache.begin();
    for(; it != _audio_cache.end();) {
        // If the audio buffers are erased, we can remove the descriptor from the cache.
        if(it->second.audio->RemoveGameModeOwner(gm)) {
            delete it->second.audio;
            // Make sure the iterator doesn't get flawed after erase.
            _audio_cache.erase(it++);
        } else {
            ++it;
        }
    }
}

const std::string AudioEngine::CreateALErrorString()
{
    switch(_al_error_code) {
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

const std::string AudioEngine::CreateALCErrorString()
{
    switch(_alc_error_code) {
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

void AudioEngine::DEBUG_PrintInfo()
{
    const ALCchar *c;

    PRINT_WARNING << "*** Audio Information ***" << std::endl;

    PRINT_WARNING << "Maximum number of sources:   " << _max_sources << std::endl;
    PRINT_WARNING << "Maximum audio cache size:    " << _max_cache_size << std::endl;
    PRINT_WARNING << "Default audio device:        " << alcGetString(_device, ALC_DEFAULT_DEVICE_SPECIFIER) << std::endl;
    PRINT_WARNING << "OpenAL Version:              " << alGetString(AL_VERSION) << std::endl;
    PRINT_WARNING << "OpenAL Renderer:             " << alGetString(AL_RENDERER) << std::endl;
    PRINT_WARNING << "OpenAL Vendor:               " << alGetString(AL_VENDOR) << std::endl;

    CheckALError();

    PRINT_WARNING << "Available OpenAL Extensions:" << std::endl;
    c = alGetString(AL_EXTENSIONS);
    bool new_extension = true;
    while(c[0]) {
        if(new_extension) {
            PRINT_WARNING << " - ";
            new_extension = false;
            continue;
        } else if(c[0] == ' ') {
            PRINT_WARNING << std::endl;
            new_extension = true;
            c++;
            continue;
        }

        PRINT_WARNING << c[0];
        c++;
    }
}

private_audio::AudioSource *AudioEngine::_AcquireAudioSource()
{
    // (1) Find and return the first source that does not have an owner
    for(std::vector<AudioSource *>::iterator i = _audio_sources.begin(); i != _audio_sources.end(); ++i) {
        if((*i)->owner == nullptr) {
            return *i;
        }
    }

    // (2) If all sources are owned, find one that is in the initial or stopped state and change its ownership
    for(std::vector<AudioSource *>::iterator i = _audio_sources.begin(); i != _audio_sources.end(); ++i) {
        ALint state;
        alGetSourcei((*i)->source, AL_SOURCE_STATE, &state);
        if(state == AL_INITIAL || state == AL_STOPPED) {
            (*i)->owner->_source = nullptr;
            (*i)->Reset(); // this call sets the source owner pointer to nullptr
            return *i;
        }
    }

    // (3) Return nullptr in the (extremely rare) case that all sources are owned and actively playing or paused
    return nullptr;
}



bool AudioEngine::_LoadAudio(const std::string &filename, bool is_music, vt_mode_manager::GameMode *gm)
{
    if(!DoesFileExist(filename))
        return false;

    std::map<std::string, private_audio::AudioCacheElement>::iterator it = _audio_cache.find(filename);
    if(it != _audio_cache.end()) {

        if (gm)
            it->second.audio->AddGameModeOwner(gm);

        // Return a success since basically everything will keep on working as expected.
        return true;
    }

    // Creates the new audio object and adds its potential game mode owner.
    AudioDescriptor *audio = nullptr;
    if (is_music)
        audio = new MusicDescriptor();
    else
        audio = new SoundDescriptor();

    if (gm)
        audio->AddGameModeOwner(gm);

    // (1) If the cache is not full, try loading the audio and adding it in
    if(_audio_cache.size() < _max_cache_size) {
        if(audio->LoadAudio(filename) == false) {
            IF_PRINT_WARNING(AUDIO_DEBUG) << "could not add new audio file into cache because load operation failed: " << filename << std::endl;
            delete audio;
            return false;
        }

        _audio_cache.insert(std::make_pair(filename, AudioCacheElement(SDL_GetTicks(), audio)));
        return true;
    }

    // (2) The cache is full, so find an element to remove. First make sure that at least one piece of audio is stopped
    std::map<std::string, AudioCacheElement>::iterator lru_element = _audio_cache.end();
    for(std::map<std::string, AudioCacheElement>::iterator i = _audio_cache.begin(); i != _audio_cache.end(); ++i) {
        if(i->second.audio->GetState() == AUDIO_STATE_STOPPED) {
            lru_element = i;
            break;
        }
    }

    if(lru_element == _audio_cache.end()) {
        IF_PRINT_WARNING(AUDIO_DEBUG) << "failed to remove element from cache because no piece of audio was in the stopped state" << std::endl;
        delete audio;
        return false;
    }

    for(std::map<std::string, AudioCacheElement>::iterator i = _audio_cache.begin(); i != _audio_cache.end(); ++i) {
        if(i->second.audio->GetState() == AUDIO_STATE_STOPPED && i->second.last_update_time < lru_element->second.last_update_time) {
            lru_element = i;
        }
    }

    delete lru_element->second.audio;
    _audio_cache.erase(lru_element);

    if(audio->LoadAudio(filename) == false) {
        IF_PRINT_WARNING(AUDIO_DEBUG) << "could not add new audio file into cache because load operation failed: " << filename << std::endl;
        delete audio;
        return false;
    }

    _audio_cache.insert(std::make_pair(filename, AudioCacheElement(SDL_GetTicks(), audio)));
    return true;
} // bool AudioEngine::_LoadAudio(AudioDescriptor* audio, const std::string& filename)

} // namespace vt_audio
