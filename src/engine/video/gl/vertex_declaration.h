////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    vertex_declaration.h
*** \author  Authenticate, James Lammlein
*** \brief   Header file for vertex structures used in OpenGL.
*** ***************************************************************************/

#ifndef __VERTEX_DECLARATION_HEADER__
#define __VERTEX_DECLARATION_HEADER__

namespace vt_video
{
namespace gl
{

//! \brief A structure for a sprite vertex.
struct SpriteVertex
{
    float x, y, z;     // Position
    float s, t;        // Texture
};

//! \brief A structure for a text vertex.
struct TextVertex
{
    float x, y, z;     // Position
    float s, t;        // Texture
    float r, g, b, a;  // Color
};

} // namespace gl

} // namespace vt_video

#endif
