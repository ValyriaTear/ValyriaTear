///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \brief   Header file for map mode escape points handler.
*** *****************************************************************************/

#ifndef __MAP_ESCAPE_HEADER__
#define __MAP_ESCAPE_HEADER__

#include "map_location.h"

#include "common/gui/menu_window.h"
#include "common/gui/option.h"
#include "common/gui/textbox.h"

namespace vt_map
{

namespace private_map
{

/** ****************************************************************************
*** \brief A supervisor for the UI linked to escape circles.
***
*** Escape circles permits to go back to a safe location instantly.
*** ***************************************************************************/

class EscapeSupervisor
{
public:
     EscapeSupervisor();
    ~EscapeSupervisor();

    /** \brief Initializes the escape supervisor and shows the escape window.
    *** \param map_coordinates The coordinates where to make the characters escape.
    *** Can be invalid.
    **/
    void Initialize(const vt_map::MapLocation& map_location);

    //! \brief Processes input events from the user and updates the showing/hiding progress of the window
    void Update();

    /** \brief Draws the window to the screen
    *** \note If the Initialize method has not been called with a valid treasure pointer beforehand, this
    *** method will print a warning and it will not draw anything to the screen.
    **/
    void Draw();

    //! \brief Hides the window and cancel the action.
    void Cancel();

    //! \brief Hides the window and potentially make the characters escape
    void Finish();

private:
    //! \brief Map coordinates where to make the characters escape, if the cooirdinates are valid.
    vt_map::MapLocation _map_location;

    //! \brief Lists all of the drunes and inventory objects contained in the treasure
    vt_gui::MenuWindow _escape_window;

    //! \brief The available actions that a user can currently take.
    vt_gui::OptionBox _action_options;

    //! \brief A rendering of the name for the escape window
    vt_video::TextImage _window_title;

    //! \brief The TextImages used to render the map hud names and subnames
    vt_video::TextImage _map_hud_name;
    vt_video::TextImage _map_hud_subname;

    //! \brief Freestyle art image of the current map
    vt_video::StillImage _location_image;

    //! \brief Stores whether the location is valid
    bool _location_valid;

    // ---------- Private methods

    //! \brief Loads the current map location preview
    bool _LoadMapLocationPreview();

}; // class EscapeSupervisor

} // namespace private_map

} // namespace vt_map

#endif // __MAP_ESCAPE_HEADER__
