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
*** \brief   Source file for battle indicator displays.
*** ***************************************************************************/

#include "engine/system.h"
#include "engine/video/video.h"

#include "common/global/global_utils.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_actors.h"
#include "modes/battle/battle_indicators.h"
#include "modes/battle/battle_utils.h"

using namespace hoa_utils;
using namespace hoa_system;
using namespace hoa_video;
using namespace hoa_global;

namespace hoa_battle {

namespace private_battle {

//! \brief The total amount of time (in milliseconds) that the display sequence lasts for indicator elements
const uint32 INDICATOR_TIME = 3000;

//! \brief The amount of time (in milliseconds) that indicator elements fade at the beginning of the display sequence
const uint32 INDICATOR_FADEIN_TIME = 500;

//! \brief The amount of time (in milliseconds) that indicator elements fade at the end of the display sequence
const uint32 INDICATOR_FADEOUT_TIME = 1000;

//! \brief Represents the initial y (up) force applied on the indicator effect
const float INITIAL_FORCE = 12.0f;

////////////////////////////////////////////////////////////////////////////////
// IndicatorElement class
////////////////////////////////////////////////////////////////////////////////

IndicatorElement::IndicatorElement(BattleActor* actor, INDICATOR_TYPE indicator_type) :
	_actor(actor),
	_timer(INDICATOR_TIME),
	_alpha_color(1.0f, 1.0f, 1.0f, 0.0f),
	_y_force(INITIAL_FORCE),
	_x_position(0.0f),
	_y_position(0.0f),
	_x_absolute_position(0.0f),
	_y_absolute_position(0.0f),
	_indicator_type(indicator_type)
{
	if (actor == NULL)
		IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received NULL actor argument" << std::endl;

    _x_force = RandomFloat(-20.0f, 20.0f);

    // Setup a default aboslute position
    if (_actor) {
        _x_absolute_position = _actor->GetXLocation();
        _y_absolute_position = _actor->GetYLocation();
    }
}


void IndicatorElement::Start() {
	if (_timer.IsInitial() == false) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "timer was not in initial state when started" << std::endl;
	}
	_timer.Run();

	// Reinit the indicator push
	_x_force = RandomFloat(-20.0f, 20.0f);
	_x_position = 0.0f;
	_y_force = INITIAL_FORCE;
	_y_position = 0.0f;
	// Reset a default aboslute position
	if (_actor) {
		_x_absolute_position = _actor->GetXLocation();
		_y_absolute_position = _actor->GetYLocation();
	}
}


void IndicatorElement::Draw() {
    VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    // Sets the cursor position to just below the bottom center location of the sprite
    VideoManager->Move(_x_absolute_position, _y_absolute_position);

    // Takes in account the indicator current position.
    VideoManager->MoveRelative(_x_position, _y_position);
}


void IndicatorElement::Update() {
	_timer.Update();
	_UpdateDrawPosition();
}

void IndicatorElement::_UpdateDrawPosition() {
	// the time passed since the last call in ms.
	float elapsed_ms = static_cast<float>(hoa_system::SystemManager->GetUpdateTime());

	switch (_indicator_type) {
		case DAMAGE_INDICATOR:
		{
			// Indicator gravity appliance in pixels / seconds
			const float INDICATOR_WEIGHT = 26.0f;

			_y_force -= elapsed_ms / 1000 * INDICATOR_WEIGHT;

			// Compute a potential maximum fall speed
			if (_y_force < -15.0f)
				_y_force = -15.0f;

			_y_position += _y_force;

			// Resolve a ground collision
			if (_y_position <= 0.0f) {
				_y_position = 0.0f;

				// If the force is very low, the bouncing is over
				if (std::abs(_y_force) <= INDICATOR_WEIGHT / 10.0f) {
					_y_force = 0.0f;
				}
				else {
					// Make the object bounce
					_y_force = -(_y_force * 0.6f);
				}
			}

			// Make the object advance only if it still can.
			if (_y_position > 0.0f)
				_x_position += (_x_force / 1000 * elapsed_ms);
		}
		break;

		case MISS_INDICATOR:
			_y_absolute_position = _actor->GetYLocation() + (_actor->GetSpriteHeight() / 2);

			if (_actor->IsEnemy())
				_x_position += 5.0f / 1000 * elapsed_ms;
			else
				_x_position -= 5.0f / 1000 * elapsed_ms;
			break;
		case ITEM_INDICATOR:
			_y_absolute_position = _actor->GetYLocation() + _actor->GetSpriteHeight();
			_y_position -= 5.0f / 1000 * elapsed_ms;
			break;
		case NEGATIVE_STATUS_EFFECT_INDICATOR:
			_y_absolute_position = _actor->GetYLocation() + (_actor->GetSpriteHeight() / 3 * 2);

			_y_position -= 5.0f / 1000 * elapsed_ms;
			break;
		default:
		case HEALING_INDICATOR:
		case POSITIVE_STATUS_EFFECT_INDICATOR:
			_y_absolute_position = _actor->GetYLocation() + (_actor->GetSpriteHeight() / 3 * 2);

			_y_position += 5.0f / 1000 * elapsed_ms;
			break;
	}
}



bool IndicatorElement::_ComputeDrawAlpha() {
	// Case 1: Timer is not running nor paused so indicator should not be drawn
	if ((_timer.GetState() == SYSTEM_TIMER_RUNNING) && (_timer.GetState() == SYSTEM_TIMER_PAUSED)) {
		_alpha_color.SetAlpha(0.0f);
		return true;
	}
	// Case 2: Timer is in beginning stage and indicator graphic is fading in
	else if (_timer.GetTimeExpired() < INDICATOR_FADEIN_TIME) {
		_alpha_color.SetAlpha(static_cast<float>(_timer.GetTimeExpired()) / static_cast<float>(INDICATOR_FADEIN_TIME));
		return true;
	}
	// Case 3: Timer is in final stage and indicator graphic is fading out
	else if (_timer.TimeLeft() < INDICATOR_FADEOUT_TIME) {
		_alpha_color.SetAlpha(static_cast<float>(_timer.TimeLeft()) / static_cast<float>(INDICATOR_FADEOUT_TIME));
		return true;
	}
	// Case 4: Timer is in middle stage and indicator graphic should be drawn with no transparency
	else {
		return false;
	}
}

////////////////////////////////////////////////////////////////////////////////
// IndicatorText class
////////////////////////////////////////////////////////////////////////////////

IndicatorText::IndicatorText(BattleActor* actor, const std::string& text, const TextStyle& style,
								INDICATOR_TYPE indicator_type) :
	IndicatorElement(actor, indicator_type),
	_text_image(text, style)
{}



void IndicatorText::Draw() {
	IndicatorElement::Draw();

	if (_ComputeDrawAlpha())
		_text_image.Draw(_alpha_color);
	else
		_text_image.Draw();
}

////////////////////////////////////////////////////////////////////////////////
// IndicatorImage class
////////////////////////////////////////////////////////////////////////////////

IndicatorImage::IndicatorImage(BattleActor* actor, const std::string& filename,
								INDICATOR_TYPE indicator_type) :
	IndicatorElement(actor, indicator_type)
{
	if (_image.Load(filename) == false)
		PRINT_ERROR << "failed to load indicator image: " << filename << std::endl;
}



IndicatorImage::IndicatorImage(BattleActor* actor, const StillImage& image,
								INDICATOR_TYPE indicator_type) :
	IndicatorElement(actor, indicator_type),
	_image(image)
{} // TODO: check if the image argument is valid...currently seems the only possible check is that its filename is not an empty string.



void IndicatorImage::Draw() {
	IndicatorElement::Draw();

	if (_ComputeDrawAlpha())
		_image.Draw(_alpha_color);
	else
		_image.Draw();
}

////////////////////////////////////////////////////////////////////////////////
// IndicatorBlendedImage class
////////////////////////////////////////////////////////////////////////////////

IndicatorBlendedImage::IndicatorBlendedImage(BattleActor* actor,
											 const std::string& first_filename,
											 const std::string& second_filename,
											 INDICATOR_TYPE indicator_type) :
	IndicatorElement(actor, indicator_type),
	_second_alpha_color(1.0f, 1.0f, 1.0f, 0.0f)
{
	if (_first_image.Load(first_filename) == false)
		PRINT_ERROR << "failed to load first indicator image: " << first_filename << std::endl;
	if (_second_image.Load(second_filename) == false)
		PRINT_ERROR << "failed to load second indicator image: " << second_filename << std::endl;
}



IndicatorBlendedImage::IndicatorBlendedImage(BattleActor* actor,
											 const StillImage& first_image,
											 const StillImage& second_image,
											 INDICATOR_TYPE indicator_type) :
	IndicatorElement(actor, indicator_type),
	_first_image(first_image),
	_second_image(second_image),
	_second_alpha_color(1.0f, 1.0f, 1.0f, 0.0f)
{} // TODO: check if the image arguments are valid...currently seems the only possible check is that its filename is not an empty string.



void IndicatorBlendedImage::Draw() {
	IndicatorElement::Draw();

	// Case 1: Initial fade in of first image
	if (_timer.GetTimeExpired() <= INDICATOR_FADEIN_TIME) {
		_ComputeDrawAlpha();
		_first_image.Draw(_alpha_color);
	}
	// Case 2: Opaque draw of first image
	else if (_timer.GetTimeExpired() <= INDICATOR_TIME / 4) {
		_first_image.Draw();
	}
	// Case 3: Blended draw of first and second images
	else if (_timer.GetTimeExpired() <= INDICATOR_TIME / 2) {
		_alpha_color.SetAlpha(static_cast<float>((INDICATOR_TIME / 2) - _timer.GetTimeExpired())
			/ static_cast<float>(1000));
		_second_alpha_color.SetAlpha(1.0f - _alpha_color.GetAlpha());
		_first_image.Draw(_alpha_color);
		_second_image.Draw(_second_alpha_color);
	}
	// Case 4: Opaque draw of second image
	else if (_timer.GetTimeExpired() <= INDICATOR_TIME / 3 * 2) {
		_second_image.Draw();
	}
	// Case 5: Final fade out of second image
	else { // <= end
		_ComputeDrawAlpha();
		_second_image.Draw(_alpha_color);
	}
}

////////////////////////////////////////////////////////////////////////////////
// IndicatorSupervisor class
////////////////////////////////////////////////////////////////////////////////

IndicatorSupervisor::IndicatorSupervisor(BattleActor* actor) :
	_actor(actor)
{
	if (actor == NULL)
		IF_PRINT_WARNING(BATTLE_DEBUG) << "contructor received NULL actor argument" << std::endl;
}



IndicatorSupervisor::~IndicatorSupervisor() {
	for (uint32 i = 0; i < _wait_queue.size(); i++)
		delete _wait_queue[i];
	_wait_queue.clear();

	for (uint32 i = 0; i < _active_queue.size(); i++)
		delete _active_queue[i];
	_active_queue.clear();
}



void IndicatorSupervisor::Update() {
	// Update all active elements
	for (uint32 i = 0; i < _active_queue.size(); i++)
		_active_queue[i]->Update();

	// Remove all expired elements from the active queue
	while (_active_queue.empty() == false) {
		if (_active_queue.front()->IsExpired() == true) {
			delete _active_queue.front();
			_active_queue.pop_front();
		}
		else {
			// If the front element is not expired, no other elements should be expired either
			break;
		}
	}

	// TODO: determine if there is enough space to insert the next element

	if (_wait_queue.empty() == false && (_active_queue.empty() || _active_queue.back()->HasStarted())) {
		_active_queue.push_back(_wait_queue.front());
		_active_queue.back()->Start();
		_wait_queue.pop_front();
	}
}



void IndicatorSupervisor::Draw() {
	for (uint32 i = 0; i < _active_queue.size(); i++) {
		_active_queue[i]->Draw();
	}
}



void IndicatorSupervisor::AddDamageIndicator(uint32 amount) {
	const Color low_red(1.0f, 0.75f, 0.0f, 1.0f);
	const Color mid_red(1.0f, 0.50f, 0.0f, 1.0f);
	const Color high_red(1.0f, 0.25f, 0.0f, 1.0f);
	const Color full_red(Color::red);

	if (amount == 0) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function was given a zero value argument" << std::endl;
		return;
	}

	std::string text = NumberToString(amount);
	TextStyle style;

	float damage_percent = static_cast<float>(amount) / static_cast<float>(_actor->GetMaxHitPoints());
	if (damage_percent < 0.10f) {
		style.font = "text24";
		style.color = low_red;
		style.shadow_style = VIDEO_TEXT_SHADOW_BLACK;
	}
	else if (damage_percent < 0.20f) {
		style.font = "text24";
		style.color = mid_red;
		style.shadow_style = VIDEO_TEXT_SHADOW_BLACK;
	}
	else if (damage_percent < 0.30f) {
		style.font = "text24";
		style.color = high_red;
		style.shadow_style = VIDEO_TEXT_SHADOW_BLACK;
	}
	else { // (damage_percent >= 0.30f)
		style.font = "text24";
		style.color = full_red;
		style.shadow_style = VIDEO_TEXT_SHADOW_BLACK;
	}

	_wait_queue.push_back(new IndicatorText(_actor, text, style, DAMAGE_INDICATOR));
}



void IndicatorSupervisor::AddHealingIndicator(uint32 amount, bool hit_points) {
	const Color low_green(0.0f, 1.0f, 0.60f, 1.0f);
	const Color mid_green(0.0f, 1.0f, 0.30f, 1.0f);
	const Color high_green(0.0f, 1.0f, 0.15f, 1.0f);

	const Color low_blue(0.0f, 0.60f, 1.0f, 1.0f);
	const Color mid_blue(0.0f, 0.30f, 1.0f, 1.0f);
	const Color high_blue(0.0f, 0.15f, 1.0f, 1.0f);

	if (amount == 0) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function was given a zero value argument" << std::endl;
		return;
	}

	std::string text = NumberToString(amount);
	TextStyle style;

	// TODO: use different colors/shades of green for different degrees of damage. There's a
	// bug in rendering colored text that needs to be addressed first.
	float healing_percent = static_cast<float>(amount / _actor->GetMaxHitPoints());
	if (healing_percent < 0.10f) {
		style.font = "text24";
		style.color = hit_points ? low_green : low_blue;
		style.shadow_style = VIDEO_TEXT_SHADOW_BLACK;
	}
	else if (healing_percent < 0.20f) {
		style.font = "text24";
		style.color = hit_points ? mid_green : mid_blue;
		style.shadow_style = VIDEO_TEXT_SHADOW_BLACK;
	}
	else if (healing_percent < 0.30f) {
		style.font = "text24";
		style.color = hit_points ? high_green : high_blue;
		style.shadow_style = VIDEO_TEXT_SHADOW_BLACK;
	}
	else { // (healing_percent >= 0.30f)
		style.font = "text24";
		style.color = hit_points ? Color::green : Color::blue;
		style.shadow_style = VIDEO_TEXT_SHADOW_BLACK;
	}

	_wait_queue.push_back(new IndicatorText(_actor, text, style, HEALING_INDICATOR));
}



void IndicatorSupervisor::AddMissIndicator() {
	std::string text = Translate("Miss");
	TextStyle style("text24", Color::white);
	_wait_queue.push_back(new IndicatorText(_actor, text, style, MISS_INDICATOR));
}


//! \brief Tells whether the new status is better for the character than the previous one
bool IsNewStatusBetter(GLOBAL_STATUS new_status, GLOBAL_INTENSITY old_intensity, GLOBAL_INTENSITY new_intensity) {

	switch (new_status) {
	    case GLOBAL_STATUS_STRENGTH_LOWER:
	    case GLOBAL_STATUS_VIGOR_LOWER:
	    case GLOBAL_STATUS_FORTITUDE_LOWER:
	    case GLOBAL_STATUS_PROTECTION_LOWER:
	    case GLOBAL_STATUS_AGILITY_LOWER:
	    case GLOBAL_STATUS_EVADE_LOWER:
	    case GLOBAL_STATUS_HP_DRAIN:
	    case GLOBAL_STATUS_SP_DRAIN:
	    case GLOBAL_STATUS_PARALYSIS:
	    case GLOBAL_STATUS_STASIS:
			if (old_intensity < new_intensity)
				return false;
			else
				return true;
			break;
		default:
			if (old_intensity < new_intensity)
				return true;
			else
				return false;
			break;
	}

	return true; // by default
}


void IndicatorSupervisor::AddStatusIndicator(GLOBAL_STATUS old_status, GLOBAL_INTENSITY old_intensity,
	GLOBAL_STATUS new_status, GLOBAL_INTENSITY new_intensity)
{
	// If the status and intensity has not changed, only a single status icon needs to be used
	if ((old_status == new_status) && (old_intensity == new_intensity)) {
		StillImage* image = BattleMode::CurrentInstance()->GetMedia().GetStatusIcon(new_status, new_intensity);
		_wait_queue.push_back(new IndicatorImage(_actor, *image, POSITIVE_STATUS_EFFECT_INDICATOR));
	}
	// Otherwise two status icons need to be used in the indicator image
	else {
		StillImage* first_image = BattleMode::CurrentInstance()->GetMedia().GetStatusIcon(old_status, old_intensity);
		StillImage* second_image = BattleMode::CurrentInstance()->GetMedia().GetStatusIcon(new_status, new_intensity);
		INDICATOR_TYPE indicator_type = IsNewStatusBetter(new_status, old_intensity, new_intensity) ?
		    POSITIVE_STATUS_EFFECT_INDICATOR : NEGATIVE_STATUS_EFFECT_INDICATOR;
		_wait_queue.push_back(new IndicatorBlendedImage(_actor, *first_image, *second_image, indicator_type));
	}
}

void IndicatorSupervisor::AddItemIndicator(const GlobalItem& item) {
	_wait_queue.push_back(new IndicatorImage(_actor, item.GetIconImage(),
											ITEM_INDICATOR));
}

} // namespace private_battle

} // namespace hoa_battle
