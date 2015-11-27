////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
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

#include "utils/exception.h"
#include "utils/utils_strings.h"

namespace vt_video
{
namespace gl
{

RenderTarget::RenderTarget()
{
}

RenderTarget::~RenderTarget()
{
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
