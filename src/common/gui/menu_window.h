///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    menu_window.h
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for menu window class
*** ***************************************************************************/

#ifndef __MENU_WINDOW_HEADER__
#define __MENU_WINDOW_HEADER__

#include "gui.h"
#include "engine/video/screen_rect.h"
#include "engine/video/image.h"

namespace vt_gui
{

/** \name Menu Edge Bitflags
*** \brief These flags control the presence/absence of each edge of the menu window.
*** For example, if you want to show a menu with its left edge hidden, then you would pass in all
*** the flags except VIDEO_MENU_EDGE_LEFT to the MenuWindow#Create() function, or alternatively
*** you could pass the complement of that bit flag (~VIDEO_MENU_EDGE_LEFT).
**/
//@{
const int32 VIDEO_MENU_EDGE_LEFT   = 0x1;
const int32 VIDEO_MENU_EDGE_RIGHT  = 0x2;
const int32 VIDEO_MENU_EDGE_TOP    = 0x4;
const int32 VIDEO_MENU_EDGE_BOTTOM = 0x8;
const int32 VIDEO_MENU_EDGE_ALL    = 0xF;
//@}

/** \brief These enums are used to tell the visiblity state of a menu window.
*** The possible states and their meaning include:
*** - VIDEO_MENU_STATE_SHOWN: the menu is fully shown
*** - VIDEO_MENU_STATE_HIDDEN: the menu is fully hidden
**/
enum VIDEO_MENU_STATE {
    VIDEO_MENU_STATE_INVALID = -1,
    VIDEO_MENU_STATE_SHOWN   = 0,
    VIDEO_MENU_STATE_HIDDEN  = 1,
    VIDEO_MENU_STATE_TOTAL   = 2
};


namespace private_gui
{

/** ****************************************************************************
*** \brief A container class for a menu skin, consisting of border images and an interior
***
*** The GUISystem class manages all MenuSkins which have been loaded by the user.
*** Multiple menu skins may co-exist at one time and be drawn to the screen. The MenuWindow
*** class makes extensive use of MenuSkins.
***
*** \note The contructor and destructor of this class do nothing. The creation and destruction
*** of the images contained by this class is done from the GUI class.
***
*** \todo Add support for menu skin backgrounds to be tiled images versus
*** stretched images.
***
*** \todo Add support to allow menu skin backgrounds to be colors instead of
*** images.
*** ***************************************************************************/
class MenuSkin
{
public:
    MenuSkin()
    {}

    ~MenuSkin()
    {}

    //! \brief The translated skin name
    vt_utils::ustring skin_name;

    //! image which is to be used as the cursor
    vt_video::StillImage menu_cursor;

    /** \brief A 2d array that holds the border images for the menu skin
    *** The entries in this array represent the following parts:
    *** - borders[0][0]: upper left corner
    *** - borders[0][1]: top side
    *** - borders[0][2]: upper right corner
    *** - borders[1][0]: left side
    *** - borders[1][1]: center (this is not an actual image, but rather contains the background colors for the four corners)
    *** - borders[1][2]: right side
    *** - borders[2][0]: bottom left corner
    *** - borders[2][1]: bottom side
    *** - borders[2][2]: bottom right corner
    **/
    vt_video::StillImage borders[3][3];

    /** \brief Border-connecting images, used when two or more MenuWindows are side by side.
    ***  There are four tri-connectors and one quad-connector. tri_t would be an image for
    ***  a 3-way connector on the top of a MenuWindow.
    ***  - connectors[0]: top tri-connector
    ***  - connectors[1]: bottom tri-connector
    ***  - connectors[2]: left tri-connector
    ***  - connectors[3]: right tri-connector
    ***  - connectors[4]: quad connector
    **/
    vt_video::StillImage connectors[5];

    //! \brief The background image of the menu skin that fills the inside of the MenuWindow
    vt_video::StillImage background;

    /** \brief Stores the arrow icons used for scrolling through various GUI controls
    *** The size of this vector is eight. The first four images are the standard arrows and the last
    *** four are greyed out arrows (used to indicate the end of scrolling). The first four arrow
    *** images represent up, down, left, right in that order, and the last four arrows follow this
    *** format as well.
    **/
    std::vector<vt_video::StillImage> scroll_arrows;
}; // class MenuSkin

} // namespace private_gui


/** ****************************************************************************
*** \brief Represents GUI menu windows and handles their operation
***
*** A menu window is simply a rectangle drawn on the screen that has a border
*** image and a background. Menu windows exist to provide a base upon which to
*** draw text and images that are not a part of the regular game environment.
***
*** \note It is common practice to derive specific windows from this class for
*** the display of dialogue text, inventory lists, etc. This class is designed
*** with that practice in mind.
***
*** \todo Determine function/behavior of copy constructor and copy assignment
*** operator. Should these be set to private, or implemented? How should the
*** texture be copied if it is implemented?
*** ***************************************************************************/
class MenuWindow : public private_gui::GUIElement
{
    friend class GUISystem;
public:
    MenuWindow();

    ~MenuWindow() {
        // Free the memory in case its needed.
        if (_skin)
            Destroy();
    }

    /** \brief Sets the width and height of the menu.
    *** \param skin_name The name of the menu skin with which to construct this menu window.
    *** \param w The window width in pixels, which must be within the range of [0.0f, 1024.f].
    *** \param h The window height in pixels, which must be within the range of [0.0f, 768.f].
    *** \param visible_flags A combination of bit flags, VIDEO_MENU_EDGE_LEFT, etc. that indicate
    *** which edges are visible. A non-visible edge means that the border image gets stripped off.
    *** \param shared_flags A combination of bitflags, VIDEO_MENU_EDGE_LEFT, etc. that tell which
    *** edges are shared with other menus so they can use the appropriate connector images.
    *** \return False and print an error message on failure. Otherwise return true upon success.
    *** \note This function <b>must</b> be called before you attempt to draw the window.
    **/
    bool Create(const std::string &skin_name, float w, float h, int32 visible_flags = VIDEO_MENU_EDGE_ALL, int32 shared_flags = 0);

    //! \note This version of the create function does not take a skin_name argument. It uses the default menu skin.
    bool Create(float w, float h, int32 visible_flags = VIDEO_MENU_EDGE_ALL, int32 shared_flags = 0);

    /** \brief You <b>must</b> call this method when you are finished using a menu.
    *** Failure to neglect this call may result in problems like texture memory not being freed.
    **/
    void Destroy();

    /** \brief This version is for the subclasses of menu window (allows us to use a single MenuWindow variable
    *** to track the active window.
    **/
    void Update(uint32 /*frame_time*/)
    {}
    virtual void Update()
    {}

    //! \brief Draws the menu window to the screen with default color and opacity
    inline void Draw() {
        Draw(vt_video::Color::white);
    }

    //! \brief Draws the menu window to the screen with a specified color and opacity
    void Draw(const vt_video::Color& color);

    //! \brief Makes the current window visible
    void Show() {
        _window_state = VIDEO_MENU_STATE_SHOWN;
    }

    //! \brief Makes the current window hidden.
    void Hide() {
        _window_state = VIDEO_MENU_STATE_HIDDEN;
    }

    /** \brief Indicates whether the window is in the active context
    *** \return True always here, subclasses can override to change the behaviour
    **/
    virtual bool IsActive() {
        return true;
    }

    //! \name Class Member Access Functions
    //@{
    VIDEO_MENU_STATE GetState() const {
        return _window_state;
    }

    //! \note This call is somewhat expensive since it has to recreate the menu window image.
    void SetDimensions(float w, float h);

    //! \note This call is somewhat expensive since it has to recreate the menu window image.
    void SetEdgeVisibleFlags(int32 flags) {
        _edge_visible_flags = flags;
        _RecreateImage();
    }

    //! \note This call is somewhat expensive since it has to recreate the menu window image.
    void SetEdgeSharedFlags(int32 flags) {
        _edge_shared_flags = flags;
        _RecreateImage();
    }

    //! \note This call is somewhat expensive since it has to recreate the menu window image.
    void SetMenuSkin(const std::string &skin_name);
    //@}

private:
    //! \brief The dimensions of the space inside the window borders.
    float _inner_width, _inner_height;

    //! \brief Flags used to tell which edges of the menu window are visible.
    int32 _edge_visible_flags;

    //! \brief Flags used to tell which edges are shared with other windows.
    int32 _edge_shared_flags;

    //! \brief A pointer to the menu skin that the menu window currently uses
    private_gui::MenuSkin *_skin;

    //! \brief The state of the menu window (hidden, shown, hiding, showing).
    VIDEO_MENU_STATE _window_state;

    //! \brief The image that creates the window
    vt_video::CompositeImage _menu_image;

    /** \brief Used to create the menu window's image when the visible properties of the window change.
    *** \return True if the menu image was successfully created, false otherwise.
    ***
    *** \note This function may not create a window that is exactly the width and height requested.
    *** It will automatically adjust the dimensions to minimize warping. So for example, if the
    *** border artwork is all 8x8 pixel images and you try to create a menu that is 117x69, it will get
    *** rounded up to 120x72.
    **/
    bool _RecreateImage();
}; // class MenuWindow : public GUIElement

} // namespace vt_gui

#endif  // __MENU_WINDOW_HEADER__
