////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file   audio_stream.cpp
*** \author Mois�s Ferrer Serra, byaku@allacrost.org
*** \author Tyler Olsen, roots@allacrost.org
*** \brief  Implementation of the streaming audio classes
***
*** This code implements the functionality for more advanced streaming. Streaming
*** enables support for features such as customized looping.
*** ***************************************************************************/

#include "audio_stream.h"
#include "audio_input.h"

using namespace std;

namespace hoa_audio {

namespace private_audio {

AudioStream::AudioStream(AudioInput* input, bool loop) :
	_audio_input(input),
	_looping(loop),
	_loop_start_position(0),
	_loop_end_position(0),
	_read_position(0),
	_end_of_stream(false)
{
	if (_audio_input == NULL) {
		PRINT_ERROR << "input argument was NULL -- terminating program" << endl;
		exit(1);
	}

	// Loop end is initially set to the final sample
	_loop_end_position = _audio_input->GetTotalNumberSamples();
}



uint32 AudioStream::FillBuffer(uint8* buffer, uint32 size) {
	uint32 num_samples_read = 0; // The number of samples which have been read
	uint32 read_samples; // The number of samples to request the audio input to read

	while (num_samples_read < size) {
		// If looping is enabled and the end of the stream has been reached, seek to the starting position
		if (_looping == true && (_read_position == _loop_end_position || _read_position == _audio_input->GetTotalNumberSamples())) {
			_audio_input->Seek(_loop_start_position);
			_read_position = _loop_start_position;
		}

		// Determine the number of samples we should request for the input to read
		uint32 remaining_data = (_looping == true) ? _loop_end_position : _audio_input->GetTotalNumberSamples();
		remaining_data -= _read_position;
		read_samples = (size - num_samples_read < remaining_data) ? size - num_samples_read : remaining_data;
		num_samples_read += _audio_input->Read(buffer + num_samples_read * _audio_input->GetSampleSize(), read_samples, _end_of_stream);
		_read_position += num_samples_read;
		
		// Detect early exit condition
		if (_looping == false && _end_of_stream == true) {
			return num_samples_read;
		}
	}

	return num_samples_read;
}



void AudioStream::Seek(uint32 sample) {
	if (sample >= _audio_input->GetTotalNumberSamples()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to seek to position beyond sample range: " << sample << endl;
	}
	_audio_input->Seek(sample);
	_read_position = sample;
	_end_of_stream = false;
}




void AudioStream::SetLoopStart(uint32 sample) {
	if (sample >= _audio_input->GetTotalNumberSamples()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to set loop start point beyond sample range: " << sample << endl;
		return;
	}

	_loop_start_position = sample;
}



void AudioStream::SetLoopEnd(uint32 sample) {
	if (sample >= _audio_input->GetTotalNumberSamples()) {
		IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to set loop end point beyond sample range: " << sample << endl;
		return;
	}

	_loop_end_position = sample;
}

} // namespace private_audio

} // namespace hoa_audio
