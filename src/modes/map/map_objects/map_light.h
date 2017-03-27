///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_LIGHT_HEADER__
#define __MAP_LIGHT_HEADER__

#include "modes/map/map_objects/map_object.h"

namespace vt_map
{

namespace private_map
{

/** ****************************************************************************
*** \brief Represents a source of light on the map, changing its orientation
*** according to the camera view.
*** ***************************************************************************/
class Light : public MapObject
{
public:
    //! \brief setup a halo on the map, using the given animation file.
    Light(const std::string &main_flare_filename,
          const std::string &secondary_flare_filename,
          float x, float y,
          const vt_video::Color &main_color, const vt_video::Color &secondary_color);

    virtual ~Light() override
    {
    }

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static Light* Create(const std::string &main_flare_filename,
                         const std::string &secondary_flare_filename,
                         float x, float y,
                         const vt_video::Color &main_color,
                         const vt_video::Color &secondary_color);

    //! \brief Updates the object's current animation and orientation
    //! \note the actual image resources is handled by the main map object.
    void Update();

    //! \brief Draws the object to the screen, if it is visible.
    //! \note the actual image resources is handled by the main map object.
    void Draw();

    /** \brief Returns the image rectangle for the current object
    *** \param rect A MapRectangle object storing the image rectangle data
    **/
    MapRectangle GetGridImageRectangle() const;
private:
    //! Updates the angle and distance from the camera viewpoint
    void _UpdateLightAngle();

    //! \brief A reference to the current light animation.
    vt_video::AnimatedImage _main_animation;
    vt_video::AnimatedImage _secondary_animation;

    //! The blending color of the light
    vt_video::Color _main_color;
    vt_video::Color _secondary_color;

    //! The blending color with dynamic alpha, for better rendering
    vt_video::Color _main_color_alpha;
    vt_video::Color _secondary_color_alpha;

    //! used to compute the flare lines equation.
    float _a, _b;
    //! Distance between the light and the camera viewpoint.
    float _distance;

    //! Random distance factor used to make the secondary flares appear at random places
    float _distance_factor_1;
    float _distance_factor_2;
    float _distance_factor_3;
    float _distance_factor_4;

    /** \brief Used for optimization, keeps the last center position.
    *** So that we update the distance and angle only when this position has changed.
    **/
    MapPosition _last_center_pos;
}; // class Light : public MapObject

} // namespace private_map

} // namespace vt_map

#endif // __MAP_LIGHT_HEADER__
