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
*** \brief   Header file for Transform2D class
***
*** Transform2D class provides client side vector transform (matrix)
*** operations applicable to raw vertex buffers.
***
*** ***************************************************************************/

#ifndef __TRANSFORM2D_HEADER__
#define __TRANSFORM2D_HEADER__

#include <cmath>

namespace vt_video
{

class Transform2D
{
public:
    //! \brief Default constructor, sets transform to identity.
    Transform2D(void);

    Transform2D(float x, float y);

    Transform2D(float x, float y, float sx, float sy);

    Transform2D(float x, float y, float sx, float sy, float angle);

    //! \brief Move current transform by x, y.
    void Translate(float x, float y);

    //! \brief Scale current transform by sx, sy.
    void Scale(float sx, float sy);

    //! \brief Rotate current transform by angle.
    void Rotate(float angle);

    //! \brief Reset transform to identity.
    void Reset();

    /** \brief Apply transform matrix to count buffer elements.
    *** \param buffer_in Buffer read from with size = count * stride.
    *** \param buffer_out Buffer written to with size = count * stride.
    *** \param count Number of elements to be processed > 0.
    *** \param stride Size of a single element is n * sizeof(float) with n >= 2.
    **/
    void Apply(const void *buffer_in, void *buffer_out, int count, int stride) const;

private:
    float _m00;
    float _m01;
    float _m10;
    float _m11;
    float _m02;
    float _m12;
};

inline Transform2D::Transform2D(void)
{
    Reset();
}

inline Transform2D::Transform2D(float x, float y)
{
    Reset();
    Translate(x, y);
}

inline Transform2D::Transform2D(float x, float y, float sx, float sy)
{
    Reset();
    Translate(x, y);
    Scale(sx, sy);
}

inline Transform2D::Transform2D(float x, float y, float sx, float sy, float angle)
{
    Reset();
    Translate(x, y);
    Scale(sx, sy);
    Rotate(angle);
}

inline void Transform2D::Translate(float x, float y)
{
    _m02 = x * _m00 + y * _m01 + _m02;
    _m12 = x * _m10 + y * _m11 + _m12;
}

inline void Transform2D::Scale(float sx, float sy)
{
    _m00 *= sx;
    _m11 *= sy;
}

inline void Transform2D::Rotate(float angle)
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

inline void Transform2D::Reset()
{
    _m00 = 1;
    _m01 = 0;
    _m10 = 0;
    _m11 = 1;
    _m02 = 0;
    _m12 = 0;
}

inline void Transform2D::Apply(const void *buffer_in, void *buffer_out, int count, int stride) const
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
            const float x = elemi[0] * _m00 + _m02;
            const float y = elemi[1] * _m11 + _m12;
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
            const float x = elemi[0] * _m00 + elemi[1] * _m01 + _m02;
            const float y = elemi[0] * _m10 + elemi[1] * _m11 + _m12;
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

#endif // __TRANSFORM2D_HEADER__
