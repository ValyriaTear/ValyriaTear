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

#include "utils/utils_strings.h"

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
        assert(error == GL_NO_ERROR);
        if (error != GL_NO_ERROR) {
            PRINT_ERROR << "Failed to create the shader program." << std::endl;
            errors = true;
        }
    }

    // Attach the vertex shader.
    if (!errors) {
        glAttachShader(_program, vertex_shader._shader);

        GLenum error = glGetError();
        assert(error == GL_NO_ERROR);
        if (error != GL_NO_ERROR) {
            PRINT_ERROR << "Failed to attach the vertex shader to the shader program. Shader Program ID: " <<
                           vt_utils::NumberToString(_program) << " Shader ID: " <<
                           vt_utils::NumberToString(vertex_shader._shader) <<
                           std::endl;
            errors = true;
        }
    }

    // Attach the fragment shader.
    if (!errors) {
        glAttachShader(_program, fragment_shader._shader);

        GLenum error = glGetError();
        assert(error == GL_NO_ERROR);
        if (error != GL_NO_ERROR) {
            PRINT_ERROR << "Failed to attach the fragment shader to the shader program. Shader Program ID: " <<
                           vt_utils::NumberToString(_program) << " Shader ID: " <<
                           vt_utils::NumberToString(fragment_shader._shader) <<
                           std::endl;
            errors = true;
        }
    }

    // Bind the attributes.
    if (!errors) {
        std::vector<std::string>::const_iterator i;
        i = attributes.begin();
        GLuint count = 0;
        while (i != attributes.end() && !errors) {
            glBindAttribLocation(_program, count, i->c_str());

            GLenum error = glGetError();
            assert(error == GL_NO_ERROR);
            if (error != GL_NO_ERROR) {
                PRINT_ERROR << "Failed to bind attribute to shader program. Shader Program ID: " <<
                           vt_utils::NumberToString(_program) << " Attribute Location: " <<
                           vt_utils::NumberToString(count) << " Attribute Name: " << *i <<
                           std::endl;
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
        assert(error == GL_NO_ERROR);
        if (error != GL_NO_ERROR) {
            PRINT_ERROR << "Failed to link shader program. Shader Program ID: " <<
                           vt_utils::NumberToString(_program) <<
                           std::endl;
            errors = true;
        }
    }

    // Check for linkage errors.
    if (!errors) {
        GLint is_linked = -1;
        glGetProgramiv(_program, GL_LINK_STATUS, &is_linked);

        assert(is_linked != 0);
        if (is_linked == 0) { // 0 = failed to link.
            // Retrieve the linker output.
            GLint length = -1;
            glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &length);

            // Allocate space for the log.
            char* log = new char[length];
            glGetShaderInfoLog(_program, MAX_LOG_LENGTH, &length, log);

            PRINT_ERROR << "Failed to link the shader program. Shader Program ID: " <<
                           vt_utils::NumberToString(_program) << " Linker Output: " <<
                           log << std::endl;

            // Clean up the log.
            delete [] log;
            log = NULL;

            errors = true;
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
            assert(position == location);
            if (position != location) {
                PRINT_ERROR << "Failed to verify the shader program attribute. Shader Program ID: " <<
                               vt_utils::NumberToString(_program) << " Attribute Name: " << *i <<
                               std::endl;
                errors = true;
            }

            ++location;
            ++i;
        }
    }

    // Get the uniform locations.
    if (!errors) {
        std::vector<std::string>::const_iterator i;
        i = uniforms.begin();
        while (i != uniforms.end() && !errors)
        {
            GLint location = glGetUniformLocation(_program, i->c_str());
            assert(location != -1);
            if (location == -1) { // -1 == failed.
                PRINT_ERROR << "Failed to get the shader program uniform location. Shader Program ID: " <<
                               vt_utils::NumberToString(_program) << " Uniform Name: " << *i <<
                               std::endl;
                errors = true;
            }
            else {
                _uniforms[i->c_str()] = location;
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
    assert(error == GL_NO_ERROR);
    if (error != GL_NO_ERROR) {
        PRINT_ERROR << "Failed to load the shader program. Shader Program ID: " <<
                       vt_utils::NumberToString(_program) <<
                       std::endl;
        result = false;
    }

    return result;
}

bool ShaderProgram::UpdateUniform(const std::string &s, float f)
{
    bool result = true;

    glUniform1f(_uniforms[s], f);

    GLenum error = glGetError();
    assert(error == GL_NO_ERROR);
    if (error != GL_NO_ERROR) {
        PRINT_ERROR << "Failed to update the shader program uniform. Shader Program ID: " <<
                       vt_utils::NumberToString(_program) << " Uniform Name: " << s <<
                       std::endl;
        result = false;
    }

    return result;
}

bool ShaderProgram::UpdateUniform(const std::string &s, int32_t i)
{
    bool result = true;

    glUniform1i(_uniforms[s], i);

    GLenum error = glGetError();
    assert(error == GL_NO_ERROR);
    if (error != GL_NO_ERROR) {
        PRINT_ERROR << "Failed to update the shader program uniform. Shader Program ID: " <<
                       vt_utils::NumberToString(_program) << " Uniform Name: " << s <<
                       std::endl;
        result = false;
    }

    return result;
}

bool ShaderProgram::UpdateUniform(const std::string &s, const float* data, unsigned length)
{
    bool result = false;

    // This function currently only supports matrices and vectors.
    assert(data != NULL && (length == 4 || length == 16));
    if (data != NULL) {
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
    assert(error == GL_NO_ERROR);
    if (error != GL_NO_ERROR) {
        PRINT_ERROR << "Failed to update the shader program uniform. Shader Program ID: " <<
                       vt_utils::NumberToString(_program) << " Uniform Name: " << s <<
                       std::endl;
        result = false;
    }

    return result;
}

} // namespace gl

} // namespace vt_video
