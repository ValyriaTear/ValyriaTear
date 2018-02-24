////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_FINISH_HEADER__
#define __BATTLE_FINISH_HEADER__

namespace vt_battle
{

namespace private_battle
{

/** ****************************************************************************
*** \brief Interface class for managing victory or defeat in battles
*** ***************************************************************************/
class BattleFinish
{
public:
    virtual ~BattleFinish()
    {}

    //! \brief Un-hides the window display and creates the window contents
    virtual void Initialize() = 0;

    //! \brief Updates the state of the window
    virtual void Update() = 0;

    //! \brief Draws the window and its contents
    virtual void Draw() = 0;
}; // class BattleFinish

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_FINISH_HEADER__
