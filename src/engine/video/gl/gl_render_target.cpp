////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    gl_render_target.cpp
*** \author  Authenticate, James Lammlein
*** \brief   Source file for buffers for a render target.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "gl_render_target.h"

#include "engine/video/video_utils.h"

#include "utils/exception.h"
#include "utils/utils_strings.h"

namespace vt_video
{
namespace gl
{

RenderTarget::RenderTarget(unsigned width,
                           unsigned height) :
    _width(width),
    _height(height),
    _framebuffer(0),
    _texture(0),
    _renderbuffer_depth(0)
{
    assert(_width > 0);
    assert(_height > 0);

    // Enforce the standard video resolution as the minimum dimensions.
    if (_width < vt_video::VIDEO_STANDARD_RES_WIDTH) {
        _width = vt_video::VIDEO_STANDARD_RES_WIDTH;
    }

    if (_height < vt_video::VIDEO_STANDARD_RES_HEIGHT) {
        _height = vt_video::VIDEO_STANDARD_RES_HEIGHT;
    }

    // Create the framebuffer.
    GLuint framebuffers[1] = { 0 };
    glGenFramebuffers(1, framebuffers);

    if (glGetError() == GL_NO_ERROR) {
        // Store the result.
        _framebuffer = framebuffers[0];
    }
    else {
        PRINT_ERROR << "Failed to create the framebuffer." << std::endl;
        throw "Failed to create the framebuffer.";
    }

    // Bind the framebuffer.
    Bind();

    // Create the texture.
    GLuint textures[1] = { 0 };
    glGenTextures(1, textures);

    if (glGetError() == GL_NO_ERROR) {
        // Store the result.
        _texture = textures[0];
    }
    else {
        PRINT_ERROR << "Failed to create the texture." << std::endl;
        throw "Failed to create the texture.";
    }

    // Bind the texture.
    BindTexture();

    // Initialize the texture.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    if (glGetError() != GL_NO_ERROR) {
        PRINT_ERROR << "Failed to initialize the texture." << std::endl;
        throw "Failed to initialize the texture.";
    }

    // Initialize the texture filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Bind the texture to the framebuffer.
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

    if (glGetError() != GL_NO_ERROR) {
        PRINT_ERROR << "Failed to bind the texture to the framebuffer." << std::endl;
        throw "Failed to bind the texture to the framebuffer.";
    }

    // Create the depth renderbuffer.
    GLuint renderbuffers[1] = { 0 };
    glGenRenderbuffers(1, renderbuffers);

    if (glGetError() == GL_NO_ERROR) {
        // Store the result.
        _renderbuffer_depth = renderbuffers[0];
    }
    else {
        PRINT_ERROR << "Failed to create the depth renderbuffer." << std::endl;
        throw "Failed to create the depth renderbuffer.";
    }

    // Bind the depth renderbuffer.
    glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer_depth);

    // Initialize the depth renderbuffer.
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, _width, _height);

    if (glGetError() != GL_NO_ERROR) {
        PRINT_ERROR << "Failed to initialize the depth renderbuffer." << std::endl;
        throw "Failed to initialize the depth renderbuffer.";
    }

    // Bind the depth renderbuffer to the framebuffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _renderbuffer_depth);

    if (glGetError() != GL_NO_ERROR) {
        PRINT_ERROR << "Failed to bind the depth renderbuffer to the framebuffer." << std::endl;
        throw "Failed to bind the depth renderbuffer to the framebuffer.";
    }

    // Perform a final verification.
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        PRINT_ERROR << "Unable to create the framebuffer." << std::endl;
        throw "Unable to create the framebuffer.";
    }

    // Unbind all textures and buffers from the pipeline.
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget()
{
    if (_framebuffer != 0) {
        const GLuint framebuffers[] = { _framebuffer };
        glDeleteFramebuffers(1, framebuffers);
        _framebuffer = 0;
    }

    if (_texture != 0) {
        const GLuint textures[] = { _texture };
        glDeleteTextures(1, textures);
        _texture = 0;
    }

    if (_renderbuffer_depth != 0) {
        const GLuint renderbuffers[] = { _renderbuffer_depth };
        glDeleteRenderbuffers(1, renderbuffers);
        _renderbuffer_depth = 0;
    }
}

void RenderTarget::Bind()
{
    assert(_framebuffer != 0);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
}

void RenderTarget::BindTexture()
{
    assert(_texture != 0);
    glBindTexture(GL_TEXTURE_2D, _texture);
}

void RenderTarget::Resize(unsigned width,
                          unsigned height)
{
    bool errors = false;

    _width = width;
    _height = height;

    assert(_width > 0);
    assert(_height > 0);

    // Enforce the standard video resolution as the minimum dimensions.
    if (_width < vt_video::VIDEO_STANDARD_RES_WIDTH) {
        _width = vt_video::VIDEO_STANDARD_RES_WIDTH;
    }

    if (_height < vt_video::VIDEO_STANDARD_RES_HEIGHT) {
        _height = vt_video::VIDEO_STANDARD_RES_HEIGHT;
    }

    assert(_framebuffer != 0);

    // Unbind the framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    assert(_texture != 0);

    // Bind the texture.
    if (!errors) {
        BindTexture();
    }

    // Resize the texture.
    if (!errors) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to resize the texture." << std::endl;
            assert(error == GL_NO_ERROR);
        }
    }

    // Bind the depth renderbuffer.
    if (!errors) {
        glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer_depth);
    }

    // Resize the depth renderbuffer.
    if (!errors) {
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, _width, _height);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            errors = true;
            PRINT_ERROR << "Failed to resize the depth renderbuffer." << std::endl;
            assert(error == GL_NO_ERROR);
        }
    }

    // Unbind all textures and buffers from the pipeline.
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned RenderTarget::GetWidth() const
{
    return _width;
}

unsigned RenderTarget::GetHeight() const
{
    return _height;
}

RenderTarget::RenderTarget(const RenderTarget&)
{
    throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
}

RenderTarget& RenderTarget::operator=(const RenderTarget&)
{
    throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
    return *this;
}

} // namespace gl

} // namespace vt_video
