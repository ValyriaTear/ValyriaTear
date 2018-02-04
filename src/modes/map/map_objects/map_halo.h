///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_HALOS_HEADER__
#define __MAP_HALOS_HEADER__

#include "modes/map/map_objects/map_object.h"

namespace vt_map
{

namespace private_map
{

/** ****************************************************************************
*** \brief Represents a halo (source of light) on the map
*** ***************************************************************************/
class Halo : public MapObject
{
public:
    //! \brief setup a halo on the map, using the given animation file.
    Halo(const std::string& filename, float x, float y, const vt_video::Color& color);
    virtual ~Halo() override
    {
    }

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static Halo* Create(const std::string& filename, float x, float y,
                        const vt_video::Color& color);

    //! \brief Updates the object's current animation.
    //! \note the actual image resources is handled by the main map object.
    void Update() override;

    //! \brief Draws the object to the screen, if it is visible.
    //! \note the actual image resources is handled by the main map object.
    void Draw() override;

private:
    //! \brief A reference to the current map save animation.
    vt_video::AnimatedImage _animation;

    //! The blending color of the halo
    vt_video::Color _color;
};

} // namespace private_map

} // namespace vt_map

#endif // __MAP_HALOS_HEADER__
