////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file   audio_descriptor.h
*** \author Mois�s Ferrer Serra, byaku@allacrost.org
*** \author Tyler Olsen, roots@allacrost.org
*** \brief  Header file for audio descriptors, sources and buffers
***
*** This code provides the interface for the sound and music descriptors, that
*** are the units for load and manage sounds in the engine.
***
*** \note This code uses the OpenAL audio library. See http://www.openal.com/
*** ***************************************************************************/

#ifndef __AUDIO_DESCRIPTOR_HEADER__
#define __AUDIO_DESCRIPTOR_HEADER__

#ifdef __MACH__
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
#else
	#include "al.h"
	#include "alc.h"
#endif

#include "defs.h"
#include "utils.h"

#include "audio_input.h"
#include "audio_stream.h"

namespace hoa_audio {

//! \brief The set of states that AudioDescriptor class objects may be in
enum AUDIO_STATE {
	//! Audio data is not loaded
	AUDIO_STATE_UNLOADED   = 0,
	//! Audio is loaded, but is stopped
	AUDIO_STATE_STOPPED    = 1,
	//! Audio is loaded and is presently playing
	AUDIO_STATE_PLAYING    = 2,
	//! Audio is loaded and was playing, but is now paused
	AUDIO_STATE_PAUSED     = 3
};

//! \brief The possible ways for that a piece of audio data may be loaded
enum AUDIO_LOAD {
	//! \brief Load audio statically by placing the entire contents of the audio into a single OpenAL buffer
	AUDIO_LOAD_STATIC         = 0,
	//! \brief Stream the audio data from a file into a pair of OpenAL buffers
	AUDIO_LOAD_STREAM_FILE    = 1,
	//! \brief Stream the audio data from memory into a pair of OpenAL buffers
	AUDIO_LOAD_STREAM_MEMORY  = 2
};

namespace private_audio {

//! \brief The default buffer size (in bytes) for streaming buffers
const uint32 DEFAULT_BUFFER_SIZE = 8192;

//! \brief The number of buffers to use for streaming audio descriptors
const uint32 NUMBER_STREAMING_BUFFERS = 4;

/** ****************************************************************************
*** \brief Represents an OpenAL buffer
***
*** A buffer in OpenAL is simply a structure which contains raw audio data.
*** Buffers must be attached to an OpenAL source in order to play. OpenAL
*** suppports an infinte number of buffers (as long as there is enough memory).
*** ***************************************************************************/
class AudioBuffer {
	friend class AudioEngine;

public:
	AudioBuffer();

	~AudioBuffer();

	/** \brief Fills an OpenAL buffer with raw audio data
	*** \param data A pointer to the raw data to fill the buffer with
	*** \param format The format of the buffer data (mono/stereo, 8/16 bits per sample)
	*** \param size The size of the data in number of bytes
	*** \param frequency The audio frequency of the data in samples per second
	**/
	void FillBuffer(uint8* data, ALenum format, uint32 size, uint32 frequency)
		{ alBufferData(buffer, format, data, size, frequency); }

	//! \brief Returns true if this class object holds a reference to a valid OpenAL buffer
	bool IsValid() const
		{ return (alIsBuffer(buffer) == AL_TRUE); }

	//! \brief The ID of the OpenAL buffer
	ALuint buffer;
}; // class AudioBuffer


/** ****************************************************************************
*** \brief Represents an OpenAL source
***
*** OpenAL is designed to take care of the complexity of panning sound to
*** different speakers; it does this by storing the locations in 3d space of
*** both the physical sources that a sound might originate from (for example,
*** the point in space where two swords clang together), and also the 3d
*** location of the listener's "ears". A source in OpenAL is simply metadata
*** about the position of the sound; the actual sound itself comes from audio
*** data which is loaded into a buffer, and then played back through one of
*** these sources.
***
*** This metadata includes properties like position, velocity,
*** etc. None of these are actually altered by OpenAL; OpenAL does not use
*** velocity to move the sound sources for us each game tick; rather, it simply
*** uses these to calculate sound itself (velocity is actually used for
*** calculating doppler effects). We are expected to fill these values with
*** appropriate position/velocity data to keep them in sync with the game
*** objects they represent.
***
*** Those properties are not managed by this class, but rather by the
*** AudioDescriptor to which the source is attached. OpenAL (or rather, the
*** audio hardware) only allows a limited number of audio sources to exist at
*** one time, so we can't create a source for every piece of audio that is
*** loaded by the game. Therefore, we create as many sources as we can (up to
*** MAX_DEFAULT_AUDIO_SOURCES) and have the audio descriptors share between
*** sources as they need them.
***
*** \note OpenAL sources are created and by the AudioEngine class, not within the
*** AudioSource constructor. The sources are, however, deleted by the destructor.
***
*** \note You should never really need to call the IsValid() function when
*** retrieving a new AudioSource to use. This is because all AudioSource objects
*** created by AudioEngine are guaranteed to have a valid OpenAL source contained
*** by the object.
*** ***************************************************************************/
class AudioSource {
public:
	//! \param al_source A valid OpenAL source that has been generated
	AudioSource(ALuint al_source) :
		source(al_source), owner(NULL) {}

	~AudioSource();

	//! \brief Returns true if this class object holds a reference to a valid OpenAL source
	bool IsValid() const
		{ return (alIsSource(source) == AL_TRUE); }

	//! \brief Resets the default properties of the OpenAL sources and removes the owner
	void Reset();

	//! \brief The ID of the OpenAL source
	ALuint source;

	//! \brief Pointer to the descriptor associated to this source.
	AudioDescriptor* owner;
}; // class AudioSource

} // namespace private_audio

/** ****************************************************************************
*** \brief An abstract class for representing a piece of audio
***
*** This class takes the OpenAL buffer and source concepts and ties them
*** together. This class enables playback, streaming, 3D source positioning,
*** and many other features for manipulating a piece of audio. Sounds and
*** music are defined by classes which derive from this class.
***
*** \note Some features of this class are only available if the audio is loaded
*** in a streaming manner.
***
*** \note You should <b>never</b> trust the value of _state when it is set to
*** AUDIO_STATE_PLAYING. This is because the audio may stop playing on its own
*** after the play state has been set. Instead, you should call the GetState()
*** method, which guarantees that the correct state value is set.
***
*** \todo This class either needs to have its copy assignment operator defined
*** or it should be made private.
*** ***************************************************************************/
class AudioDescriptor {
	friend class AudioEngine;

public:
	AudioDescriptor();

	virtual ~AudioDescriptor()
		{ FreeAudio(); }

	AudioDescriptor(const AudioDescriptor& copy);

	/** \brief Loads a new piece of audio data from a file
	*** \param filename The name of the file that contains the new audio data (should have a .wav or .ogg file extension)
	*** \param load_type The type of loading to perform (default == AUDIO_LOAD_STATIC)
	*** \param stream_buffer_size If the loading type is streaming, the buffer size to use (default == DEFAULT_BUFFER_SIZE)
	*** \return True if the audio was succesfully loaded, false if there was an error
	***
	*** The action taken by this function depends on the load type selected. For static sounds, a single OpenAL buffer is
	*** filled. For streaming, the file/memory is prepared.
	**/
	virtual bool LoadAudio(const std::string& filename, AUDIO_LOAD load_type = AUDIO_LOAD_STATIC, uint32 stream_buffer_size = private_audio::DEFAULT_BUFFER_SIZE);

	/** \brief Frees all data resources and resets class parameters
	***
	*** It resets the _state and _offset class members, as well as deleting _data, _stream, _input, _buffer, and resets _source.
	**/
	void FreeAudio();

	const std::string GetFilename() const
		{ if (_input == NULL) return ""; else return _input->GetFilename(); }

	//! \brief Returns true if this audio represents a sound, false if the audio represents a music piece
	virtual bool IsSound() const = 0;

	/** \brief Returns the state of the audio,
	*** \note This function does not simply return the _state member. If _state is set
	*** to AUDIO_STATE_PLAYING, the source state is queried to assure that it is still
	*** playing.
	**/
	AUDIO_STATE GetState();

	/** \name Audio State Manipulation Functions
	*** \brief Performs specified operation on the audio
	***
	*** These functions will only take effect when the audio is in the state(s) specified below:
	*** - PlayAudio()     <==>   all states but the playing state
	*** - PauseAudio()    <==>   playing state
	*** - ResumeAudio()   <==>   paused state
	*** - StopAudio()     <==>   all states but the stopped state
	*** - RewindAudio()   <==>   all states
	**/
	//@{
	virtual void Play();
	virtual void Stop();
	virtual void Pause();
	virtual void Resume();
	void Rewind();
	//@}

	bool IsLooping() const
		{ return _looping; }

	/** \brief Enables/disables looping for this audio
	*** \param loop True to enable looping, false to disable it.
	**/
	void SetLooping(bool loop);

	/** \brief Sets the starting loop point, used for customized looping
	*** \param loop_start The sample position for the start loop point
	*** \note This function is only valid if the audio has been loaded with streaming support
	**/
	void SetLoopStart(uint32 loop_start);

	/** \brief Sets the ending loop point, used for customized looping
	*** \param loop_start The sample position for the end loop point
	*** \note This function is only valid if the audio has been loaded with streaming support
	**/
	void SetLoopEnd(uint32 loop_end);

	/** \brief Seeks to the requested sample position
	*** \param sample The sample position to seek to
	**/
	void SeekSample(uint32 sample);

	/** \brief Seeks to the requested playback time
	*** \param second The time to seek to, in seconds (e.g. 4.5f == 4.5 second mark)
	*** \note The position is aligned with a proper sample position, so the seek is not fully
	*** accurate.
	**/
	void SeekSecond(float second);

	//! \brief Returns the volume level for this audio
	float GetVolume() const
		{ return _volume; }

	/** \brief Sets the volume for this particular audio piece
	*** \param volume The volume level to set, ranging from [0.0f, 1.0f]
	**/
	virtual void SetVolume(float volume) = 0;

	/** \name Functions for 3D Spatial Audio
	*** These functions manipulate and retrieve the 3d properties of the audio. Note that only audio which
	*** are mono channel will be affected by these methods. Stereo channel audio will see no difference.
	**/
	//@{
	void SetPosition(const float position[3]);
	void SetVelocity(const float velocity[3]);
	void SetDirection(const float direction[3]);

	void GetPosition(float position[3]) const
		{ memcpy(&position, _position, sizeof(float) * 3); }

	void GetVelocity(float velocity[3]) const
		{ memcpy(&velocity, _velocity, sizeof(float) * 3); }

	void GetDirection(float direction[3]) const
		{ memcpy(&direction, _direction, sizeof(float) * 3); }
	//@}

	//! \brief Prints various properties about the audio data managed by this class
	void DEBUG_PrintInfo();

protected:
	//! \brief The current state of the audio (playing, stopped, etc.)
	AUDIO_STATE _state;

	//! \brief A pointer to the buffer(s) being used by the audio (1 buffer for static sounds, 2 for streamed ones)
	private_audio::AudioBuffer* _buffer;

	//! \brief A pointer to the source object being used by the audio
	private_audio::AudioSource* _source;

	//! \brief A pointer to the input object that manages the data
	private_audio::AudioInput* _input;

	//! \brief A pointer to the stream object (set to NULL if the audio was loaded statically)
	private_audio::AudioStream* _stream;

	//! \brief A pointer to where the data is streamed to
	uint8* _data;

	//! \brief The format of the audio (mono/stereo, 8/16 bits per second).
	ALenum _format;

	//! \brief Flag for indicating if the audio should loop or not
	bool _looping;

	//! \brief The audio position that was last seeked, in samples.
	uint32 _offset;

	/** \brief The volume of the audio, ranging from 0.0f to 1.0f
	*** This isn't actually the true volume of the audio, but rather the modulation
	*** value of the global sound or music volume level. For example, if this object
	*** represented a sound and the volume was set to 0.75f, and the global sound
	*** volume in AudioEngine was 0.80f, the true volume would be (0.75 * 0.8 = 0.6).
	*** By default this member is set to 1.0f.
	**/
	float _volume;

	//! \brief Size of the streaming buffer, if the audio was loaded for streaming
	uint32 _stream_buffer_size;

	//! \brief The 3D orientation properties of the audio
	//@{
	float _position[3];
	float _velocity[3];
	float _direction[3];
	//@}

	/** \brief Sets the local volume control for this particular audio piece
	*** \param volume The volume level to set, ranging from [0.0f, 1.0f]
	*** This should be thought of as a helper function to the SetVolume methods
	*** for the derived classes, which modulate the volume level of the sound/music
	*** by the global sound and music volume controls in the AudioEngine class.
	**/
	void _SetVolumeControl(float volume);

private:
	/** \brief Updates the audio during playback
	*** This function is only useful for streaming audio that is currently in the play state. If either of these two
	*** conditions are not met, the function will return since it has nothing to do.
	**/
	void _Update();

	/** \brief Acquires an audio source for playback
	*** This function is called whenever an audio piece is loaded and whenever the Play operation is specified on
	*** the audio, but the audio currently does not have a source. It is not guaranteed that the source acquisition
	*** will be successful, as all other sources may be occupied by other audio.
	**/
	void _AcquireSource();

	/** \brief Sets all of the relevant properties for the OpenAL source
	*** This function should be called whenever a new source is allocated for the audio to use.
	*** It sets all of the necessary properties for the OpenAL source, such as the volume (gain),
	*** enables looping if requested, etc.
	**/
	void _SetSourceProperties();

	/** \brief Prepares streaming buffers when a new source is acquired or after a seeking operation.
	*** This is a special case, since the already queued buffers must be unqueued, and the new
	*** ones must be refilled. This function should only be called for streaming audio.
	**/
	void _PrepareStreamingBuffers();
}; // class AudioDescriptor


/** ****************************************************************************
*** \brief An class for representing a piece of sound audio
***
*** Sounds are almost always in the .wav file format.
*** ***************************************************************************/
class SoundDescriptor : public AudioDescriptor {
public:
	SoundDescriptor();

	~SoundDescriptor();

	SoundDescriptor(const SoundDescriptor& copy);

	bool IsSound() const
		{ return true; }

	/** \brief Sets the volume of the sound
	*** \param volume The volume to set the sound, value between [0.0, 1.0]
	*** This value will be modulated by the global sound volume found in the
	*** AudioEngine class.
	**/
	void SetVolume(float volume);
}; // class SoundDescriptor : public AudioDescriptor


/** ****************************************************************************
*** \brief A class for representing a piece of music audio
***
*** Music is almost always in the .ogg file format.
***
*** \note Looping is enabled for music by default
*** ***************************************************************************/
class MusicDescriptor : public AudioDescriptor {
public:
	MusicDescriptor();

	~MusicDescriptor();

	MusicDescriptor(const MusicDescriptor& copy);

	bool LoadAudio(const std::string& filename, AUDIO_LOAD load_type = AUDIO_LOAD_STREAM_FILE, uint32 stream_buffer_size = private_audio::DEFAULT_BUFFER_SIZE);

	bool IsSound() const
		{ return false; }

	/** \brief Sets the volume of the music
	*** \param volume The volume to set the music, value between [0.0, 1.0]
	*** This value will be modulated by the global music volume found in the
	*** AudioEngine class.
	**/
	void SetVolume(float volume);

	/** \brief Plays the selected music, after stopping the previous playing music
	*** No two pieces of music are allowed to play simultaneously, meaning that
	*** calling this method on one music also effectively calls stop on another
	*** piece of music that was playing when the call was made
	**/
	void Play();
}; // class MusicDescriptor : public AudioDescriptor

} // namespace hoa_audio

#endif
