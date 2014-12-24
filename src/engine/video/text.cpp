///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    text.cpp
*** \author  Lindsay Roberts, linds@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for text rendering
***
*** This code makes use of the SDL_ttf font library for representing fonts,
*** font glyphs, and text.
***
*** \note Normally the int data type should not be used in the game code,
*** however it is used periodically throughout this file as the SDL_ttf library
*** requests integer arguments.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "text.h"
#include "video.h"

#include "engine/script/script_read.h"

// The script filename used to configure the text styles used in game.
const std::string _font_script_filename = "dat/config/fonts.lua";

using namespace vt_utils;
using namespace vt_video::private_video;

namespace vt_video
{

TextSupervisor *TextManager = NULL;

// Useful character types for text formatting
const uint16 NEW_LINE = '\n';
const uint16 SPACE_CHAR = 0x20;

// -----------------------------------------------------------------------------
// TextStyle class
// -----------------------------------------------------------------------------

TextStyle::TextStyle(const std::string& font)
{
    const TextStyle& default_style = TextManager->GetDefaultStyle();
    _font = font;
    _color = default_style.GetColor();
    _shadow_style = default_style.GetShadowStyle();
    _shadow_offset_x = default_style.GetShadowOffsetX();
    _shadow_offset_y = default_style.GetShadowOffsetY();
    _font_property = TextManager->_GetFontProperties(_font);
    _UpdateTextShadowColor();
}

TextStyle::TextStyle(const Color& color)
{
    const TextStyle& default_style = TextManager->GetDefaultStyle();
    _font = default_style.GetFontName();
    _color = color;
    _shadow_style = default_style.GetShadowStyle();
    _shadow_offset_x = default_style.GetShadowOffsetX();
    _shadow_offset_y = default_style.GetShadowOffsetY();
    _font_property = TextManager->_GetFontProperties(_font);
    _UpdateTextShadowColor();
}

TextStyle::TextStyle(TEXT_SHADOW_STYLE style)
{
    const TextStyle& default_style = TextManager->GetDefaultStyle();
    _font = default_style.GetFontName();
    _color = default_style.GetColor();
    _shadow_style = style;
    _shadow_offset_x = default_style.GetShadowOffsetX();
    _shadow_offset_y = default_style.GetShadowOffsetY();
    _font_property = TextManager->_GetFontProperties(_font);
    _UpdateTextShadowColor();
}

TextStyle::TextStyle(const std::string& font, const Color& color)
{
    const TextStyle& default_style = TextManager->GetDefaultStyle();
    _font = font;
    _color = color;
    _shadow_style = default_style.GetShadowStyle();
    _shadow_offset_x = default_style.GetShadowOffsetX();
    _shadow_offset_y = default_style.GetShadowOffsetY();
    _font_property = TextManager->_GetFontProperties(_font);
    _UpdateTextShadowColor();
}

TextStyle::TextStyle(const std::string& font, TEXT_SHADOW_STYLE style)
{
    const TextStyle& default_style = TextManager->GetDefaultStyle();
    _font = font;
    _color = default_style.GetColor();
    _shadow_style = style;
    _shadow_offset_x = default_style.GetShadowOffsetX();
    _shadow_offset_y = default_style.GetShadowOffsetY();
    _font_property = TextManager->_GetFontProperties(_font);
    _UpdateTextShadowColor();
}

TextStyle::TextStyle(const Color& color, TEXT_SHADOW_STYLE style)
{
    const TextStyle& default_style = TextManager->GetDefaultStyle();
    _font = default_style.GetFontName();
    _color = color;
    _shadow_style = style;
    _shadow_offset_x = default_style.GetShadowOffsetX();
    _shadow_offset_y = default_style.GetShadowOffsetY();
    _font_property = TextManager->_GetFontProperties(_font);
    _UpdateTextShadowColor();
}

TextStyle::TextStyle(const std::string& font, const Color& color, TEXT_SHADOW_STYLE style)
{
    const TextStyle& default_style = TextManager->GetDefaultStyle();
    _font = font;
    _color = color;
    _shadow_style = style;
    _shadow_offset_x = default_style.GetShadowOffsetX();
    _shadow_offset_y = default_style.GetShadowOffsetY();
    _font_property = TextManager->_GetFontProperties(_font);
    _UpdateTextShadowColor();
}

TextStyle::TextStyle(const std::string& font, const Color& color, TEXT_SHADOW_STYLE style, int32 shadow_x, int32 shadow_y)
{
    _font = font;
    _color = color;
    _shadow_style = style;
    _shadow_offset_x = shadow_x;
    _shadow_offset_y = shadow_y;
    _font_property = TextManager->_GetFontProperties(_font);
    _UpdateTextShadowColor();
}

void TextStyle::SetFont(const std::string& font)
{
    _font = font;
    _font_property = TextManager->_GetFontProperties(font);
}

void TextStyle::_UpdateTextShadowColor()
{
    switch(_shadow_style) {
    default:
    case VIDEO_TEXT_SHADOW_NONE:
        _shadow_color = Color::clear;
        break;
    case VIDEO_TEXT_SHADOW_DARK:
        _shadow_color = Color::black;
        _shadow_color[3] = _color[3] * 0.5f;
        break;
    case VIDEO_TEXT_SHADOW_LIGHT:
        _shadow_color = Color::white;
        _shadow_color[3] = _color[3] * 0.5f;
        break;
    case VIDEO_TEXT_SHADOW_BLACK:
        _shadow_color = Color::black;
        _shadow_color[3] = _color[3];
        break;
    case VIDEO_TEXT_SHADOW_COLOR:
        _shadow_color = _color;
        _shadow_color[3] = _color[3] * 0.5f;
        break;
    case VIDEO_TEXT_SHADOW_INVCOLOR:
        _shadow_color = Color(1.0f - _color[0], 1.0f - _color[1], 1.0f - _color[2], _color[3] * 0.5f);
        break;
    }
}

namespace private_video
{

// Endian-dependent bit masks for the different color channels
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
static const uint32 RMASK = 0xFF000000;
static const uint32 GMASK = 0x00FF0000;
static const uint32 BMASK = 0x0000FF00;
static const uint32 AMASK = 0x000000FF;
#else
static const uint32 RMASK = 0x000000FF;
static const uint32 GMASK = 0x0000FF00;
static const uint32 BMASK = 0x00FF0000;
static const uint32 AMASK = 0xFF000000;
#endif

// -----------------------------------------------------------------------------
// TextTexture class
// -----------------------------------------------------------------------------

TextTexture::TextTexture(const vt_utils::ustring &string_, const TextStyle &style_) :
    BaseTexture(),
    string(string_),
    style(style_)
{
    // Enable image smoothing for text
    smooth = true;
}



TextTexture::~TextTexture()
{
    // Remove this instance from the texture manager
    TextureManager->_UnregisterTextTexture(this);
}



bool TextTexture::Regenerate()
{
    if(texture_sheet) {
        texture_sheet->RemoveTexture(this);
        TextureManager->_RemoveSheet(texture_sheet);
        texture_sheet = NULL;
    }

    ImageMemory buffer;
    if(TextManager->_RenderText(string, style, buffer) == false)
        return false;

    width = buffer.width;
    height = buffer.height;

    TexSheet *sheet = TextureManager->_InsertImageInTexSheet(this, buffer, true);
    if(sheet == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TextureManager::_InsertImageInTexSheet() returned NULL" << std::endl;
        free(buffer.pixels);
        buffer.pixels = NULL;
        return false;
    }

    texture_sheet = sheet;
    free(buffer.pixels);
    buffer.pixels = NULL;

    return true;
}



bool TextTexture::Reload()
{
    // Regenerate text image if it is not already loaded in a texture sheet
    if(texture_sheet == NULL)
        return Regenerate();

    ImageMemory buffer;
    if(TextManager->_RenderText(string, style, buffer) == false)
        return false;

    if(texture_sheet->CopyRect(x, y, buffer) == false) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TextureSheet::CopyRect() failed" << std::endl;
        free(buffer.pixels);
        buffer.pixels = NULL;
        return false;
    }

    free(buffer.pixels);
    buffer.pixels = NULL;
    return true;
}

// -----------------------------------------------------------------------------
// TextElement class
// -----------------------------------------------------------------------------

TextElement::TextElement() :
    ImageDescriptor(),
    text_texture(NULL)
{}



TextElement::TextElement(TextTexture *texture) :
    ImageDescriptor(),
    text_texture(texture)
{
    SetTexture(texture);
}



TextElement::~TextElement()
{
    Clear();
}



void TextElement::Clear()
{
    ImageDescriptor::Clear(); // This call will remove the texture reference for us
    text_texture = NULL;
}


void TextElement::Draw() const
{
    Draw(Color::white);
}

void TextElement::Draw(const Color &draw_color) const
{
    // Don't draw anything if this image is completely transparent (invisible).
    if (IsFloatEqual(draw_color[3], 0.0f))
        return;

    VideoManager->PushMatrix();
    _DrawOrientation();

    if(draw_color == Color::white) {
        _DrawTexture(_color);
    } else {
        Color modulated_colors[4];
        modulated_colors[0] = _color[0] * draw_color;
        modulated_colors[1] = _color[1] * draw_color;
        modulated_colors[2] = _color[2] * draw_color;
        modulated_colors[3] = _color[3] * draw_color;

        _DrawTexture(modulated_colors);
    }

    VideoManager->PopMatrix();
}

void TextElement::SetTexture(TextTexture *texture)
{
    // Do nothing if the texture pointer is not going to change
    if(text_texture == texture) {
        return;
    }

    // Remove references and possibly delete the existing texture
    if(text_texture != NULL) {
        _RemoveTextureReference();

        text_texture = NULL;
        _texture = NULL;
    }

    // Set the new texture
    if(texture == NULL) {
        text_texture = NULL;
        _texture = NULL;
        _width = 0.0f;
        _height = 0.0f;
    } else {
        texture->AddReference();
        text_texture = texture;
        _texture = texture;

        _width = static_cast<float>(texture->width);
        _height = static_cast<float>(texture->height);
    }
}

} // namespace private_video

// -----------------------------------------------------------------------------
// TextImage class
// -----------------------------------------------------------------------------

TextImage::TextImage() :
    ImageDescriptor(),
    _max_width(1024)
{
    _style = TextManager->GetDefaultStyle();
}



TextImage::TextImage(const ustring& text, const TextStyle& style) :
    ImageDescriptor(),
    _text(text),
    _style(style),
    _max_width(1024)
{
    _Regenerate();
}



TextImage::TextImage(const std::string& text, const TextStyle& style) :
    ImageDescriptor(),
    _text(MakeUnicodeString(text)),
    _style(style),
    _max_width(1024)
{
    _Regenerate();
}



TextImage::TextImage(const TextImage &copy) :
    ImageDescriptor(copy),
    _text(copy._text),
    _style(copy._style),
    _max_width(copy._max_width)
{
    for(uint32 i = 0; i < copy._text_sections.size(); i++) {
        _text_sections.push_back(new TextElement(*(copy._text_sections[i])));
    }
}



TextImage &TextImage::operator=(const TextImage &copy)
{
    // Prevents object assignment to itself
    if(this == &copy)
        return *this;

    // Remove references to any existing text sections
    for(uint32 i = 0; i < _text_sections.size(); ++i)
        delete _text_sections[i];

    _text_sections.clear();

    _text = copy._text;
    _style = copy._style;
    _max_width = copy._max_width;
    for(uint32 i = 0; i < copy._text_sections.size(); ++i)
        _text_sections.push_back(new TextElement(*(copy._text_sections[i])));

    return *this;
}

void TextImage::Clear()
{
    ImageDescriptor::Clear();
    _text.clear();
    for(uint32 i = 0; i < _text_sections.size(); ++i)
        delete _text_sections[i];

    _text_sections.clear();
    _width = 0;
    _height = 0;
    // Don't reset the max width as the normal flow might want a new text again
    // with the same constraints.
}

void TextImage::Draw(const Color& draw_color) const
{
    // Don't draw anything if this image is completely transparent (invisible).
    if (IsFloatEqual(draw_color[3], 0.0f))
        return;

    // Save the draw cursor position before drawing this text.
    VideoManager->PushMatrix();

    for (uint32 i = 0; i < _text_sections.size(); ++i) {
        if (_style.GetShadowStyle() != VIDEO_TEXT_SHADOW_NONE) {
            // Draw the text's shadow.
            const float dx = VideoManager->_current_context.coordinate_system.GetHorizontalDirection() * _style.GetShadowOffsetX();
            const float dy = VideoManager->_current_context.coordinate_system.GetVerticalDirection() * _style.GetShadowOffsetY();
            VideoManager->MoveRelative(dx, dy);
            _text_sections[i]->Draw(draw_color * _style.GetShadowColor());
            VideoManager->MoveRelative(-dx, -dy);
        }

        // Draw the text.
        _text_sections[i]->Draw(draw_color * _style.GetColor());

        // Move the draw cursor one line down.
        VideoManager->MoveRelative(0.0f, _style.GetFontProperties()->line_skip * -VideoManager->_current_context.coordinate_system.GetVerticalDirection());
    }

    // Restore the position of the draw cursor.
    VideoManager->PopMatrix();
}

void TextImage::_Regenerate()
{
    _width = 0.0f;
    _height = 0.0f;

    for (uint32 i = 0; i < _text_sections.size(); ++i)
        delete _text_sections[i];

    _text_sections.clear();

    if(_text.empty())
        return;

    FontProperties* fp = _style.GetFontProperties();
    if (fp == NULL || fp->ttf_font == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid font or font properties" << std::endl;
        return;
    }

    // Iterate through each line of text and render a text texture for each one.
    std::vector<ustring> lines_array = TextManager->WrapText(_text, fp->ttf_font, _max_width);
    std::vector<ustring>::iterator line_iter;
    for(line_iter = lines_array.begin(); line_iter != lines_array.end(); ++line_iter) {

        TextElement *new_element = new TextElement();
        // If this line is only a newline character or is an empty string, create an empty TextElement object
        if((*line_iter) == ustring(&NEW_LINE) || (*line_iter).empty()) {
            new_element->SetDimensions(0.0f, static_cast<float>(fp->line_skip));
        }
        // Otherwise, create a new TextTexture to be managed by the new element
        else {
            // PRINT_DEBUG << **line_iter << std::endl;
            TextTexture *texture = new TextTexture(*line_iter, _style);
            if(texture->Regenerate() == false) {
                IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TextTexture::_Regenerate() failed" << std::endl;
            }
            TextureManager->_RegisterTextTexture(texture);

            // Resize the TextImage width if this line is wider than the current width
            if(texture->width > _width)
                _width = static_cast<float>(texture->width);

            new_element->SetTexture(texture); // Automatically adds a reference to texture
        }
        _text_sections.push_back(new_element);

        // Increase height by the font specified line height
        _height += fp->line_skip;
    }
} // void TextImage::_Regenerate()

// -----------------------------------------------------------------------------
// TextSupervisor class
// -----------------------------------------------------------------------------

TextSupervisor::TextSupervisor() :
    _text_texture(0),
    _text_texture_width(0),
    _text_texture_height(0)
{
    glGenTextures(1, &_text_texture);
    if (_text_texture == 0) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "call to glGenTextures() failed" << std::endl;
        assert(_text_texture != 0);
    }
}

TextSupervisor::~TextSupervisor()
{
    // Clean up the text texture.
    if (_text_texture != 0) {
        GLuint textures[] = { _text_texture };
        glDeleteTextures(1, textures);
        _text_texture = 0;
    }

    // Remove all loaded fonts.  Then, shutdown the SDL_ttf library.
    for (std::map<std::string, FontProperties *>::iterator it = _font_map.begin(); it != _font_map.end(); ++it)
        delete it->second;

    TTF_Quit();
}

bool TextSupervisor::SingletonInitialize()
{
    if(TTF_Init() < 0) {
        PRINT_ERROR << "SDL_ttf initialization failed" << std::endl;
        return false;
    }

    return true;
}

//! Loads all the defaults fonts available in the game (those available to every locales).
//! And sets a default text style.
//! The function will exit the game if no valid textstyle was loaded
//! or if the default text style is invalid.
bool TextSupervisor::LoadFonts(const std::string& locale_name)
{
    vt_script::ReadScriptDescriptor font_script;

    //Checking the file existence and validity.
    if(!font_script.OpenFile(_font_script_filename)) {
        PRINT_ERROR << "Couldn't open font file: " << _font_script_filename << std::endl;
        return false;
    }

    if(!font_script.DoesTableExist("fonts")) {
        PRINT_ERROR << "No 'fonts' table in file: " << _font_script_filename << std::endl;
        font_script.CloseFile();
        return false;
    }

    std::vector<std::string> locale_names;
    font_script.ReadTableKeys("fonts", locale_names);
    if(locale_names.empty() || !font_script.OpenTable("fonts")) {
        PRINT_ERROR << "No local array defined in the 'fonts' table of file: "
                    << _font_script_filename << std::endl;
        font_script.CloseFile();
        return false;
    }

    std::string style_default = font_script.ReadString("font_default_style");
    if(style_default.empty()) {
        PRINT_ERROR << "No default text style defined in: " << _font_script_filename
                    << std::endl;
        font_script.CloseFile();
        return false;
    }

    // Search for a 'default' array and the specific locale array
    bool default_locale_array_found = false;
    bool specific_locale_array_found = false;

    // We only keep the array we need: the default one and the locale specific one.
    for(uint32 j = 0; j < locale_names.size(); ++j) {
        std::string locale = locale_names[j];
        // Keep the default array
        if (!strcasecmp(locale.c_str(), "default")) {
            default_locale_array_found = true;
            continue;
        }

        if (locale_name.empty())
            continue;

        if (!strcasecmp(locale.c_str(), locale_name.c_str()))
            specific_locale_array_found = true;
    }

    // If there is no default arrays. Exit now as the script file is invalid.
    if (!default_locale_array_found) {
        PRINT_ERROR << "Can't load fonts. No 'default' local array found in file: " << _font_script_filename << std::endl;
        font_script.CloseFile();
        return false;
    }

    // We set the arrays we want to parse.
    locale_names.clear();
    // The default one must come in first to permit locale specific fonts to override them.
    locale_names.push_back("default");
    if (specific_locale_array_found)
        locale_names.push_back(locale_name);

    // We now parse the wanted tables only, and the (re)load the fonts accordingly.
    for(uint32 j = 0; j < locale_names.size(); ++j) {
        std::string locale = locale_names[j];

        std::vector<std::string> style_names;
        font_script.ReadTableKeys(locale, style_names);
        if(style_names.empty()) {
            PRINT_ERROR << "No text styles defined in the table '"<< locale << "' of file: "
                        << _font_script_filename << std::endl;
            font_script.CloseFile();
            return false;
        }

        if (!font_script.OpenTable(locale)) { // locale
            PRINT_ERROR << "Can't open locale table '"<< locale << "' of file: "
                        << _font_script_filename << std::endl;
            font_script.CloseFile();
            return false;
        }

        for(uint32 i = 0; i < style_names.size(); ++i) {

            if (!font_script.OpenTable(style_names[i])) { // Text style
                PRINT_ERROR << "Can't open text style table '" << style_names[i] << "' of locale: '" << locale << "' in file: "
                            << _font_script_filename << std::endl;
                font_script.CloseFile();
                continue;
            }

            std::string font_file = font_script.ReadString("font");
            uint32 font_size = font_script.ReadInt("size");

            if(!_LoadFont(style_names[i], font_file, font_size)) {
                // Check whether the default font is invalid
                if(style_default == style_names[i]) {
                    font_script.CloseAllTables();
                    font_script.CloseFile();
                    PRINT_ERROR << "The default text style '" << style_default
                                << "' couldn't be loaded in file: " << _font_script_filename
                                << std::endl;
                    return false;
                }
                else {
                    PRINT_WARNING << "The text style '" << style_names[i]
                                << "' couldn't be loaded in file: " << _font_script_filename
                                << std::endl;
                }
            }

            font_script.CloseTable(); // Text style
        } // load each TextStyle

        font_script.CloseTable(); // locale
    }
    font_script.CloseTable(); // fonts

    font_script.CloseFile();

    // Setup the default font
    SetDefaultStyle(TextStyle(style_default, Color::white, VIDEO_TEXT_SHADOW_BLACK, 1, -2));
    return true;
}

bool TextSupervisor::_LoadFont(const std::string& textstyle_name, const std::string& font_filename, uint32 font_size)
{
    if(font_size == 0) {
        PRINT_ERROR << "Attempted to load a text style of size zero: " << textstyle_name << std::endl;
        return false;
    }

    // Check whether the TextStyle name is not already taken
    bool reload = false;
    std::map<std::string, FontProperties *>::iterator it = _font_map.find(textstyle_name);
    if(it != _font_map.end()) {
        reload = true;

        // Let's check whether the requested font is exactly the same than before
        // and do nothing in this case so we don't hurt performance.
        FontProperties *fp = it->second;
        if (fp && fp->font_filename == font_filename && fp->font_size == font_size)
            return true;
    }

    // Attempt to load the font
    TTF_Font *font = TTF_OpenFont(font_filename.c_str(), font_size);
    if(font == NULL) {
        PRINT_ERROR << "Call to TTF_OpenFont() failed to load the font file: " << font_filename << std::endl
        << TTF_GetError() << std::endl;
        return false;
    }

    // Get or Create a new FontProperties object for this font and set all of the properties according to SDL_ttf
    FontProperties* fp = reload ? it->second : new FontProperties();

    if (fp == NULL) {
        PRINT_ERROR << "Invalid Font Properties instance for text style: " << textstyle_name << std::endl;
        return false;
    }

    // We first clear the font before setting a new one in case of a reload.
    if (reload)
        fp->ClearFont();

    fp->ttf_font = font;
    fp->font_filename = font_filename;
    fp->font_size = font_size;
    fp->height = TTF_FontHeight(font);
    fp->line_skip = TTF_FontLineSkip(font);
    fp->ascent = TTF_FontAscent(font);
    fp->descent = TTF_FontDescent(font);

    // If the text style is new, we add it to the font cache map
    if (!reload)
        _font_map[textstyle_name] = fp;

    return true;
}

void TextSupervisor::_FreeFont(const std::string &font_name)
{
    std::map<std::string, FontProperties*>::iterator it = _font_map.find(font_name);
    if(it == _font_map.end()) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "argument font name was invalid: " << font_name << std::endl;
        return;
    }

    // Free the font and remove it from the font cache
    delete it->second;

    // Remove the data from the map once freed.
    _font_map.erase(it);
}

FontProperties *TextSupervisor::_GetFontProperties(const std::string &font_name)
{
    if(_IsFontValid(font_name) == false) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "argument font name was invalid: " << font_name << std::endl;
        return NULL;
    }

    return _font_map[font_name];
}

void TextSupervisor::Draw(const ustring &text, const TextStyle &style)
{
    if (text.empty()) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "empty string was passed to function" << std::endl;
        return;
    }

    FontProperties *fp = style.GetFontProperties();
    if (fp == NULL || fp->ttf_font == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "failed because font was invalid: " << style.GetFontName() << std::endl;
        return;
    }

    VideoManager->PushState();

    // Break the string into lines and render the shadow and text for each line
    uint16 buffer[2048];
    size_t last_line = 0;
    do {
        // Find the next new line character in the string and save the line
        size_t next_line;
        for(next_line = last_line; next_line < text.length(); next_line++) {
            if(text[next_line] == NEW_LINE)
                break;

            buffer[next_line - last_line] = text[next_line];
        }
        buffer[next_line - last_line] = 0;
        last_line = next_line + 1;

        // If this line is empty, skip on to the next one
        if(buffer[0] == 0) {
            VideoManager->MoveRelative(0, -fp->line_skip * VideoManager->_current_context.coordinate_system.GetVerticalDirection());
            continue;
        }

        // Save the draw cursor position before drawing this text.
        VideoManager->PushMatrix();

        // If text shadows are enabled...
        if (style.GetShadowStyle() != VIDEO_TEXT_SHADOW_NONE) {
            // Draw the text with its shadow.
            _RenderText(buffer, fp, style.GetColor(), style.GetShadowOffsetX(), style.GetShadowOffsetY(), style.GetShadowColor());
        } else {
            // Draw the text.
            _RenderText(buffer, fp, style.GetColor());
        }

        // Restore the position of the draw cursor.
        VideoManager->PopMatrix();

        // Move the draw cursor one line down.
        VideoManager->MoveRelative(0, -fp->line_skip * VideoManager->_current_context.coordinate_system.GetVerticalDirection());

    } while (last_line < text.length());

    VideoManager->PopState();
}

int32 TextSupervisor::CalculateTextWidth(TTF_Font* ttf_font, const vt_utils::ustring &text)
{
    if(ttf_font == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "Invalid font" << std::endl;
        return -1;
    }

    int32 width;
    if(TTF_SizeUNICODE(ttf_font, text.c_str(), &width, NULL) == -1) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "Call to TTF_SizeUNICODE failed with TTF error: " << TTF_GetError() << std::endl;
        return -1;
    }

    return width;
}



int32 TextSupervisor::CalculateTextWidth(TTF_Font* ttf_font, const std::string &text)
{
    if(ttf_font == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "Invalid font" << std::endl;
        return -1;
    }

    int32 width;
    if(TTF_SizeText(ttf_font, text.c_str(), &width, NULL) == -1) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "Call to TTF_SizeText failed with TTF error: " << TTF_GetError() << std::endl;
        return -1;
    }

    return width;
}

std::vector<vt_utils::ustring> TextSupervisor::WrapText(const vt_utils::ustring& text,
                                                        TTF_Font* ttf_font,
                                                        uint32 max_width)
{
    std::vector<vt_utils::ustring> lines_array;
    if (text.empty() || max_width == 0) {
        // This can happen when called with uninit // gui objects.
        return lines_array;
    }

    // We split the text using new lines in a first row
    ustring temp_text = text;
    uint32 text_length = temp_text.length();

    for (uint32 i = 0; i < text_length; ++i) {
        if(!(temp_text[i] == NEW_LINE))
            continue;

        // We add the substring (except the end line)
        if (i > 0)
            lines_array.push_back(temp_text.substr(0, i));
        else // It's only a new line
            lines_array.push_back(ustring());

        // We reached the string's end
        if (i + 1 == text_length) {
            temp_text.clear();
            break;
        }

        // We then cut the temp string used part (and the new line)
        temp_text = temp_text.substr(i + 1);
        text_length = temp_text.length();
        i = -1; // Will be set to 0 after the loop end.
    }

    // If there is still some text, we push the rest in the vector
    if (temp_text.length() > 0)
        lines_array.push_back(temp_text);

    // We then perform word wrapping in a loop until all the text is added
    // And copy it into the new vector
    std::vector<vt_utils::ustring> wrapped_lines_array;
    uint32 num_lines = lines_array.size();
    for (uint32 line_index = 0; line_index < num_lines; ++line_index) {
        ustring temp_line = lines_array[line_index];

        // If it's an empty string, we add a blank line.
        if (temp_line.empty()) {
            wrapped_lines_array.push_back(temp_line);
            continue;
        }

        while(!temp_line.empty()) {
            int32 text_width = TextManager->CalculateTextWidth(ttf_font, temp_line);

            // If the text can fit in the text box, add the whole line and return
            if(text_width < (int32)max_width) {
                wrapped_lines_array.push_back(temp_line);
                break;
            }

            // Otherwise, find the maximum number of words which can fit and make that substring a line
            // Word boundaries are found by calling the == 0x20 test
            ustring wrapped_line;
            int32 num_wrapped_chars = 0;
            int32 last_breakable_index = -1;
            int32 line_length = static_cast<int32>(temp_line.length());
            while (num_wrapped_chars < line_length) {
                wrapped_line += temp_line[num_wrapped_chars];
                // If we meet a space character (0x20), we can wrap the text
                if(temp_line[num_wrapped_chars] == SPACE_CHAR) {
                    int32 text_width = TextManager->CalculateTextWidth(ttf_font, wrapped_line);

                    if(text_width < (int32)max_width) {
                        // We haven't gone past the breaking point: mark this as a possible breaking point
                        last_breakable_index = num_wrapped_chars;
                    } else {
                        // We exceeded the maximum width, so go back to the previous breaking point.
                        // If there was no previous breaking point (== -1), then just break it off at
                        // the current character position.
                        if(last_breakable_index != -1)
                            num_wrapped_chars = last_breakable_index;
                        break;
                    }
                } // (temp_line[num_wrapped_chars] == SPACE_CHAR)
                ++num_wrapped_chars;
            } // while (num_wrapped_chars < line_length)

            // Figure out the number of characters in the wrapped line and construct the wrapped line
            text_width = TextManager->CalculateTextWidth(ttf_font, wrapped_line);
            if(text_width >= (int32)max_width && last_breakable_index != -1) {
                num_wrapped_chars = last_breakable_index;
            }
            wrapped_line = temp_line.substr(0, num_wrapped_chars);

            // Add the new wrapped line to the text.
            wrapped_lines_array.push_back(wrapped_line);

            // If there is no more text remaining, we have finished.
            if (num_wrapped_chars + 1 >= line_length)
                break;
            // Otherwise, we need to grab the rest of the text that remains to be added and loop again.
            else
                temp_line = temp_line.substr(num_wrapped_chars + 1, line_length - num_wrapped_chars);
        } // while (temp_line.empty() == false)
    } // for each lines of text

    // Returns the wrapped lines.
    return wrapped_lines_array;
}

void TextSupervisor::_RenderText(const uint16* text, FontProperties* font_properties, const Color& color)
{
    if (text == NULL || *text == 0) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid argument, empty or null string" << std::endl;
        assert(text != NULL && *text != 0);
        return;
    }

    if (font_properties == NULL || font_properties->ttf_font == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid argument, NULL font properties or NULL ttf font" << std::endl;
        assert(font_properties != NULL && font_properties->ttf_font != NULL);
        return;
    }

    // Render the text.
    const SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderUNICODE_Blended(font_properties->ttf_font, text, white);
    if (surface == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TTF_RenderUNICODE_Blended() failed" << std::endl;
        assert(surface != NULL);
        return;
    }

    // Retrieve the size of the text.
    int font_width = 0, font_height = 0;
    if (TTF_SizeUNICODE(font_properties->ttf_font, text, &font_width, &font_height) != 0) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TTF_SizeUNICODE() failed" << std::endl;
        SDL_FreeSurface(surface);
        assert(false);
        return;
    }

    // Enable texturing.
    VideoManager->EnableTexture2D();

    // Bind the OpenGL texture.
    TextureManager->_BindTexture(_text_texture);

    // Lock the SDL surface.
    SDL_LockSurface(surface);

    //
    // Send the surface pixel data to OpenGL.
    //

    if (_text_texture_width == static_cast<GLuint>(surface->w) &&
        _text_texture_height == static_cast<GLuint>(surface->h)) {
        // The size of the old texture is the same.  Just update the pixel data.
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _text_texture_width, _text_texture_height, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    } else {
        // The size of the old texture is different.  Update the storage definition as well as the pixel data.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    }

    // Update the texture's width and height.
    _text_texture_width = surface->w;
    _text_texture_height = surface->h;

    // Unlock the SDL surface.
    SDL_UnlockSurface(surface);

    // Update some of the OpenGL texture parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Enable blending.
    VideoManager->EnableBlending();

    // Update the blending function.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Push the matrix stack.
    VideoManager->PushMatrix();

    // Update the transmation matrix.
    CoordSys& coordinate_system = VideoManager->_current_context.coordinate_system;
    float x_offset = ((VideoManager->_current_context.x_align + 1) * font_width) * 0.5f * -coordinate_system.GetHorizontalDirection();
    float y_offset = ((VideoManager->_current_context.y_align + 1) * font_height) * 0.5f * -coordinate_system.GetVerticalDirection();
    VideoManager->MoveRelative(x_offset, y_offset);

    // Load the shader program.
    gl::ShaderProgram* shader_program = VideoManager->LoadShaderProgram(gl::shader_programs::Sprite);
    assert(shader_program != NULL);

    // Calculate the vertex positions.
    std::vector<float> vertex_positions;

    // Vertex one.
    vertex_positions.push_back(0.0f);
    vertex_positions.push_back(0.0f);
    vertex_positions.push_back(0.0f);

    // Vertex two.
    vertex_positions.push_back(font_width);
    vertex_positions.push_back(0.0f);
    vertex_positions.push_back(0.0f);

    // Vertex three.
    vertex_positions.push_back(font_width);
    vertex_positions.push_back(font_height);
    vertex_positions.push_back(0.0f);

    // Vertex four.
    vertex_positions.push_back(0.0f);
    vertex_positions.push_back(font_height);
    vertex_positions.push_back(0.0f);

    // Calculate the vertex texture coordinates.
    std::vector<float> vertex_texture_coordinates;

    // Vertex one.
    vertex_texture_coordinates.push_back(0.0f);
    vertex_texture_coordinates.push_back(0.0f);

    // Vertex two.
    vertex_texture_coordinates.push_back(1.0f);
    vertex_texture_coordinates.push_back(0.0f);

    // Vertex three.
    vertex_texture_coordinates.push_back(1.0f);
    vertex_texture_coordinates.push_back(1.0f);

    // Vertex four.
    vertex_texture_coordinates.push_back(0.0f);
    vertex_texture_coordinates.push_back(1.0f);

    // The vertex colors.
    std::vector<float> vertex_colors;

    // Vertex one.
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);

    // Vertex two.
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);

    // Vertex three.
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);

    // Vertex four.
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);

    // Draw the text.
    VideoManager->DrawSprite(shader_program, vertex_positions, vertex_texture_coordinates, vertex_colors, color);

    // Unload the shader program.
    VideoManager->UnloadShaderProgram();

    // Restore the transformation stack.
    VideoManager->PopMatrix();

    // Clean up.
    if (surface != NULL) {
        SDL_FreeSurface(surface);
        surface = NULL;
    }
}

void TextSupervisor::_RenderText(const uint16* text, FontProperties* font_properties,
                                 const Color& color,
                                 float shadow_offset_x, float shadow_offset_y,
                                 const Color& color_shadow)
{
    if (text == NULL || *text == 0) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid argument, empty or null string" << std::endl;
        assert(text != NULL && *text != 0);
        return;
    }

    if (font_properties == NULL || font_properties->ttf_font == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid argument, NULL font properties or NULL ttf font" << std::endl;
        assert(font_properties != NULL && font_properties->ttf_font != NULL);
        return;
    }

    // Render the text.
    const SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderUNICODE_Blended(font_properties->ttf_font, text, white);
    if (surface == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TTF_RenderUNICODE_Blended() failed" << std::endl;
        assert(surface != NULL);
        return;
    }

    // Retrieve the size of the text.
    int font_width = 0, font_height = 0;
    if (TTF_SizeUNICODE(font_properties->ttf_font, text, &font_width, &font_height) != 0) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TTF_SizeUNICODE() failed" << std::endl;
        SDL_FreeSurface(surface);
        assert(false);
        return;
    }

    // Enable texturing.
    VideoManager->EnableTexture2D();

    // Bind the OpenGL texture.
    TextureManager->_BindTexture(_text_texture);

    // Lock the SDL surface.
    SDL_LockSurface(surface);

    //
    // Send the surface pixel data to OpenGL.
    //

    if (_text_texture_width == static_cast<GLuint>(surface->w) &&
        _text_texture_height == static_cast<GLuint>(surface->h)) {
        // The size of the old texture is the same.  Just update the pixel data.
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _text_texture_width, _text_texture_height, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    } else {
        // The size of the old texture is different.  Update the storage definition as well as the pixel data.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    }

    // Update the texture's width and height.
    _text_texture_width = surface->w;
    _text_texture_height = surface->h;

    // Unlock the SDL surface.
    SDL_UnlockSurface(surface);

    // Update some of the OpenGL texture parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Enable blending.
    VideoManager->EnableBlending();

    // Update the blending function.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //
    // Draw the shadow first.
    //

    // Push the transformation stack.
    VideoManager->PushMatrix();

    // Apply the shadow offset.
    const float delta_x = VideoManager->_current_context.coordinate_system.GetHorizontalDirection() * shadow_offset_x;
    const float delta_y = VideoManager->_current_context.coordinate_system.GetVerticalDirection() * shadow_offset_y;
    VideoManager->MoveRelative(delta_x, delta_y);

    // Update the transmation matrix.
    CoordSys& coordinate_system = VideoManager->_current_context.coordinate_system;
    float x_offset = ((VideoManager->_current_context.x_align + 1) * font_width) * 0.5f * -coordinate_system.GetHorizontalDirection();
    float y_offset = ((VideoManager->_current_context.y_align + 1) * font_height) * 0.5f * -coordinate_system.GetVerticalDirection();
    VideoManager->MoveRelative(x_offset, y_offset);

    // Load the shader program.
    gl::ShaderProgram* shader_program = VideoManager->LoadShaderProgram(gl::shader_programs::Sprite);
    assert(shader_program != NULL);

    // Calculate the vertex positions.
    std::vector<float> vertex_positions;

    // Vertex one.
    vertex_positions.push_back(0.0f);
    vertex_positions.push_back(0.0f);
    vertex_positions.push_back(0.0f);

    // Vertex two.
    vertex_positions.push_back(font_width);
    vertex_positions.push_back(0.0f);
    vertex_positions.push_back(0.0f);

    // Vertex three.
    vertex_positions.push_back(font_width);
    vertex_positions.push_back(font_height);
    vertex_positions.push_back(0.0f);

    // Vertex four.
    vertex_positions.push_back(0.0f);
    vertex_positions.push_back(font_height);
    vertex_positions.push_back(0.0f);

    // Calculate the vertex texture coordinates.
    std::vector<float> vertex_texture_coordinates;

    // Vertex one.
    vertex_texture_coordinates.push_back(0.0f);
    vertex_texture_coordinates.push_back(0.0f);

    // Vertex two.
    vertex_texture_coordinates.push_back(1.0f);
    vertex_texture_coordinates.push_back(0.0f);

    // Vertex three.
    vertex_texture_coordinates.push_back(1.0f);
    vertex_texture_coordinates.push_back(1.0f);

    // Vertex four.
    vertex_texture_coordinates.push_back(0.0f);
    vertex_texture_coordinates.push_back(1.0f);

    // The vertex colors.
    std::vector<float> vertex_colors;

    // Vertex one.
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);

    // Vertex two.
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);

    // Vertex three.
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);

    // Vertex four.
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);
    vertex_colors.push_back(1.0f);

    // Draw the shadow.
    VideoManager->DrawSprite(shader_program, vertex_positions, vertex_texture_coordinates, vertex_colors, color_shadow);

    // Restore the transformation stack.
    VideoManager->PopMatrix();

    //
    // Draw the text second.
    //

    // Push the transformation stack.
    VideoManager->PushMatrix();

    // Update the transmation matrix.
    VideoManager->MoveRelative(x_offset, y_offset);

    // Draw the text.
    VideoManager->DrawSprite(shader_program, vertex_positions, vertex_texture_coordinates, vertex_colors, color);

    // Unload the shader program.
    VideoManager->UnloadShaderProgram();

    // Restore the transformation stack.
    VideoManager->PopMatrix();

    // Clean up.
    if (surface != NULL) {
        SDL_FreeSurface(surface);
        surface = NULL;
    }
}

bool TextSupervisor::_RenderText(const vt_utils::ustring& text, TextStyle& style, ImageMemory& buffer)
{
    FontProperties* font_properties = style.GetFontProperties();
    if (font_properties == NULL || font_properties->ttf_font == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "The TextStyle argument using font:'" << style.GetFontName() << "' was invalid" << std::endl;
        assert(font_properties != NULL && font_properties->ttf_font == NULL);
        return false;
    }

    // Render the text.
    const SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderUNICODE_Blended(font_properties->ttf_font, text.c_str(), white);
    if (surface == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TTF_RenderUNICODE_Blended() failed" << std::endl;
        assert(surface != NULL);
        return false;
    }

    // Copy the text to the buffer.
    buffer.pixels = static_cast<uint8*>(calloc(surface->w * surface->h, 4));
    uint32 num_bytes = surface->w * surface->h * 4;
    for (uint32 j = 0; j < num_bytes; j += 4) {
        ((uint8*)buffer.pixels)[j + 0] = ((uint8*)surface->pixels)[j + 0]; // r
        ((uint8*)buffer.pixels)[j + 1] = ((uint8*)surface->pixels)[j + 1]; // g
        ((uint8*)buffer.pixels)[j + 2] = ((uint8*)surface->pixels)[j + 2]; // b
        ((uint8*)buffer.pixels)[j + 3] = ((uint8*)surface->pixels)[j + 3]; // alpha
    }

    buffer.width = surface->w;
    buffer.height = surface->h;

    // Clean up.
    if (surface != NULL) {
        SDL_FreeSurface(surface);
        surface = NULL;
    }

    return true;
}

}  // namespace vt_video
