////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file   audio_stream.h
*** \author Mois�s Ferrer Serra, byaku@allacrost.org
*** \author Tyler Olsen, roots@allacrost.org
*** \brief  Header file for class for streaming audio from diferent sources
***
*** This code implements functionality for advanced streaming operations
*** ***************************************************************************/

#ifndef __AUDIO_STREAM_HEADER__
#define __AUDIO_STREAM_HEADER__

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

namespace hoa_audio {

namespace private_audio {

/** ****************************************************************************
*** \brief Handles streaming audio from input data sources
***
*** This class operates on a AudioInput object to stream the audio data to the
*** listener. The primary purpose of streaming is to support customized looping,
*** where specific parts of a piece of audio can be looped rather than the
*** entire audio itself.
***
*** \note The _end_of_stream will never be set to true while the stream has
*** looping enabled.
***
*** \todo Customized looping support is only very rudimentary right now (one
*** start, one end position). We need full support added to this class to be
*** able to do operations such as: play section A once, loop section B 3 times,
*** then loop section C infinitely. We may also want to be able to apply audio
*** effects to the different streaming portions.
*** ***************************************************************************/
class AudioStream {
public:
	/** \brief Class constructor which initializes the audio stream
	*** \param input A pointer to the AudioInput object which will manage the input data
	*** \param loop If true, enables looping for the audio stream
	**/
	AudioStream (AudioInput* input, bool loop);

	~AudioStream()
		{}

	/** \brief Fills a buffer with data read from the stream
	*** \param buffer A pointer to the buffer where the data will be loaded to
	*** \param size The total number of samples to read
	*** \return The number of samples which were read, which may be different from size
	**/
	uint32 FillBuffer(uint8* buffer, uint32 size);

	/** \brief Seeks the audio stream to the specified sample
	*** \param sample The sample number to seek the stream to
	*** \note This will also automatically invoke the Seek method on the AudioInput object
	**/
	void Seek(uint32 sample);

	//! \brief Returns true if the audio stream is looping
	bool IsLooping() const
		{ return _looping; }

	/** \brief Enables/disables looping for this stream
	*** \param loop True to enable looping, false to disable it.
	**/
	void SetLooping(bool loop)
		{ _looping = loop; if (loop) _end_of_stream = false; }

	/** \brief Sets the sample to serve as the start position for looping
	*** \param sample The sample number to be the new starting position
	**/
	void SetLoopStart(uint32 sample);

	/** \brief Sets the sample to serve as the end position for looping
	*** \param sample The sample number to be the new ending position
	**/
	void SetLoopEnd(uint32 sample);

	//! \brief Returns true if the stream has finished playing
	bool GetEndOfStream() const
		{ return _end_of_stream; }

private:
	//! \brief Pointer to an AudioInput object that holds the audio data
	AudioInput* _audio_input;

	//! \brief Set to true when the stream should loop
	bool _looping;

	//! \brief The sample that represents the start position of the loop
	uint32 _loop_start_position;

	//! \brief The sample that represents the end position of the loop
	uint32 _loop_end_position;

	//! \brief The sample position from where the next read operation will be performed
	uint32 _read_position;

	//! \brief True if the end of the stream was reached, false otherwise
	bool _end_of_stream;
}; // class AudioStream

} // namespace private_audio

} // namespace hoa_audio

#endif // __AUDIO_STREAM_HEADER__
