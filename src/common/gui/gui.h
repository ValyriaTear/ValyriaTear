///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file   gui.h
*** \author Raj Sharma, roos@allacrost.org
*** \brief  Header file for GUI code
***
*** This code implements the base structures of the video engine's GUI system.
*** ***************************************************************************/

#ifndef __GUI_HEADER__
#define __GUI_HEADER__

#include "defs.h"
#include "utils.h"

#include "screen_rect.h"
#include "image.h"
#include "text.h"

namespace hoa_gui {

//! \brief The singleton pointer for the GUI manager
extern GUISystem* GUIManager;

namespace private_gui {

//! \brief 50% alpha colors used for debug drawing of GUI element outlines
//@{
const hoa_video::Color alpha_black(0.0f, 0.0f, 0.0f, 0.5f);
const hoa_video::Color alpha_white(1.0f, 1.0f, 1.0f, 0.5f);
//@}

//! \brief Constants used as indeces to access the GUISystem#_scroll_arrows vector
//@{
const uint32 SCROLL_UP           = 0;
const uint32 SCROLL_DOWN         = 1;
const uint32 SCROLL_LEFT         = 2;
const uint32 SCROLL_RIGHT        = 3;
const uint32 SCROLL_UP_GREY      = 4;
const uint32 SCROLL_DOWN_GREY    = 5;
const uint32 SCROLL_LEFT_GREY    = 6;
const uint32 SCROLL_RIGHT_GREY   = 7;
//@}


/** ****************************************************************************
*** \brief An abstract base class for all GUI elements (windows + controls).
*** This class contains basic functions such as Draw(), Update(), etc.
*** ***************************************************************************/
class GUIElement {
public:
	GUIElement();

	//! \note The destructor must be re-implemented in all children of this class.
	virtual ~GUIElement()
		{}

	//! \brief Draws the GUI element to the screen.
	virtual void Draw() = 0;

	/** \brief Updates the state of the element.
	*** \param frame_time The time that has elapsed since the last frame was drawn, in milliseconds
	**/
	virtual void Update(uint32 frame_time) = 0;

	/** \brief Does a self-check on all its members to see if all its members have been set to valid values.
	*** \param errors - A reference to a string to be filled with error messages if any errors are found.
	*** \return True if everything is initialized correctly, false otherwise.
	*** This is used internally to make sure we have a valid object before doing any complicated operations.
	*** If it detects any problems, it generates a list of errors and returns it by reference so they can be displayed.
	**/
	virtual bool IsInitialized(std::string &errors) = 0;

	/** \brief Sets the width and height of the element
	*** \param w The width to set for the element
	*** \param h The height to set for the element
	*** If either the width or height arguments are negative (or zero) no change will take place. This method only sets the
	*** _width and _height members. Deriving classes may need to override this function to take into consideration
	*** changes that must take place when the element is re-sized.
	**/
	virtual void SetDimensions(float w, float h);

	/** \brief Sets the position of the object.
	*** \param x A reference to store the x coordinate of the object.
	*** \param y A reference to store the y coordinate of the object.
	*** \note X and y are in terms of a 1024x768 coordinate system
	**/
	void SetPosition(float x, float y)
		{ _x_position = x; _y_position = y; }

	/** \brief Sets the alignment of the element.
	*** \param xalign Valid values include VIDEO_X_LEFT, VIDEO_X_CENTER, or VIDEO_X_RIGHT.
	*** \param yalign Valid values include VIDEO_Y_TOP, VIDEO_Y_CENTER, or VIDEO_Y_BOTTOM.
	**/
	void SetAlignment(int32 xalign, int32 yalign);

	/** \brief Returns the width and height of the GUI element
	*** \param w Reference to a variable to hold the width
	*** \param h Reference to a variable to hold the height
	**/
	void GetDimensions(float& w, float& h) const
		{ w = _width; h = _height; }

	/** \brief Gets the position of the object.
	*** \param x A reference to store the x coordinate of the object.
	*** \param y A reference to store the y coordinate of the object.
	*** \note X and y are in terms of a 1024x768 coordinate system
	**/
	void GetPosition(float &x, float &y) const
		{ x = _x_position; y = _y_position; }

	/** \brief Gets the x and y alignment of the element.
	*** \param xalign - x alignment of the object
	*** \param yalign - y alignment of the object
	**/
	void GetAlignment(int32 &xalign, int32 &yalign) const
		{ xalign = _xalign; yalign = _yalign; }

	/** \brief Calculates and returns the four edges for an aligned rectangle
	*** \param left A reference where to store the coordinates of the rectangle's left edge.
	*** \param right A reference where to store the coordinates of the rectangle's right edge.
	*** \param bottom A reference where to store the coordinates of the rectangle's bttom edge.
	*** \param top A reference where to store the coordinates of the rectangle's top edge.
	***
	*** Given a rectangle specified in VIDEO_X_LEFT and VIDEO_Y_BOTTOM orientation, this function
	*** transforms the rectangle based on the video engine's alignment flags.
	*** \todo I think this function needs to be renamed. It seems to only be used to compute the
	*** four edges of the GUI element. It should be called "CalculateEdges" or somthing more
	*** specific if it is only used in this manner.
	**/
	virtual void CalculateAlignedRect(float &left, float &right, float &bottom, float &top);

protected:
	//! \brief Members for determining the element's draw alignment.
	int32 _xalign, _yalign;

	//! \brief The x and y position of the gui element.
	float _x_position, _y_position;

	//! \brief The dimensions of the GUI element in pixels.
	float _width, _height;

	//! \brief Used to determine if the object is in a valid state.
	//! \note This member is set after every change to any of the object's settings.
	bool  _initialized;

	//! \brief Contains the errors that need to be resolved if the object is in an invalid state (not ready for rendering).
	std::string _initialization_errors;

	//! \brief Draws an outline of the element boundaries
	virtual void _DEBUG_DrawOutline();
}; // class GUIElement


/** ****************************************************************************
*** \brief GUIControl is a type of GUI element, specifically for controls.
*** This is for functions that controls have, but menu windows don't have, such
*** as the SetOwner() function.
*** ***************************************************************************/
class GUIControl : public GUIElement {
public:
	GUIControl()
		{ _owner = NULL; }

	virtual ~GUIControl()
		{}

	/** \brief Calculates and returns the four edges for an aligned rectangle
	*** \param left A reference where to store the coordinates of the rectangle's left edge.
	*** \param right A reference where to store the coordinates of the rectangle's right edge.
	*** \param bottom A reference where to store the coordinates of the rectangle's bttom edge.
	*** \param top A reference where to store the coordinates of the rectangle's top edge.
	*** \note The difference between this function and the one for GUI elements is that
	*** controls must take their owner window into account.
	**/
	virtual void CalculateAlignedRect(float &left, float &right, float &bottom, float &top);

	/** \brief Sets the menu window which "owns" this control.
	*** \param owner_window A pointer to the menu that owns the control.
	*** \note If the control is not owned by any menu window, then set the owner to NULL.
	*** When a control is owned by a menu, it means that it obeys the menu's scissoring
	*** rectangle so that the control won't be drawn outside of the bounds of the menu.
	*** It also means that the position of the control is relative to the position of the
	*** window. (i.e. control.position += menu.position).
	**/
	virtual void SetOwner(MenuWindow *owner_window)
		{ _owner = owner_window; }

protected:
	/** \brief A pointer to the menu which owns this control.
	*** When the owner is set to NULL, the control can draw to any part of the screen
	*** (so scissoring is ignored) and drawing coordinates are not modified.
	**/
	MenuWindow *_owner;

	/** \brief Draws an outline of the control boundaries
	*** \note This implementation uses the
	***
	**/
	virtual void _DEBUG_DrawOutline();
}; // class GUIControl : public GUIElement

} // namespace private_gui


/** ****************************************************************************
*** \brief A helper class to the video engine to manage all of the GUI functionality.
***
*** There is exactly one instance of this class, which is both created and destroyed
*** by the VideoEngine class. This class is essentially an extension of the GameVideo
*** class which manages the GUI system. It also handles the drawing of the
*** average frames per second (FPS) on the screen.
*** ***************************************************************************/
class GUISystem : public hoa_utils::Singleton<GUISystem> {
	friend class hoa_utils::Singleton<GUISystem>;
	friend class hoa_video::VideoEngine;
	friend class MenuWindow;
	friend class TextBox;
	friend class OptionBox;
public:
	GUISystem();

	~GUISystem();

	bool SingletonInitialize();

	/** \name Methods for loading of menu skins
	***
	*** These methods all attempt to load a menu skin. The differences between these implementations are
	*** whether the skin includes a background image, single background color, multiple background colors,
	*** or some combination thereof. Only the skin_name and border_image arguments are mandatory for all
	*** versions of this function to have
	***
	*** \param skin_name The name that will be used to refer to the skin after it is successfully loaded
	*** \param border_image The filename for the multi-image that contains the menu's border images
	*** \param background_image The filename for the skin's background image (optional)
	*** \param top_left Sets the background color for the top left portion of the skin
	*** \param top_right Sets the background color for the top right portion of the skin
	*** \param bottom_left Sets the background color for the bottom left portion of the skin
	*** \param bottom_right Sets the background color for the bottom right portion of the skin
	*** \param make_default If this skin should be the default menu skin to be used, set this argument to true
	*** \return True if the skin was loaded successfully, or false in case of an error
	***
	*** A few notes about this function:
	*** - If you set a background image, any background colors will not be visible unless the background image has some transparency
	*** - If no other menu skins are loaded when this function is called, the default skin will automatically be set to this skin,
	***   regardless of the value of the make_default parameter.
	**/
	//@{
	//! \brief Loads a background image with no background colors
	bool LoadMenuSkin(std::string skin_name, std::string border_image, std::string background_image, bool make_default = false);

	//! \brief Loads a single background color with no background image
	bool LoadMenuSkin(std::string skin_name, std::string border_image, hoa_video::Color background_color, bool make_default = false);

	//! \brief Loads multiple multiple background colors with no background image
	bool LoadMenuSkin(std::string skin_name, std::string border_image, hoa_video::Color top_left, hoa_video::Color top_right,
		hoa_video::Color bottom_left, hoa_video::Color bottom_right, bool make_default = false);

	//! \brief Loads a background image with a single background color
	bool LoadMenuSkin(std::string skin_name, std::string border_image, std::string background_image,
		hoa_video::Color background_color, bool make_default = false);

	//! \brief Loads a background image with multiple background colors
	bool LoadMenuSkin(std::string skin_name, std::string border_image, std::string background_image,
		hoa_video::Color top_left, hoa_video::Color top_right, hoa_video::Color bottom_left, hoa_video::Color bottom_right, bool make_default = false);
	//@}

	/** \brief Deletes a menu skin that has been loaded
	*** \param skin_name The name of the loaded menu skin that should be removed
	***
	*** This function could fail on one of two circumstances. First, if there is no MenuSkin loaded for
	*** the key skin_name, the function will do nothing. Second, if any MenuWindow objects are still
	*** referencing the skin that is trying to be deleted, the function will print a warning message
	*** and not delete the skin. Therefore, <b>before you call this function, you must delete any and all
	*** MenuWindow objects which make use of this skin, or change the skin used by those objects</b>.
	**/
	void DeleteMenuSkin(std::string& skin_name);

	//! \brief Returns true if there is a menu skin avialable corresponding to the argument name
	bool IsMenuSkinAvailable(std::string& skin_name) const;

	/** \brief Sets the default menu skin to use from the set of pre-loaded skins
	*** \param skin_name The name of the already loaded menu skin that should be made the default skin
	***
	*** If the skin_name does not refer to a valid skin, a warning message will be printed and no change
	*** will occur.
	*** \note This method will <b>not</b> change the skins of any active menu windows.
	**/
	void SetDefaultMenuSkin(std::string& skin_name);

	//! \brief Returns true if GUI elements should have outlines drawn over their boundaries
	bool DEBUG_DrawOutlines() const
		{ return _DEBUG_draw_outlines; }

	// Don't commit this.
	std::vector<hoa_video::StillImage>* GetScrollArrows()
		{ return &_scroll_arrows; }
	
	/** \brief Debug functioning for enabling/disabling the drawing of GUI element boundaries
	*** \param enable Set to true to enable outlines, false to disable
	**/
	void DEBUG_EnableGUIOutlines(bool enable)
		{ _DEBUG_draw_outlines = enable; }

private:
	/** \brief Stores the arrow icons used for scrolling through various GUI controls
	*** The size of this vector is eight. The first four images are the standard arrows and the last
	*** four are greyed out arrows (used to indicate the end of scrolling). The first four arrow
	*** images represent up, down, left, right in that order, and the last four arrows follow this
	*** format as well.
	**/
	std::vector<hoa_video::StillImage> _scroll_arrows;

	/** \brief A map containing all of the menu skins which have been loaded
	*** The string argument is the reference name of the menu, which is defined
	*** by the user when they load a new skin.
	***
	**/
	std::map<std::string, private_gui::MenuSkin> _menu_skins;

	/** \brief A map containing all of the actively created MenuWindow objects
	*** The integer key is the MenuWindow's ID number. This primary purpose of this map is to coordinate menu windows
	*** with menu skins. A menu skin can not be deleted when a menu window is still using that skin, and menu windows
	*** must be re-drawn when the properties of a menu skin that it uses changes.
	**/
	std::map<uint32, MenuWindow*> _menu_windows;

	/** \brief A pointer to the default menu skin that GUI objects will use if a skin is not explicitly declared
	*** If no menu skins exist, this member will be NULL. It will never be NULL as long as one menu skin is loaded.
	*** If the default menu skin is deleted by the user, an alternative default skin will automatically be set.
	**/
	hoa_gui::private_gui::MenuSkin* _default_skin;

	//! \brief The next ID to assign to a MenuWindow when one is created
	uint32 _next_window_id;

	/** \brief Draws an outline of the boundary for all GUI elements drawn to the screen when true
	*** The VideoEngine class contains the method that modifies this variable.
	**/
	bool _DEBUG_draw_outlines;

	// ---------- Private methods

	/** \brief Returns a pointer to the MenuSkin of a corresponding skin name
	*** \param skin_name The name of the menu skin to grab
	*** \return A pointer to the MenuSkin, or NULL if the skin name was not found
	**/
	private_gui::MenuSkin* _GetMenuSkin(std::string& skin_name);

	//! \brief Returns a pointer to the default menu skin
	private_gui::MenuSkin* _GetDefaultMenuSkin() const
		{ return _default_skin; }

	/** \brief Returns the next available MenuWindow ID for a MenuWindow to use
	*** \return The ID number for the MenuWindow to use
	*** This method should only need to be called from the MenuWindow constructor.
	**/
	uint32 _GetNextMenuWindowID()
		{ _next_window_id++; return (_next_window_id - 1); }

	/** \brief Adds a newly created MenuWindow into the map of existing windows
	*** \param new_window A pointer to the newly created MenuWindow
	*** Don't call this method anywhere else but from MenuWindow::Create(), or you may cause problems.
	**/
	void _AddMenuWindow(MenuWindow* new_window);

	/** \brief Removes an existing MenuWindow from the map of existing windows
	*** \param old_window A pointer to the MenuWindow to be removed
	*** Don't call this method anywhere else but from MenuWindow::Destroy(), or you may cause problems.
	**/
	void _RemoveMenuWindow(MenuWindow* old_window);
}; // class GUISystem : public hoa_utils::Singleton<GUISystem>

} // namespace hoa_gui

#include "menu_window.h"
#include "option.h"
#include "textbox.h"

#endif // __GUI_HEADER__
