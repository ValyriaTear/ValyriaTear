////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shader_program.cpp
*** \author  Authenticate, James Lammlein
*** \brief   Source file for shader programs used in OpenGL.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "shader_program.h"

#include "shader.h"

namespace vt_video
{
namespace gl
{

// Constants.
const GLsizei MAX_LOG_LENGTH = 200;

ShaderProgram::ShaderProgram(const Shader& vertex_shader,
                             const Shader& fragment_shader,
                             const std::vector<std::string>& attributes,
                             const std::vector<std::string>& uniforms) :
    _program(-1)
{
    bool errors = false;

    _shaders.push_back(vertex_shader._shader);
    _shaders.push_back(fragment_shader._shader);

    // Create the program.
    if (!errors) {
        _program = glCreateProgram();

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Attach the vertex shader.
    if (!errors) {
        glAttachShader(_program, vertex_shader._shader);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Attach the fragment shader.
    if (!errors) {
        glAttachShader(_program, fragment_shader._shader);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Bind the attributes.
    if (!errors) {
        std::vector<std::string>::const_iterator i;
        i = attributes.begin();
        GLuint count = 0;
        while (i != attributes.end() && !errors){
            glBindAttribLocation(_program, count, i->c_str());

            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                errors = true;
            }

            ++count;
            ++i;
        }
    }

    // Link the shader program.
    if (!errors) {
        glLinkProgram(_program);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
        }
    }

    // Check for linkage errors.
    if (!errors) {
        GLint is_linked = -1;
        glGetProgramiv(_program, GL_LINK_STATUS, &is_linked);

        if (is_linked == 0) { // 0 = failed to link.
            errors = true;

#           ifdef _DEBUG
                GLint length = -1;
                glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &length);

                char* log = new char[length];
                glGetShaderInfoLog(_program, MAX_LOG_LENGTH, &length, log);
                delete [] log;
#           endif
        }
    }

    // Verify the attributes.
    if (!errors) {
        std::vector<std::string>::const_iterator i;
        i = attributes.begin();
        GLint location = 0;
        while (i != attributes.end() && !errors)
        {
            GLint position = glGetAttribLocation(_program, i->c_str());
            if (position != location) {
                errors = true;
            }

            ++location;
            ++i;
        }
    }

    // Set the uniform locations.
    if (!errors) {
        std::vector<std::string>::const_iterator i;
        i = uniforms.begin();
        while (i != uniforms.end() && !errors)
        {
            GLint location = glGetUniformLocation(_program, i->c_str());
            if (location == -1) { // -1 == failed.
                errors = true;
            }
            else {
                _uniforms.insert(std::unordered_map<std::string, int>::value_type(i->c_str(), location));
            }

            ++i;
        }
    }
}

ShaderProgram::~ShaderProgram()
{
    std::list<GLint>::const_iterator i;
    for (i = _shaders.begin(); i != _shaders.end(); ++i)
    {
        glDetachShader(_program, *i);
    }
    _shaders.clear();

    if (_program != 0) {
        glDeleteProgram(_program);
        _program = 0;
    }
}

bool ShaderProgram::Load()
{
    bool result = true;

    glUseProgram(_program);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        result = false;
    }

    return result;
}

bool ShaderProgram::UpdateUniform(const std::string &s, float f)
{
    bool result = true;

    glUniform1f(_uniforms[s], f);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        result = false;
    }

    return result;
}

bool ShaderProgram::UpdateUniform(const std::string &s, int32_t i)
{
    bool result = true;

    glUniform1i(_uniforms[s], i);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        result = false;
    }

    return result;
}

bool ShaderProgram::UpdateUniform(const std::string &s, const float* data, unsigned length)
{
    bool result = false;

    // This function currently only supports matrices and vectors.
    assert(data != nullptr && (length == 4 || length == 16));
    if (data != nullptr) {
        if (length == 4) {
            // The vector case.
            glUniform4f(_uniforms[s], data[0], data[1], data[2], data[3]);
            result = true;
        }
        else if (length == 16) {
            // The matrix case.
            glUniformMatrix4fv(_uniforms[s], 1, true, data);
            result = true;
        }
    }

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        result = false;
    }

    return result;
}

} // namespace gl

} // namespace vt_video
