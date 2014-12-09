////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shader.h
*** \author  Authenticate, James Lammlein
*** \brief   Header file for shaders used in OpenGL.
*** ***************************************************************************/

#ifndef __SHADER_HEADER__
#define __SHADER_HEADER__

namespace vt_video
{
namespace gl
{

//! \brief A class for an OpenGL shader.
class Shader
{
public:
    Shader(GLenum type, const std::string &data);
    ~Shader();

    GLint _shader;

private:
    Shader(const Shader&) {}
    Shader& operator=(const Shader&) { return *this; }
};

} // namespace gl

} // namespace vt_video

#endif
