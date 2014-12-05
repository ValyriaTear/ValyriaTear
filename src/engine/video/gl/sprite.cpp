////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    sprite.cpp
*** \author  Authenticate, James Lammlein
*** \brief   Source file for buffers for a sprite.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "sprite.h"

namespace vt_video
{
namespace gl
{

Sprite::Sprite(const std::vector<float>& vertex_positions,
               const std::vector<float>& vertex_texture_coordinates,
               const std::vector<unsigned>& indices) :
    _number_of_indices(0),
    _vao(0),
    _vertex_position_buffer(0),
    _vertex_texture_coordinate_buffer(0),
    _index_buffer(0)
{
    bool errors = false;

    assert(!vertex_positions.empty() && vertex_positions.size() % 3 == 0);
    assert(!vertex_texture_coordinates.empty() && vertex_texture_coordinates.size() % 2 == 0);
    assert(!indices.empty() && indices.size() % 3 == 0);

    // Create the vertex array object.
    if (!errors) {
        GLuint arrays[1] = { 0 };
        glGenVertexArrays(1, arrays);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        } else {
            // Store the result.
            _vao = arrays[0];
        }
    }

    // Bind the vertex array object.
    if (!errors) {
        glBindVertexArray(_vao);
    }

    // Create the vertex buffer objects.
    if (!errors) {
        GLuint buffers[3] = { 0 };
        glGenBuffers(3, buffers);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        } else {
            // Store the results.
            _vertex_position_buffer = buffers[0];
            _vertex_texture_coordinate_buffer = buffers[1];
            _index_buffer = buffers[2];
        }
    }

    // Bind the vertex buffer.
    if (!errors) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_position_buffer);
    }

    // Set up the vertex data.
    if (!errors) {
        glBufferData(GL_ARRAY_BUFFER, vertex_positions.size() * sizeof(float), &vertex_positions.front(), GL_DYNAMIC_DRAW);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Store the vertex data into slot 0.
    if (!errors) {
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, NULL);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Enable the attribute index.
    if (!errors) {
        glEnableVertexAttribArray(0);
    }

    // Bind the texture coordinate buffer.
    if (!errors) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_texture_coordinate_buffer);
    }

    // Set up the texture coordinate data.
    if (!errors) {
        glBufferData(GL_ARRAY_BUFFER, vertex_texture_coordinates.size() * sizeof(float), &vertex_texture_coordinates.front(), GL_DYNAMIC_DRAW);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Store the vertex data into slot 1.
    if (!errors) {
        glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, NULL);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Enable the attribute index.
    if (!errors) {
        glEnableVertexAttribArray(1);
    }

    // Bind the index buffer.
    if (!errors) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);
    }

    // Set up the index data.
    if (!errors) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices.front(), GL_STATIC_DRAW);
        _number_of_indices = indices.size();

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Unbind the vertex array object from the pipeline.
    glBindVertexArray(0);

    // Unbind the active buffers from the pipeline.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Sprite::~Sprite()
{
    if (_vao != 0) {
        const GLuint arrays[] = { _vao };
        glDeleteVertexArrays(1, arrays);
        _vao = 0;
    }

    if (_vertex_position_buffer != 0) {
        const GLuint buffers[] = { _vertex_position_buffer };
        glDeleteBuffers(1, buffers);
        _vertex_position_buffer = 0;
    }

    if (_vertex_texture_coordinate_buffer != 0) {
        const GLuint buffers[] = { _vertex_texture_coordinate_buffer };
        glDeleteBuffers(1, buffers);
        _vertex_texture_coordinate_buffer = 0;
    }

    if (_index_buffer != 0) {
        const GLuint buffers[] = { _index_buffer };
        glDeleteBuffers(1, buffers);
        _index_buffer = 0;
    }
}

void Sprite::Draw()
{
    // Bind the vertex array object.
    glBindVertexArray(_vao);

    // Bind the index buffer.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);

    // Draw the sprite.
    glDrawElements(GL_TRIANGLES, _number_of_indices, GL_UNSIGNED_INT, NULL);

    // Unbind the vertex array object from the pipeline.
    glBindVertexArray(0);

    // Unbind the active buffers from the pipeline.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Sprite::Draw(const std::vector<float>& vertex_positions,
                  const std::vector<float>& vertex_texture_coordinates)
{
    bool errors = false;

    assert(!vertex_positions.empty() && vertex_positions.size() % 3 == 0);
    assert(!vertex_texture_coordinates.empty() && vertex_texture_coordinates.size() % 2 == 0);

    // Bind the vertex buffer.
    if (!errors) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_position_buffer);
    }

    // Update the vertex data.
    if (!errors) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_positions.size() * sizeof(float), &vertex_positions.front());

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Bind the texture coordinate buffer.
    if (!errors) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_texture_coordinate_buffer);
    }

    // Update the texture coordinate data.
    if (!errors) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_texture_coordinates.size() * sizeof(float), &vertex_texture_coordinates.front());

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Unbind the texture coordinate buffer from the pipeline.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Draw the sprite.
    if (!errors) {
        Draw();
    }
}

} // namespace gl

} // namespace vt_video
