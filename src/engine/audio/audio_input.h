////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file   audio_input.h
*** \author Mois�s Ferrer Serra, byaku@allacrost.org
*** \author Aaron Smith - etherstar@allacrost.org
*** \brief  Header file for classes that provide input for sounds
***
*** This code provides classes for loading sounds (WAV and OGG). It also
*** provides the functionality for basic streaming operations, both from memory
*** and from a file.
***
*** \note This code uses the Ogg/Vorbis libraries for loading Ogg files. WAV
*** files are loaded via custom loading code
*** ***************************************************************************/

#ifndef __AUDIO_INPUT_HEADER__
#define __AUDIO_INPUT_HEADER__

#ifdef __MACH__
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
#else
	#include "al.h"
	#include "alc.h"
#endif

#include "defs.h"
#include "utils.h"

#include <vorbis/vorbisfile.h>
#include <fstream>

namespace hoa_audio {

namespace private_audio {

/** ****************************************************************************
*** \brief Abstract class for audio input objects
***
*** This class is responsible for managing the audio input, whether the input
*** source is from a file or data that is stored in memory. This class also
*** retains a series of variables that describe the input data Additionally,
*** data read and seek operations are implemented by derivatives of this class.
*** ***************************************************************************/
class AudioInput {
public:
	AudioInput();

	virtual ~AudioInput ()
		{};

	/** \brief Prepares and initializes the input stream for reading
	*** \return True if the stream was successfully opened and is ready to read
	*** This function is responsible for prepairing the stream from where the
	*** data is going to be read. For instance, if it is a file, it will be
	*** responsible of getting a file descriptor, open the file and seek the data.
	*** This function also sets many of the class members which describe the
	*** input data's properties.
	**/
	virtual bool Initialize() = 0;

	/** \brief Seeks the stream to a specific sample
	*** \param sample_position The sample position to place the read cursor
	*** If the cursor exceeds the maximum sample position, the read cursor will
	*** not change.
	**/
	virtual void Seek(uint32 sample_position) = 0;

	/** \brief Reads data up to a specified number of samples
	*** \param data_buffer The buffer to store the read audio data in
	*** \param number_samples The number of samples to read
	*** \param end A boolean reference which will be set to true if the last of the data was read
	*** \return The number of samples which were read
	*** Reads up to the specified number of samples, and stores it in a buffer. It can store from 0 to
	*** the spcified number of samples. The buffer must hold enough information for the data.
	**/
	virtual uint32 Read(uint8* data_buffer, uint32 number_samples, bool& end) = 0;

	//! \name Class member access functions
	//@{
	const std::string& GetFilename() const
		{ return _filename; }

	uint32 GetSamplesPerSecond() const
		{ return _samples_per_second; }

	uint16 GetBitsPerSample() const
		{ return _bits_per_sample; }

	uint16 GetNumberChannels() const
		{ return _number_channels; }

	uint32 GetTotalNumberSamples() const
		{ return _total_number_samples; }

	uint32 GetDataSize() const
		{ return _data_size; }

	float GetPlayTime() const
		{ return _play_time; }

	uint16 GetSampleSize() const
		{ return _sample_size; }
	//@}

protected:
	//! \brief The name of the audio file operated on by the class
	std::string _filename;

	//! \brief The number of samples per second (typically 11025, 22050, 44100)
	uint32 _samples_per_second;

	//! \brief The number of bits per sample (typically 8 or 16)
	uint16 _bits_per_sample;

	//! \brief Channels of the sound (1 = mono, 2 = stereo)
	uint16 _number_channels;

	//! \brief The total number of samples of the audio piece
	uint32 _total_number_samples;

	//! \brief The size of the audio data in bytes
	uint32 _data_size;

	//! \brief The size of an individual sample in bytes (_bits_per_sample * _channels / 8)
	uint16 _sample_size;

	//! \brief The total play ime of the audio piece in seconds (_samples / _samples_per_second)
	float _play_time;
}; // class AudioInput


/** ****************************************************************************
*** \brief Manages input extraced from .wav files
***
*** Wav files are usually used for sounds. This class implements its own custom
*** wav file parser/loader to interpret the data from the file into meaningful
*** audio data.
*** ***************************************************************************/
class WavFile : public AudioInput {
public:
	WavFile(const std::string& file_name) :
		AudioInput() { _filename = file_name; }

	~WavFile()
		{ if (_file_input) _file_input.close(); }

	//! \brief Inherited functions from AudioInput class
	//@{
	//! \todo Enable this function to handle loading of more complex WAV files
	bool Initialize();

	void Seek(uint32 sample_position);

	uint32 Read(uint8* data_buffer, uint32 number_samples, bool& end);
	//@}

private:
	//! \brief The input I/O stream for the file
	std::ifstream _file_input;

	//! \brief The offset to where the data begins in the file (past the header information)
	std::streampos _data_init;
}; // class WavFile : public AudioInput


/** ****************************************************************************
*** \brief Manages input extracted from .ogg files
***
*** Ogg files are typically used for music. The functions in this class
*** make calls to the libvorbis library in order to read and seek through
*** the audio data.
*** ***************************************************************************/
class OggFile : public AudioInput {
public:
	OggFile(const std::string& file_name) :
		AudioInput(), _read_buffer_position(0), _read_buffer_size(0) { _filename = file_name; }

	~OggFile()
		{ ov_clear(&_vorbis_file); }

	//! \brief Inherited functions from AudioInput class
	//@{
	bool Initialize();

	void Seek(uint32 sample_position);

	uint32 Read(uint8* data_buffer, uint32 number_samples, bool& end);
	//@}

private:
	//! \brief Contains information about the Vorbis Ogg file
	OggVorbis_File _vorbis_file;

	//! \brief Temporary buffer for reading data.
	unsigned char _read_buffer[4096];

	//! \brief Position of previous read data (for the emporal buffer).
	uint16 _read_buffer_position;

	//! \brief Size of available data on the buffer (for the emporal buffer).
	uint16 _read_buffer_size;

	/** \brief A wrapper function for file seek operations
	*** \param ffile A pointer to the FILE struct which represents the input stream
	*** \param off The number of bytes to offset from the stream's origin
	*** \param whence The position to read, added to the off paramater to determine the actual seek position
	*** \return The return value of the fseek function, or -1 if the file pointer was invalid
	***
	*** \note This function is required and used only by the Windows platform. It is static out
	*** of necessity (it gets used as a function pointer) and uses the normally forbidden int type
	*** since it is a wrapper to a C function which expects those types.
	*/
	static int _FileSeekWrapper(FILE* file, ogg_int64_t off, int whence);
}; // class OggFile : public AudioInput


/** ****************************************************************************
*** \brief Manages audio input data that is stored in memory
***
*** The class requires a pointer to a section of memory where the audio data is
*** stored, and then operates off of that data. This is useful for efficient
*** streaming operations so that I/O files containing the data do not need to
*** be continually accessed.
*** ***************************************************************************/
class AudioMemory : public AudioInput {
public:
	/** \brief The class must be constructed using existing audio input data
	*** \param input A pointer to the already initialized AudioInput for this class to use
	*** This constructor will allocate enough memory to hold the entire audio data and
	*** fill that memory with the audio data read from the input argument
	**/
	AudioMemory(AudioInput* input);
	
	AudioMemory(const AudioMemory& audio_memory);
	AudioMemory& operator=(const AudioMemory& other_audio_memory);

	~AudioMemory();

	//! \brief Inherited functions from AudioInput class
	//@{
	//! \note Audio memory does not need to be initialized, as that is done in the class constructor
	bool Initialize()
		{ return true; }

	void Seek(uint32 sample_position);

	uint32 Read(uint8* buffer, uint32 size, bool& end);
	//@}

private:
	//! \brief The memory location where all the audio is stored
	uint8* _audio_data;

	//! \brief Position in the data where the next read operation will be performed
	uint32 _data_position;
}; // class AudioMemory : public AudioInput

} // namespace private_audio

} // namespace hoa_audio

#endif // __AUDIO_INPUT_HEADER__
