///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_WORLDMAP_WINDOW__
#define __MENU_WORLDMAP_WINDOW__

#include "common/global/global.h"
#include "common/gui/textbox.h"

#include "common/gui/menu_window.h"
#include "common/gui/option.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

/**
*** \brief handles showing the currently set world map
*** upon selection, based on the key press we cycle thru locations that are
*** set as "revealed" on the map
***
*** \note WorldMap has no left window. This means that the entire screen rendering takes place here
*** based on the Wold Map selection here, we update the WorldMapState such that for the
*** bottom window render, we have all the information needed to show
***
**/
class WorldMapWindow : public vt_gui::MenuWindow
{
    friend class vt_menu::MenuMode;
    friend class WorldMapState;

    enum WORLDMAP_NAVIGATION {
        WORLDMAP_NOPRESS,   //no key press.
        WORLDMAP_CANCEL,   //a cancel press to exit from viewing the window
        WORLDMAP_LEFT,      //a left press to move "up" the list of locations
        WORLDMAP_RIGHT      //a right press to move "down" the list of locations
    };
public:
    WorldMapWindow();

    virtual ~WorldMapWindow() override
    {
        _location_marker.Clear();
        _location_pointer.Clear();
    }

    /*!
    * \brief Draws window
    * \return success/failure
    */
    void Draw();

    //! \brief Performs updates
    void Update();

     /*!
    * \brief Result of whether or not this window is active
    * \return true if this window is active
    */
    bool IsActive()
    {
        return _active;
    }

    /*!
    * \brief switch the active state of this window, and do any associated work
    * \param activate or deactivate
    */
    void Activate(bool new_state);

    /*!
    * \brief gets the WorldMapLocation pointer to the currently pointing
    * location, or nullptr if it deson't exist
    * \return Pointer to the currently indexes WorldMapLocation
    */
    vt_global::WorldMapLocation *GetCurrentViewingLocation()
    {
        const std::vector<std::string> &current_location_ids = vt_global::GlobalManager->GetViewableLocationIds();
        const uint32_t N = current_location_ids.size();
        if( N == 0 || _location_pointer_index > N)
            return nullptr;
        return vt_global::GlobalManager->GetWorldLocation(current_location_ids[_location_pointer_index]);
    }

private:

    //! \brief based on the worldmap selection, sets the pointer on the
    //! current map
    void _SetSelectedLocation(WORLDMAP_NAVIGATION worldmap_goto);

    //! \brief draws the locations and the pointer based on
    //! the currently active location ids and what we have selected
    //! \param window_position_x The X position of the window
    //! \param window_position_y The Y position of the window
    void _DrawViewableLocations(float window_position_x,
                                float window_position_y);

    //! \brief pointer to the currently loaded world map image
    vt_video::StillImage *_current_world_map;

    //! \brief the location marker. this is loaded in the ctor
    vt_video::AnimatedImage _location_marker;

    //! \brief the location pointer. this is loaded in the ctor
    vt_video::StillImage _location_pointer;

    //! \brief offsets for the current image to view in the center of the window
    float _current_image_x_offset;
    float _current_image_y_offset;

    //! \brief the current index to the location the pointer should be on
    uint32_t _location_pointer_index;

    //! \brief indicates whether this window is active or not
    bool _active;

};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_WORLDMAP_WINDOW__
