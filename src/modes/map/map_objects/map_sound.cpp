///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_objects/map_sound.h"

#include "modes/map/map_mode.h"
#include "modes/map/map_object_supervisor.h"

#include "engine/audio/audio.h"

using namespace vt_common;

namespace vt_map
{

namespace private_map
{

SoundObject::SoundObject(const std::string& sound_filename, float x, float y, float strength):
    MapObject(NO_LAYER_OBJECT), // This is a special object
    _strength(strength),
    _sound_volume(0.0f),
    _max_sound_volume(1.0f),
    _time_remaining(0.0f),
    _activated(true),
    _playing(false)
{
    _object_type = SOUND_TYPE;

    // We use the AudioManager to mutualize the sound descriptors instances.
    bool loaded = true;
    loaded = vt_audio::AudioManager->LoadSound(sound_filename, MapMode::CurrentInstance());
    _sound = vt_audio::AudioManager->RetrieveSound(sound_filename);
    if (!loaded || _sound == nullptr) {
        PRINT_WARNING << "Couldn't load environmental sound file: "
            << sound_filename << std::endl;
    }

    // Invalidates negative or near 0 values.
    if (_strength <= 0.2f)
        _strength = 0.0f;

    _tile_position.x = x;
    _tile_position.y = y;

    _collision_mask = NO_COLLISION;

    if (_sound) {
        _sound->SetLooping(true);
        _sound->SetVolume(0.0f);
        _sound->Stop();
    }

    // Register the object to the sound vector
    MapMode::CurrentInstance()->GetObjectSupervisor()->AddAmbientSound(this);
}

SoundObject* SoundObject::Create(const std::string& sound_filename,
                                 float x, float y, float strength)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new SoundObject(sound_filename, x, y, strength);
}

void SoundObject::SetMaxVolume(float max_volume)
{
    _max_sound_volume = max_volume;

    if (_max_sound_volume < 0.0f)
        _max_sound_volume = 0.0f;
    else if (_max_sound_volume > 1.0f)
        _max_sound_volume = 1.0f;
}

void SoundObject::UpdateVolume()
{
    // Don't activate a sound which is too weak to be heard anyway.
    if (_strength < 1.0f || _max_sound_volume <= 0.0f) {
        _sound_volume = 0.0f;
        return;
    }

    if (!_activated) {
        _sound_volume = 0.0f;
        return;
    }

    // Update the volume only every 100ms
    _time_remaining -= static_cast<int32_t>(vt_system::SystemManager->GetUpdateTime());
    if (_time_remaining > 0)
        return;
    _time_remaining = 100;

    // N.B.: The distance between two point formula is:
    // squareroot((x2 - x1)^2+(y2 - y1)^2)
    MapMode *mm = MapMode::CurrentInstance();
    if(!mm)
        return;

    const MapFrame& frame = mm->GetMapFrame();

    Position2D center;
    center.x = frame.screen_edges.left + (frame.screen_edges.right - frame.screen_edges.left) / 2.0f;
    center.y = frame.screen_edges.top + (frame.screen_edges.bottom - frame.screen_edges.top) / 2.0f;

    float distance = _tile_position.GetDistance2(center);
    //distance = sqrtf(_distance); <-- We don't actually need it as it is slow.

    float strength2 = _strength * _strength;

    if (distance >= strength2) {
        if (_playing) {
            _sound_volume = 0.0f;
            _playing = false;
        }
        return;
    }

    // We add a one-half-tile neutral margin where nothing happens
    // to avoid the edge case where the sound repeatedly starts/stops
    // because of the camera position rounding.
    if (distance >= (strength2 - 0.5f))
        return;

    _sound_volume = _max_sound_volume - (_max_sound_volume * (distance / strength2));
    _playing = true;
}

void SoundObject::ApplyVolume()
{
    if (!_sound)
        return;

    // Stop sound if needed.
    if (_sound_volume <= 0.1f) {
        if (_sound->GetState() == vt_audio::AUDIO_STATE_PLAYING
               || _sound->GetState() == vt_audio::AUDIO_STATE_FADE_IN) {
           _sound->FadeOut(1000.0f);
        }
        return;
    }

    if (_sound->GetState() != vt_audio::AUDIO_STATE_PLAYING
            && _sound->GetState() != vt_audio::AUDIO_STATE_FADE_IN) {
        _sound->FadeIn(1000.0f);
    }
    _sound->SetVolume(_sound_volume);
}

void SoundObject::Stop()
{
    if (!_activated)
        return;

    if (_sound)
        _sound->FadeOut(1000.0f);
    _activated = false;
}

void SoundObject::Start()
{
    if (_activated)
        return;

    _activated = true;

    // Restores the sound state
    UpdateVolume();
}

} // namespace private_map

} // namespace vt_map
