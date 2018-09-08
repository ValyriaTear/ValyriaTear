////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "modes/battle/battle_objects/battle_animation.h"

#include "utils/utils_common.h"
#include "engine/video/video.h"

using namespace vt_video;

namespace vt_battle
{

namespace private_battle
{

BattleAnimation::BattleAnimation(const std::string& animation_filename):
    BattleObject(),
    _visible(true),
    _can_be_removed(false)
{
    if(!_animation.LoadFromAnimationScript(animation_filename))
        PRINT_WARNING << "Invalid battle animation file requested: "
                      << animation_filename << std::endl;
}

void BattleAnimation::DrawSprite()
{
    if(!IsVisible() || CanBeRemoved())
        return;

    VideoManager->Move(GetXLocation(), GetYLocation());
    _animation.Draw();
}

} // namespace private_battle

} // namespace vt_battle
