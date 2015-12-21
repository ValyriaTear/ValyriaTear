////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    gl_render_target.h
*** \author  Authenticate, James Lammlein
*** \brief   Header file for buffers for a render target.
*** ***************************************************************************/

#ifndef __RENDER_TARGET_HEADER__
#define __RENDER_TARGET_HEADER__

namespace vt_video
{
namespace gl
{

//! \brief This class represents a render target.
class RenderTarget
{
public:
    explicit RenderTarget(unsigned width,
                          unsigned height);
    ~RenderTarget();

    //! \brief Binds the render target's framebuffer to the pipeline.
    void Bind();

    //! \brief Binds the render target's texture to the pipeline.
    void BindTexture();

    //! \brief Resizes the render target.
    void Resize(unsigned width,
                unsigned height);

    //! \brief Gets the width of the render target.
    unsigned GetWidth() const;

    //! \brief Gets the height of the render target.
    unsigned GetHeight() const;

private:
    //
    // The copy constructor and assignment operator are hidden by design
    // to cause compilation errors when attempting to copy or assign this class.
    //

    RenderTarget(const RenderTarget& sprite);
    RenderTarget& operator=(const RenderTarget& sprite);

    unsigned _width;
    unsigned _height;

    GLuint _framebuffer;
    GLuint _texture;
    GLuint _renderbuffer_depth;
};

} // namespace gl

} // namespace vt_video

#endif
