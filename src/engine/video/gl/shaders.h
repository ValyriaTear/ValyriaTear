////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shaders.h
*** \author  Authenticate, James Lammlein
*** \brief   Header file for shader definitions.
*** ***************************************************************************/

#ifndef __SHADERS_HEADER__
#define __SHADERS_HEADER__

namespace vt_video
{
namespace gl
{
namespace shaders
{

enum Shaders
{
    VertexParticle = 0,
    VertexSolid,
    VertexSolidPer,
    VertexSprite,
    FragmentParticle,
    FragmentSolid,
    FragmentSolidPer,
    FragmentSprite,
    FragmentGrayscaleSprite,
    Count
};

} // namespace shaders

} // namespace gl

} // namespace vt_video

#endif
