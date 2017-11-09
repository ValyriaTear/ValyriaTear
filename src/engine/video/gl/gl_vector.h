///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2015 by Valyria Tear Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    gl_vetor.h
*** \author  authenticate
*** \brief   Header file for the Vector class.
***
*** Vector class provides vector operations.
***
*** ***************************************************************************/

#ifndef __GL_VECTOR_HEADER__
#define __GL_VECTOR_HEADER__

namespace vt_video
{
namespace gl
{

// Forward declarations.
class Transform;
class Vector4f;

//! \brief Vector transformation operator.
Vector4f operator*(const Transform& transform, const Vector4f& vector);

class Vector4f
{
    //! \brief Vector transformation operator.
    friend Vector4f operator*(const Transform& transform, const Vector4f& vector);

public:
    //! \brief Default constructor.
    Vector4f();

    //! \brief Constructor.
    Vector4f(float x, float y, float z, float w);

    //! \brief Division-assignment operator.
    Vector4f& operator/=(float scale);

    float _x;
    float _y;
    float _z;
    float _w;
};

} // namespace gl

} // namespace vt_video

#endif // __GL_VECTOR_HEADER__
