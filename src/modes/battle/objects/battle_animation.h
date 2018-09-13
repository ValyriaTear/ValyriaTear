////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_ANIMATION_HEADER__
#define __BATTLE_ANIMATION_HEADER__

#include "battle_object.h"

#include "engine/video/image.h"

namespace vt_battle
{

namespace private_battle
{

//! \brief A class representing animated images used as battle objects:
//! used also for spell effects, attack effects, ...
class BattleAnimation : public BattleObject
{
public:
    BattleAnimation(const std::string& animation_filename);

    //! Used to be drawn at the right time by the battle mode.
    void DrawSprite();


    void Update() {
        _animation.Update();
    }

    //! Permits to restart the animation.
    void Reset() {
        _animation.ResetAnimation();
    }

    void SetVisible(bool show) {
        _visible = show;
    }

    bool IsVisible() const {
        return _visible;
    }

    //! Tells whether the effect can be scheduled for removal from memory.
    bool CanBeRemoved() const {
        return _can_be_removed;
    }

    void Remove() {
        _can_be_removed = true;
    }

    //! Get the animatedImage for deeper manipulations.
    vt_video::AnimatedImage& GetAnimatedImage() {
        return _animation;
    }

protected:
    //! The particle effect class used internally
    vt_video::AnimatedImage _animation;

    //! Set whether the animation is drawn.
    bool _visible;

    //! Set whether the animation can be removed from memory (now useless).
    bool _can_be_removed;
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_ANIMATION_HEADER__
