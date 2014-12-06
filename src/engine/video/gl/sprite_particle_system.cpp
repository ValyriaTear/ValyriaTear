////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    sprite_particle_system.cpp
*** \author  Authenticate, James Lammlein
*** \brief   Source file for buffers for a particle system.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "sprite_particle_system.h"

namespace vt_video
{
namespace gl
{

SpriteParticleSystem::SpriteParticleSystem() :
    _number_of_indices(0),
    _vao(0),
    _vertex_position_buffer(0),
    _vertex_color_buffer(0),
    _vertex_texture_coordinate_buffer(0),
    _index_buffer(0)
{
    bool errors = false;

    // The vertex positions.
    std::vector<float> vertex_positions;

    // Vertex one.
    vertex_positions.push_back(0.0f);
    vertex_positions.push_back(0.0f);
    vertex_positions.push_back(0.0f);

    // Vertex two.
    vertex_positions.push_back(0.0f);
    vertex_positions.push_back(1.0f);
    vertex_positions.push_back(0.0f);

    // Vertex three.
    vertex_positions.push_back(1.0f);
    vertex_positions.push_back(1.0f);
    vertex_positions.push_back(0.0f);

    // Vertex four.
    vertex_positions.push_back(1.0f);
    vertex_positions.push_back(0.0f);
    vertex_positions.push_back(0.0f);

    // The vertex colors.
    std::vector<float> vertex_colors;

    // Vertex one.
    vertex_colors.push_back(0.0f);
    vertex_colors.push_back(0.0f);
    vertex_colors.push_back(0.0f);
    vertex_colors.push_back(0.0f);

    // Vertex two.
    vertex_colors.push_back(0.0f);
    vertex_colors.push_back(0.0f);
    vertex_colors.push_back(0.0f);
    vertex_colors.push_back(0.0f);

    // Vertex three.
    vertex_colors.push_back(0.0f);
    vertex_colors.push_back(0.0f);
    vertex_colors.push_back(0.0f);
    vertex_colors.push_back(0.0f);

    // Vertex four.
    vertex_colors.push_back(0.0f);
    vertex_colors.push_back(0.0f);
    vertex_colors.push_back(0.0f);
    vertex_colors.push_back(0.0f);

    // The vertex texture coordinates.
    std::vector<float> vertex_texture_coordinates;

    // Vertex one.
    vertex_texture_coordinates.push_back(0.0f);
    vertex_texture_coordinates.push_back(1.0f);

    // Vertex two.
    vertex_texture_coordinates.push_back(1.0f);
    vertex_texture_coordinates.push_back(1.0f);

    // Vertex three.
    vertex_texture_coordinates.push_back(1.0f);
    vertex_texture_coordinates.push_back(0.0f);

    // Vertex four.
    vertex_texture_coordinates.push_back(0.0f);
    vertex_texture_coordinates.push_back(0.0f);

    // The indices.
    std::vector<unsigned> indices;

    // The first triangle.
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    // The second triangle.
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

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
        GLuint buffers[4] = { 0 };
        glGenBuffers(4, buffers);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        } else {
            // Store the results.
            _vertex_position_buffer = buffers[0];
            _vertex_color_buffer = buffers[1];
            _vertex_texture_coordinate_buffer = buffers[2];
            _index_buffer = buffers[3];
        }
    }

    // Bind the vertex position buffer.
    if (!errors) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_position_buffer);
    }

    // Set up the vertex position data.
    if (!errors) {
        glBufferData(GL_ARRAY_BUFFER, vertex_positions.size() * sizeof(float), &vertex_positions.front(), GL_STATIC_DRAW);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Store the vertex position data into slot 0.
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

    // Bind the vertex color buffer.
    if (!errors) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_color_buffer);
    }

    // Set up the vertex color data.
    if (!errors) {
        glBufferData(GL_ARRAY_BUFFER, vertex_colors.size() * sizeof(float), &vertex_colors.front(), GL_STATIC_DRAW);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Store the vertex color data into slot 1.
    if (!errors) {
        glVertexAttribPointer(1, 4, GL_FLOAT, false, 0, NULL);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Enable the attribute index.
    if (!errors) {
        glEnableVertexAttribArray(1);
    }

    // Bind the vertex texture coordinates buffer.
    if (!errors) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_texture_coordinate_buffer);
    }

    // Set up the vertex texture coordinates data.
    if (!errors) {
        glBufferData(GL_ARRAY_BUFFER, vertex_texture_coordinates.size() * sizeof(float), &vertex_texture_coordinates.front(), GL_STATIC_DRAW);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Store the vertex texture coordinates data into slot 2.
    if (!errors) {
        glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, NULL);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
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

SpriteParticleSystem::~SpriteParticleSystem()
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

    if (_vertex_color_buffer != 0) {
        const GLuint buffers[] = { _vertex_color_buffer };
        glDeleteBuffers(1, buffers);
        _vertex_color_buffer = 0;
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

void SpriteParticleSystem::Draw()
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

void SpriteParticleSystem::Draw(float* vertex_positions,
                                float* vertex_colors,
                                float* vertex_texture_coordinates,
                                unsigned number_of_vertices)
{
    bool errors = false;

    // Define some constants.
    const unsigned VERTICES_PER_PARTICLE = 4;
    const unsigned POSITIONS_PER_VERTEX = 3;
    const unsigned COLORS_PER_VERTEX = 4;
    const unsigned TEXTURE_COORDINATES_PER_VERTEX = 2;

    assert(vertex_positions != NULL);
    assert(vertex_colors != NULL);
    assert(vertex_texture_coordinates != NULL);
    assert(number_of_vertices % VERTICES_PER_PARTICLE == 0);

    // Bind the vertex position buffer.
    if (!errors) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_position_buffer);
    }

    // Update the vertex position data.
    if (!errors) {
        glBufferData(GL_ARRAY_BUFFER,
                     number_of_vertices * POSITIONS_PER_VERTEX * sizeof(float),
                     vertex_positions,
                     GL_STATIC_DRAW);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
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
                     GL_STATIC_DRAW);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
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
                     GL_STATIC_DRAW);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Create the index buffer's data.
    std::vector<unsigned> indices;
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
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices.front(), GL_STATIC_DRAW);
        _number_of_indices = indices.size();

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
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

} // namespace gl

} // namespace vt_video
