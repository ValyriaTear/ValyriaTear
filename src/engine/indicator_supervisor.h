////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    indicator_supervisor.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for battle indicator displays.
***
*** This code contains the implementation of various indicators and indicator
*** supporting classes. Indicators are small images and text that appear
*** alongside battle sprites to inform the player about status changes such as
*** damage, healing, and elemental or status effects.
*** ***************************************************************************/

#ifndef __INDICATOR_SUPERVISOR_HEADER__
#define __INDICATOR_SUPERVISOR_HEADER__

#include "engine/video/text.h"

#include "modes/battle/battle_utils.h"

namespace vt_common
{
class ShortNoticeWindow;
}

namespace vt_mode_manager
{

//! \brief The total amount of time (in milliseconds) that the display sequence lasts for indicator elements
const uint32_t INDICATOR_TIME = 3000;

/** \brief the indicator types.
*** According to the indicator type, the draw position computation won't be the same
**/
enum INDICATOR_TYPE {
    DAMAGE_INDICATOR = 0,
    HEALING_INDICATOR = 1,
    TEXT_INDICATOR = 2,
    POSITIVE_STATUS_EFFECT_INDICATOR = 3,
    NEGATIVE_STATUS_EFFECT_INDICATOR = 4,
    ITEM_INDICATOR = 5
};

/** ****************************************************************************
*** \brief An abstract class for displaying information
***
*** Indicators are text or graphics that appear in a game mode.
*** They typically represent changes to the actor such as numeric text representing
*** damage or healing, icons representing status effects, etc. The standard draw
*** sequence for elements comes in three stages to make the indicators presence and
*** disappearance more natural.
*** ***************************************************************************/
class IndicatorElement
{
public:
    //! \param x_position, y_position The indicator base position on screen.
    //! \param indicator_type tells the indicator use in game.
    IndicatorElement(float x_position, float y_position, INDICATOR_TYPE indicator_type);

    virtual ~IndicatorElement()
    {}

    //! \brief Begins the display of the indicator element
    void Start();

    //! \brief Updates the timer and the draw coordinates
    virtual void Update();

    //! \brief Returns a floating point value that represents the height of the element drawn
    virtual float ElementHeight() const = 0;

    //! \brief Draws the indicator information to the screen.
    virtual void Draw()
    {}

    //! \brief Returns true when the indicator element has expired and should be removed
    bool IsExpired() const {
        return _timer.IsFinished();
    }

    //! \brief Returns if the indicator has already been started.
    bool HasStarted() const {
        return _timer.GetTimeExpired() > 0;
    }

    const vt_system::SystemTimer& GetTimer() const {
        return _timer;
    }

    float GetXOrigin() const {
        return _x_origin_position;
    }

    float GetYOrigin() const {
        return _y_origin_position;
    }

    void SetXOrigin(float x) {
        _x_origin_position = x;
    }

    void SetYOrigin(float y) {
        _y_origin_position = y;
    }

    bool UseParallax() const {
        return _use_parallax;
    }

    //! \brief Tells whether the indicator will make use of the camera movement.
    void SetUseParallax(bool use_parallax) {
        _use_parallax = use_parallax;
    }

    INDICATOR_TYPE GetType() const {
        return _indicator_type;
    }
    //@}

protected:
    //! \brief Used to monitor the display progress
    vt_system::SystemTimer _timer;

    //! \brief A modulation color used to modify the alpha (transparency) of the drawn element
    vt_video::Color _alpha_color;

    //! \brief Represent the force currently applied on the effects per second on the x axis.
    float _x_force;

    //! \brief Represent the force currently applied on the effects per second on the y axis.
    float _y_force;

    //! \brief Represent the origin x position of the effect.
    float _x_origin_position;

    //! \brief Represent the origin y position of the effect.
    float _y_origin_position;

    //! \brief Represent the current x position of the effect, relative to its base coordinates.
    float _x_relative_position;

    //! \brief Represent the current y position of the effect, relative to its base coordinates.
    float _y_relative_position;

    //! \brief Permits to apply parallax effects on the indicator.
    //! This is useful when the indicator tries to simulate the camera movement.
    bool _use_parallax;

    /** \brief the indicator type.
    *** According to the indicator type, the draw position computation won't be the same
    **/
    INDICATOR_TYPE _indicator_type;

    //! \brief Updates the draw indicator effect position
    void _UpdateDrawPosition();

    /** \brief Calculates the standard alpha (transparency) value for drawing the element
    *** Calling this function will set the alpha value of the _alpha_color member. Indicator elements
    *** generally fade in and fade out to make their appearance more seamless on the battle field.
    *** Alpha gradually increases from 0.0f to 1.0f in the first stage, remains at 1.0f for a majority
    *** of the time, then gradually decreases back to 0.0f as the display finishes.
    **/
    void _ComputeDrawAlpha();
}; // class IndicatorElement


/** ****************************************************************************
*** \brief Displays an item of text
***
*** Text indicators are normally used to display numeric text representing the
*** amount of damage dealt to the actor or the amount of healing performed. Another
*** common use is to display the word "Miss" when the actor is a target for a skill
*** that did not connect successfully. The style of the rendered text can also be
*** varied and is typically used for drawing text in different colors such as red
*** for damage and green for healing. The text size may be made larger to indicate
*** more powerful or otherwise significant changes as well.
*** ***************************************************************************/
class IndicatorText : public IndicatorElement
{
public:
    /** \param x_position, y_position The indicator base position on screen.
    *** \param text The text to use to render the text image
    *** \param style The style to use to render the text image
    *** \param indicator_type tells the indicator use in game.
    **/
    IndicatorText(float x_position, float y_position,
                  const std::string &text, const vt_video::TextStyle &style,
                  INDICATOR_TYPE indicator_type);

    ~IndicatorText()
    {}

    //! \brief Returns the height of the rendered text image
    float ElementHeight() const {
        return _text_image.GetHeight();
    }

    //! \brief Draws the text image
    void Draw();

protected:
    //! \brief The rendered image of the text to display
    vt_video::TextImage _text_image;
}; // class IndicatorText  : public IndicatorElement



/** ****************************************************************************
*** \brief Displays an image indicator
***
*** This indicator displays a single image and is typically used for illustrating
*** elemental and status effects.
*** ***************************************************************************/
class IndicatorImage : public IndicatorElement
{
public:
    /** \param x_position, y_position The indicator base position on screen.
    *** \param filename The name of the image file to load
    *** \param indicator_type tells the indicator use in game.
    **/
    IndicatorImage(float x_position, float y_position, const std::string &filename,
                   INDICATOR_TYPE indicator_type);

    /** \param actor A valid pointer to the actor object this indicator
    *** \param image A const reference to the loaded image object to display
    *** \param indicator_type tells the indicator use in game.
    **/
    IndicatorImage(float x_position, float y_position, const vt_video::StillImage &image,
                   INDICATOR_TYPE indicator_type);

    ~IndicatorImage()
    {}

    //! \brief Returns the height of the image
    float ElementHeight() const {
        return _image.GetHeight();
    }

    //! \brief Draws the image
    void Draw();

    //! \brief Returns a reference to the image use
    vt_video::StillImage& GetImage() {
        return _image;
    }

protected:
    //! \brief The image to display as an indicator
    vt_video::StillImage _image;
}; // class IndicatorImage : public IndicatorElement



/** ****************************************************************************
*** \brief Displays two images blended together
***
*** This indicator is similar to the IndicatorImage class in that it displays
*** an image. The difference is that it uses two images that blend together to
*** indicate some sort of change, usually for a status or elemental effect. It
*** uses a modified draw sequence from standard indicator elements in five stages.
***
*** -# Transparent fade in of the first image
*** -# First image drawn at full opacity
*** -# Fade out first image while simultaneously fading in second image
*** -# Second image drawn at full opacity
*** -# Transparent fade out of the second image
***
*** \note Both images should share the same dimensions. If they do not this can
*** cause issues as the image height is used to
*** ***************************************************************************/
class IndicatorBlendedImage : public IndicatorElement
{
public:
    /** \param x_position, y_position The indicator base position on screen.
    *** \param first_filename The name of the first image file to load
    *** \param second_filename The name of the second image file to load
    *** \param indicator_type tells the indicator use in game.
    **/
    IndicatorBlendedImage(float x_position, float y_position, const std::string &first_filename,
                          const std::string &second_filename,
                          INDICATOR_TYPE indicator_type);

    /** \param x_position, y_position The indicator base position on screen.
    *** \param first_image A const reference to the first loaded image to display
    *** \param second_image A const reference to the second loaded image to display
    *** \param indicator_type tells the indicator use in game.
    **/
    IndicatorBlendedImage(float x_position, float y_position, const vt_video::StillImage &first_image,
                          const vt_video::StillImage &second_image,
                          INDICATOR_TYPE indicator_type);

    ~IndicatorBlendedImage()
    {}

    //! \brief Returns the height of the blended image
    float ElementHeight() const {
        return _first_image.GetHeight();
    }

    //! \brief Draws the first and/or second image blended appropriately
    void Draw();

    //! \brief Returns a reference to the first image
    vt_video::StillImage& GetFirstImage() {
        return _first_image;
    }

    //! \brief Returns a reference to the second image
    vt_video::StillImage& GetSecondImage() {
        return _second_image;
    }


protected:
    //! \brief The first image to display in the blended element
    vt_video::StillImage _first_image;

    //! \brief The second image to display in the blended element
    vt_video::StillImage _second_image;

    /** \brief A modulation color used to modify the alpha (transparency) of the second image
    *** \note This is only used when both the first and second images are being drawn blended
    *** together. The first image will use the inherited _alpha_color member in this case.
    *** _alpha_color is also used for both the standard element fade in and fade out effects.
    **/
    vt_video::Color _second_alpha_color;
}; // class IndicatorBlendedImage : public IndicatorElement


/** ****************************************************************************
*** \brief Manages all indicator display and update
***
*** Text indicators are normally used to display numeric text representing the
*** amount of damage dealt to the actor or the amount of healing performed. Another
*** common use is to display the word "Miss" when the actor is a target for a skill
*** that did not connect successfully. The style of the rendered text can also be
*** varied and is typically used for drawing text in different colors such as red
*** for damage and green for healing. The text size may be made larger to indicate
*** more powerful or otherwise significant changes as well.
*** ***************************************************************************/
class IndicatorSupervisor
{
public:
    IndicatorSupervisor()
    {}

    ~IndicatorSupervisor();

    //! \brief Processes the two FIFO queues
    void Update();

    //! \brief Draws all elements present in the active queue
    void Draw();

    /** \brief Creates indicator text representing a numeric amount of damage dealt
    *** \param amount The amount of damage to display, in hit points. Should be non-zero.
    *** \param style The text style the damage should be shown with (font + color + shadow)
    *** \param use_parallax Whether the indicator will follow the camera movement.
    ***
    *** This function will not actually cause any damage to come to the actor (that is, the actor's
    *** hit points are not modified by this function). The degree of damage relative to the character's
    *** maximum hit points determines the color and size of the text rendered.
    **/
    void AddDamageIndicator(float x_position, float y_position,
                            uint32_t amount, const vt_video::TextStyle& style,
                            bool use_parallax = false);

    /** \brief Creates indicator text representing a numeric amount of healing dealt
    *** \param amount The amount of healing to display, in points. Should be non-zero.
    *** \param style The text style the damage should be shown with (font + color + shadow)
    *** \param use_parallax Whether the indicator will follow the camera movement.
    ***
    *** This function will not actually cause any healing to come to the actor (that is, the actor's
    *** hit points are not modified by this function). The degree of healing relative to the character's
    *** maximum hit points determines the color and size of the text rendered.
    **/
    void AddHealingIndicator(float x_position, float y_position, uint32_t amount,
                             const vt_video::TextStyle& style, bool use_parallax = false);

    /** \brief Creates indicator text showing a miss on the actor
    *** Miss text is always drawn with the same style in a small font with white text
    **/
    void AddMissIndicator(float x_position, float y_position);

    /** \brief Creates an indicator that illustrates a change in an actor's status
    *** \param status The type of the status effect
    *** \param old_intensity The previous intensity of the status effect
    *** \param new_intensity The new intensity of the status effect
    ***
    *** The arguments are used to determine which status icon images to use for the indicator. Most of the time
    *** this function will choose two status icons (one old, and one new) for use in the indicator. Occasionally
    *** we wish to show an indicator even when the status type and intensity of the effect did not change. For
    *** example, when the timer for the status effect is reset. If the status and intensity arguments are equal,
    *** only a single icon image will be used in the indicator.
    **/
    void AddStatusIndicator(float x_position, float y_position,
                            vt_global::GLOBAL_STATUS status, vt_global::GLOBAL_INTENSITY old_intensity,
                            vt_global::GLOBAL_INTENSITY new_intensity);

    //! \brief Creates indicator with item image above the character
    void AddItemIndicator(float x_position, float y_position,
                          const vt_global::GlobalItem& item);

    //! \brief Adds a parallax effect to damage and healing indicators, permitting to simulate the camera offset movement.
    void AddParallax(float x_parallax, float y_parallax);

    //! \brief Adds a small text info + optional icon on screen for a few seconds.
    void AddShortNotice(const vt_utils::ustring& message, const std::string& icon_image_filename = std::string(),
                        uint32_t display_time = 3000);

private:
    //! \brief A FIFO queue container of elements that are waiting to be started and added to the active elements container
    std::deque<IndicatorElement *> _wait_queue;

    //! \brief A FIFO queue container of all elements that have begun and are going through their display sequence
    std::deque<IndicatorElement *> _active_queue;

    //! \brief A FIFO container used to display a short message with optional icons.
    std::deque<vt_common::ShortNoticeWindow *> _short_notices;

    //! Check the waiting queue and fix potential overlaps depending on the element position and type.
    //! \param element the Indicator Element which is about to be added.
    //! \return whether there were overlapping elements whose positions were fixed.
    bool _FixPotentialIndicatorOverlapping(IndicatorElement* element);
}; // class IndicatorSupervisor

} // namespace vt_mode_manager

#endif // __INDICATOR_SUPERVISOR_HEADER__
