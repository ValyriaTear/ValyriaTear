////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_EMOTES_HEADER__
#define __GLOBAL_EMOTES_HEADER__

#include "modes/map/map_utils.h"

#include "engine/video/image.h"

#include <string>
#include <map>

namespace vt_global
{

class EmoteHandler
{
public:
    EmoteHandler() {}
    ~EmoteHandler() {}

    //! \brief Clear emotes data
    void Clear();

    //! \brief loads the emotes used for character feelings expression in the given lua file.
    void LoadEmotes(const std::string& emotes_filename);

    //! \brief Set up the offsets for the given emote animation and sprite direction.
    void GetEmoteOffset(float& x, float& y,
                        const std::string& emote_id,
                        vt_map::private_map::ANIM_DIRECTIONS dir);

    //! \brief Tells whether an emote id exists and is valid
    bool DoesEmoteExist(const std::string& emote_id);

    //! \brief Get a pointer reference to the given emote animation. Don't delete it!
    vt_video::AnimatedImage* GetEmoteAnimation(const std::string& emote_id);

private:
    //! \brief A map containing all the emote animations
    std::map<std::string, vt_video::AnimatedImage> _emotes;
    //! \brief The map continaing the four sprite direction offsets (x and y value).
    std::map<std::string, std::vector<std::pair<float, float> > > _emotes_offsets;
};

} // namespace vt_global

#endif // __GLOBAL_EMOTES_HEADER__
