///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    text.cpp
*** \author  Lindsay Roberts, linds@allacrost.org
*** \brief   Source file for text rendering
***
*** This code makes use of the SDL_ttf font library for representing fonts,
*** font glyphs, and text.
***
*** \note Normally the int data type should not be used in Allacrost code,
*** however it is used periodically throughout this file as the SDL_ttf library
*** requests integer arguments.
*** ***************************************************************************/

#include <cassert>
#include <cstdarg>
#include <math.h>

#include "video.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_video::private_video;

template<> hoa_video::TextSupervisor* Singleton<hoa_video::TextSupervisor>::_singleton_reference = NULL;

namespace hoa_video {

TextSupervisor* TextManager = NULL;

// -----------------------------------------------------------------------------
// TextStyle class
// -----------------------------------------------------------------------------

TextStyle::TextStyle() :
	font(VideoManager->Text()->GetDefaultStyle().font),
	color(VideoManager->Text()->GetDefaultStyle().color),
	shadow_style(VideoManager->Text()->GetDefaultStyle().shadow_style),
	shadow_offset_x(VideoManager->Text()->GetDefaultStyle().shadow_offset_x),
	shadow_offset_y(VideoManager->Text()->GetDefaultStyle().shadow_offset_y)
{}



TextStyle::TextStyle(string fnt) :
	font(fnt),
	color(VideoManager->Text()->GetDefaultStyle().color),
	shadow_style(VideoManager->Text()->GetDefaultStyle().shadow_style),
	shadow_offset_x(VideoManager->Text()->GetDefaultStyle().shadow_offset_x),
	shadow_offset_y(VideoManager->Text()->GetDefaultStyle().shadow_offset_y)
{}



TextStyle::TextStyle(Color c) :
	font(VideoManager->Text()->GetDefaultStyle().font),
	color(c),
	shadow_style(VideoManager->Text()->GetDefaultStyle().shadow_style),
	shadow_offset_x(VideoManager->Text()->GetDefaultStyle().shadow_offset_x),
	shadow_offset_y(VideoManager->Text()->GetDefaultStyle().shadow_offset_y)
{}



TextStyle::TextStyle(TEXT_SHADOW_STYLE style) :
	font(VideoManager->Text()->GetDefaultStyle().font),
	color(VideoManager->Text()->GetDefaultStyle().color),
	shadow_style(style),
	shadow_offset_x(VideoManager->Text()->GetDefaultStyle().shadow_offset_x),
	shadow_offset_y(VideoManager->Text()->GetDefaultStyle().shadow_offset_y)
{}



TextStyle::TextStyle(string fnt, Color c) :
	font(fnt),
	color(c),
	shadow_style(VideoManager->Text()->GetDefaultStyle().shadow_style),
	shadow_offset_x(VideoManager->Text()->GetDefaultStyle().shadow_offset_x),
	shadow_offset_y(VideoManager->Text()->GetDefaultStyle().shadow_offset_y)
{}



TextStyle::TextStyle(string fnt, TEXT_SHADOW_STYLE style) :
	font(fnt),
	color(VideoManager->Text()->GetDefaultStyle().color),
	shadow_style(style),
	shadow_offset_x(VideoManager->Text()->GetDefaultStyle().shadow_offset_x),
	shadow_offset_y(VideoManager->Text()->GetDefaultStyle().shadow_offset_y)
{}



TextStyle::TextStyle(Color c, TEXT_SHADOW_STYLE style) :
	font(VideoManager->Text()->GetDefaultStyle().font),
	color(c),
	shadow_style(style),
	shadow_offset_x(VideoManager->Text()->GetDefaultStyle().shadow_offset_x),
	shadow_offset_y(VideoManager->Text()->GetDefaultStyle().shadow_offset_y)
{}



TextStyle::TextStyle(string fnt, Color c, TEXT_SHADOW_STYLE style) :
	font(fnt),
	color(c),
	shadow_style(style),
	shadow_offset_x(VideoManager->Text()->GetDefaultStyle().shadow_offset_x),
	shadow_offset_y(VideoManager->Text()->GetDefaultStyle().shadow_offset_y)
{}



TextStyle::TextStyle(string fnt, Color c, TEXT_SHADOW_STYLE style, int32 shadow_x, int32 shadow_y) :
	font(fnt),
	color(c),
	shadow_style(style),
	shadow_offset_x(shadow_x),
	shadow_offset_y(shadow_y)
{}

namespace private_video {

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

TextTexture::TextTexture(const hoa_utils::ustring& string_, const TextStyle& style_) :
	BaseTexture(),
	string(string_),
	style(style_)
{
	// Enable image smoothing for text
	smooth = true;
}



TextTexture::~TextTexture() {
	// Remove this instance from the texture manager
	TextureManager->_UnregisterTextTexture(this);
}



bool TextTexture::Regenerate() {
	if (texture_sheet) {
		texture_sheet->RemoveTexture(this);
		TextureManager->_RemoveSheet(texture_sheet);
		texture_sheet = NULL;
	}

	ImageMemory buffer;
	if (TextManager->_RenderText(string, style, buffer) == false)
		return false;

	width = buffer.width;
	height = buffer.height;

	TexSheet* sheet = TextureManager->_InsertImageInTexSheet(this, buffer, true);
	if (sheet == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TextureManager::_InsertImageInTexSheet() returned NULL" << endl;
		free(buffer.pixels);
		buffer.pixels = NULL;
		return false;
	}

	texture_sheet = sheet;
	free(buffer.pixels);
	buffer.pixels = NULL;

	return true;
}



bool TextTexture::Reload() {
	// Regenerate text image if it is not already loaded in a texture sheet
	if (texture_sheet == NULL)
		return Regenerate();

	ImageMemory buffer;
	if (TextManager->_RenderText(string, style, buffer) == false)
		return false;

	if (texture_sheet->CopyRect(x, y, buffer) == false) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TextureSheet::CopyRect() failed" << endl;
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



TextElement::TextElement(TextTexture* texture) :
	ImageDescriptor(),
	text_texture(texture)
{
	SetTexture(texture);
}



TextElement::~TextElement() {
	Clear();
}



void TextElement::Clear() {
	ImageDescriptor::Clear(); // This call will remove the texture reference for us
	text_texture = NULL;
}



void TextElement::Draw() const {
	// If real lighting is enabled, draw images normally since the light overlay
	// will take care of the modulation. If not, (i.e. no overlay is being used)
	// then pass the light color so the vertex colors can do the modulation
	if (VideoManager->_uses_lights == false && (VideoManager->_light_color != Color::white))
		Draw(VideoManager->_light_color);
	else
		Draw(Color::white);
}



void TextElement::Draw(const Color& draw_color) const {
	// Don't draw anything if this image is completely transparent (invisible)
	if (IsFloatEqual(draw_color[3], 0.0f) == true) {
		return;
	}

	glPushMatrix();
	_DrawOrientation();

	float modulation = VideoManager->_screen_fader.GetFadeModulation();
	// Used to determine if the image color should be modulated by any degree due to screen fading effects
	bool skip_modulation = (draw_color == Color::white && IsFloatEqual(modulation, 1.0f));
	if (skip_modulation) {
		_DrawTexture(_color);
	}
	else {
		Color fade_color(modulation, modulation, modulation, 1.0f);
		Color modulated_colors[4];

		fade_color = draw_color * fade_color;
		modulated_colors[0] = _color[0] * fade_color;
		modulated_colors[1] = _color[1] * fade_color;
		modulated_colors[2] = _color[2] * fade_color;
		modulated_colors[3] = _color[3] * fade_color;
		_DrawTexture(modulated_colors);
	}

	glPopMatrix();
} // void TextElement::Draw(const Color& draw_color) const



void TextElement::SetTexture(TextTexture* texture) {
	// Do nothing if the texture pointer is not going to change
	if (text_texture == texture) {
		return;
	}

	// Remove references and possibly delete the existing texture
	if (text_texture != NULL) {
		_RemoveTextureReference();

		text_texture = NULL;
		_texture = NULL;
	}

	// Set the new texture
	if (texture == NULL) {
		text_texture = NULL;
		_texture = NULL;
		_width = 0.0f;
		_height = 0.0f;
	}
	else {
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
	ImageDescriptor()
{}



TextImage::TextImage(const ustring& string, TextStyle style) :
	ImageDescriptor(),
	_string(string),
	_style(style)
{
	_Regenerate();
}



TextImage::TextImage(const string& string, TextStyle style) :
	ImageDescriptor(),
	_string(MakeUnicodeString(string)),
	_style(style)
{
	_Regenerate();
}



TextImage::TextImage(const TextImage& copy) :
	ImageDescriptor(copy),
	_string(copy._string),
	_style(copy._style)
{
	for (uint32 i = 0; i < copy._text_sections.size(); i++) {
		_text_sections.push_back(new TextElement(*(copy._text_sections[i])));
	}
}



TextImage& TextImage::operator=(const TextImage& copy) {
	// Handle the case were a dumbass assigns an object to itself
	if (this == &copy) {
		return *this;
	}

	// Remove references to any existing text sections
	for (uint32 i = 0; i < _text_sections.size(); i++) {
		delete _text_sections[i];
	}
	_text_sections.clear();

	_string = copy._string;
	_style = copy._style;
	for (uint32 i = 0; i < copy._text_sections.size(); i++) {
		_text_sections.push_back(new TextElement(*(copy._text_sections[i])));
	}

	return *this;
}



void TextImage::Clear() {
	ImageDescriptor::Clear();
	_string.clear();
	for (uint32 i = 0; i < _text_sections.size(); i++) {
		delete _text_sections[i];
	}
	_text_sections.clear();
	_width = 0;
	_height = 0;
}



void TextImage::Draw() const {
	glPushMatrix();
	for (uint32 i = 0; i < _text_sections.size(); ++i) {
		_text_sections[i]->Draw();
		VideoManager->MoveRelative(0.0f, TextManager->GetFontProperties(_style.font)->line_skip * -VideoManager->_current_context.coordinate_system.GetVerticalDirection());
	}
	glPopMatrix();
}



void TextImage::Draw(const Color& draw_color) const {
	// Don't draw anything if this image is completely transparent (invisible)
	if (IsFloatEqual(draw_color[3], 0.0f) == true) {
		return;
	}

	glPushMatrix();
	for (uint32 i = 0; i < _text_sections.size(); ++i) {
		_text_sections[i]->Draw(draw_color);
		VideoManager->MoveRelative(0.0f, TextManager->GetFontProperties(_style.font)->line_skip * -VideoManager->_current_context.coordinate_system.GetVerticalDirection());
	}
	glPopMatrix();
}



void TextImage::_Regenerate() {
	const uint16 NEW_LINE = '\n';
	const uint16 END_STRING = '\0';

	_width = 0;
	_height = 0;
	for (uint32 i = 0; i < _text_sections.size(); i++) {
		delete _text_sections[i];
	}
	_text_sections.clear();

	if (_string.empty()) {
		return;
	}

	FontProperties* fp = TextManager->GetFontProperties(_style.font);
	if (TextManager->IsFontValid(_style.font) == false || fp == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid font or font properties" << endl;
		return;
	}

	TextManager->_CacheGlyphs(_string.c_str(), fp);

	// 1) Dissect the unicode string into an array of lines of text
	std::vector<uint16*> line_array;
	uint16* reformatted_text = new uint16[_string.size() + 1];
	uint16* text_iter = reformatted_text;
	uint16* last_line = reformatted_text;

	for (const uint16* char_iter = _string.c_str(); *char_iter; ++char_iter) {
		if (*char_iter == NEW_LINE) {
			*text_iter++ = END_STRING;
			line_array.push_back(last_line);
			last_line = text_iter;
		}
		else {
			*text_iter++ = *char_iter;
		}
	}
	line_array.push_back(last_line);
	*text_iter = END_STRING;

	// 2) Iterate through each line of text and render a TextTexture for each one
	vector<uint16*>::iterator line_iter;
	for (line_iter = line_array.begin(); line_iter != line_array.end(); ++line_iter) {
		TextElement* new_element = new TextElement();
		// If this line is only a newline character or is an empty string, create an empty TextElement object
		if (**line_iter == NEW_LINE || **line_iter == END_STRING) {
			new_element->SetDimensions(0.0f, static_cast<float>(fp->line_skip));
		}
		// Otherwise, create a new TextTexture to be managed by the new element
		else {
// 			PRINT_DEBUG << **line_iter << endl;
			TextTexture* texture = new TextTexture(*line_iter, _style);
			if (texture->Regenerate() == false) {
				IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TextTexture::_Regenerate() failed" << endl;
			}
			TextureManager->_RegisterTextTexture(texture);

			// Resize the TextImage width if this line is wider than the current width
			if (texture->width > _width)
				_width = static_cast<float>(texture->width);

			new_element->SetTexture(texture); // Automatically adds a reference to texture
		}
		_text_sections.push_back(new_element);

		// Increase height by the font specified line height
		_height += fp->line_skip;
	}

	delete[] reformatted_text;
} // void TextImage::_Regenerate()

// -----------------------------------------------------------------------------
// TextSupervisor class
// -----------------------------------------------------------------------------

// When TextSupervisor is created, the
TextSupervisor::TextSupervisor() :
	_default_style("", Color(), VIDEO_TEXT_SHADOW_INVALID, 0, 0)
{}



TextSupervisor::~TextSupervisor() {
	// Remove all loaded fonts and cached glyphs, then shutdown the SDL_ttf library
	for (map<string, FontProperties*>::iterator i = _font_map.begin(); i != _font_map.end(); i++) {
		FontProperties* fp = i->second;

		if (fp->ttf_font)
			TTF_CloseFont(fp->ttf_font);

		if (fp->glyph_cache) {
			for (std::map<uint16, FontGlyph*>::iterator j = fp->glyph_cache->begin(); j != fp->glyph_cache->end(); j++) {
				delete (*j).second;
			}
			delete fp->glyph_cache;
		}

		delete fp;
	}

	TTF_Quit();
}



bool TextSupervisor::SingletonInitialize() {
	if (TTF_Init() < 0) {
		PRINT_ERROR << "SDL_ttf initialization failed" << endl;
		return false;
	}

	if (LoadFont("img/fonts/junicode_regular.ttf", "debug_font", 16) == false) {
		PRINT_ERROR << "could not load the debug font" << endl;
		TTF_Quit();
		return false;
	}

	return true;
}



bool TextSupervisor::LoadFont(const string& filename, const string& font_name, uint32 size, bool make_default)
{
	// Make sure that the font name is not already taken
	if (IsFontValid(font_name) == true) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "a font with the desired reference name already existed: " << font_name << endl;
		return false;
	}

	if (size == 0) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "attempted to load a font of point size zero" << font_name << endl;
		return false;
	}

	// Attempt to load the font
	TTF_Font* font = TTF_OpenFont(filename.c_str(), size);
	if (font == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TTF_OpenFont() failed to load the font file: " << filename << endl;
		return false;
	}

	// Create a new FontProperties object for this font and set all of the properties according to SDL_ttf
	FontProperties* fp = new FontProperties;
	fp->ttf_font = font;
	fp->height = TTF_FontHeight(font);
	fp->line_skip = TTF_FontLineSkip(font);
	fp->ascent = TTF_FontAscent(font);
	fp->descent = TTF_FontDescent(font);

	// Create the glyph cache for the font and add it to the font map
	fp->glyph_cache = new std::map<uint16, FontGlyph*>;
	_font_map[font_name] = fp;
	return true;
} // bool TextSupervisor::LoadFont(...)



void TextSupervisor::FreeFont(const std::string& font_name) {
	if (_font_map.find(font_name) == _font_map.end()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "argument font name was invalid: " << font_name << endl;
		return;
	}

	// TODO: implement the rest of this function
}



FontProperties* TextSupervisor::GetFontProperties(const std::string& font_name) {
	if (IsFontValid(font_name) == false) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "argument font name was invalid: " << font_name << endl;
		return NULL;
	}

	return _font_map[font_name];
}



void TextSupervisor::Draw(const ustring& text, const TextStyle& style) {
	if (text.empty()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "empty string was passed to function" << endl;
		return;
	}

	if (IsFontValid(style.font) == false) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "failed because font was invalid: " << style.font << endl;
		return;
	}

	FontProperties* fp = _font_map[style.font];
	VideoManager->PushState();

	// Break the string into lines and render the shadow and text for each line
	uint16 buffer[2048];
	const uint16 NEWLINE = '\n';
	size_t last_line = 0;
	do {
		// Find the next new line character in the string and save the line
		size_t next_line;
		for (next_line = last_line; next_line < text.length(); next_line++) {
			if (text[next_line] == NEWLINE)
				break;

			buffer[next_line - last_line] = text[next_line];
		}
		buffer[next_line - last_line] = 0;
		last_line = next_line + 1;

		// If this line is empty, skip on to the next one
		if (buffer[0] == 0) {
			VideoManager->MoveRelative(0, -fp->line_skip * VideoManager->_current_context.coordinate_system.GetVerticalDirection());
			continue;
		}

		// Save the draw cursor position before drawing this text
		glPushMatrix();

		// If text shadows are enabled, draw the shadow first
		if (style.shadow_style != VIDEO_TEXT_SHADOW_NONE) {
			glPushMatrix();
			VideoManager->MoveRelative(VideoManager->_current_context.coordinate_system.GetHorizontalDirection() * style.shadow_offset_x, 0.0f);
			VideoManager->MoveRelative(0.0f, VideoManager->_current_context.coordinate_system.GetVerticalDirection() * style.shadow_offset_y);
			_DrawTextHelper(buffer, fp, _GetTextShadowColor(style));
			glPopMatrix();
		}

		// Now draw the text itself, restore the position of the draw cursor, and move the draw cursor one line down
		_DrawTextHelper(buffer, fp, style.color);
		glPopMatrix();
		VideoManager->MoveRelative(0, -fp->line_skip * VideoManager->_current_context.coordinate_system.GetVerticalDirection());

	} while (last_line < text.length());

	VideoManager->PopState();
} // void TextSupervisor::Draw(const ustring& text)



int32 TextSupervisor::CalculateTextWidth(const std::string& font_name, const hoa_utils::ustring& text) {
	if (IsFontValid(font_name) == false) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "font name argument was invalid: " << font_name << endl;
		return -1;
	}

	int32 width;
	if (TTF_SizeUNICODE(_font_map[font_name]->ttf_font, text.c_str(), &width, NULL) == -1) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TTF_SizeUNICODE failed with TTF error: " << TTF_GetError() << endl;
		return -1;
	}

	return width;
}



int32 TextSupervisor::CalculateTextWidth(const std::string& font_name, const std::string& text) {
	if (IsFontValid(font_name) == false) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "font name argument was invalid: " << font_name << endl;
		return -1;
	}

	int32 width;
	if (TTF_SizeText(_font_map[font_name]->ttf_font, text.c_str(), &width, NULL) == -1) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TTF_SizeText failed with TTF error: " << TTF_GetError() << endl;
		return -1;
	}

	return width;
}



Color TextSupervisor::_GetTextShadowColor(const TextStyle& style) const {
	Color shadow_color;

	if (style.shadow_style == VIDEO_TEXT_SHADOW_NONE) {
		return shadow_color;
	}

	switch (style.shadow_style) {
		case VIDEO_TEXT_SHADOW_DARK:
			shadow_color = Color::black;
			shadow_color[3] = style.color[3] * 0.5f;
			break;
		case VIDEO_TEXT_SHADOW_LIGHT:
			shadow_color = Color::white;
			shadow_color[3] = style.color[3] * 0.5f;
			break;
		case VIDEO_TEXT_SHADOW_BLACK:
			shadow_color = Color::black;
			shadow_color[3] = style.color[3];
			break;
		case VIDEO_TEXT_SHADOW_COLOR:
			shadow_color = style.color;
			shadow_color[3] = style.color[3] * 0.5f;
			break;
		case VIDEO_TEXT_SHADOW_INVCOLOR:
			shadow_color = Color(1.0f - style.color[0], 1.0f - style.color[1], 1.0f - style.color[2], style.color[3] * 0.5f);
			break;
		default:
			IF_PRINT_WARNING(VIDEO_DEBUG) << "unknown text shadow style: " << style.shadow_style << endl;
			break;
	}

	return shadow_color;
}



void TextSupervisor::_CacheGlyphs(const uint16* text, FontProperties* fp) {
	if (fp == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "FontProperties argument was null" << endl;
		return;
	}

	// Empty string means there are no glyphs to cache
	if (*text == 0) {
		return;
	}

	static const SDL_Color glyph_color = { 0xFF, 0xFF, 0xFF, 0xFF }; // Opaque white color
	static const uint16 fall_back_glyph = '?'; // If we can't cache a particular glyph, we fall back to this one

	TTF_Font* font = fp->ttf_font;
	SDL_Surface* initial = NULL;
	SDL_Surface* intermediary = NULL;
	int32 w, h;
	GLuint texture;

	// Go through each character in the string and cache those glyphs that have not already been cached
	for (const uint16* character_ptr = text; *character_ptr != 0; ++character_ptr) {
		// A reference for legibility
		const uint16& character = *character_ptr;

		// Check if glyph already cached. If so, move on to the next character
		if (fp->glyph_cache->find(character) != fp->glyph_cache->end())
			continue;

		// Attempt to create the initial SDL_Surface that contains the rendered glyph
		initial = TTF_RenderGlyph_Blended(font, character, glyph_color);
		if (initial == NULL) {
			IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TTF_RenderGlyph_Blended() failed, resorting to fall back glyph: '?'" << endl;
			initial = TTF_RenderGlyph_Blended(font, fall_back_glyph, glyph_color);
			if (initial == NULL) {
				IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TTF_RenderGlyph_Blended() failed for fall back glyph, aborting glyph caching" << endl;
				return;
			}
		}

		w = RoundUpPow2(initial->w + 1);
		h = RoundUpPow2(initial->h + 1);

		intermediary = SDL_CreateRGBSurface(0, w, h, 32, RMASK, GMASK, BMASK, AMASK);
		if (intermediary == NULL) {
			SDL_FreeSurface(initial);
			IF_PRINT_WARNING(VIDEO_DEBUG) << "call to SDL_CreateRGBSurface() failed" << endl;
			return;
		}


		if (SDL_BlitSurface(initial, 0, intermediary, 0) < 0) {
			SDL_FreeSurface(initial);
			SDL_FreeSurface(intermediary);
			IF_PRINT_WARNING(VIDEO_DEBUG) << "call to SDL_BlitSurface() failed" << endl;
			return;
		}

		glGenTextures(1, &texture);
		TextureManager->_BindTexture(texture);


		SDL_LockSurface(intermediary);

		uint32 num_bytes = w * h * 4;
		for (uint32 j = 0; j < num_bytes; j += 4) {
			(static_cast<uint8*>(intermediary->pixels))[j+3] = (static_cast<uint8*>(intermediary->pixels))[j+2];
			(static_cast<uint8*>(intermediary->pixels))[j+0] = 0xff;
			(static_cast<uint8*>(intermediary->pixels))[j+1] = 0xff;
			(static_cast<uint8*>(intermediary->pixels))[j+2] = 0xff;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, intermediary->pixels );
		SDL_UnlockSurface(intermediary);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (VideoManager->CheckGLError()) {
			SDL_FreeSurface(initial);
			SDL_FreeSurface(intermediary);
			IF_PRINT_WARNING(VIDEO_DEBUG) << "an OpenGL error was detected: " << VideoManager->CreateGLErrorString() << endl;
			return;
		}

		int minx, maxx;
		int miny, maxy;
		int advance;
		if (TTF_GlyphMetrics(font, character, &minx, &maxx, &miny, &maxy, &advance) != 0) {
			SDL_FreeSurface(initial);
			SDL_FreeSurface(intermediary);
			IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TTF_GlyphMetrics() failed" << endl;
			return;
		}

		FontGlyph* glyph = new FontGlyph;
		glyph->texture = texture;
		glyph->min_x = minx;
		glyph->min_y = miny;
		glyph->top_y = fp->ascent - maxy;
		glyph->width = initial->w + 1;
		glyph->height = initial->h + 1;
		glyph->max_x = static_cast<float>(initial->w + 1) / static_cast<float>(w);
		glyph->max_y = static_cast<float>(initial->h + 1) / static_cast<float>(h);
		glyph->advance = advance;

		fp->glyph_cache->insert(pair<uint16, FontGlyph*>(character, glyph));

		SDL_FreeSurface(initial);
		SDL_FreeSurface(intermediary);
	}
} // void TextSupervisor::_CacheGlyphs(const uint16* text, FontProperties* fp)



void TextSupervisor::_DrawTextHelper(const uint16* const text, FontProperties* fp, Color text_color) {
	if (*text == 0) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid argument, empty string" << endl;
		return;
	}

	if (fp == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid argument, NULL font properties" << endl;
		return;
	}

	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);

	CoordSys& cs = VideoManager->_current_context.coordinate_system;

	_CacheGlyphs(text, fp);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);

	glPushMatrix();

	int font_width, font_height;
	if (TTF_SizeUNICODE(fp->ttf_font, text, &font_width, &font_height) != 0) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TTF_SizeUNICODE() failed" << endl;
		return;
	}

	float xoff = ((VideoManager->_current_context.x_align + 1) * font_width) * 0.5f * -cs.GetHorizontalDirection();
	float yoff = ((VideoManager->_current_context.y_align + 1) * font_height) * 0.5f * -cs.GetVerticalDirection();

	VideoManager->MoveRelative(xoff, yoff);

	float modulation = VideoManager->_screen_fader.GetFadeModulation();
	Color final_color = text_color * modulation;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	GLint vertices[8];
	GLfloat tex_coords[8];
	glVertexPointer(2, GL_INT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, tex_coords);

	// Iterate through each character in the string and render the character glyphs one at a time
	int xpos = 0;
	for (const uint16* glyph = text; *glyph != 0; glyph++) {
		FontGlyph* glyph_info = (*fp->glyph_cache)[*glyph];

		int x_hi = glyph_info->width;
		int y_hi = glyph_info->height;
		if (cs.GetHorizontalDirection() < 0.0f)
			x_hi = -x_hi;
		if (cs.GetVerticalDirection() < 0.0f)
			y_hi = -y_hi;

		int min_x, min_y;
		min_x = glyph_info->min_x * static_cast<int>(cs.GetHorizontalDirection()) + xpos;
		min_y = glyph_info->min_y * static_cast<int>(cs.GetVerticalDirection());

		float tx, ty;
		tx = glyph_info->max_x;
		ty = glyph_info->max_y;

		TextureManager->_BindTexture(glyph_info->texture);
		if (VideoManager->CheckGLError()) {
			IF_PRINT_WARNING(VIDEO_DEBUG) << "OpenGL error detected: " << VideoManager->CreateGLErrorString() << endl;
			return;
		}

		vertices[0] = min_x;
		vertices[1] = min_y;
		vertices[2] = min_x + x_hi;
		vertices[3] = min_y;
		vertices[4] = min_x + x_hi;
		vertices[5] = min_y + y_hi;
		vertices[6] = min_x;
		vertices[7] = min_y + y_hi;
		tex_coords[0] = 0.0f;
		tex_coords[1] = ty;
		tex_coords[2] = tx;
		tex_coords[3] = ty;
		tex_coords[4] = tx;
		tex_coords[5] = 0.0f;
		tex_coords[6] = 0.0f;
		tex_coords[7] = 0.0f;

		glColor4fv((GLfloat*)&final_color);
		glDrawArrays(GL_QUADS, 0, 4);

		xpos += glyph_info->advance;
	} // for (const uint16* glyph = text; *glyph != 0; glyph++)

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();

	glDisable(GL_ALPHA_TEST);
} // void TextSupervisor::_DrawTextHelper(const uint16* const text, FontProperties* fp, Color color)



bool TextSupervisor::_RenderText(hoa_utils::ustring& string, TextStyle& style, ImageMemory& buffer) {
	FontProperties* fp = _font_map[style.font];
	TTF_Font* font = fp->ttf_font;

	if (font == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "font of TextStyle argument '" << style.font << "' was invalid" << endl;
		return false;
	}

	static const SDL_Color white_color = { 0xFF, 0xFF, 0xFF, 0xFF };

	SDL_Surface* initial = NULL;
	SDL_Surface* intermediary = NULL;

	// Width and height of each line of text
	int32 line_w, line_h;
	// Minimum Y value of the line
	int32 min_y = 0;
	// Calculated line width
	int32 calc_line_width = 0;
	// Pixels left of '0' the first character extends, if any
	int32 line_start_x = 0;

	if (TTF_SizeUNICODE(font, string.c_str(), &line_w, &line_h) == -1) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TTF_SizeUNICODE() failed" << endl;
		return false;
	}

	_CacheGlyphs(string.c_str(), fp);

	// Calculate the width of the width and minimum y value of the text
	const uint16* char_ptr;
	for (char_ptr = string.c_str(); *char_ptr != '\0'; ++char_ptr) {
		FontGlyph* glyphinfo = (*fp->glyph_cache)[*char_ptr];
		if (glyphinfo->top_y < min_y)
			min_y = glyphinfo->top_y;
		calc_line_width += glyphinfo->advance;
	}

	// Subtract one pixel from the minimum y value (TODO: explain why)
	min_y -= 1;

	// Check if the first character starts left of pixel 0, and set
// 	char_ptr = string.c_str();
	if (*char_ptr) {
		FontGlyph* first_glyphinfo = (*fp->glyph_cache)[*char_ptr];
		if (first_glyphinfo->min_x < 0)
			line_start_x = first_glyphinfo->min_x;
	}

	// TTF_SizeUNICODE can underestimate line width as a result of its micro positioning.
	// Check if this condition is true and if so, set the line width appropriately.
	if (calc_line_width > line_w)
		line_w = calc_line_width;

	// Adjust line dimensions by negative starting offsets if present
	line_w -= line_start_x;
	line_h -= min_y;

	// Allocate enough memory for the entire text surface to reside on
	uint8* intermed_buf = static_cast<uint8*>(calloc(line_w * line_h, 4));
	intermediary = SDL_CreateRGBSurfaceFrom(intermed_buf, line_w, line_h, 32, line_w * 4, RMASK, GMASK, BMASK, AMASK);
	if (intermediary == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "call to SDL_CreateRGBSurfaceFrom() failed" << endl;
		return false;
	}

	// Go through the string and render each glyph one by one
	SDL_Rect surf_target;
	int32 xpos = -line_start_x;
	int32 ypos = -min_y;
	for (char_ptr = string.c_str(); *char_ptr != '\0'; ++char_ptr) {
		FontGlyph* glyphinfo = (*fp->glyph_cache)[*char_ptr];

		// Render the glyph
		initial = TTF_RenderGlyph_Blended(font, *char_ptr, white_color);
		if (initial == NULL) {
			IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TTF_RenderGlyph_Blended() failed" << endl;
			return false;
		}

		surf_target.x = xpos + glyphinfo->min_x;
		surf_target.y = ypos + glyphinfo->top_y;

		// Add the glyph to the end of the rendered string
		if (SDL_BlitSurface(initial, NULL, intermediary, &surf_target) < 0) {
			SDL_FreeSurface(initial);
			SDL_FreeSurface(intermediary);
			free(intermed_buf);
			IF_PRINT_WARNING(VIDEO_DEBUG) << "call to SDL_BlitSurface() failed, SDL error: " << SDL_GetError() << endl;
			return false;
		}
		SDL_FreeSurface(initial);
		xpos += glyphinfo->advance;
	}

	SDL_LockSurface(intermediary);

	uint8 color_mult[] = {
		static_cast<uint8>(style.color[0] * 0xFF),
		static_cast<uint8>(style.color[1] * 0xFF),
		static_cast<uint8>(style.color[2] * 0xFF)
	};

	uint32 num_bytes = intermediary->w * intermediary->h * 4;
	for (uint32 j = 0; j < num_bytes; j += 4) {
		((uint8*)intermediary->pixels)[j+3] = ((uint8*)intermediary->pixels)[j+2];
		((uint8*)intermediary->pixels)[j+0] = color_mult[0];
		((uint8*)intermediary->pixels)[j+1] = color_mult[1];
		((uint8*)intermediary->pixels)[j+2] = color_mult[2];
	}

	buffer.width = line_w;
	buffer.height = line_h;
	buffer.pixels = intermed_buf;

	SDL_UnlockSurface(intermediary);
	SDL_FreeSurface(intermediary);

	return true;
} // bool TextSupervisor::_RenderText(hoa_utils::ustring& string, TextStyle& style, ImageMemory& buffer)

}  // namespace hoa_video
