////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    gl_shader_program.h
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
    ShaderProgram(const Shader* vertex_shader,
                  const Shader* fragment_shader,
                  const std::vector<std::string>& attributes);
    ~ShaderProgram();

    bool Load();

    bool UpdateUniform(const std::string& uniform, float value);
    bool UpdateUniform(const std::string& uniform, int32_t value);
    bool UpdateUniform(const std::string& uniform, const float* data, uint32_t length);

private:
    GLuint _program;

    const Shader* _vertex_shader;
    const Shader* _fragment_shader;

    //! \brief The copy constructor and assignment operator are hidden by design
    //! to cause compilation errors when attempting to copy or assign this class.
    ShaderProgram(const ShaderProgram& shader_program);
    ShaderProgram& operator=(const ShaderProgram& shader_program);
};

} // namespace gl

} // namespace vt_video

#endif
