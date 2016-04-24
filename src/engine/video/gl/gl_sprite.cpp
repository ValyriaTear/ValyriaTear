////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    gl_sprite.cpp
*** \author  Authenticate, James Lammlein
*** \brief   Source file for buffers for a sprite.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "gl_sprite.h"

#include "utils/exception.h"
#include "utils/utils_strings.h"

#ifdef __APPLE__
#   define glBindVertexArray    glBindVertexArrayAPPLE
#   define glGenVertexArrays    glGenVertexArraysAPPLE
#   define glGenerateMipmap     glGenerateMipmapEXT
#   define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif

namespace vt_video
{
namespace gl
{

//
// Constants.
//

const unsigned INDICES[] =
{
    0, 1, 2, // Triangle One.
    0, 2, 3  // Triangle Two.
};

const unsigned VERTICES_PER_SPRITE = 4;
const unsigned INDICES_PER_SPRITE = sizeof(INDICES) / sizeof(*INDICES);
const unsigned POSITIONS_PER_VERTEX = 3;
const unsigned TEXTURE_COORDINATES_PER_VERTEX = 2;
const unsigned COLORS_PER_VERTEX = 4;

Sprite::Sprite() :
    _vao(0),
    _vertex_position_buffer(0),
    _vertex_texture_coordinate_buffer(0),
    _vertex_color_buffer(0),
    _index_buffer(0)
{
    bool errors = false;

    //
    // Initialize the sprite buffers with some default data.
    //

    // The vertex positions.
    const float VERTEX_POSITIONS[] =
    {
        0.0f, 0.0f, 0.0f, // Vertex One.
        0.0f, 1.0f, 0.0f, // Vertex Two.
        1.0f, 1.0f, 0.0f, // Vertex Three.
        1.0f, 0.0f, 0.0f  // Vertex Four.
    };
    assert(sizeof(VERTEX_POSITIONS) / sizeof(*VERTEX_POSITIONS) % POSITIONS_PER_VERTEX == 0);

    // The vertex texture coordinates.
    const float VERTEX_TEXTURE_COORDINATES[] =
    {
        0.0f, 1.0f, // Vertex One.
        1.0f, 1.0f, // Vertex Two.
        1.0f, 0.0f, // Vertex Three.
        0.0f, 0.0f  // Vertex Four.
    };
    assert(sizeof(VERTEX_TEXTURE_COORDINATES) / sizeof(*VERTEX_TEXTURE_COORDINATES) % TEXTURE_COORDINATES_PER_VERTEX == 0);

    // The vertex colors.
    const float VERTEX_COLORS[] =
    {
        1.0f, 1.0f, 1.0f, 1.0f, // Vertex One.
        1.0f, 1.0f, 1.0f, 1.0f, // Vertex Two.
        1.0f, 1.0f, 1.0f, 1.0f, // Vertex Three.
        1.0f, 1.0f, 1.0f, 1.0f  // Vertex Four.
    };
    assert(sizeof(VERTEX_COLORS) / sizeof(*VERTEX_COLORS) % COLORS_PER_VERTEX == 0);

    // Create the vertex array object.
    if (!errors) {
        GLuint arrays[1] = { 0 };
        glGenVertexArrays(1, arrays);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to create the vertex array object." << std::endl;
            assert(error == GL_NO_ERROR);
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
        GLuint buffers[4] = { 0 };
        glGenBuffers(4, buffers);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to create the vertex array object's position, texture coordinate, color, and index buffers. VAO ID: " <<
                           vt_utils::NumberToString(_vao) <<
                           std::endl;
            assert(error == GL_NO_ERROR);
        } else {
            // Store the results.
            _vertex_position_buffer = buffers[0];
            _vertex_texture_coordinate_buffer = buffers[1];
            _vertex_color_buffer = buffers[2];
            _index_buffer = buffers[3];
        }
    }

    // Bind the vertex position buffer.
    if (!errors) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_position_buffer);
    }

    // Set up the vertex position data.
    if (!errors) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_POSITIONS), VERTEX_POSITIONS, GL_DYNAMIC_DRAW);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to store the vertex position data. VAO ID: " <<
                           vt_utils::NumberToString(_vao) << " Buffer ID: " <<
                           vt_utils::NumberToString(_vertex_position_buffer) <<
                           std::endl;
            assert(error == GL_NO_ERROR);
        }
    }

    // Store the vertex position data into slot 0.
    if (!errors) {
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to set the vertex position data attribute pointer. VAO ID: " <<
                           vt_utils::NumberToString(_vao) << " Buffer ID: " <<
                           vt_utils::NumberToString(_vertex_position_buffer) <<
                           std::endl;
            assert(error == GL_NO_ERROR);
        }
    }

    // Enable the attribute index.
    if (!errors) {
        glEnableVertexAttribArray(0);
    }

    // Bind the vertex texture coordinate buffer.
    if (!errors) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_texture_coordinate_buffer);
    }

    // Set up the vertex texture coordinate data.
    if (!errors) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_TEXTURE_COORDINATES), VERTEX_TEXTURE_COORDINATES, GL_DYNAMIC_DRAW);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to store the vertex texture coordinate data. VAO ID: " <<
                           vt_utils::NumberToString(_vao) << " Buffer ID: " <<
                           vt_utils::NumberToString(_vertex_texture_coordinate_buffer) <<
                           std::endl;
            assert(error == GL_NO_ERROR);
        }
    }

    // Store the vertex texture coordinate data into slot 1.
    if (!errors) {
        glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, nullptr);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to set the vertex texture coordinate data attribute pointer. VAO ID: " <<
                           vt_utils::NumberToString(_vao) << " Buffer ID: " <<
                           vt_utils::NumberToString(_vertex_texture_coordinate_buffer) <<
                           std::endl;
            assert(error == GL_NO_ERROR);
        }
    }

    // Enable the attribute index.
    if (!errors) {
        glEnableVertexAttribArray(1);
    }

    // Bind the vertex color buffer.
    if (!errors) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_color_buffer);
    }

    // Set up the vertex color data.
    if (!errors) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_COLORS), VERTEX_COLORS, GL_DYNAMIC_DRAW);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to store the vertex color data. VAO ID: " <<
                           vt_utils::NumberToString(_vao) << " Buffer ID: " <<
                           vt_utils::NumberToString(_vertex_color_buffer) <<
                           std::endl;
            assert(error == GL_NO_ERROR);
        }
    }

    // Store the vertex color data into slot 2.
    if (!errors) {
        glVertexAttribPointer(2, 4, GL_FLOAT, false, 0, nullptr);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to set the vertex color data attribute pointer. VAO ID: " <<
                           vt_utils::NumberToString(_vao) << " Buffer ID: " <<
                           vt_utils::NumberToString(_vertex_color_buffer) <<
                           std::endl;
            assert(error == GL_NO_ERROR);
        }
    }

    // Enable the attribute index.
    if (!errors) {
        glEnableVertexAttribArray(2);
    }

    // Bind the index buffer.
    if (!errors) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);
    }

    // Set up the index data.
    if (!errors) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            PRINT_ERROR << "Failed to store the index data. VAO ID: " <<
                           vt_utils::NumberToString(_vao) << " Buffer ID: " <<
                           vt_utils::NumberToString(_index_buffer) <<
                           std::endl;
            assert(error == GL_NO_ERROR);
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

    if (_vertex_color_buffer != 0) {
        const GLuint buffers[] = { _vertex_color_buffer };
        glDeleteBuffers(1, buffers);
        _vertex_color_buffer = 0;
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
    glDrawElements(GL_TRIANGLES, INDICES_PER_SPRITE, GL_UNSIGNED_INT, nullptr);

    // Unbind the vertex array object from the pipeline.
    glBindVertexArray(0);

    // Unbind the active buffers from the pipeline.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Sprite::Draw(float* vertex_positions,
                  float* vertex_texture_coordinates,
                  float* vertex_colors)
{
    bool errors = false;

    assert(vertex_positions != nullptr);
    assert(vertex_texture_coordinates != nullptr);
    assert(vertex_colors != nullptr);

    // Bind the vertex position buffer.
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_position_buffer);

    // Update the vertex position data.
    glBufferSubData(GL_ARRAY_BUFFER, 0, VERTICES_PER_SPRITE * POSITIONS_PER_VERTEX * sizeof(float), vertex_positions);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        errors = true;
        PRINT_ERROR << "Failed to update the vertex position data. VAO ID: " <<
                        vt_utils::NumberToString(_vao) << " Buffer ID: " <<
                        vt_utils::NumberToString(_vertex_position_buffer) <<
                        std::endl;
        assert(error == GL_NO_ERROR);
    }

    // Bind the vertex texture coordinate buffer.
    if (!errors) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_texture_coordinate_buffer);
    }

    // Update the vertex texture coordinate data.
    if (!errors) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, VERTICES_PER_SPRITE * TEXTURE_COORDINATES_PER_VERTEX * sizeof(float), vertex_texture_coordinates);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to update the vertex texture coordinate data. VAO ID: " <<
                           vt_utils::NumberToString(_vao) << " Buffer ID: " <<
                           vt_utils::NumberToString(_vertex_texture_coordinate_buffer) <<
                           std::endl;
            assert(error == GL_NO_ERROR);
        }
    }

    // Bind the vertex color buffer.
    if (!errors) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_color_buffer);
    }

    // Update the vertex color data.
    if (!errors) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, VERTICES_PER_SPRITE * COLORS_PER_VERTEX * sizeof(float), vertex_colors);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to update the vertex color data. VAO ID: " <<
                           vt_utils::NumberToString(_vao) << " Buffer ID: " <<
                           vt_utils::NumberToString(_vertex_color_buffer) <<
                           std::endl;
            assert(error == GL_NO_ERROR);
        }
    }

    // Unbind the buffers from the pipeline.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Draw the sprite.
    if (!errors) {
        Draw();
    }
}

Sprite::Sprite(const Sprite&)
{
    throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
}

Sprite& Sprite::operator=(const Sprite&)
{
    throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
    return *this;
}

} // namespace gl

} // namespace vt_video
