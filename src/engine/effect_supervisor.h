///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file   effect_supervisor.h
*** \author Yohann Ferreira, yohann ferreira orange fre
*** \brief  Source file for managing visual ambient effects
*** **************************************************************************/

#ifndef __EFFECT_SUPERVISOR_HEADER__
#define __EFFECT_SUPERVISOR_HEADER__

#include "engine/video/color.h"
#include "engine/video/image.h"
#include "engine/video/shake.h"

namespace vt_mode_manager
{

// Useful common info about ambient effects
// shared between game modes.
struct AmbientOverlayInfo {
    //! Image used as ambient overlay
    std::string filename;
    //! x and y speed of the ambient overlay (in pixel per second)
    float x_speed;
    float y_speed;
    //! Current shifting state
    float x_shift;
    float y_shift;
    //! Tells whether the ambient effect is enabled
    bool active;
    //! The camera shifting since the last update
    float x_parallax;
    float y_parallax;
    //! Set whether the parallax effect is active.
    bool is_parallax;
};

struct AmbientLightInfo {
    //! true if ambient lights are enabled
    bool active;

    //! Image used as overlay for ambient lightning
    vt_video::Color color;
};

// Lightnings
struct LightningInfo {
    //! true if a lightning effect is active
    bool active;

    //! Active lightning effect id;
    int16 active_id;

    //! tells whether the lightning effect has to loop
    bool loop;
};

struct AmbientEffectsInfo {
    AmbientOverlayInfo overlay;
    AmbientLightInfo light;
    LightningInfo lightning;
};


//! \brief Supervises the scene effects part (light, ambient texture, lightnings)
class EffectSupervisor
{
public:
    EffectSupervisor();

    ~EffectSupervisor()
    {}

    /** \brief turn on the ligt color for the scene
     * \param color the light color to use
     */
    void EnableLightingOverlay(const vt_video::Color &color);

    /** \brief disables scene lighting
     */
    void DisableLightingOverlay();

    /** \brief Load and enable the textured ambient overlay
    *** the speed x and y factor are used to make the overlay slide on the screen.
    *** \param parallax indicates whether the overlay shifing should counter the camera movement,
    *** thus creating a parallax effect.
    **/
    void EnableAmbientOverlay(const std::string &filename,
                              float x_speed, float y_speed,
                              bool parallax = false);

    //! \brief disables the textured ambient overlay
    void DisableAmbientOverlay();

    //! \brief Adds to the ovleray parallax values. Used by the map mode when the camera is moving.
    void AddParallax(float x, float y) {
        _info.overlay.x_parallax += x;
        _info.overlay.y_parallax += y;
    }

    //! \brief Gets the camera pixel movement from last update
    //! Useful to simulate parallax effects at the scripting level.
    float GetCameraXMovement() const
    { return _info.overlay.x_parallax; }
    float GetCameraYMovement() const
    { return _info.overlay.y_parallax; }

    /** \brief Enable the lightning overlay
     * \param id the lighning effect id (See the lightning effect lua script)
     * \param loop Whether the effect should be looped.
     */
    void EnableLightning(int16 id, bool loop = false);

    /** \brief disables the lightning overlay
     */
    void DisableLightning() {
        _info.lightning.active = false;
    }

    /** \brief Update the timer-based ambient effects.
    *** \param frame_time the time in ms that has passed since the last call.
    **/
    void Update(uint32 frame_time);

    /** \brief call after all map images are drawn to apply lighting and texture overlays.
     *         All menu and text rendering should occur AFTER this call, so that
     *         they are not affected by lighting.
     */
    void DrawEffects();

    /** \brief disables all the active overlay effects, including particles and halos
     *  (useful when switching modes)
     */
    void DisableEffects();

    /** \brief Adds a new shaking effect to the screen
    ***
    *** \param force The initial force of the shake
    *** \param falloff_time The number of milliseconds that the effect should last for. 0 indicates infinite time.
    *** \param falloff_method Specifies the method of falloff. The default is VIDEO_FALLOFF_NONE.
    *** \note If you want to manually control when the shaking stops, set the falloff_time to zero
    *** and the falloff_method to VIDEO_FALLOFF_NONE.
    **/
    void ShakeScreen(float force, uint32 falloff_time, ShakeFalloff falloff_method = SHAKE_FALLOFF_NONE);

    //! \brief Terminates all current screen shake effects
    void StopShaking() {
        _shake_forces.clear();
        _x_shake = 0.0f;
        _y_shake = 0.0f;
    }

    bool IsScreenShaking() const {
        return !_shake_forces.empty();
    }

    //! \brief Give back the shaking offsets
    void GetShakingOffsets(float &shake_x, float &shake_y) {
        shake_x = _x_shake;
        shake_y = _y_shake;
    }

private:
    /** \brief Load the lightning effects lua script
     *  \param script_file a lua script file which contains lightning intensities stored
     *  as floats (0.0 - 1.0f). and sound events for several lightning effects.
     * \return success/failure
     */
    bool _LoadLightnings(const std::string &script_file);

    /** \brief call this every frame to draw any lightning effects. You should make
     *         sure to place this call in an appropriate spot. In particular, you should
     *         draw the lightning before drawing the GUI. The lightning is drawn by
     *         using a fullscreen overlay.
     */
    void _DrawLightning();

    /** \brief updated all the lightning related timers and trigger sounds events.
    *** \param frame_time The number of milliseconds that have elapsed for the current rendering frame
     */
    void _UpdateLightning(uint32 frame_time);

    /** \brief updated all the ambient overlay related timers
    *** \param frame_time The number of milliseconds that have elapsed for the current rendering frame
     */
    void _UpdateAmbientOverlay(uint32 frame_time);

    //! Image used as ambient overlay
    vt_video::StillImage _ambient_overlay_img;

    //! Image used as overlay for ambient lightning
    vt_video::StillImage _light_overlay_img;

    struct LightningVideoManagerInfo {
        //! Image used as overlay for lightning effects
        vt_video::StillImage _lightning_overlay_img;

        //! current time of currently active lightning effect (time since it started in ms)
        int32 _lightning_current_time;

        //! how many milliseconds to do the lightning effects for
        std::vector<int32> _lightning_end_times;

        //! intensity data for lightning effects
        std::vector< std::vector <float> > _lightning_data;

        struct lightning_sound_event {
            // The time to trigger the sound at, in ms.
            int32 time;
            // The sound event filename
            std::string sound_filename;
        };
        //! The sound events to play when displaying a lightning effect.
        std::vector< std::vector<lightning_sound_event> > _lightning_sound_events;

        /** the current lightning sound event vector, made for efficiency,
        *** And used to play the sound for the current lightning effect.
        **/
        std::deque<lightning_sound_event> _current_lightning_sound_events;
    } _lightning_inner_info;

    AmbientEffectsInfo _info;

    // Shaking screen related members
    //! current shake forces affecting screen
    std::deque<ShakeForce> _shake_forces;

    //! X offset to shake the screen by (if any)
    float _x_shake;

    //! Y offset to shake the screen by (if any)
    float _y_shake;

    /** \brief Updates all active shaking effects
    *** \param frame_time The number of milliseconds that have elapsed
    *** for the current rendering frame
    **/
    void _UpdateShake(uint32 frame_time);
};

} // namespace vt_mode_manager

#endif
