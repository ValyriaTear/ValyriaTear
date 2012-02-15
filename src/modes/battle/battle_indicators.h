////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_indicators.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for battle indicator displays.
***
*** This code contains the implementation of various indicators and indicator
*** supporting classes. Indicators are small images and text that appear
*** alongside battle sprites to inform the player about status changes such as
*** damage, healing, and elemental or status effects.
*** ***************************************************************************/

#ifndef __BATTLE_INDICATORS_HEADER__
#define __BATTLE_INDICATORS_HEADER__

#include "defs.h"
#include "utils.h"

#include "battle_utils.h"

namespace hoa_battle {

namespace private_battle {

//! \brief The total amount of time (in milliseconds) that the display sequence lasts for indicator elements
const uint32 INDICATOR_TIME = 5000;

//! \brief The amount of time (in milliseconds) that indicator elements fade at the beginning of the display sequence
const uint32 INDICATOR_FADEIN_TIME = 500;

//! \brief The amount of time (in milliseconds) that indicator elements fade at the end of the display sequence
const uint32 INDICATOR_FADEOUT_TIME = 1000;

// These constants are used to determine the draw position at various phases of the indicator animation.
// The animation (as it pertains to draw position) is determined in a number of phases.
// Phase 01: Indicator starts from below the sprite and shoots upward to a peak
// Phase 02: Indicator drops back down to the bottom of the sprite
// Phase 03: Indicator moves upward again to a smaller peak (a "bounce")
// Phase 04: Indicator falls back to bottom of the sprite
// Phase 05: Indicator rests at bottom of sprite
// Phase 06: Indicator falls as it fades away
const uint32 PHASE01_END    = 750;
const uint32 PHASE02_END    = 1500;
const uint32 PHASE03_END    = 2000;
const uint32 PHASE04_END    = 2500;
const uint32 PHASE05_END    = 4000;
const uint32 PHASE06_END    = INDICATOR_TIME;

/** ****************************************************************************
*** \brief An abstract class for displaying information about a change in an actor's state
***
*** Indicators are text or graphics that appear next to actor sprites in battle.
*** They typically represent changes to the actor such as numeric text representing
*** damage or healing, icons representing status effects, etc. The standard draw
*** sequence for elements comes in three stages to make the indicators presence and
*** disappearance more natural.
***
*** -# An initial transparent fade in of element over a short period of time
*** -# Element draw at full opacity
*** -# Finishing with a brief transparent fade out of the element
***
*** \note Indicators are drawn at different orientations for different actors. For
*** example, indicator elements and draw to the left of character actors and to
*** the right for enemy actors.
*** ***************************************************************************/
class IndicatorElement {
public:
	//! \param actor A valid pointer to the actor object this indicator
	IndicatorElement(BattleActor* actor);

	virtual ~IndicatorElement()
		{}

	//! \brief Begins the display of the indicator element
	void Start();

	//! \brief Updates the timer
	virtual void Update();

	//! \brief Returns a floating point value that represents the height of the element drawn
	virtual float ElementHeight() const = 0;

	//! \brief Draws the indicator information to the screen
	virtual void Draw() = 0;

	//! \brief Returns true when the indicator element has expired and should be removed
	bool IsExpired() const
		{ return _timer.IsFinished(); }

	//! \brief Returns if the indicator has already be started. Here, started means the indicator has gone half the way from PHASE 1
	bool HasStarted() const
		{ return _timer.GetTimeExpired() >= PHASE01_END/2; }

	//! \name Class member accessor methods
	//@{
	const BattleActor* GetActor() const
		{ return _actor; }

	const hoa_system::SystemTimer& GetTimer() const
		{ return _timer; }
	//@}

protected:
	//! \brief The actor that the indicator element will be displayed for
	BattleActor* _actor;

	//! \brief Used to monitor the display progress
	hoa_system::SystemTimer _timer;

	//! \brief A modulation color used to modify the alpha (transparency) of the drawn element
	hoa_video::Color _alpha_color;


	/** \brief Calculates and sets the draw cursor position to the place where the indicator should be drawn
	***
	*** This method also sets the appropriate draw flags for the indicator. This method should be called at
	*** the beginning of every Draw() method in derived classes.
	**/
	void _CalculateDrawPosition();

	/** \brief Calculates the standard alpha (transparency) value for drawing the element
	*** \return True if the alpha value is 1.0f and thus the indicator should be drawn with no alpha applied
	***
	*** Calling this function will set the alpha value of the _alpha_color member. Indicator elements
	*** generally fade in and fade out to make their appearance more seamless on the battle field.
	*** Alpha gradually increases from 0.0f to 1.0f in the first stage, remains at 1.0f for a majority
	*** of the time, then gradually decreases back to 0.0f as the display finishes.
	**/
	bool _CalculateDrawAlpha();
}; // class IndicatorElement


/** ****************************************************************************
*** \brief Displays an item of text next to an actor
***
*** Text indicators are normally used to display numeric text representing the
*** amount of damage dealt to the actor or the amount of healing performed. Another
*** common use is to display the word "Miss" when the actor is a target for a skill
*** that did not connect successfully. The style of the rendered text can also be
*** varied and is typically used for drawing text in different colors such as red
*** for damage and green for healing. The text size may be made larger to indicate
*** more powerful or otherwise significant changes as well.
*** ***************************************************************************/
class IndicatorText : public IndicatorElement {
public:
	/** \param actor A valid pointer to the actor object
	*** \param text The text to use to render the text image
	*** \param style The style to use to render the text image
	**/
	IndicatorText(BattleActor* actor, std::string& text, hoa_video::TextStyle& style);

	~IndicatorText()
		{}

	//! \brief Returns the height of the rendered text image
	float ElementHeight() const
		{ return _text_image.GetHeight(); }

	//! \brief Draws the text image
	void Draw();

protected:
	//! \brief The rendered image of the text to display
	hoa_video::TextImage _text_image;
}; // class IndicatorText  : public IndicatorElement



/** ****************************************************************************
*** \brief Displays an image next to an actor
***
*** This indicator displays a single image and is typically used for illustrating
*** elemental and status effects.
*** ***************************************************************************/
class IndicatorImage : public IndicatorElement {
public:
	/** \param actor A valid pointer to the actor object this indicator
	*** \param filename The name of the image file to load
	**/
	IndicatorImage(BattleActor* actor, const std::string& filename);

	/** \param actor A valid pointer to the actor object this indicator
	*** \param image A const reference to the loaded image object to display
	**/
	IndicatorImage(BattleActor* actor, const hoa_video::StillImage& image);

	~IndicatorImage()
		{}

	//! \brief Returns the height of the image
	float ElementHeight() const
		{ return _image.GetHeight(); }

	//! \brief Draws the image
	void Draw();

	//! \brief Returns a reference to the image use
	hoa_video::StillImage& GetImage()
		{ return _image; }

protected:
	//! \brief The image to display as an indicator
	hoa_video::StillImage _image;
}; // class IndicatorImage : public IndicatorElement



/** ****************************************************************************
*** \brief Displays two images blended together next to an actor
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
class IndicatorBlendedImage : public IndicatorElement {
public:
	/** \param actor A valid pointer to the actor object this indicator
	*** \param first_filename The name of the first image file to load
	*** \param second_filename The name of the second image file to load
	**/
	IndicatorBlendedImage(BattleActor* actor, const std::string& first_filename, const std::string& second_filename);

	/** \param actor A valid pointer to the actor object this indicator
	*** \param first_image A const reference to the first loaded image to display
	*** \param second_image A const reference to the second loaded image to display
	**/
	IndicatorBlendedImage(BattleActor* actor, const hoa_video::StillImage& first_image, const hoa_video::StillImage& second_image);

	~IndicatorBlendedImage()
		{}

	//! \brief Returns the height of the blended image
	float ElementHeight() const
		{ return _first_image.GetHeight(); }

	//! \brief Draws the first and/or second image blended appropriately
	void Draw();

	//! \brief Returns a reference to the first image
	hoa_video::StillImage& GetFirstImage()
		{ return _first_image; }

	//! \brief Returns a reference to the second image
	hoa_video::StillImage& GetSecondImage()
		{ return _second_image; }


protected:
	//! \brief The first image to display in the blended element
	hoa_video::StillImage _first_image;

	//! \brief The second image to display in the blended element
	hoa_video::StillImage _second_image;

	/** \brief A modulation color used to modify the alpha (transparency) of the second image
	*** \note This is only used when both the first and second images are beind drawn blended
	*** together. The first image will use the inherited _alpha_color member in this case.
	*** _alpha_color is also used for both the standard element fade in and fade out effects.
	**/
	hoa_video::Color _second_alpha_color;
}; // class IndicatorBlendedImage : public IndicatorElement


/** ****************************************************************************
*** \brief Manages all indicator elements for an actor
***
*** Text indicators are normally used to display numeric text representing the
*** amount of damage dealt to the actor or the amount of healing performed. Another
*** common use is to display the word "Miss" when the actor is a target for a skill
*** that did not connect successfully. The style of the rendered text can also be
*** varied and is typically used for drawing text in different colors such as red
*** for damage and green for healing. The text size may be made larger to indicate
*** more powerful or otherwise significant changes as well.
*** ***************************************************************************/
class IndicatorSupervisor {
public:
	//! \param actor A valid pointer to the actor object that this class is responsible for
	IndicatorSupervisor(BattleActor* actor);

	~IndicatorSupervisor();

	//! \brief Processes the two FIFO queues
	void Update();

	//! \brief Draws all elements present in the active queue
	void Draw();

	/** \brief Creates indicator text representing a numeric amount of damage dealt
	*** \param amount The amount of damage to display, in hit points. Should be non-zero.
	***
	*** This function will not actually cause any damage to come to the actor (that is, the actor's
	*** hit points are not modified by this function). The degree of damage relative to the character's
	*** maximum hit points determines the color and size of the text rendered.
	**/
	void AddDamageIndicator(uint32 amount);

	/** \brief Creates indicator text representing a numeric amount of healing dealt
	*** \param amount The amount of healing to display, in hit points. Should be non-zero.
	***
	*** This function will not actually cause any healing to come to the actor (that is, the actor's
	*** hit points are not modified by this function). The degree of healing relative to the character's
	*** maximum hit points determines the color and size of the text rendered.
	**/
	void AddHealingIndicator(uint32 amount);

	/** \brief Creates indicator text showing a miss on the actor
	*** Miss text is always drawn with the same style in a small font with white text
	**/
	void AddMissIndicator();

	/** \brief Creates an indicator that illustrates a change in an actor's status
	*** \param old_status The type of the old status effect
	*** \param old_intensity The intensity of the old status effect
	*** \param new_status The type of the new status effect
	*** \param new_intensity The intensity of the new status effect
	***
	*** The arguments are used to determine which status icon images to use for the indicator. Most of the time
	*** this function will choose two status icons (one old, and one new) for use in the indicator. Occasionally
	*** we wish to show an indicator even when the status type and intensity of the effect did not change. For
	*** example, when the timer for the status effect is reset. If the status and intensity arguments are equal,
	*** only a single icon image will be used in the indicator.
	**/
	void AddStatusIndicator(hoa_global::GLOBAL_STATUS old_status, hoa_global::GLOBAL_INTENSITY old_intensity,
		hoa_global::GLOBAL_STATUS new_status, hoa_global::GLOBAL_INTENSITY new_intensity);

private:
	//! \brief A pointer to the actor that this class supervises indicator elements for
	BattleActor* _actor;

	//! \brief A FIFO queue container of elements that are waiting to be started and added to the active elements container
	std::deque<IndicatorElement*> _wait_queue;

	//! \brief A FIFO queue container of all elements that have begun and are going through their display sequence
	std::deque<IndicatorElement*> _active_queue;
}; // class IndicatorSupervisor

} // namespace private_battle

} // namespace hoa_battle

#endif // __BATTLE_INDICATORS_HEADER__
