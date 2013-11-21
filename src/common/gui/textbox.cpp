///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    textbox.h
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for TextBox class
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "textbox.h"

#include "common/gui/menu_window.h"
#include "engine/video/video.h"

using namespace vt_utils;
using namespace vt_video;
using namespace vt_video::private_video;
using namespace vt_gui;
using namespace vt_gui::private_gui;

namespace vt_gui
{

//! \brief The unicode version of the newline character, used for string parsing
const uint16 NEWLINE_CHARACTER = static_cast<uint16>('\n');

TextBox::TextBox() :
    _display_speed(0.0f),
    _text_xalign(VIDEO_X_LEFT),
    _text_yalign(VIDEO_Y_BOTTOM),
    _num_chars(0),
    _finished(false),
    _current_time(0),
    _end_time(0),
    _mode(VIDEO_TEXT_INSTANT),
    _text_height(0.0f),
    _text_xpos(0.0f),
    _text_ypos(0.0f)
{
    _width = 0.0f;
    _height = 0.0f;
    _text_style = TextManager->GetDefaultStyle();
}

TextBox::TextBox(float x, float y, float width, float height, const TEXT_DISPLAY_MODE &mode) :
    _display_speed(0.0f),
    _text_xalign(VIDEO_X_LEFT),
    _text_yalign(VIDEO_Y_BOTTOM),
    _num_chars(0),
    _finished(false),
    _current_time(0),
    _end_time(0),
    _mode(mode),
    _text_height(0.0f),
    _text_xpos(0.0f),
    _text_ypos(0.0f)
{
    _width = width;
    _height = height;
    _text_style = TextManager->GetDefaultStyle();
    SetPosition(x, y);
}


void TextBox::ClearText()
{
    _finished = true;
    _text.clear();
    _num_chars = 0;
    _text_save.clear();
    _text_image.Clear();
}



void TextBox::Update(uint32 time)
{
    if (_finished)
        return;

    _current_time += time;

    if(_text.empty() == false && _current_time > _end_time)
        _finished = true;
}



void TextBox::Draw()
{
    if(_mode != VIDEO_TEXT_INSTANT && _text.empty())
        return;

    // Don't draw text window if parent window is hidden
    if(_owner && _owner->GetState() == VIDEO_MENU_STATE_HIDDEN)
        return;

    VideoManager->PushState();

    VideoManager->SetDrawFlags(_xalign, _yalign, VIDEO_BLEND, 0);

    // Set the draw cursor, draw flags, and draw the text
    if (_mode == VIDEO_TEXT_INSTANT) {
        VideoManager->Move(_text_xpos, _text_ypos);
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);
        _text_image.Draw();
    }
    else {
        VideoManager->Move(0.0f, _text_ypos);
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);
        _DrawTextLines(_text_xpos, _text_ypos, _scissor_rect);
    }

    if(GUIManager->DEBUG_DrawOutlines())
        _DEBUG_DrawOutline();

    VideoManager->PopState();
} // void TextBox::Draw()



void TextBox::SetDimensions(float w, float h)
{
    if(w <= 0.0f || w > 1024.0f) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid width argument: " << w << std::endl;
        return;
    }

    if(h <= 0.0f || h > 768.0f) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid height argument: " << h << std::endl;
        return;
    }

    _width = w;
    _height = h;
    _ReformatText();
}



void TextBox::SetTextAlignment(int32 xalign, int32 yalign)
{
    _text_xalign = xalign;
    _text_yalign = yalign;
    _ReformatText();
}



void TextBox::SetTextStyle(const TextStyle &style)
{
    if(style.GetFontProperties() == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "Function failed because it was passed an invalid font name: " << style.GetFontName() << std::endl;
        return;
    }

    _text_style = style;
    _text_image.SetStyle(style);

    _ReformatText();
}



void TextBox::SetDisplayMode(const TEXT_DISPLAY_MODE &mode)
{
    if(mode < VIDEO_TEXT_INSTANT || mode >= VIDEO_TEXT_TOTAL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "function failed because of an invalid mode argument: " << mode << std::endl;
        return;
    }

    _mode = mode;
}



void TextBox::SetDisplaySpeed(float display_speed)
{
    if(display_speed <= 0.0f) {
        if(VIDEO_DEBUG)
            PRINT_ERROR << "VIDEO WARNING: TextBox::SetDisplaySpeed() failed due to an invalid display speed: "
                        << display_speed << std::endl;
        return;
    }

    _display_speed = display_speed;
}



void TextBox::SetDisplayText(const std::string &text)
{
    SetDisplayText(MakeUnicodeString(text));
}



void TextBox::SetDisplayText(const ustring &text)
{
    // If the text hasn't changed, don't recompute the textbox.
    if (_text_save == text)
        return;

    _text_save = text;
    _ReformatText();

    // Reset the timer since new text has been set
    _current_time = 0;

    // Determine how much time the text will take to display depending on the display mode, speed, and size of the text
    _finished = false;
    switch(_mode) {
    default:
        _mode = VIDEO_TEXT_INSTANT;
    case VIDEO_TEXT_INSTANT:
        _end_time = 0;
        // Set finished to true only if the display mode is VIDEO_TEXT_INSTANT
        _finished = true;
        break;

    case VIDEO_TEXT_CHAR:     // All three of these modes display one character at a time
    case VIDEO_TEXT_FADECHAR:
    case VIDEO_TEXT_REVEAL:
        // We desire the total number of milliseconds to render the string.
        // Display speed is in character per second, so cancel the character term and multiply by 1000 to get ms
        _end_time = static_cast<int32>(1000.0f * _num_chars / _display_speed);
        break;

    case VIDEO_TEXT_FADELINE:   // Displays one line at a time
        // Instead of _num_chars in the other calculation, we use number of lines times CHARS_PER_LINE
        _end_time = static_cast<int32>(1000.0f * (_text.size() * CHARS_PER_LINE) / _display_speed);
        break;
    };

} // void TextBox::SetDisplayText(const ustring& text)



void TextBox::_ReformatText()
{
    // Go through the text ustring and determine where the newline characters can be found,
    // examining one line at a time and adding it to the _text vector.
    _text.clear();
    _num_chars = 0;

    FontProperties* fp = _text_style.GetFontProperties();

    // If font not set, return (leave _text vector empty)
    if(fp == NULL || fp->ttf_font == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "Textbox font properties are invalid" << std::endl;
        return;
    }

    if (_mode == VIDEO_TEXT_INSTANT) {
        _text_image.SetWordWrapWidth(_width);
        _text_image.SetText(_text_save);
    }
    else {
        // Get the wrapped text lines
        _text = TextManager->WrapText(_text_save, fp->ttf_font, _width);

        // Compute the number of chars
        const size_t temp_length = _text_save.length();
        size_t startline_pos = 0;
        uint32 new_lines = 0;
        while(startline_pos < temp_length) {
            size_t newline_pos = _text_save.find(NEWLINE_CHARACTER, startline_pos);
            if(newline_pos == ustring::npos)
                break;
            ++new_lines;
            startline_pos = newline_pos + 1;
        }
        _num_chars = _text_save.length() - new_lines;
    }

    // Update the scissor cache
    // Stores the positions of the four sides of the rectangle
    float left   = 0.0f;
    float right  = _width;
    float bottom = 0.0f;
    float top    = _height;

    VideoManager->PushState();

    VideoManager->SetDrawFlags(_xalign, _yalign, VIDEO_BLEND, 0);
    CalculateAlignedRect(left, right, bottom, top);
    VideoManager->PopState();

    // Create a screen rectangle for the position and apply any scissoring
    int32 x, y, w, h;

    x = static_cast<int32>(left < right ? left : right);
    y = static_cast<int32>(top < bottom ? top : bottom);
    w = static_cast<int32>(right - left);
    h = static_cast<int32>(top - bottom);

    if(w < 0)
        w = -w;
    if(h < 0)
        h = -h;

    _scissor_rect.Set(x, y, w, h);

    // Update the text height
    _text_height = _CalculateTextHeight();
    // Calculate the height of the text and check it against the height of the textbox.
    if(_text_height > _height) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "tried to display text of height (" << _text_height
                                      << ") in a window of lower height (" << _height << ")" << std::endl;
    }

    // Determine the vertical position of the text based on the alignment
    if(_text_yalign == VIDEO_Y_TOP) {
        _text_ypos = top;
    } else if(_text_yalign == VIDEO_Y_CENTER) {
        _text_ypos = top - (VideoManager->_current_context.coordinate_system.GetVerticalDirection() * (_height - _text_height) * 0.5f);
    } else { // (_yalign == VIDEO_Y_BOTTOM)
        _text_ypos = top - (VideoManager->_current_context.coordinate_system.GetVerticalDirection() * (_height - _text_height));
    }

    // Determine the horizontal position of the text based on the alignment
    if(_text_xalign == VIDEO_X_LEFT) {
        _text_xpos = left;
    } else if(_text_xalign == VIDEO_X_CENTER) {
        _text_xpos = (left + right) * 0.5f; // * 0.5 equals /2.
    } else { // (_text_xalign == VIDEO_X_RIGHT)
        _text_xpos = right;
    }
} // void TextBox::_ReformatText()

float TextBox::_CalculateTextHeight()
{
    if (_mode == VIDEO_TEXT_INSTANT)
        return _text_image.GetHeight();

    if(_text.empty())
        return 0;

    FontProperties* fp = _text_style.GetFontProperties();
    return static_cast<float>(fp->height + fp->line_skip * (static_cast<float>(_text.size()) - 1));
}




void TextBox::_DrawTextLines(float text_x, float text_y, ScreenRect scissor_rect)
{
    FontProperties* fp = _text_style.GetFontProperties();
    TTF_Font* ttf_font = fp->ttf_font;
    int32 num_chars_drawn = 0;

    // Calculate the fraction of the text to display
    float percent_complete;
    if(_finished)
        percent_complete = 1.0f;
    else
        percent_complete = static_cast<float>(_current_time) / static_cast<float>(_end_time);

    // Iterate through the loop for every line of text and draw it
    for(int32 line = 0; line < static_cast<int32>(_text.size()); ++line) {
        // (1): Calculate the x draw offset for this line and move to that position
        float line_width = static_cast<float>(TextManager->CalculateTextWidth(ttf_font, _text[line]));
        int32 x_align = VideoManager->_ConvertXAlign(_text_xalign);
        float x_offset = text_x + ((x_align + 1) * line_width) * 0.5f * VideoManager->_current_context.coordinate_system.GetHorizontalDirection();

        VideoManager->MoveRelative(x_offset, 0.0f);

        int32 line_size = static_cast<int32>(_text[line].size());

        // (2): Draw the text depending on the display mode and whether or not the gradual display is finished
        if(_finished || _mode == VIDEO_TEXT_INSTANT) {
            TextManager->Draw(_text[line], _text_style);
        }
        else if(_mode == VIDEO_TEXT_CHAR) {
            // Determine which character is currently being rendered
            int32 cur_char = static_cast<int32>(percent_complete * _num_chars);

            // If the current character to draw is after this line, render the entire line
            if(num_chars_drawn + line_size < cur_char) {
                TextManager->Draw(_text[line], _text_style);
            }
            // The current character to draw is on this line: figure out which characters on this line should be drawn
            else {
                int32 num_completed_chars = cur_char - num_chars_drawn;
                if(num_completed_chars > 0) {
                    ustring substring = _text[line].substr(0, num_completed_chars);
                    TextManager->Draw(substring);
                }
            }
        } // else if (_mode == VIDEO_TEXT_CHAR)

        else if(_mode == VIDEO_TEXT_FADECHAR) {
            // Figure out which character is currently being rendered
            float fade_cur_char = percent_complete * _num_chars;
            int32 cur_char = static_cast<int32>(fade_cur_char);
            float cur_percent = fade_cur_char - cur_char;

            // If the current character to draw is after this line, draw the whole line
            if(num_chars_drawn + line_size <= cur_char) {
                TextManager->Draw(_text[line], _text_style);
            }
            // The current character is on this line: draw any previous characters on this line as well as the current character
            else {
                int32 num_completed_chars = cur_char - num_chars_drawn;

                // Continue only if this line has at least one character that should be drawn
                if(num_completed_chars >= 0) {
                    ustring substring;

                    // Draw any fully completed characters at full opacity
                    if(num_completed_chars > 0) {
                        substring = _text[line].substr(0, num_completed_chars);
                        TextManager->Draw(substring, _text_style);
                    }

                    // Draw the current character that is being faded in at the appropriate alpha level
                    Color saved_color = _text_style.GetColor();
                    Color current_color = saved_color;
                    current_color[3] *= cur_percent;
                    _text_style.SetColor(current_color);

                    VideoManager->MoveRelative(static_cast<float>(TextManager->CalculateTextWidth(ttf_font, substring)), 0.0f);
                    TextManager->Draw(_text[line].substr(num_completed_chars, 1), _text_style);
                    _text_style.SetColor(saved_color);
                }
            }
        } // else if (_mode == VIDEO_TEXT_FADECHAR)

        else if(_mode == VIDEO_TEXT_FADELINE) {
            // Deteremine which line is currently being rendered
            float fade_lines = percent_complete * _text.size();
            int32 lines = static_cast<int32>(fade_lines);
            float cur_percent = fade_lines - lines;

            // If this line comes before the line being rendered, simply draw the line and be done with it
            if(line < lines) {
                TextManager->Draw(_text[line], _text_style);
            }
            // Otherwise if this is the line being rendered, determine the amount of alpha for the line being faded in and draw it
            else if(line == lines) {
                Color saved_color = _text_style.GetColor();
                Color current_color = saved_color;
                current_color[3] *= cur_percent;
                _text_style.SetColor(current_color);

                TextManager->Draw(_text[line], _text_style);
                _text_style.SetColor(saved_color);
            }
        } // else if (_mode == VIDEO_TEXT_FADELINE)

        else if(_mode == VIDEO_TEXT_REVEAL) {
            // Determine which character is currently being rendered
            float fade_cur_char = percent_complete * _num_chars;
            int32 cur_char = static_cast<int32>(fade_cur_char);
            float cur_percent = fade_cur_char - cur_char;
            int32 num_completed_chars = cur_char - num_chars_drawn;

            // If the current character comes after this line, simply render the entire line
            if(num_chars_drawn + line_size <= cur_char) {
                TextManager->Draw(_text[line], _text_style);
            }
            // If the line contains the current character, draw all previous characters as well as the current one
            else if(num_completed_chars >= 0) {
                ustring substring;

                // If there are already completed characters on this line, draw them in full
                if(num_completed_chars > 0) {
                    substring = _text[line].substr(0, num_completed_chars);
                    TextManager->Draw(substring, _text_style);
                }

                // Now draw the current character from the line, partially scissored according to the amount that is complete
                ustring cur_char_string = _text[line].substr(num_completed_chars, 1);

                // Create a rectangle for the current character, in window coordinates
                int32 char_x, char_y, char_w, char_h;
                char_x = static_cast<int32>(x_offset + VideoManager->_current_context.coordinate_system.GetHorizontalDirection()
                                            * TextManager->CalculateTextWidth(ttf_font, substring));
                char_y = static_cast<int32>(text_y - VideoManager->_current_context.coordinate_system.GetVerticalDirection()
                                            * (fp->height + fp->descent));

                if(VideoManager->_current_context.coordinate_system.GetHorizontalDirection() < 0.0f)
                    char_y = static_cast<int32>(VideoManager->_current_context.coordinate_system.GetBottom()) - char_y;

                if(VideoManager->_current_context.coordinate_system.GetVerticalDirection() < 0.0f)
                    char_x = static_cast<int32>(VideoManager->_current_context.coordinate_system.GetLeft()) - char_x;

                char_w = TextManager->CalculateTextWidth(ttf_font, cur_char_string);
                char_h = fp->height;

                // Multiply the width by percentage done to determine the scissoring dimensions
                char_w = static_cast<int32>(cur_percent * char_w);
                VideoManager->MoveRelative(VideoManager->_current_context.coordinate_system.GetHorizontalDirection()
                                           * TextManager->CalculateTextWidth(ttf_font, substring), 0.0f);

                // Construct the scissor rectangle using the character dimensions and draw the revealing character
                VideoManager->PushState();
                ScreenRect char_scissor_rect(char_x, char_y, char_w, char_h);
                scissor_rect.Intersect(char_scissor_rect);
                VideoManager->EnableScissoring();
                VideoManager->SetScissorRect(scissor_rect);
                TextManager->Draw(cur_char_string, _text_style);
                VideoManager->PopState();
            }
            // In the else case, the current character is before the line, so we don't draw anything for this line at all
        } // else if (_mode == VIDEO_TEXT_REVEAL)

        else {
            // Invalid display mode: just render the text instantly
            TextManager->Draw(_text[line]);
            IF_PRINT_WARNING(VIDEO_DEBUG) << "an unknown/unsupported text display mode was active: " << _mode << std::endl;
        }

        // (3): Prepare to draw the next line and move the draw cursor appropriately
        num_chars_drawn += line_size;
        // VideoManager->MoveRelative(-xOffset, fp.line_skip * -cs._vertical_direction);
        text_y += fp->line_skip * -VideoManager->_current_context.coordinate_system.GetVerticalDirection();
        VideoManager->Move(0.0f, text_y);
    } // for (int32 line = 0; line < static_cast<int32>(_text.size()); ++line)
} // void TextBox::_DrawLines(float text_x, float text_y, ScreenRect scissor_rect)



void TextBox::_DEBUG_DrawOutline()
{
    // Stores the positions of the four sides of the rectangle
    float left   = 0.0f;
    float right  = _width;
    float bottom = 0.0f;
    float top    = _height;

    // Draw the outline of the textbox
    VideoManager->Move(0.0f, 0.0f);
    CalculateAlignedRect(left, right, bottom, top);
    VideoManager->DrawRectangleOutline(left, right, bottom, top, 3, alpha_black);
    VideoManager->DrawRectangleOutline(left, right, bottom, top, 1, alpha_white);

    // Draw the inner boundaries for each line of text
    FontProperties* fp = _text_style.GetFontProperties();
    uint32 possible_lines = _height / fp->line_skip;
    float line_height = fp->line_skip * -VideoManager->_current_context.coordinate_system.GetVerticalDirection();
    float line_offset = top;

    for(uint32 i = 1; i <= possible_lines; ++i) {
        line_offset += line_height;
        VideoManager->DrawLine(left, line_offset, right, line_offset, 3, alpha_black);
        VideoManager->DrawLine(left, line_offset, right, line_offset, 1, alpha_white);
    }
}

}  // namespace vt_gui
