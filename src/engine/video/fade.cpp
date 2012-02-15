///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software 
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////


#include <cassert>
#include <cstdarg>
#include <math.h>

#include "fade.h"
#include "video.h"

using namespace std;

using namespace hoa_utils;

namespace hoa_video {

namespace private_video {

ScreenFader::ScreenFader() :
	current_color(0.0f, 0.0f, 0.0f, 0.0f),
	initial_color(0.0f, 0.0f, 0.0f, 0.0f),
	final_color(0.0f, 0.0f, 0.0f, 0.0f),
	current_time(0),
	end_time(0),
	is_fading(false),
	use_fade_overlay(false),
	fade_overlay_color(0.0f, 0.0f, 0.0f, 0.0f),
	fade_modulation(1.0f),
	interpolate_rgb_values(false)
{}



void ScreenFader::BeginFade(const Color &final, uint32 time) {
	is_fading = true;

	end_time = time;

	initial_color = current_color;
	final_color = final;
	current_time = 0;
	
	// Figure out if this is a simple fade or if an overlay is required
	// A simple fade is defined as a fade from clear to black, from black
	// to clear, or from somewhere between clear and black to either clear
	// or black. More simply, it's a fade where both the initial and final
	// color's RGB values are zeroed out

	use_fade_overlay = true;	

	if ((IsFloatEqual(initial_color[0], 0.0f) && IsFloatEqual(initial_color[1], 0.0f)
		&& IsFloatEqual(initial_color[2], 0.0f) && IsFloatEqual(final_color[0], 0.0f)
		&& IsFloatEqual(final_color[1], 0.0f) && IsFloatEqual(final_color[2], 0.0f)))
	{
		use_fade_overlay = false;
	}
	else {
		fade_modulation = 1.0f;
	}

	// If we are fading to or from transparent, then the RGB values do not need to be interpolated
	if (IsFloatEqual(final_color[3], 0.0f)) {
		interpolate_rgb_values = true;
		current_color[0] = initial_color[0];
		current_color[1] = initial_color[1];
		current_color[2] = initial_color[2];
	}
	else if (IsFloatEqual(initial_color[3], 0.0f)) {
		interpolate_rgb_values = true;
		current_color[0] = final_color[0];
		current_color[1] = final_color[1];
		current_color[2] = final_color[2];
	}
	else {
		interpolate_rgb_values = false;
	}

	Update(0); // Do an initial update
} // void ScreenFader::BeginFade(const Color &final, uint32 time)



void ScreenFader::Update(uint32 time) {
	if (is_fading == false)
		return;

	// Check for fading finish condition
	if (current_time >= end_time) {
		current_color = final_color;
		is_fading = false;
		
		if (use_fade_overlay) {
			// Check if we have faded to black or clear. If so, we can use modulation
			if (IsFloatEqual(final_color[3], 0.0f) || (IsFloatEqual(final_color[0], 0.0f)
				&& IsFloatEqual(final_color[1], 0.0f) && IsFloatEqual(final_color[2], 0.0f)))
			{
				use_fade_overlay = false;
				fade_modulation = 1.0f - final_color[3];
			}
		}
		else
			fade_modulation = 1.0f - final_color[3];

		return;
	}

	// Calculate the new interpolated color
	float percent_complete = static_cast<float>(current_time) / static_cast<float>(end_time);

	if (interpolate_rgb_values == true) {
		current_color[0] = Lerp(percent_complete, initial_color[0], final_color[0]);
		current_color[1] = Lerp(percent_complete, initial_color[1], final_color[1]);
		current_color[2] = Lerp(percent_complete, initial_color[2], final_color[2]);
	}
	current_color[3] = Lerp(percent_complete, initial_color[3], final_color[3]);

	if (use_fade_overlay == false)
		fade_modulation = 1.0f - current_color[3];
	else
		fade_overlay_color = current_color;

	current_time += time;
} // void FadeScreen::Update(uint32 time)

} // namespace private_video

}  // namespace hoa_video
