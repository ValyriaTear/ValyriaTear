///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2013 by Valyria Tear Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    transform.h
*** \author  logzero
*** \author  authenticate
*** \brief   Source file for the Transform class.
***
*** Transform class provides matrix operations.
***
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "transform.h"

namespace vt_video
{
namespace gl
{

static float ComputeDotProduct(float* a, float* b)
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

Transform::Transform()
{
    Reset();
}

Transform::Transform(float m00, float m01, float m02, float m03,
                     float m10, float m11, float m12, float m13,
                     float m20, float m21, float m22, float m23,
                     float m30, float m31, float m32, float m33)
{
    _row0[0] = m00;
    _row0[1] = m01;
    _row0[2] = m02;
    _row0[3] = m03;

    _row1[0] = m10;
    _row1[1] = m11;
    _row1[2] = m12;
    _row1[3] = m13;

    _row2[0] = m20;
    _row2[1] = m21;
    _row2[2] = m22;
    _row2[3] = m23;

    _row3[0] = m30;
    _row3[1] = m31;
    _row3[2] = m32;
    _row3[3] = m33;
}

void Transform::Translate(float x, float y)
{
    Transform translation;
    translation._row0[3] = x;
    translation._row1[3] = y;

    _Multiply(translation);
}

void Transform::Scale(float sx, float sy)
{
    Transform scale;
    scale._row0[0] = sx;
    scale._row1[1] = sy;

    _Multiply(scale);
}

void Transform::Rotate(float angle)
{
    float cosa = cosf(angle);
    float sina = sinf(angle);

    Transform rotation;
    rotation._row0[0] = cosa;
    rotation._row0[1] = -sina;
    rotation._row1[0] = sina;
    rotation._row1[1] = cosa;

    _Multiply(rotation);
}

void Transform::Reset()
{
    // Clear the rows.
    memset(_row0, 0, sizeof(_row0));
    memset(_row1, 0, sizeof(_row1));
    memset(_row2, 0, sizeof(_row2));
    memset(_row3, 0, sizeof(_row3));

    // Set the identity.
    _row0[0] = 1.0f;
    _row1[1] = 1.0f;
    _row2[2] = 1.0f;
    _row3[3] = 1.0f;
}

void Transform::Apply(const void *buffer_in, void *buffer_out, int count, int stride) const
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
        const float x = elemi[0] * _row0[0] + elemi[1] * _row0[1] + _row0[3];
        const float y = elemi[0] * _row1[0] + elemi[1] * _row1[1] + _row1[3];
        elemo[0] = x;
        elemo[1] = y;
        elemi += 2;
        elemo += 2;
        for (int j = 0; j < stridef - 2; ++j) {
            *elemo++ = *elemi++;
        }
    }
}

void Transform::Apply(float* buffer) const
{
    assert(buffer != nullptr);

    memcpy(buffer, _row0, sizeof(_row0));
    buffer += 4;

    memcpy(buffer, _row1, sizeof(_row1));
    buffer += 4;

    memcpy(buffer, _row2, sizeof(_row2));
    buffer += 4;

    memcpy(buffer, _row3, sizeof(_row3));
}

void Transform::_Multiply(const Transform& transform)
{
    // Allocate space for the result.
    float row0[4] = { 0 };
    float row1[4] = { 0 };
    float row2[4] = { 0 };
    float row3[4] = { 0 };

    float* results[4] = { row0, row1, row2, row3 };

    // Allocate space for the columns.
    float col0[4] = { transform._row0[0], transform._row1[0], transform._row2[0], transform._row3[0] };
    float col1[4] = { transform._row0[1], transform._row1[1], transform._row2[1], transform._row3[1] };
    float col2[4] = { transform._row0[2], transform._row1[2], transform._row2[2], transform._row3[2] };
    float col3[4] = { transform._row0[3], transform._row1[3], transform._row2[3], transform._row3[3] };

    float* rows[4] = { _row0, _row1, _row2, _row3 };
    float* cols[4] = { col0, col1, col2, col3 };

    // For each row...
    for (unsigned i = 0; i < 4; ++i)
    {
        float* row = rows[i];
        float* result = results[i];

        // For each column...
        for (unsigned j = 0; j < 4; ++j)
        {
            float* col = cols[j];

            // Compute and store the result.
            result[j] = ComputeDotProduct(row, col);
        }
    }

    // Store the final results.
    memcpy(_row0, row0, sizeof(row0));
    memcpy(_row1, row1, sizeof(row1));
    memcpy(_row2, row2, sizeof(row2));
    memcpy(_row3, row3, sizeof(row3));
}

} // namespace gl

} // namespace vt_video
