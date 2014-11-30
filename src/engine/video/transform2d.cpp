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
    _x = x * _m00 + y * _m01 + _x;
    _y = x * _m10 + y * _m11 + _y;
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
    float m00 = cosa * _m00 - sina * _m10;
    float m01 = cosa * _m01 - sina * _m11;
    float m10 = sina * _m00 + cosa * _m10;
    float m11 = sina * _m01 + cosa * _m11;
    _m00 = m00;
    _m01 = m01;
    _m10 = m10;
    _m11 = m11;
}

void Transform2D::Reset()
{
    _m00 = 1;
    _m01 = 0;
    _m10 = 0;
    _m11 = 1;
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

    // fast path for the case of no rotation, worth it?
    if (_m01 == 0) {
        for (int i = 0; i < count; ++i) {
            const float x = elemi[0] * _m00 + _x;
            const float y = elemi[1] * _m11 + _y;
            elemo[0] = x;
            elemo[1] = y;
            elemi += 2;
            elemo += 2;
            for (int j = 0; j < stridef - 2; ++j) {
                *elemo++ = *elemi++;
            }
        }
    } else {
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
}

}  // namespace vt_video
