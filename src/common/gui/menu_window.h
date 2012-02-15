///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    menu_window.h
*** \author  Raj Sharma, roos@allacrost.org
*** \brief   Header file for menu window class
*** ***************************************************************************/

#ifndef __MENU_WINDOW_HEADER__
#define __MENU_WINDOW_HEADER__

#include "defs.h"
#include "utils.h"

#include "gui.h"
#include "screen_rect.h"
#include "image.h"

namespace hoa_gui {

//! \brief How many milliseconds it takes for a window to scroll in or out of view.
const int32 VIDEO_MENU_SCROLL_TIME = 200;


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


/** \brief Menu display modes control how the menu window appears or disappears.
*** The specific display modes include:
*** - VIDEO_MENU_INSTANT: appears/disappears instantly
*** - VIDEO_MENU_EXPAND_FROM_CENTER: starts as a thin horizontal line at center and expands out
**/
enum VIDEO_MENU_DISPLAY_MODE {
	VIDEO_MENU_INVALID            = -1,
	VIDEO_MENU_INSTANT            =  0,
	VIDEO_MENU_EXPAND_FROM_CENTER =  1,
	VIDEO_MENU_TOTAL              =  2
};


/** \brief These enums are used to tell the visiblity state of a menu window.
*** The possible states and their meaning include:
*** - VIDEO_MENU_STATE_SHOWN: the menu is fully shown
*** - VIDEO_MENU_STATE_SHOWING: the menu is still in the process of scrolling on to the screen
*** - VIDEO_MENU_STATE_HIDING: the menu is scrolling out of view, but is not completely hidden yet
*** - VIDEO_MENU_STATE_HIDDEN: the menu is fully hidden
**/
enum VIDEO_MENU_STATE {
	VIDEO_MENU_STATE_INVALID = -1,
	VIDEO_MENU_STATE_SHOWN   = 0,
	VIDEO_MENU_STATE_SHOWING = 1,
	VIDEO_MENU_STATE_HIDING  = 2,
	VIDEO_MENU_STATE_HIDDEN  = 3,
	VIDEO_MENU_STATE_TOTAL   = 4
};


namespace private_gui {

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
class MenuSkin {
public:
	MenuSkin()
		{}

	~MenuSkin()
		{}

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
	hoa_video::StillImage borders[3][3];

	/** \brief Border-connecting images, used when two or more MenuWindows are side by side.
	***  There are four tri-connectors and one quad-connector. tri_t would be an image for
	***  a 3-way connector on the top of a MenuWindow.
	***  - connectors[0]: top tri-connector
	***  - connectors[1]: bottom tri-connector
	***  - connectors[2]: left tri-connector
	***  - connectors[3]: right tri-connector
	***  - connectors[4]: quad connector
	**/
	hoa_video::StillImage connectors[5];

	//! \brief The (optional) background image of the menu skin that fills the inside of the MenuWindow
	hoa_video::StillImage background;
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
*** \todo Allow the user to specify an arbitrary amount of time for showing/
*** hiding the menu window.
***
*** \todo Determine function/behavior of copy constructor and copy assignment
*** operator. Should these be set to private, or implemented? How should the
*** texture be copied if it is implemented?
***
*** \todo Why doesn't the class destructor do what the Destroy function implements?
*** It would be much safer for the destructor to auto-destroy anything, or at the
*** very least it could print a warning if the class destructor is called when
*** destroy has not yet been invoked. If this is changed, remember to update
*** the documentation on the wiki for this as well.
*** ***************************************************************************/
class MenuWindow : public private_gui::GUIElement {
	friend class GUISystem;
public:
	MenuWindow();

	~MenuWindow()
		{}

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
	bool Create(std::string skin_name, float w, float h, int32 visible_flags = VIDEO_MENU_EDGE_ALL, int32 shared_flags = 0);

	//! \note This version of the create function does not take a skin_name argument. It uses the default menu skin.
	bool Create(float w, float h, int32 visible_flags = VIDEO_MENU_EDGE_ALL, int32 shared_flags = 0);

	/** \brief You <b>must</b> call this method when you are finished using a menu.
	*** Failure to neglect this call may result in problems like texture memory not being freed.
	**/
	void Destroy();

	/** \brief Updates the menu window, used for gradual show/hide effects.
	*** \param frame_time The time that has elapsed since the previous frame, in milliseconds.
	**/
	void Update(uint32 frame_time);

	/** \brief This version is for the subclasses of menu window (allows us to use a single MenuWindow variable
	*** to track the active window.
	**/
	virtual void Update()
		{}

	//! \brief Draws the menu window to the screen.
	void Draw();

	/** \brief Causes the menu to begin making itself visible.
	*** Depending on the display mode, the menu might show instantly or gradually.
	*** You can check for when the menu is fully shown by checking if GetState()
	*** returns VIDEO_MENU_STATE_SHOWN (until then, it is VIDEO_MENU_STATE_SHOWING).
	*** \note The time it takes for the menu to show is VIDEO_MENU_SCROLL_TIME.
	**/
	void Show();

	/** \brief Causes the menu to begin making itself not visible.
	*** Depending on the display mode, the menu might hide instantly or gradually.
	*** If it's gradual, you should still continue calling Draw() even after you call
	*** Hide() until it's fully hidden. You can check if it's fully hidden by checking
	*** if GetState() returns VIDEO_MENU_STATE_HIDDEN (until then, it will be
	*** VIDEO_MENU_STATE_HIDING).
	*** \note The time it takes for the menu to show is VIDEO_MENU_SCROLL_TIME
	**/
	void Hide();

	/** \brief Does a self-check on all its members to see if all its members have been set to valid values.
	*** \param &errors A reference to a string to be filled if any errors are found.
	*** \return True if menu window is properly initialized, false if it is not.
	***
	*** This is used internally to make sure we have a valid object before doing any complicated operations.
	*** If it detects any problems, it generates a list of errors and returns it by reference so they can be
	*** displayed
	**/
	bool IsInitialized(std::string &errors);

	/** \brief Indicates whether the window is in the active context
	*** \return True always here, subclasses can override to change the behaviour
	**/
	virtual bool IsActive()
		{ return true; }

	//! \name Class Member Access Functions
	//@{
	VIDEO_MENU_DISPLAY_MODE GetDisplayMode() const
		{ return _display_mode; }

	VIDEO_MENU_STATE GetState() const
		{ return _window_state; }

	/** \note When the window is in the process of showing or hiding, subsequent calls to this function
	*** (in between calls to Update()) will yield different results as the active dimensions of the window
	*** are changing. When the window is fully shown or fully hidden, this function will always return the
	*** same scissor rectangle that is reflective of the window's full size.
	***
	**/
	hoa_video::ScreenRect GetScissorRect() const
		{ return _scissor_rect; }

	//! \note This call is somewhat expensive since it has to recreate the menu window image.
	void SetDimensions(float w, float h);

	//! \note This call is somewhat expensive since it has to recreate the menu window image.
	void SetEdgeVisibleFlags(int32 flags)
		{ _edge_visible_flags = flags; _RecreateImage(); }

	//! \note This call is somewhat expensive since it has to recreate the menu window image.
	void SetEdgeSharedFlags(int32 flags)
		{ _edge_shared_flags = flags; _RecreateImage(); }

	//! \note This call is somewhat expensive since it has to recreate the menu window image.
	void SetMenuSkin(std::string& skin_name);

	void SetDisplayMode(VIDEO_MENU_DISPLAY_MODE mode);
	//@}

private:
	//! \brief The current id of this object.
	int32 _id;

	//! \brief The dimensions of the space inside the window borders.
	float _inner_width, _inner_height;

	//! \brief Flags used to tell which edges of the menu window are visible.
	int32 _edge_visible_flags;

	//! \brief Flags used to tell which edges are shared with other windows.
	int32 _edge_shared_flags;

	//! \brief A pointer to the menu skin that the menu window currently uses
	private_gui::MenuSkin* _skin;

	//! \brief The state of the menu window (hidden, shown, hiding, showing).
	VIDEO_MENU_STATE _window_state;

	//! \brief The number of milliseconds that have passed since the menu was shown.
	int32 _display_timer;

	//! \brief The image that creates the window
	hoa_video::CompositeImage _menu_image;

	//! \brief The window's display mode (instant, expand from center, etc).
	VIDEO_MENU_DISPLAY_MODE _display_mode;

	//! \brief Set to true if scissoring needs to be used on the window.
	bool _is_scissored;

	//! \brief The rectangle used for scissoring, set during each call to Update().
	hoa_video::ScreenRect _scissor_rect;

	/** \brief Used to create the menu window's image when the visible properties of the window change.
	*** \return True if the menu image was successfully created, false otherwise.
	***
	*** \note This function may not create a window that is exactly the width and height requested.
	*** It will automatically adjust the dimneions to minimalize warping. So for example, if the
	*** border artwork is all 8x8 pixel images and you try to create a menu that is 117x69, it will get
	*** rounded up to 120x72.
	**/
	bool _RecreateImage();
}; // class MenuWindow : public GUIElement

} // namespace hoa_gui

#endif  // __MENU_WINDOW_HEADER__
