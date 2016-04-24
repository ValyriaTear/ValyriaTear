////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    gl_shader.cpp
*** \author  Authenticate, James Lammlein
*** \brief   Source file for shaders used in OpenGL.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "gl_shader.h"

#include "utils/exception.h"
#include "utils/utils_strings.h"

namespace vt_video
{
namespace gl
{

Shader::Shader(GLenum type, const std::string &data) :
    _shader(0)
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
    if (errors)
        return;

    GLint is_compiled = -1;
    glGetShaderiv(_shader, GL_COMPILE_STATUS, &is_compiled);

    // Return if the shader compilation went well
    if (is_compiled != 0)
        return;

    // Retrieve the compiler output.
    GLint length = 0;
    glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &length);

    // Allocate space for the log.
    char* log = new char[length];
    memset(log, 0, length);
    glGetShaderInfoLog(_shader, length, &length, log);

    PRINT_ERROR << "Failed to compile the shader. Shader ID: " <<
                    vt_utils::NumberToString(_shader) << " Compiler Output: " <<
                    log << std::endl;

    // Clean up the log.
    delete [] log;
    log = nullptr;

    assert(is_compiled != 0);
}

Shader::~Shader()
{
    if (_shader != 0) {
        glDeleteShader(_shader);
        _shader = 0;
    }
}

Shader::Shader(const Shader&)
{
    throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
}

Shader& Shader::operator=(const Shader&)
{
    throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
    return *this;
}

} // namespace gl

} // namespace vt_video
