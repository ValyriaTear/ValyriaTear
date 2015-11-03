////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    color.h
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for the Color class.
*** ***************************************************************************/

#ifndef __COLOR_HEADER__
#define __COLOR_HEADER__

#include "utils/utils_numeric.h"

namespace vt_video
{

/** ****************************************************************************
*** \brief Representation of a single RGBA color.
***
*** This class encapsulates an array of 4 floats, and allows you to do basic
*** operations like adding and multiplying colors.
*** ***************************************************************************/
class Color
{
public:
    /** \brief Default colors for user convenience.
    *** These are defined in the file video.cpp. All colors are opaque (1.0f alpha value) except for "clear".
    **/
    //@{
    static const Color clear;     //!< Clear (transparent) color (r=0.0, g=0.0, b=0.0, a=0.0)
    static const Color white;     //!< White color (r=1.0, g=1.0, b=1.0, a=1.0)
    static const Color gray;      //!< Gray color (r=0.5, g=0.5, b=0.5, a=1.0)
    static const Color black;     //!< Black color (r=0.0, g=0.0, b=0.0, a=1.0)
    static const Color red;       //!< Red color (r=1.0, g=0.0, b=0.0, a=1.0)
    static const Color orange;    //!< Orangecolor (r=1.0, g=0.4, b=0.0, a=1.0)
    static const Color yellow;    //!< Yellow color (r=1.0, g=1.0, b=0.0, a=1.0)
    static const Color green;     //!< Green color (r=0.0, g=1.0, b=0.0, a=1.0)
    static const Color aqua;      //!< Aqua color (r=0.0, g=1.0, b=1.0, a=1.0)
    static const Color blue;      //!< Blue color (r=0.0, g=0.0, b=1.0, a=1.0)
    static const Color violet;    //!< Violet color (r=1.0, g=0.0, b=1.0, a=1.0)
    static const Color brown;     //!< Brown color (r=0.6, g=0.3, b=0.1, a=1.0)
    //@}

    Color() {
        _colors[0] = 0.0f;
        _colors[1] = 0.0f;
        _colors[2] = 0.0f;
        _colors[3] = 1.0f;
    }

    Color(float r, float g, float b, float a) {
        _colors[0] = r;
        _colors[1] = g;
        _colors[2] = b;
        _colors[3] = a;
    }

    //! \brief Overloaded Operators
    //@{
    bool operator == (const Color &c) const {
        return (vt_utils::IsFloatEqual(_colors[0], c._colors[0]) && vt_utils::IsFloatEqual(_colors[1], c._colors[1]) &&
                vt_utils::IsFloatEqual(_colors[2], c._colors[2]) && vt_utils::IsFloatEqual(_colors[3], c._colors[3]));
    }

    bool operator != (const Color &c) const {
        return _colors[0] != c._colors[0] || _colors[1] != c._colors[1] || _colors[2] != c._colors[2] || _colors[3] != c._colors[3];
    }

    Color operator + (const Color &c) const {
        Color col = Color(_colors[0] + c._colors[0], _colors[1] + c._colors[1], _colors[2] + c._colors[2], _colors[3] + c._colors[3]);
        if(col[0] > 1.0f) col[0] = 1.0f;
        else if(col[0] < 0.0f) col[0] = 0.0f;
        if(col[1] > 1.0f) col[1] = 1.0f;
        else if(col[1] < 0.0f) col[1] = 0.0f;
        if(col[2] > 1.0f) col[2] = 1.0f;
        else if(col[2] < 0.0f) col[2] = 0.0f;
        if(col[3] > 1.0f) col[3] = 1.0f;
        else if(col[3] < 0.0f) col[3] = 0.0f;
        return col;
    }

    Color &operator *= (const Color &c) {
        _colors[0] *= c._colors[0];
        _colors[1] *= c._colors[1];
        _colors[2] *= c._colors[2];
        _colors[3] *= c._colors[3];
        return *this;
    }

    Color operator * (const Color &c) const {
        return Color(_colors[0] * c._colors[0], _colors[1] * c._colors[1], _colors[2] * c._colors[2], _colors[3] * c._colors[3]);
    }

    Color operator * (float f) const {
        return Color(_colors[0] * f, _colors[1] * f, _colors[2] * f, _colors[3]);
    }

    /** \note No checking of array bounds are done here for efficiency reasons. If safety is a concern, use the
    *** class member access functions instead.
    **/
    float &operator[](int32_t i) {
        return _colors[i];
    }

    /** \note No checking of array bounds are done here for efficiency reasons. If safety is a concern, use the
    *** class member access functions instead.
    **/
    const float &operator[](int32_t i) const {
        return _colors[i];
    }
    //@}

    /** \brief Converts the color into a SDL_Color structure
    *** \return A new SDL_Color with the four uint8_t values ranging from 0 to 255
    **/
    SDL_Color CreateSDLColor() {
        SDL_Color sdl_color;
        sdl_color.r = static_cast<uint8_t>(_colors[0] * 0xFF);
        sdl_color.g = static_cast<uint8_t>(_colors[1] * 0xFF);
        sdl_color.b = static_cast<uint8_t>(_colors[2] * 0xFF);
        sdl_color.a = static_cast<uint8_t>(_colors[3] * 0xFF);
        return sdl_color;
    }

    //! \brief Class member access functions
    //@{
    const float *GetColors() const {
        return _colors;
    }

    float GetRed() const {
        return _colors[0];
    }

    float GetGreen() const {
        return _colors[1];
    }

    float GetBlue() const {
        return _colors[2];
    }

    float GetAlpha() const {
        return _colors[3];
    }

    void SetRed(float r) {
        _colors[0] = r;
        if(_colors[0] > 1.0f) _colors[0] = 1.0f;
        else if(_colors[0] < 0.0f) _colors[0] = 0.0f;
    }

    void SetGreen(float g) {
        _colors[1] = g;
        if(_colors[1] > 1.0f) _colors[1] = 1.0f;
        else if(_colors[1] < 0.0f) _colors[1] = 0.0f;
    }

    void SetBlue(float b) {
        _colors[2] = b;
        if(_colors[2] > 1.0f) _colors[2] = 1.0f;
        else if(_colors[2] < 0.0f) _colors[2] = 0.0f;
    }

    void SetAlpha(float a) {
        _colors[3] = a;
        if(_colors[3] > 1.0f) _colors[3] = 1.0f;
        else if(_colors[3] < 0.0f) _colors[3] = 0.0f;
    }

    void SetColor(float r, float g, float b, float alpha) {
        SetRed(r);
        SetGreen(g);
        SetBlue(b);
        SetAlpha(alpha);
    }

    void SetColor(const Color& c) {
        SetRed(c.GetRed());
        SetGreen(c.GetGreen());
        SetBlue(c.GetBlue());
        SetAlpha(c.GetAlpha());
    }
    //@}

private:
    /** \brief The four RGBA values that represent the color
    *** These values range from 0.0 to 1.0. The indeces of the array represent:
    *** red, green, blue, and alpha in that order.
    **/
    float _colors[4];
}; // class Color

}  // namespace vt_video

#endif // __COLOR_HEADER__
