////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software 
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    coord_sys.h
*** \author  Raj Sharma, roos@allacrost.org
*** \brief   Header file for the CoordSys class.
*** ***************************************************************************/

#ifndef __COORD_SYS_HEADER__
#define __COORD_SYS_HEADER__

#include "defs.h"
#include "utils.h"

namespace hoa_video {

/** ****************************************************************************
*** \brief Determines the drawing coordinates
***
*** The CoordSys structure holds a "coordinate system" defined by a rectangle
*** (left, right, bottom, and top) which determines how drawing coordinates
*** are mapped to the screen.
***
*** \note The default coordinate system is (0, 1024, 0, 768), which is the same
*** as the game's default 1024x768 resolution.
*** ***************************************************************************/
class CoordSys {
public:
	CoordSys()
		{}

	CoordSys(float left, float right, float bottom, float top)
		{
			_left = left; _right = right; _bottom = bottom; _top = top;
			if (_right > _left) _horizontal_direction = 1.0f; else _horizontal_direction = -1.0f;
			if (_top > _bottom) _vertical_direction = 1.0f; else _vertical_direction = -1.0f;
		}

	//! \brief Class member access functions
	//@{
	float GetVerticalDirection() const
		{ return _vertical_direction; }

	float GetHorizontalDirection() const
		{ return _horizontal_direction; }

	float GetLeft() const
		{ return _left; }

	float GetRight() const
		{ return _right; }

	float GetBottom() const
		{ return _bottom; }

	float GetTop() const
		{ return _top; }

	float GetWidth() const
		{ return fabs(_left - _right); }

	float GetHeight() const
		{ return fabs(_top - _bottom); }
	//@}

	//! \brief Normalisation functions
	//@{
	void ConvertNormalisedToLocal(float& localX, float& localY, float normalisedX, float normalisedY) const
		{ localX = _left + normalisedX * (_right - _left);
		  localY = _bottom + normalisedY * (_top - _bottom); }
	void ConvertLocalToNormalised(float& normalisedX, float& normalisedY, float localX, float localY) const
		{ normalisedX = (_left - localX) / (_right - _left);
		  normalisedY = (_bottom - localY) / (_top - _bottom); }
	//@}

private:
	//! \brief If the y-coordinates increase from bottom to top, this is 1.0f. Otherwise it is -1.0f.
	float _vertical_direction;

	//! \brief If the y-coordinates increase from left to right, this is 1.0f. Otherwise it is -1.0f.
	float _horizontal_direction;

	//! \brief The values of the four sides of the screen that determine the drawing coordinates.
	float _left, _right, _bottom, _top;
}; // class CoordSys

} // namespace hoa_video

#endif // __COORD_SYS_HEADER__
