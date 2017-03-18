////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
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
*** \author Yohann Ferreira, yohann ferreira orange fr
*** \brief  Implementation of the streaming audio classes
***
*** This code implements the functionality for more advanced streaming. Streaming
*** enables support for features such as customized looping.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "audio_stream.h"

#include "audio_input.h"

namespace vt_audio
{

extern bool AUDIO_DEBUG;

namespace private_audio
{

AudioStream::AudioStream(AudioInput *input, bool loop) :
    _audio_input(input),
    _looping(loop),
    _loop_start_position(0),
    _loop_end_position(0),
    _read_position(0),
    _end_of_stream(false)
{
    if(_audio_input == nullptr) {
        PRINT_ERROR << "input argument was nullptr -- terminating program" << std::endl;
        exit(1);
    }

    // Loop end is initially set to the final sample
    _loop_end_position = _audio_input->GetTotalNumberSamples();
}



uint32_t AudioStream::FillBuffer(uint8_t *buffer, uint32_t size)
{
    uint32_t num_samples_read = 0; // The number of samples which have been read

    while(num_samples_read < size) {
        // If looping is enabled and the end of the stream has been reached, seek to the starting position
        if(_looping && (_read_position == _loop_end_position || _read_position == _audio_input->GetTotalNumberSamples())) {
            _audio_input->Seek(_loop_start_position);
            _read_position = _loop_start_position;
        }

        // Determine the number of samples we should request for the input to read
        uint32_t remaining_data = (_looping) ? _loop_end_position : _audio_input->GetTotalNumberSamples();
        remaining_data -= _read_position;

        // The number of samples to request the audio input to read
        uint32_t read_samples = (size - num_samples_read < remaining_data) ? size - num_samples_read : remaining_data;
        num_samples_read += _audio_input->Read(buffer + num_samples_read * _audio_input->GetSampleSize(),
                                               read_samples, _end_of_stream);
        _read_position += num_samples_read;

        // Detect early exit condition
        if(_looping == false && _end_of_stream) {
            return num_samples_read;
        }
    }

    return num_samples_read;
}



void AudioStream::Seek(uint32_t sample)
{
    if(sample >= _audio_input->GetTotalNumberSamples()) {
        IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to seek to position beyond sample range: " << sample << std::endl;
    }
    _audio_input->Seek(sample);
    _read_position = sample;
    _end_of_stream = false;
}



void AudioStream::SetLoopStart(uint32_t sample)
{
    if(sample >= _audio_input->GetTotalNumberSamples()) {
        IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to set loop start point beyond sample range: " << sample << std::endl;
        return;
    }

    _loop_start_position = sample;
}



void AudioStream::SetLoopEnd(uint32_t sample)
{
    if(sample >= _audio_input->GetTotalNumberSamples()) {
        IF_PRINT_WARNING(AUDIO_DEBUG) << "tried to set loop end point beyond sample range: " << sample << std::endl;
        return;
    }

    _loop_end_position = sample;
}

} // namespace private_audio

} // namespace vt_audio
