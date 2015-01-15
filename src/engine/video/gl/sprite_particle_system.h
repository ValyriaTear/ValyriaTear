////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    sprite_particle_system.h
*** \author  Authenticate, James Lammlein
*** \brief   Header file for buffers for a particle system.
*** ***************************************************************************/

#ifndef __PARTICLE_SYSTEM_HEADER__
#define __PARTICLE_SYSTEM_HEADER__

namespace vt_video
{
namespace gl
{

//! \brief A class for a particle system.
class ParticleSystem
{
public:
    ParticleSystem();
    ~ParticleSystem();

    //! \brief Draws all sprites in a particle system.
    void Draw();

    //! \brief Draws all sprites in a particle system.
    void Draw(float* vertex_positions,
              float* vertex_texture_coordinates,
              float* vertex_colors,
              unsigned number_of_vertices);

    unsigned _number_of_indices;

    GLuint _vao;
    GLuint _vertex_position_buffer;
    GLuint _vertex_texture_coordinate_buffer;
    GLuint _vertex_color_buffer;
    GLuint _index_buffer;

private:
    ParticleSystem(const ParticleSystem&) {}
    ParticleSystem& operator=(const ParticleSystem&) { return *this; }
};

} // namespace gl

} // namespace vt_video

#endif
