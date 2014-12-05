////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shader_program.h
*** \author  Authenticate, James Lammlein
*** \brief   Header file for shader programs used in OpenGL.
*** ***************************************************************************/

#ifndef __SHADER_PROGRAM_HEADER__
#define __SHADER_PROGRAM_HEADER__

namespace vt_video
{
namespace gl
{

// Forward declarations.
class Shader;

//! \brief A class for an OpenGL shader program.
class ShaderProgram
{
public:
    ShaderProgram(const Shader& vertex_shader,
                  const Shader& fragment_shader,
                  const std::vector<std::string>& attributes,
                  const std::vector<std::string>& uniforms);
    ~ShaderProgram();

    bool Load();

    bool UpdateUniform(const std::string &s, float f);
    bool UpdateUniform(const std::string &s, int i);
    bool UpdateUniform(const std::string &s, const float* data, unsigned length);

    GLint _program;
    std::list<GLint> _shaders;
    std::map<std::string, int> _uniforms;

private:
    ShaderProgram(const ShaderProgram&) {}
    ShaderProgram& operator=(const ShaderProgram&) { return *this; }
};

} // namespace gl

} // namespace vt_video

#endif
