////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shader.cpp
*** \author  Authenticate, James Lammlein
*** \brief   Source file for shaders used in OpenGL.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "shader.h"

namespace vt_video
{
namespace gl
{

// Constants.
const GLsizei MAX_LOG_LENGTH = 200;

Shader::Shader(GLenum type, const std::string &data) :
    _shader(-1)
{
    bool errors = false;

    // Create the shader.
    if (!errors) {
        _shader = glCreateShader(type);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Send the source code to the shader.
    if (!errors) {
        const GLint length[] = { static_cast<GLint>(data.length()) };
        const GLchar* strings[] = { data.c_str() };

        glShaderSource(_shader, 1, strings, length);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Compile the shader.
    if (!errors) {
        glCompileShader(_shader);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Check for shader syntax errors.
    if (!errors) {
        GLint isCompiled = -1;
        glGetShaderiv(_shader, GL_COMPILE_STATUS, &isCompiled);

        if (isCompiled == 0) { // 0 = failed to compile.
            errors = true;

#           ifdef _DEBUG
                GLint length = -1;
                glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &length);
                
                char* log = new char[length];
                glGetShaderInfoLog(_shader, MAX_LOG_LENGTH, &length, log);
                delete [] log;
#           endif
        }
    }
}

Shader::~Shader()
{
    if (_shader != 0) {
        glDeleteShader(_shader);
        _shader = 0;
    }
}

} // namespace gl

} // namespace vt_video
