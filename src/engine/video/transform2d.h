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
*** \brief   Header file for Transform2D class
***
*** Transform2D class provides vector transform (matrix)
*** operations applicable to raw vertex buffers.
***
*** ***************************************************************************/

#ifndef __TRANSFORM2D_HEADER__
#define __TRANSFORM2D_HEADER__

namespace vt_video
{

class Transform2D
{
public:
    //! \brief Default constructor.
    Transform2D();

    //! \brief Constructor.
    Transform2D(float x, float y);

    //! \brief Constructor.
    Transform2D(float x, float y, float sx, float sy);

    //! \brief Constructor.
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
    float _x;
    float _y;
};

}  // namespace vt_video

#endif // __TRANSFORM2D_HEADER__
