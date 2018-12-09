////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "emote_handler.h"

#include "script/script_read.h"

using namespace vt_video;

namespace vt_global
{

void EmoteHandler::LoadEmotes(const std::string& emotes_filename)
{
    // First, clear the list in case of reloading
    _emotes.clear();

    vt_script::ReadScriptDescriptor emotes_script;
    if(!emotes_script.OpenFile(emotes_filename))
        return;

    if(!emotes_script.DoesTableExist("emotes")) {
        emotes_script.CloseFile();
        return;
    }

    std::vector<std::string> emotes_id;
    emotes_script.ReadTableKeys("emotes", emotes_id);

    // Read all the values
    emotes_script.OpenTable("emotes");
    for(uint32_t i = 0; i < emotes_id.size(); ++i) {

        if(!emotes_script.DoesTableExist(emotes_id[i]))
            continue;
        emotes_script.OpenTable(emotes_id[i]);

        std::string animation_file = emotes_script.ReadString("animation");

        AnimatedImage anim;
        if(anim.LoadFromAnimationScript(animation_file)) {
            // NOTE: The map mode should one day be fixed to use the same coords
            // than everything else, thus making possible to remove this
            vt_map::private_map::ScaleToMapZoomRatio(anim);

            _emotes.insert(std::make_pair(emotes_id[i], anim));

            // The vector containing the offsets
            std::vector<std::pair<float, float> > emote_offsets;
            emote_offsets.resize(vt_map::private_map::NUM_ANIM_DIRECTIONS);

            // For each directions
            for(uint32_t j = 0; j < vt_map::private_map::NUM_ANIM_DIRECTIONS; ++j) {
                emotes_script.OpenTable(j);

                std::pair<float, float> offsets;
                offsets.first = emotes_script.ReadFloat("x");
                offsets.second = emotes_script.ReadFloat("y");

                emote_offsets[j] = offsets;

                emotes_script.CloseTable(); // direction table.
            }

            _emotes_offsets.insert(std::make_pair(emotes_id[i], emote_offsets));
        }

        emotes_script.CloseTable(); // emote_id[i]
    }
    emotes_script.CloseAllTables();
    emotes_script.CloseFile();
}

void EmoteHandler::GetEmoteOffset(float& x, float& y,
                                  const std::string& emote_id,
                                  vt_map::private_map::ANIM_DIRECTIONS dir)
{
    x = 0.0f;
    y = 0.0f;

    if(dir < vt_map::private_map::ANIM_SOUTH || dir >= vt_map::private_map::NUM_ANIM_DIRECTIONS)
        return;

    std::map<std::string, std::vector<std::pair<float, float> > >::const_iterator it =
        _emotes_offsets.find(emote_id);

    if(it == _emotes_offsets.end())
        return;

    x = it->second[dir].first;
    y = it->second[dir].second;
}

bool EmoteHandler::DoesEmoteExist(const std::string& emote_id) {
    return (_emotes.count(emote_id));
}

//! \brief Get a pointer reference to the given emote animation. Don't delete it!
vt_video::AnimatedImage* EmoteHandler::GetEmoteAnimation(const std::string& emote_id) {
    if(_emotes.find(emote_id) != _emotes.end()) return &_emotes.at(emote_id);
    else return nullptr;
}

} // namespace vt_global
