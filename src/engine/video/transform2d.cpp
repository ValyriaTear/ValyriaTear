///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2013 by Valyria Tear Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    transform2d.h
*** \author  logzero
*** \author  authenticate
*** \brief   Source file for Transform2D class
***
*** Transform2D class provides vector transform (matrix)
*** operations applicable to raw vertex buffers.
***
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "transform2d.h"

namespace vt_video
{

Transform2D::Transform2D()
{
    Reset();
}

Transform2D::Transform2D(float x, float y)
{
    Reset();
    Translate(x, y);
}

Transform2D::Transform2D(float x, float y, float sx, float sy)
{
    Reset();
    Translate(x, y);
    Scale(sx, sy);
}

Transform2D::Transform2D(float x, float y, float sx, float sy, float angle)
{
    Reset();
    Translate(x, y);
    Scale(sx, sy);
    Rotate(angle);
}

void Transform2D::Translate(float x, float y)
{
    _x += x;
    _y += y;
}

void Transform2D::Scale(float sx, float sy)
{
    _m00 *= sx;
    _m11 *= sy;
}

void Transform2D::Rotate(float angle)
{
    float cosa = cosf(angle);
    float sina = sinf(angle);
    float m00 = _m00 * cosa + _m01 * sina;
    float m01 = _m00 * -sina + _m01 * cosa;
    float m10 = _m10 * cosa + _m11 * sina;
    float m11 = _m10 * -sina + _m11 * cosa;
    _m00 = m00;
    _m01 = m01;
    _m10 = m10;
    _m11 = m11;
}

void Transform2D::Reset()
{
    ResetRotation();
    ResetTranslation();
}

void Transform2D::ResetRotation()
{
    _m00 = 1;
    _m01 = 0;
    _m10 = 0;
    _m11 = 1;
}

void Transform2D::ResetTranslation()
{
    _x = 0;
    _y = 0;
}

void Transform2D::Apply(const void *buffer_in, void *buffer_out, int count, int stride) const
{
    assert(buffer_in != 0);
    assert(buffer_out != 0);
    assert(count > 0);
    assert(stride > 0);
    assert(stride % sizeof(float) == 0);

    const int stridef = stride / sizeof(float);
    const float * elemi = static_cast<const float *>(buffer_in);
    float * elemo = static_cast<float *>(buffer_out);

    for (int i = 0; i < count; ++i) {
        const float x = elemi[0] * _m00 + elemi[1] * _m01 + _x;
        const float y = elemi[0] * _m10 + elemi[1] * _m11 + _y;
        elemo[0] = x;
        elemo[1] = y;
        elemi += 2;
        elemo += 2;
        for (int j = 0; j < stridef - 2; ++j) {
            *elemo++ = *elemi++;
        }
    }
}

}  // namespace vt_video
