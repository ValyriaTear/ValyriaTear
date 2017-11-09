////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    gl_shader.h
*** \author  Authenticate, James Lammlein
*** \brief   Header file for shaders used in OpenGL.
*** ***************************************************************************/

#ifndef __GL_SHADER_HEADER__
#define __GL_SHADER_HEADER__

#include "utils/gl_include.h"

#include <string>

namespace vt_video
{
namespace gl
{

// Forward declarations.
class ShaderProgram;

//! \brief A class for an OpenGL shader.
class Shader
{
    friend class ShaderProgram;

public:
    Shader(GLenum type, const std::string &data);
    ~Shader();

private:
    GLuint _shader;

    //
    // The copy constructor and assignment operator are hidden by design
    // to cause compilation errors when attempting to copy or assign this class.
    //

    Shader(const Shader& shader);
    Shader& operator=(const Shader& shader);
};

} // namespace gl

} // namespace vt_video

#endif // __GL_SHADER_HEADER__
