////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file   audio_descriptor.cpp
*** \author Mois�s Ferrer Serra, byaku@allacrost.org
*** \author Tyler Olsen, roots@allacrost.org
*** \brief  Source for audio descriptors, sources and buffers
***
*** This code provides the funcionality for load sounds and music in the engine.
*** it provides all the funtions available for them, as basic ones (play, stop,...)
*** seeking and more.
***
*** \note This code uses the OpenAL audio library. See http://www.openal.com/
*** ***************************************************************************/

#include "audio.h"
#include "audio_descriptor.h"

using namespace std;
using namespace hoa_audio::private_audio;

namespace hoa_audio {

namespace private_audio {

////////////////////////////////////////////////////////////////////////////////
// AudioBuffer class methods
////////////////////////////////////////////////////////////////////////////////

AudioBuffer::AudioBuffer() :
	buffer(0)
{
	if (AudioManager->CheckALError()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "OpenAL error detected before buffer generation: " << AudioManager->CreateALErrorString() << endl;
	}
	
	alGenBuffers(1, &buffer);

	if (AudioManager->CheckALError()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "OpenAL error detected after buffer generation: "
			<< AudioManager->CreateALErrorString() << endl;
		buffer = 0;
	}
}



AudioBuffer::~AudioBuffer() {
	if (IsValid()) {
		alDeleteBuffers(1, &buffer);
	}
}

////////////////////////////////////////////////////////////////////////////////
// AudioSource class methods
////////////////////////////////////////////////////////////////////////////////

AudioSource::~AudioSource() {
	if (IsValid()) {
		alSourceStop(source);
		alDeleteSources(1, &source);
	}
	else {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "OpenAL source was invalid upon destruction" << endl;
	}
}



void AudioSource::Reset() {
	owner = NULL;

	if (IsValid() == false) {
		return;
	}

	alSourcei(source, AL_LOOPING, AL_FALSE);
	alSourcef(source, AL_GAIN, 1.0f);
	alSourcei(source, AL_SAMPLE_OFFSET, 0);		// This line will cause AL_INVALID_ENUM error in linux/Solaris. It is normal.
	alSourcei(source, AL_BUFFER, 0);

	if (AudioManager->CheckALError()) {
		#ifdef WIN32
			IF_PRINT_WARNING(AUDIO_DEBUG) << "resetting source failed: " << AudioManager->CreateALErrorString() << endl;
		#endif
	}
}

} // namespace private_audio

////////////////////////////////////////////////////////////////////////////////
// AudioDescriptor class methods
////////////////////////////////////////////////////////////////////////////////

AudioDescriptor::AudioDescriptor() :
	_state(AUDIO_STATE_UNLOADED),
	_buffer(NULL),
	_source(NULL),
	_input(NULL),
	_stream(NULL),
	_data(NULL),
	_looping(false),
	_offset(0),
	_volume(1.0f),
	_stream_buffer_size(0)
{
	_position[0] = 0.0f;
	_position[1] = 0.0f;
	_position[2] = 0.0f;
	_velocity[0] = 0.0f;
	_velocity[1] = 0.0f;
	_velocity[2] = 0.0f;
	_direction[0] = 0.0f;
	_direction[1] = 0.0f;
	_direction[2] = 0.0f;
}



AudioDescriptor::AudioDescriptor(const AudioDescriptor& copy) :
	_state(AUDIO_STATE_UNLOADED),
	_buffer(NULL),
	_source(NULL),
	_input(NULL),
	_stream(NULL),
	_data(NULL),
	_looping(copy._looping),
	_offset(0),
	_volume(copy._volume),
	_stream_buffer_size(0)
{
	_position[0] = 0.0f;
	_position[1] = 0.0f;
	_position[2] = 0.0f;
	_velocity[0] = 0.0f;
	_velocity[1] = 0.0f;
	_velocity[2] = 0.0f;
	_direction[0] = 0.0f;
	_direction[1] = 0.0f;
	_direction[2] = 0.0f;

	// If the copy is not in the unloaded state, print a warning
	if (copy._state != AUDIO_STATE_UNLOADED) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "created a copy of an already initialized AudioDescriptor" << endl;
	}
}


bool AudioDescriptor::LoadAudio(const string& filename, AUDIO_LOAD load_type, uint32 stream_buffer_size) {
	if (!AUDIO_ENABLE)
		return true;

	// Clean out any audio resources being used before trying to set new ones
	FreeAudio();

	// Load the input file for the audio
	if (filename.size() <= 3) { // Name of file is at least 3 letters (so the extension is in there)
		IF_PRINT_WARNING(AUDIO_DEBUG) << "file name argument is too short: " << filename << endl;
		return false;
	}
	// Convert the file extension to uppercase and use it to create the proper input type
	string file_extension = filename.substr(filename.size() - 3, 3);
	for (string::iterator i = file_extension.begin(); i != file_extension.end(); i++)
		*i = toupper(*i);

	// Based on the extension of the file, load properly one
	if (file_extension.compare("WAV") == 0) {
		_input = new WavFile(filename);
	}
	else if (file_extension.compare("OGG") == 0) {
		_input = new OggFile(filename);
	}
	else {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed due to unsupported input file extension: " << file_extension << endl;
		return false;
	}

	if (_input->Initialize() == false) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed to load and initialize audio file: " << filename << endl;
		return false;
	}

	// Retreive audio data properties from the newly initialized input
	if (_input->GetBitsPerSample() == 8) {
		if (_input->GetNumberChannels() == 1) {
			_format = AL_FORMAT_MONO8;
		}
		else {
			_format = AL_FORMAT_STEREO8;
		}
	}
	else { // 16 bits per sample
		if (_input->GetNumberChannels() == 1) {
			_format = AL_FORMAT_MONO16;
		}
		else {
			_format = AL_FORMAT_STEREO16;
		}
	}

	// Load the audio data depending upon the load type requested
	if (load_type == AUDIO_LOAD_STATIC) {
		// For static sounds just 1 buffer is needed. We create it as an array here, so that
		// later we can delete it with a call of delete[], similar to the streaming cases
		_buffer = new AudioBuffer[1];

		// Create space in memory for the audio data to be read and passed to the OpenAL buffer
		_data = new uint8[_input->GetDataSize()];
		bool all_data_read = false;
		if (_input->Read(_data, _input->GetTotalNumberSamples(), all_data_read) != _input->GetTotalNumberSamples()) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "failed to read entire audio data stream for file: " << filename << endl;
			return false;
		}

		// Pass the buffer data to the OpenAL buffer
		_buffer->FillBuffer(_data, _format, _input->GetDataSize(), _input->GetSamplesPerSecond());
		delete[] _data;
		_data = NULL;

		// Attempt to acquire a source for the new audio to use
		_AcquireSource();
		if (_source == NULL) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "could not acquire audio source for new audio file: " << filename << endl;
		}
	} // if (load_type == AUDIO_LOAD_STATIC)

	// Stream the audio from the file data
	else if (load_type == AUDIO_LOAD_STREAM_FILE) {
		_buffer = new AudioBuffer[NUMBER_STREAMING_BUFFERS]; // For streaming we need to use multiple buffers
		_stream = new AudioStream(_input, _looping);
		_stream_buffer_size = stream_buffer_size;

		_data = new uint8[_stream_buffer_size * _input->GetSampleSize()];

		// Attempt to acquire a source for the new audio to use
		_AcquireSource();
		if (_source == NULL) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "could not acquire audio source for new audio file: " << filename << endl;
		}
	} // else if (load_type == AUDIO_LOAD_STREAM_FILE)

	// Allocate memory for the audio data to remain in and stream it from that location
	else if (load_type == AUDIO_LOAD_STREAM_MEMORY) {
		_buffer = new AudioBuffer[NUMBER_STREAMING_BUFFERS]; // For streaming we need to use multiple buffers
		_stream = new AudioStream(_input, _looping);
		_stream_buffer_size = stream_buffer_size;

		_data = new uint8[_stream_buffer_size * _input->GetSampleSize()];

		// We need to replace the _input member with a AudioMemory class object
		AudioInput* temp_input = _input;
		_input = new AudioMemory(temp_input);
		delete temp_input;

		// Attempt to acquire a source for the new audio to use
		_AcquireSource();
		if (_source == NULL) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "could not acquire audio source for new audio file: " << filename << endl;
		}
	} // else if (load_type == AUDIO_LOAD_STREAM_MEMORY) {

	else {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "unknown load_type argument passed: " << load_type << endl;
		return false;
	}

	if (AudioManager->CheckALError())
		IF_PRINT_WARNING(AUDIO_DEBUG) << "OpenAL generated the following error: " << AudioManager->CreateALErrorString() << endl;

	_state = AUDIO_STATE_STOPPED;
	return true;
} // bool AudioDescriptor::LoadAudio(const string& file_name, AUDIO_LOAD load_type, uint32 stream_buffer_size)



void AudioDescriptor::FreeAudio() {
	if (_source != NULL)
		Stop();

	_state = AUDIO_STATE_UNLOADED;
	_offset = 0;

	// If the source is still attached to a sound, reset to the default parameters the source
	if (_source != NULL) {
		_source->Reset();
		_source = NULL;
	}

	if (_buffer != NULL) {
		delete[] _buffer;
		_buffer = NULL;
	}

	if (_input != NULL) {
		delete _input;
		_input = NULL;
	}

	if (_stream != NULL) {
		delete _stream;
		_stream = NULL;
	}

	if (_data != NULL) {
		delete[] _data;
		_data = NULL;
	}
}



AUDIO_STATE AudioDescriptor::GetState() {
	// If the last set state was the playing state, we have to double check
	// with the OpenAL source to make sure that the audio is still playing.
	if (_state == AUDIO_STATE_PLAYING) {
		// If the descriptor no longe
		if (_source == NULL || _data == NULL) {
			_state = AUDIO_STATE_STOPPED;
		}
		else {
			ALint source_state;
			alGetSourcei(_source->source, AL_SOURCE_STATE, &source_state);
			if (AudioManager->CheckALError()) {
				IF_PRINT_WARNING(AUDIO_DEBUG) << "getting the source's state failed: " << AudioManager->CreateALErrorString() << endl;
			}
			if (source_state != AL_PLAYING) {
				_state = AUDIO_STATE_STOPPED;
			}
		}
	}

	return _state;
}



void AudioDescriptor::Play() {
	if (!AUDIO_ENABLE)
		return;

	if (_source == NULL) {
		_AcquireSource();
		if (_source == NULL) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "did not have access to valid AudioSource" << endl;
			return;
		}
		_SetSourceProperties();
	}

	if (_stream && _stream->GetEndOfStream()) {
		_stream->Seek(_offset);
		_PrepareStreamingBuffers();
	}

	// Temp: Checks if there is already an AL error in the buffer. If it is, print error and clear buffer.
	if (AudioManager->CheckALError()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "audio error occured some time before playing source: " << AudioManager->CreateALErrorString() << endl;
	}

	alSourcePlay(_source->source);
	if (AudioManager->CheckALError()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "playing the source failed: " << AudioManager->CreateALErrorString() << endl;
	}
	_state = AUDIO_STATE_PLAYING;
}



void AudioDescriptor::Stop() {
	if (_state == AUDIO_STATE_STOPPED || _state == AUDIO_STATE_UNLOADED)
		return;

	if (_source == NULL) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "did not have access to valid AudioSource" << endl;
		return;
	}

	// Temp: Checks if there is already an AL error in the buffer. If it is, print error and clear buffer.
	if (AudioManager->CheckALError()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "audio error occured some time before stopping source: " << AudioManager->CreateALErrorString() << endl;
	}

	alSourceStop(_source->source);
	if (AudioManager->CheckALError()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "stopping the source failed: " << AudioManager->CreateALErrorString() << endl;
	}
	_state = AUDIO_STATE_STOPPED;
}



void AudioDescriptor::Pause() {
	if (_state == AUDIO_STATE_PAUSED || _state == AUDIO_STATE_UNLOADED)
		return;

	if (_source == NULL) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "did not have access to valid AudioSource" << endl;
		return;
	}

	alSourcePause(_source->source);
	if (AudioManager->CheckALError()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "pausing the source failed: " << AudioManager->CreateALErrorString() << endl;
	}
	_state = AUDIO_STATE_PAUSED;
}



void AudioDescriptor::Resume() {
	if (_state != AUDIO_STATE_PAUSED)
		return;

	Play();
}



void AudioDescriptor::Rewind() {
	if (_source == NULL) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "did not have access to valid AudioSource" << endl;
		return;
	}

	alSourceRewind(_source->source);
	if (AudioManager->CheckALError()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "rewinding the source failed: " << AudioManager->CreateALErrorString() << endl;
	}
}



void AudioDescriptor::SetLooping(bool loop) {
	if (_looping == loop)
		return;

	_looping = loop;
	if (_stream != NULL) {
		_stream->SetLooping(_looping);
	}
	else if (_source != NULL) {
		if (_looping)
			alSourcei(_source->source, AL_LOOPING, AL_TRUE);
		else
			alSourcei(_source->source, AL_LOOPING, AL_FALSE);
	}
}



void AudioDescriptor::SetLoopStart(uint32 loop_start) {
	if (_stream == NULL) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "the audio data was not loaded with streaming properties, this operation is not permitted" << endl;
		return;
	}
	_stream->SetLoopStart(loop_start);
}



void AudioDescriptor::SetLoopEnd(uint32 loop_end) {
	if (_stream == NULL) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "the audio data was not loaded with streaming properties, this operation is not permitted" << endl;
		return;
	}
	_stream->SetLoopEnd(loop_end);
}



void AudioDescriptor::SeekSample(uint32 sample) {
	if (sample >= _input->GetTotalNumberSamples()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed because requested seek time fell outside the valid range of samples: " << sample << endl;
		return;
	}

	_offset = sample;

	if (_stream) {
		_stream->Seek(_offset);
		_PrepareStreamingBuffers();
	}
	else if (_source != NULL) {
		alSourcei(_source->source, AL_SAMPLE_OFFSET, _offset);
		if (AudioManager->CheckALError()) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "setting a source's offset failed: " << AudioManager->CreateALErrorString() << endl;
		}
	}
}



void AudioDescriptor::SeekSecond(float second) {
	if (second < 0.0f) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "function received invalid argument that was less than 0.0f: " << second << endl;
		return;
	}

	uint32 pos = static_cast<uint32>(second * _input->GetSamplesPerSecond());
	if (pos >= _input->GetTotalNumberSamples()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed because requested seek time fell outside the valid range of samples: " << pos << endl;
		return;
	}

	_offset = pos;
	if (_stream) {
		_stream->Seek(_offset);
		_PrepareStreamingBuffers();
	}
	else if (_source != NULL) {
		alSourcei(_source->source, AL_SEC_OFFSET, _offset);
		if (AudioManager->CheckALError()) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "setting a source's offset failed: " << AudioManager->CreateALErrorString() << endl;
		}
	}
}



void AudioDescriptor::SetPosition(const float position[3]) {
	if (_format != AL_FORMAT_MONO8 && _format != AL_FORMAT_MONO16) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "audio is stereo channel and will not be effected by function call" << endl;
		return;
	}

	memcpy(_position, position, sizeof(float) * 3);
	if (_source != NULL) {
		alSourcefv(_source->source, AL_POSITION, _position);
		if (AudioManager->CheckALError()) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "setting a source's position failed: " << AudioManager->CreateALErrorString() << endl;
		}
	}
}



void AudioDescriptor::SetVelocity(const float velocity[3]) {
	if (_format != AL_FORMAT_MONO8 && _format != AL_FORMAT_MONO16) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "audio is stereo channel and will not be effected by function call" << endl;
		return;
	}

	memcpy(_velocity, velocity, sizeof(float) * 3);
	if (_source != NULL) {
		alSourcefv(_source->source, AL_VELOCITY, _position);
		if (AudioManager->CheckALError()) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "setting a source's velocity failed: " << AudioManager->CreateALErrorString() << endl;
		}
	}
}



void AudioDescriptor::SetDirection(const float direction[3]) {
	if (_format != AL_FORMAT_MONO8 && _format != AL_FORMAT_MONO16) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "audio is stereo channel and will not be effected by function call" << endl;
		return;
	}

	memcpy(_direction, direction, sizeof(float) * 3);
	if (_source != NULL) {
		alSourcefv(_source->source, AL_DIRECTION, _direction);
		if (AudioManager->CheckALError()) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "setting a source's direction failed: " << AudioManager->CreateALErrorString() << endl;
		}
	}
}



void AudioDescriptor::DEBUG_PrintInfo() {
	cout << "*** Audio Descriptor Information ***" << endl;

	if (_input == NULL) {
		PRINT_WARNING << "no audio data loaded" << endl;
		return;
	}

	uint16 num_channels = 0;
	uint16 bits_per_sample = 0;
	switch (_format) {
		case AL_FORMAT_MONO8:
			num_channels = 1;
			bits_per_sample = 8;
			break;
		case AL_FORMAT_MONO16:
			num_channels = 1;
			bits_per_sample = 16;
			break;
		case AL_FORMAT_STEREO8:
			num_channels = 1;
			bits_per_sample = 8;
			break;
		case AL_FORMAT_STEREO16:
			num_channels = 2;
			bits_per_sample = 16;
			break;
		default:
			IF_PRINT_WARNING(AUDIO_DEBUG) << "unknown audio format: " << _format << endl;
			break;
	}

	cout << "Filename:          " << _input->GetFilename() << endl;
	cout << "Channels:          " << num_channels << endl;
	cout << "Bits Per Sample:   " << bits_per_sample << endl;
	cout << "Frequency:         " << _input->GetSamplesPerSecond() << endl;
	cout << "Samples:           " << _input->GetTotalNumberSamples() << endl;
	cout << "Time:              " << _input->GetPlayTime() << endl;

	if (_stream != NULL) {
		cout << "Audio load type:    streamed" << endl;
		cout << "Stream buffer size (samples): " << _stream_buffer_size << endl;
	}
	else {
		cout << "Audio load type:    static" << endl;
	}
} // void AudioDescriptor::DEBUG_PrintInfo()



void AudioDescriptor::_SetVolumeControl(float volume) {
	if (volume < 0.0f) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to set volume less than 0.0f" << endl;
		_volume = 0.0f;
	}
	else if (volume > 1.0f) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to set volume greater than 1.0f" << endl;
		_volume = 1.0f;
	}
	else {
		_volume = volume;
	}
}



void AudioDescriptor::_Update() {
	// Only streaming audio that is playing requires periodic updates
	if (_stream == NULL || _state != AUDIO_STATE_PLAYING)
		return;
	
	ALint queued = 0;
	alGetSourcei(_source->source, AL_BUFFERS_QUEUED, &queued);
	if (AudioManager->CheckALError()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "getting queued sources failed: " << AudioManager->CreateALErrorString() << endl;
	}
	
	// If there are no more buffers and the end of stream was reached, stop the sound
	if (queued != 0 && _stream->GetEndOfStream()) {
		_state = AUDIO_STATE_STOPPED;
		return;
	}

	ALint buffers_processed = 0;
	alGetSourcei(_source->source, AL_BUFFERS_PROCESSED, &buffers_processed);
	if (AudioManager->CheckALError()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "getting processed sources failed: " << AudioManager->CreateALErrorString() << endl;
	}
	
	// If any buffers have finished playing, attempt to refill them
	if (buffers_processed > 0) {
		ALuint buffer_finished;
		alSourceUnqueueBuffers(_source->source, 1, &buffer_finished);
		if (AudioManager->CheckALError()) {
			IF_PRINT_WARNING(AUDIO_DEBUG) << "unqueuing a source failed: " << AudioManager->CreateALErrorString() << endl;
		}

		uint32 size = _stream->FillBuffer(_data, _stream_buffer_size);
		if (size > 0) { // Make sure that there is data available to fill
			alBufferData(buffer_finished, _format, _data, size * _input->GetSampleSize(), _input->GetSamplesPerSecond());
			if (AudioManager->CheckALError()) {
				IF_PRINT_WARNING(AUDIO_DEBUG) << "buffering data failed: " << AudioManager->CreateALErrorString() << endl;
			}
			alSourceQueueBuffers(_source->source, 1, &buffer_finished);
			if (AudioManager->CheckALError()) {
				IF_PRINT_WARNING(AUDIO_DEBUG) << "queueing a source failed: " << AudioManager->CreateALErrorString() << endl;
			}
		}
		
		// This ensures that if a streaming audio piece is stopped because the buffers ran out
		// of audio data for the source to play, the audio will be automatically replayed again.
		ALint state;
		alGetSourcei(_source->source, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING) {
			alSourcePlay(_source->source);
			if (AudioManager->CheckALError()) {
				IF_PRINT_WARNING(AUDIO_DEBUG) << "playing a source failed: " << AudioManager->CreateALErrorString() << endl;
			}
		}
	}
} // void AudioDescriptor::_Update()



void AudioDescriptor::_AcquireSource() {
	if (_source != NULL) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "function was invoked when object already had a source acquired" << endl;
		return;
	}

	if (_buffer == NULL) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "function was invoked when object did not have an audio buffer attached" << endl;
		return;
	}

	_source = AudioManager->_AcquireAudioSource();
	if (_source == NULL) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "could not acquire audio source for new audio file: " << _input->GetFilename() << endl;
		return;
	}

	_source->owner = this;
	_SetSourceProperties();
	if (_stream == NULL)
		alSourcei(_source->source, AL_BUFFER, _buffer->buffer);
	else
		_PrepareStreamingBuffers();
}



void AudioDescriptor::_SetSourceProperties() {
	if (_source == NULL) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "function was invoked when class did not have access to an audio source" << endl;
		return;
	}

	// Set volume (gain)
	float volume_multiplier = 0.0f;
	if (IsSound())
		volume_multiplier = AudioManager->GetSoundVolume();
	else
		volume_multiplier = AudioManager->GetMusicVolume();

	alSourcef(_source->source, AL_GAIN, _volume * volume_multiplier);
	if (AudioManager->CheckALError()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "changing volume on a source failed: " << AudioManager->CreateALErrorString() << endl;
	}
	
	// Set looping (source has looping disabled by default, so only need to check the true case)
	if (_stream != NULL) {
		_stream->SetLooping(_looping);
	}
	else if (_source != NULL) {
		if (_looping) {
			alSourcei(_source->source, AL_LOOPING, AL_TRUE);
			if (AudioManager->CheckALError()) {
				IF_PRINT_WARNING(AUDIO_DEBUG) << "setting a source to loop failed: " << AudioManager->CreateALErrorString() << endl;
			}
		}
	}
	
	//! \todo More properties need to be set here, such as source position, etc.
}



void AudioDescriptor::_PrepareStreamingBuffers() {
	if (_stream == NULL) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "_stream pointer was NULL, meaning this function should never have been called" << endl;
		return;
	}

	if (_source == NULL) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed because no source was available for this object to utilize" << endl;
		return;
	}

	bool was_playing = false;
	if (AudioManager->CheckALError()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "OpenAL error detected: " << AudioManager->CreateALErrorString() << endl;
	}

	// Stop the audio if it is playing and detatch the buffer from the source
	if (_state == AUDIO_STATE_PLAYING) {
		was_playing = true;
		Stop();
	}
	alSourcei(_source->source, AL_BUFFER, 0);

	// Fill each buffer with audio data
	for (uint32 i = 0; i < NUMBER_STREAMING_BUFFERS; i++) {
		uint32 read = _stream->FillBuffer(_data, _stream_buffer_size);
		if (read > 0) {
			_buffer[i].FillBuffer(_data, _format, read * _input->GetSampleSize(), _input->GetSamplesPerSecond());
			if (_source != NULL)
				alSourceQueueBuffers(_source->source, 1, &_buffer[i].buffer);
		}
	}
	
	if (AudioManager->CheckALError()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed to fill all buffers: " << AudioManager->CreateALErrorString() << endl;
	}

	if (was_playing) {
		Play();
	}
}

////////////////////////////////////////////////////////////////////////////////
// SoundDescriptor class methods
////////////////////////////////////////////////////////////////////////////////

SoundDescriptor::SoundDescriptor() :
	AudioDescriptor()
{
	AudioManager->_registered_sounds.push_back(this);
}



SoundDescriptor::~SoundDescriptor() {
	for (list<SoundDescriptor*>::iterator i = AudioManager->_registered_sounds.begin();
		i != AudioManager->_registered_sounds.end(); i++) {
		if (*i == this) {
			AudioManager->_registered_sounds.erase(i);
			return;
		}
	}
}



SoundDescriptor::SoundDescriptor(const SoundDescriptor& copy) :
	AudioDescriptor(copy)
{
	AudioManager->_registered_sounds.push_back(this);
}



void SoundDescriptor::SetVolume(float volume) {
	AudioDescriptor::_SetVolumeControl(volume);

	float sound_volume = _volume * AudioManager->GetSoundVolume();

	if (_source) {
		alSourcef(_source->source, AL_GAIN, sound_volume);
	}
}

////////////////////////////////////////////////////////////////////////////////
// MusicDescriptor class methods
////////////////////////////////////////////////////////////////////////////////

MusicDescriptor::MusicDescriptor() :
	AudioDescriptor()
{
	_looping = true;
	AudioManager->_registered_music.push_back(this);
}



MusicDescriptor::~MusicDescriptor() {
	if (AudioManager->_active_music == this) {
		AudioManager->_active_music = NULL;
	}

	for (list<MusicDescriptor*>::iterator i = AudioManager->_registered_music.begin();
		i != AudioManager->_registered_music.end(); i++) {
		if (*i == this) {
			AudioManager->_registered_music.erase(i);
			return;
		}
	}
}



MusicDescriptor::MusicDescriptor(const MusicDescriptor& copy) :
	AudioDescriptor(copy)
{
	AudioManager->_registered_music.push_back(this);
}



bool MusicDescriptor::LoadAudio(const std::string& filename, AUDIO_LOAD load_type, uint32 stream_buffer_size) {
	return AudioDescriptor::LoadAudio(filename, load_type, stream_buffer_size);
}



void MusicDescriptor::Play() {
	if (!AUDIO_ENABLE)
		return;

	if (AudioManager->_active_music == this) {
		// This is slightly hacky, the real reason and when map mode returns to the top of the mode stack
		// _data is null for some reason.  So when GetState is called the check for _data changes the _state to
		// AUDIO_STATE_STOPPED, but the music is still playing.  This fixes that, but we should find the real reason.
		if (AudioManager->_active_music->_state != AUDIO_STATE_PLAYING)
			AudioManager->_active_music->_state = AUDIO_STATE_PLAYING;
		return;
	}
	else if (AudioManager->_active_music != NULL) {
		AudioManager->_active_music->Stop();
	}

	AudioManager->_active_music = this;
	AudioDescriptor::Play();
}



void MusicDescriptor::SetVolume(float volume) {
	AudioDescriptor::_SetVolumeControl(volume);

	float music_volume = _volume * AudioManager->GetMusicVolume();

	if (_source) {
		alSourcef(_source->source, AL_GAIN, (ALfloat)music_volume);
	}
}

} // namespace hoa_audio
