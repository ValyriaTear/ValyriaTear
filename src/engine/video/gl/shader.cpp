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

#include "utils/utils_strings.h"

namespace vt_video
{
namespace gl
{

// Constants.
const GLsizei MAX_LOG_LENGTH = 512;

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
            PRINT_ERROR << "Failed to create the shader." << std::endl;
            assert(error == GL_NO_ERROR);
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
            PRINT_ERROR << "Failed to set the shader's source. Shader ID: " <<
                           vt_utils::NumberToString(_shader) <<
                           std::endl;
            assert(error == GL_NO_ERROR);
        }
    }

    // Compile the shader.
    if (!errors) {
        glCompileShader(_shader);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to compile the shader. Shader ID: " <<
                           vt_utils::NumberToString(_shader) <<
                           std::endl;
            assert(error == GL_NO_ERROR);
        }
    }

    // Check for shader syntax errors.
    if (!errors) {
        GLint is_compiled = -1;
        glGetShaderiv(_shader, GL_COMPILE_STATUS, &is_compiled);

        if (is_compiled == 0) { // 0 = failed to compile.
            errors = true;

            // Retrieve the compiler output.
            GLint length = 0;
            glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &length);

            // Allocate space for the log.
            char* log = new char[length];
            memset(log, 0, length);
            glGetShaderInfoLog(_shader, MAX_LOG_LENGTH, &length, log);

            PRINT_ERROR << "Failed to compile the shader. Shader ID: " <<
                           vt_utils::NumberToString(_shader) << " Compiler Output: " <<
                           log << std::endl;

            // Clean up the log.
            delete [] log;
            log = NULL;

            assert(is_compiled != 0);
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
