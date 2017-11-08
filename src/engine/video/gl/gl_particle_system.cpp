////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    gl_particle_system.cpp
*** \author  Authenticate, James Lammlein
*** \brief   Source file for buffers for a particle system.
*** ***************************************************************************/

#include "common/include_pch.h"
#include "gl_particle_system.h"

#include "utils/exception.h"
#include "utils/utils_strings.h"
#include "utils/utils_common.h"

namespace vt_video
{
namespace gl
{

//
// Constants.
//

const unsigned VERTICES_PER_PARTICLE = 4;
const unsigned INDICES_PER_PARTICLE = 6;
const unsigned POSITIONS_PER_VERTEX = 3;
const unsigned TEXTURE_COORDINATES_PER_VERTEX = 2;
const unsigned COLORS_PER_VERTEX = 4;

#ifdef __APPLE__
#define glBindVertexArray glBindVertexArrayAPPLE
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glGenerateMipmap glGenerateMipmapEXT
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif

ParticleSystem::ParticleSystem() :
    _number_of_indices(0),
    _vao(0),
    _vertex_position_buffer(0),
    _vertex_texture_coordinate_buffer(0),
    _vertex_color_buffer(0),
    _index_buffer(0)
{
    bool errors = false;

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
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

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
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

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
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

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
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        _number_of_indices = 0;

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

ParticleSystem::~ParticleSystem()
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

void ParticleSystem::Draw()
{
    // Bind the vertex array object.
    glBindVertexArray(_vao);

    // Bind the index buffer.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);

    // Draw the particle system.
    glDrawElements(GL_TRIANGLES, _number_of_indices, GL_UNSIGNED_INT, nullptr);

    // Unbind the vertex array object from the pipeline.
    glBindVertexArray(0);

    // Unbind the active buffers from the pipeline.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ParticleSystem::Draw(float* vertex_positions,
                          float* vertex_texture_coordinates,
                          float* vertex_colors,
                          unsigned number_of_vertices)
{
    bool errors = false;

    assert(vertex_positions != nullptr);
    assert(vertex_texture_coordinates != nullptr);
    assert(vertex_colors != nullptr);
    assert(number_of_vertices % VERTICES_PER_PARTICLE == 0);

    // Bind the vertex position buffer.
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_position_buffer);

    // Update the vertex position data.
    if (!errors) {
        glBufferData(GL_ARRAY_BUFFER,
                     number_of_vertices * POSITIONS_PER_VERTEX * sizeof(float),
                     vertex_positions,
                     GL_DYNAMIC_DRAW);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to update the vertex position data. VAO ID: " <<
                           vt_utils::NumberToString(_vao) << " Buffer ID: " <<
                           vt_utils::NumberToString(_vertex_position_buffer) <<
                           std::endl;
            assert(error == GL_NO_ERROR);
        }
    }

    // Bind the vertex texture coordinate buffer.
    if (!errors) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_texture_coordinate_buffer);
    }

    // Update the vertex texture coordinate data.
    if (!errors) {
        glBufferData(GL_ARRAY_BUFFER,
                     number_of_vertices * TEXTURE_COORDINATES_PER_VERTEX * sizeof(float),
                     vertex_texture_coordinates,
                     GL_DYNAMIC_DRAW);

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
        glBufferData(GL_ARRAY_BUFFER,
                     number_of_vertices * COLORS_PER_VERTEX * sizeof(float),
                     vertex_colors,
                     GL_DYNAMIC_DRAW);

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

    // Create the index buffer's data.
    std::vector<unsigned> indices;
    indices.reserve(number_of_vertices / VERTICES_PER_PARTICLE * INDICES_PER_PARTICLE);
    if (!errors) {
        // For each particle...
        unsigned number_of_particles = number_of_vertices / VERTICES_PER_PARTICLE;
        for (unsigned i = 0; i < number_of_particles; ++i)
        {
            // Compute the starting index of the particle.
            unsigned index = i * VERTICES_PER_PARTICLE;

            //
            // Store the particle's indices.
            //

            // Triangle one.
            indices.push_back(index + 0);
            indices.push_back(index + 1);
            indices.push_back(index + 2);

            // Triangle two.
            indices.push_back(index + 0);
            indices.push_back(index + 2);
            indices.push_back(index + 3);
        }
    }

    // Bind the index buffer.
    if (!errors) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);
    }

    // Update the index data.
    if (!errors) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(unsigned),
                     &indices.front(),
                     GL_DYNAMIC_DRAW);
        _number_of_indices = indices.size();

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to update the index data. VAO ID: " <<
                           vt_utils::NumberToString(_vao) << " Buffer ID: " <<
                           vt_utils::NumberToString(_index_buffer) <<
                           std::endl;
            assert(error == GL_NO_ERROR);
        }
    }

    // Unbind the buffers from the pipeline.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Draw the particle system.
    if (!errors) {
        Draw();
    }
}

ParticleSystem::ParticleSystem(const ParticleSystem&)
{
    throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
}

ParticleSystem& ParticleSystem::operator=(const ParticleSystem&)
{
    throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
    return *this;
}

} // namespace gl

} // namespace vt_video
