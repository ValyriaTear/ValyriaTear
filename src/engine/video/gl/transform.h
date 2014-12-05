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
*** \brief   Header file for the Transform class.
***
*** Transform class provides matrix operations.
***
*** ***************************************************************************/

#ifndef __TRANSFORM_HEADER__
#define __TRANSFORM_HEADER__

namespace vt_video
{
namespace gl
{

class Transform
{
public:
    //! \brief Default constructor.
    Transform();

    //! \brief Constructor.
    Transform(float m00, float m01, float m02, float m03,
              float m10, float m11, float m12, float m13,
              float m20, float m21, float m22, float m23,
              float m30, float m31, float m32, float m33);

    //! \brief Moves the current transform by x and y.
    void Translate(float x, float y);

    //! \brief Scales the current transform by sx and sy.
    void Scale(float sx, float sy);

    //! \brief Rotates current transform by the angle in degrees.
    void Rotate(float angle);

    //! \brief Resets the transform to the identity.
    void Reset();

    /** \brief Apply transform matrix to count buffer elements.
    *** \param buffer_in Buffer read from with size = count * stride.
    *** \param buffer_out Buffer written to with size = count * stride.
    *** \param count Number of elements to be processed > 0.
    *** \param stride Size of a single element is n * sizeof(float) with n >= 2.
    **/
    void Apply(const void *buffer_in, void *buffer_out, int count, int stride) const;

    //! \brief Applies the transform to the buffer.  The buffer must have at least 16 elements!
    void Apply(float* buffer) const;

private:
    //! \brief A helper function to multiply transforms.
    void _Multiply(const Transform& transform);

    float _row0[4];
    float _row1[4];
    float _row2[4];
    float _row3[4];
};

} // namespace gl

} // namespace vt_video

#endif
