////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    sprite_colored.h
*** \author  Authenticate, James Lammlein
*** \brief   Header file for buffers for a colored sprite.
*** ***************************************************************************/

#ifndef __SPRITE_COLORED_HEADER__
#define __SPRITE_COLORED_HEADER__

namespace vt_video
{
namespace gl
{

//! \brief A class for a colored sprite.
class SpriteColored
{
public:
    SpriteColored();
    ~SpriteColored();

    void Draw();
    void Draw(const std::vector<float>& vertex_positions,
              const std::vector<float>& vertex_colors);

    unsigned _number_of_indices;

    GLuint _vao;
    GLuint _vertex_position_buffer;
    GLuint _vertex_color_buffer;
    GLuint _index_buffer;

private:
    SpriteColored(const SpriteColored&) {}
    SpriteColored& operator=(const SpriteColored&) { return *this; }
};

} // namespace gl

} // namespace vt_video

#endif
