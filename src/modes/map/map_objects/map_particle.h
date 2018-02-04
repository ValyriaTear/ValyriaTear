///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_PARTICLE_OBJECT_HEADER__
#define __MAP_PARTICLE_OBJECT_HEADER__

#include "modes/map/map_objects/map_object.h"

namespace vt_mode_manager {
class ParticleEffect;
}

namespace vt_map
{

namespace private_map
{

/** ****************************************************************************
*** \brief Represents particle object on the map
*** ***************************************************************************/
class ParticleObject : public MapObject
{
public:
    ParticleObject(const std::string& filename, float x, float y, MapObjectDrawLayer layer);
    virtual ~ParticleObject() override;

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static ParticleObject* Create(const std::string& filename, float x, float y, MapObjectDrawLayer layer);

    //! \brief Updates the object's current animation.
    //! \note the actual image resources is handled by the main map object.
    void Update() override;

    //! \brief Draws the object to the screen, if it is visible.
    //! \note the actual image resources is handled by the main map object.
    void Draw() override;

    //! \brief Start or restart the particle effect
    void Stop();

    //! \brief Stop the particle effect
    bool Start();

    //! \brief Tells whether there are particles still alive,
    //! even if the whole particle effect is stopping.
    bool IsAlive() const;

private:
    //! \brief A reference to the current map save animation.
    vt_mode_manager::ParticleEffect* _particle_effect;
}; // class ParticleObject : public MapObject

} // namespace private_map

} // namespace vt_map

#endif // __MAP_PARTICLE_OBJECT_HEADER__
