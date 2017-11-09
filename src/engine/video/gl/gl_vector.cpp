///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2015 by Valyria Tear Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    gl_vector.cpp
*** \author  authenticate
*** \brief   Source file for the Vector class.
***
*** Vector class provides vector operations.
***
*** ***************************************************************************/

#include "gl_vector.h"

#include "gl_transform.h"

#include <cassert>

namespace vt_video
{
namespace gl
{

static float _ComputeDotProduct(const float* a, const float* b)
{
    float result = 0.0f;

    assert(a != nullptr);
    assert(b != nullptr);

    for (unsigned i = 0; i < 4; ++i)
    {
        result += a[i] * b[i];
    }

    return result;
}

Vector4f operator*(const Transform& transform, const Vector4f& vector)
{
    Vector4f result = Vector4f();

    // Store the vector in an array.
    float vector_inner[4] = { vector._x, vector._y, vector._z, vector._w };

    // Compute the result.
    result._x = _ComputeDotProduct(transform._row0, vector_inner);
    result._y = _ComputeDotProduct(transform._row1, vector_inner);
    result._z = _ComputeDotProduct(transform._row2, vector_inner);
    result._w = _ComputeDotProduct(transform._row3, vector_inner);

    return result;
}

Vector4f::Vector4f() :
    _x(0.0f),
    _y(0.0f),
    _z(0.0f),
    _w(0.0f)
{
}

Vector4f::Vector4f(float x, float y, float z, float w) :
    _x(x),
    _y(y),
    _z(z),
    _w(w)
{
}

Vector4f& Vector4f::operator/=(float scale)
{
    _x /= scale;
    _y /= scale;
    _z /= scale;
    _w /= scale;

    return *this;
}

} // namespace gl

} // namespace vt_video
