////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file   audio_input.cpp
*** \author Mois�s Ferrer Serra - byaku@allacrost.org
*** \author Aaron Smith - etherstar@allacrost.org
*** \brief  Source for the classes that provide input for sounds
***
*** This code provides classes for loading sounds (WAV and OGG). It also
*** provides the functionality for basic streaming operations, both from memory and from
*** file
***
*** \note This code uses Ogg-vorbis library for loading Ogg files
*** ***************************************************************************/

#include "audio_input.h"
#include <SDL/SDL_endian.h>

using namespace std;

namespace hoa_audio {

namespace private_audio {

////////////////////////////////////////////////////////////////////////////////
// AudioInput class methods
////////////////////////////////////////////////////////////////////////////////

AudioInput::AudioInput() :
	_filename(""),
	_samples_per_second(0),
	_bits_per_sample(0),
	_number_channels(0),
	_total_number_samples(0),
	_data_size(0),
	_sample_size(0),
	_play_time(0.0f)
{}

/* Some quick macros for byte swapping. SDL has some nice fast asm methods, so we'll use those */
// NOTE: these should probably be moved to the utils.h file
#ifdef __BIG_ENDIAN__
#define SWAP_U32_FROM_LITTLE(x) { x = SDL_Swap32(x); }
#define SWAP_U16_FROM_LITTLE(x) { x = SDL_Swap16(x); }
#else
// not a big-endian system - no byte swapping needed
#define SWAP_U32_FROM_LITTLE(x) { }
#define SWAP_U16_FROM_LITTLE(x) { }
#endif

////////////////////////////////////////////////////////////////////////////////
// WavFile class methods
////////////////////////////////////////////////////////////////////////////////

bool WavFile::Initialize() {
	uint32 size;

	char buffer[4];

	_file_input.open(_filename.c_str(), std::ios::binary);
	if (_file_input.fail()) {
		_file_input.clear();
		return false;
	}

	// Check that the initial chunk ID is "RIFF" -- 4 bytes
	_file_input.read(buffer, 4);
	if (buffer[0] != 'R' || buffer[1] != 'I' || buffer[2] != 'F' || buffer[3] != 'F') {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed because initial chunk ID was not \"RIFF\"" << endl;
		return false;
	}

	// Get chunk size (file size - 8) -- 4 bytes
	_file_input.read(buffer, 4);
	memcpy(&size, buffer, 4);
	SWAP_U32_FROM_LITTLE(size);

	// Check format to be "WAVE" -- 4 bytes
	_file_input.read(buffer, 4);
	if (buffer[0] != 'W' || buffer[1] != 'A' || buffer[2] != 'V' || buffer[3] != 'E') {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed because file format was not \"WAVE\"" << endl;
		return false;
	}

	// Check SubChunk ID to be "fmt " -- 4 bytes
	_file_input.read(buffer, 4);
	if (buffer[0] != 'f' || buffer[1] != 'm' || buffer[2] != 't' || buffer[3] != ' ') {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed because initial subchunk ID was not \"fmt \"" << endl;
		return false;
	}

	// Check subchunk size (to be 16) -- 4 bytes
	_file_input.read(buffer, 4);
	memcpy(&size, buffer, 4);
	SWAP_U32_FROM_LITTLE(size);
	if (size != 16) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed because subchunk size was not equal to 16" << endl;
		return false;
	}

	// Check audio format (only PCM supported currently) -- 2 bytes
	_file_input.read(buffer, 2);
	size = 0;
	memcpy(&size, buffer, 2);
	SWAP_U32_FROM_LITTLE(size);
	if (size != 1) { // PCM == 1
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed because audio format was not PCM" << endl;
		return false;
	}

	// Get the number of channels (only mono and stereo supported) -- 2 bytes
	_file_input.read(buffer, 2);
	memcpy(&_number_channels, buffer, 2);
	SWAP_U16_FROM_LITTLE(_number_channels);
	if (_number_channels != 1 && _number_channels != 2) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed because number of channels was neither mono nor stereo" << endl;
		return false;
	}

	// Get sample rate (usually 11025, 22050, or 44100 Hz) -- 4 bytes
	_file_input.read(buffer, 4);
	memcpy(&_samples_per_second, buffer, 4);
	SWAP_U32_FROM_LITTLE(_samples_per_second);

	// Get byte rate -- 4 bytes
	uint32 byte_rate;
	_file_input.read(buffer, 4);
	memcpy(&byte_rate, buffer, 4);
	SWAP_U32_FROM_LITTLE(byte_rate);

	// Get block alignment (channels * bits_per_sample / 8) -- 2 bytes
	_file_input.read(buffer, 2);
	memcpy(&_sample_size, buffer, 2);
	SWAP_U16_FROM_LITTLE(_sample_size);

	// Get bits per sample -- 2 bytes
	_file_input.read(buffer, 2);
	memcpy(&_bits_per_sample, buffer, 2);
	SWAP_U16_FROM_LITTLE(_bits_per_sample);
	if (_sample_size != (_number_channels * _bits_per_sample)/8) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed because WAV file was internally inconsistent (block alignment should have been " << ((_number_channels * _bits_per_sample) / 8) << ", was " << _sample_size << ")" << endl;
		return false;
	}

	// Check subchunk 2 ID (to be "data") -- 4 bytes
	_file_input.read(buffer, 4);
	if (buffer[0] != 'd' || buffer[1] != 'a' || buffer[2] != 't' || buffer[3] != 'a') {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed because subchunk 2 ID was not \"data\"" << endl;
		return false;
	}

	// Check subchunk 2 size -- 4 bytes
	_file_input.read(buffer, 4);
	memcpy(&_data_size, buffer, 4);
	SWAP_U32_FROM_LITTLE(_data_size);

	_data_init = _file_input.tellg();
	_total_number_samples = _data_size / _sample_size;
	_play_time = static_cast<float>(_total_number_samples) / static_cast<float>(_samples_per_second);
	return true;
} // bool WavFile::Initialize()



void WavFile::Seek(uint32 sample_position) {
	uint32 sample = sample_position * _sample_size;

	if (sample >= _data_size) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed because desired seek position exceeded the range of samples: " << sample << endl;
		return;
	}

	_file_input.clear();
	_file_input.seekg(static_cast<std::streamoff>(sample + _data_init), std::ios_base::beg);
}



uint32 WavFile::Read(uint8* buffer, uint32 size, bool& end) {
	_file_input.read(reinterpret_cast<char*>(buffer), size * _sample_size);

	uint32 read = _file_input.gcount() / _sample_size;
	end = (read != size);

#ifdef __BIG_ENDIAN__
	if (_bits_per_sample == 16)
	{
		uint8 tmp;
		// iterate through
		for (uint32 i = 0; i < read * _sample_size; i += 2)
		{
			tmp = buffer[i];
			buffer[i] = buffer[i + 1];
			buffer[i + 1] = tmp;
		}
	}
#endif

	return read;
}

////////////////////////////////////////////////////////////////////////////////
// OggFile class methods
////////////////////////////////////////////////////////////////////////////////

bool OggFile::Initialize() {
	// Windows requires a special loading method in order load ogg files
	// properly when dynamically linking vorbis libs. The workaround is
	// to use the ov_open_callbacks function
	#ifdef WIN32
		// Callbacks struct defining the open, closing, seeking and location behaviors.
		ov_callbacks callbacks =  {
			(size_t (*)(void*, size_t, size_t, void*)) fread,
			(int (*)(void*, ogg_int64_t, int)) _FileSeekWrapper,
			(int (*)(void*)) fclose,
			(long (*)(void*)) ftell
		};

		FILE* file = fopen(_filename.c_str(), "rb");

		if (ov_open_callbacks(file, &_vorbis_file, NULL, 0, callbacks) < 0) {
			fclose(file);
			IF_PRINT_WARNING(AUDIO_DEBUG) << "input file does not appear to be an Ogg bitstream: " << _filename << endl;
			return false;
		}

	#else
		// File loading code for non Win32 platforms.  Much simpler.
		FILE* file = fopen (_filename.c_str(), "rb");

		if (ov_open(file, &_vorbis_file, NULL, 0) < 0) {
			fclose(file);
			IF_PRINT_WARNING(AUDIO_DEBUG) << "input file does not appear to be an Ogg bitstream: " << _filename << endl;
			return false;
		}
	#endif

	_number_channels = _vorbis_file.vi->channels;
	_samples_per_second = _vorbis_file.vi->rate;
	_bits_per_sample = 16;
	_total_number_samples = static_cast<uint32>(ov_pcm_total(&_vorbis_file, -1));
	_play_time = static_cast<float>(ov_time_total(&_vorbis_file, -1));
	_sample_size = _number_channels * _bits_per_sample / 8;
	_data_size = _total_number_samples * _sample_size;

	return true;
} // bool OggFile::Initialize()



void OggFile::Seek(uint32 cursor) {
	if (ov_seekable(&_vorbis_file) == 0) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed because Ogg file was not seekable: " << _filename << endl;
		return;
	}

	ov_pcm_seek(&_vorbis_file, cursor);

	// Reset the temporary buffer by setting the position to 0
	_read_buffer_position = 0;
	_read_buffer_size = 0;
}



uint32 OggFile::Read(uint8* buffer, uint32 size, bool& end) {
	int current_section;
	uint32 read =0;
	end = false;

	// First get data from the temporary buffer if it holds any
	if (_read_buffer_size > 0) {
		if (_read_buffer_size > size*_sample_size) {
			read = size*_sample_size;
		}
		else {
			read = _read_buffer_size;
		}
		memcpy(buffer, _read_buffer + _read_buffer_position, read);
		_read_buffer_size -= read;
		_read_buffer_position += read;
	}

	while ((read < (size * _sample_size)) && !end) {
		_read_buffer_position = 0;
		int32 num_bytes_read = 0;

		#ifdef __BIG_ENDIAN__
			num_bytes_read = ov_read(&_vorbis_file, (char*)_read_buffer, 4096, 1, _bits_per_sample/8, 1, &current_section);
		#else
			num_bytes_read = ov_read(&_vorbis_file, (char*)_read_buffer, 4096, 0, _bits_per_sample/8, 1, &current_section);
		#endif

		if (num_bytes_read == 0) { // Indicates EOF was reached
			end = true;
			break;
		}
		else if (num_bytes_read < 0) { // Indicates a read error occurred
			if (num_bytes_read == OV_HOLE) {
				IF_PRINT_WARNING(AUDIO_DEBUG) << "ov_read failed with return code: OV_HOLE" << endl;
			}
			else if (num_bytes_read == OV_EBADLINK) {
				IF_PRINT_WARNING(AUDIO_DEBUG) << "ov_read failed with return code: OV_EBADLINK" << endl;
			}
			else {
				IF_PRINT_WARNING(AUDIO_DEBUG) << "ov_read failed with return code: " << num_bytes_read << endl;
			}
			break;
		}
		else {
			//! \todo Take into account differences of sample rate when reading OGG
			_read_buffer_size = num_bytes_read;
			num_bytes_read = ((size * _sample_size) - read > static_cast<uint32>(num_bytes_read)) ? num_bytes_read : (size * _sample_size) - read;
			memcpy(buffer + read, _read_buffer + _read_buffer_position, num_bytes_read);
			read += num_bytes_read;
			_read_buffer_size -= num_bytes_read;
			_read_buffer_position += num_bytes_read;
		}
	}

	return read / _sample_size;
} // uint32 OggFile::Read(uint8* buffer, uint32 size, bool& end)



int OggFile::_FileSeekWrapper(FILE* file, ogg_int64_t off, int whence) {
	if (file == NULL) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "file pointer was NULL in argument list" << endl;
		return -1;
	}
	else {
		return fseek(file, static_cast<long>(off), whence);
	}
}

////////////////////////////////////////////////////////////////////////////////
// AudioMemory class methods
////////////////////////////////////////////////////////////////////////////////


AudioMemory::AudioMemory(AudioInput* input) :
	AudioInput(),
	_audio_data(NULL),
	_data_position(0)
{
	_filename = input->GetFilename();
	_samples_per_second = input->GetSamplesPerSecond();
	_bits_per_sample = input->GetBitsPerSample();
	_number_channels = static_cast<uint8>(input->GetNumberChannels());
	_total_number_samples = input->GetTotalNumberSamples();
	_sample_size = input->GetSampleSize();
	_play_time = input->GetPlayTime();
	_data_size = input->GetDataSize();

	_audio_data = new uint8[input->GetDataSize()];
	bool all_data_read = false;
	input->Read(_audio_data, input->GetTotalNumberSamples(), all_data_read);
	if (all_data_read == false) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "failed to read entire audio data stream for file: " << _filename << endl;
	}
}

AudioMemory::AudioMemory(const AudioMemory& audio_memory) :
	AudioInput(),
	_audio_data(NULL),
	_data_position(0)
{
	_filename = audio_memory._filename;
	_samples_per_second = audio_memory.GetSamplesPerSecond();
	_bits_per_sample = audio_memory.GetBitsPerSample();
	_number_channels = static_cast<uint8>(audio_memory.GetNumberChannels());
	_total_number_samples = audio_memory.GetTotalNumberSamples();
	_sample_size = audio_memory.GetSampleSize();
	_play_time = audio_memory.GetPlayTime();
	_data_size = audio_memory.GetDataSize();

	_audio_data = new uint8[_data_size];
	memcpy(_audio_data, audio_memory._audio_data, _data_size);
}

AudioMemory& AudioMemory::operator=(const AudioMemory& audio_memory)
{
	if (_audio_data != NULL) {
		delete[] _audio_data;
		_audio_data = NULL;
	}
	_filename = audio_memory._filename;
	_samples_per_second = audio_memory.GetSamplesPerSecond();
	_bits_per_sample = audio_memory.GetBitsPerSample();
	_number_channels = static_cast<uint8>(audio_memory.GetNumberChannels());
	_total_number_samples = audio_memory.GetTotalNumberSamples();
	_sample_size = audio_memory.GetSampleSize();
	_play_time = audio_memory.GetPlayTime();
	_data_size = audio_memory.GetDataSize();

	_data_position = audio_memory._data_position;
	_audio_data = new uint8[_data_size];
	memcpy(_audio_data, audio_memory._audio_data, _data_size);

	return *this;
}


AudioMemory::~AudioMemory() {
	if (_audio_data != NULL) {
		delete[] _audio_data;
		_audio_data = NULL;
	}
}



void AudioMemory::Seek(uint32 sample_position) {
	if (_data_position >= _total_number_samples) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "attempted to seek postion beyond the maximum number of samples: "
			<< sample_position << endl;
		return;
	}

	_data_position = sample_position;
}



uint32 AudioMemory::Read(uint8* buffer, uint32 size, bool& end) {
	// Clamp the number of samples to read in case there are not enough because of end of stream
	uint32 read = (_total_number_samples - _data_position >= size) ? size : (_total_number_samples - _data_position);

	// Copy the data in the buffer and move the read cursor
	memcpy(buffer, buffer + _data_position * _sample_size, read * _sample_size);
	_data_position += read;
	end = (_data_position == _total_number_samples);

	return read;
}

} // namespace private_audio

} // namespace hoa_audio
