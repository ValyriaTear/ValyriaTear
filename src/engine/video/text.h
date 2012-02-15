///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    text.h
*** \author  Lindsay Roberts, linds@allacrost.org
*** \brief   Header file for text rendering
***
*** This code makes use of the SDL_ttf font library for representing fonts,
*** font glyphs, and text.
*** ***************************************************************************/

#ifndef __TEXT_HEADER__
#define __TEXT_HEADER__

#include "defs.h"
#include "utils.h"

typedef struct _TTF_Font TTF_Font;

namespace hoa_video {

//! \brief The singleton pointer for the instance of the text supervisor
extern TextSupervisor* TextManager;


//! \brief Styles for setting the type of text shadows.
enum TEXT_SHADOW_STYLE {
	VIDEO_TEXT_SHADOW_INVALID = -1,

	//! \brief No text shadow is drawn, even if shadows are enabled.
	VIDEO_TEXT_SHADOW_NONE = 0,
	//! \brief Shadowed area is darkened (this is the default).
	VIDEO_TEXT_SHADOW_DARK = 1,
	//! \brief Shadowed area is lightened.
	VIDEO_TEXT_SHADOW_LIGHT = 2,
	//! \brief Shadowed area is completely black.
	VIDEO_TEXT_SHADOW_BLACK = 3,
	//! \brief Shadowed area is the same color of the text, but has less alpha.
	VIDEO_TEXT_SHADOW_COLOR = 4,
	//! \brief Shadowed area is the inverse of the text's color (e.g. white text, black shadow).
	VIDEO_TEXT_SHADOW_INVCOLOR = 5,

	VIDEO_TEXT_SHADOW_TOTAL = 6
};


/** ****************************************************************************
*** \brief A structure to hold properties about a particular font glyph
*** ***************************************************************************/
class FontGlyph {
public:
	//! \brief The index of the GL texture for this glyph.
	GLuint texture;

	//! \brief The width and height of the glyph in pixels.
	int32 width, height;

	//! \brief The mininum x and y pixel coordinates of the glyph in texture space (refer to TTF_GlyphMetrics).
	int min_x, min_y;

	//! \brief The maximum x and y pixel coordinates of the glyph in texture space (refer to TTF_GlyphMetrics).
	float max_x, max_y;

	//! \brief The amount of space between glyphs.
	int32 advance;

	//! \brief The top y value of the glyph.
	int top_y;
}; // class FontGlyph


/** ****************************************************************************
*** \brief A structure which holds properties about fonts
*** ***************************************************************************/
class FontProperties {
public:
	//! \brief The maximum height of all of the glyphs for this font.
	int32 height;

	//! \brief SDL_ttf's recommended amount of spacing between lines.
	int32 line_skip;

	//! \brief The height above and below baseline of font
	int32 ascent, descent;

	//! \brief A pointer to SDL_TTF's font structure.
	TTF_Font* ttf_font;

	//! \brief A pointer to a cache which holds all of the glyphs used in this font.
	std::map<uint16, FontGlyph*>* glyph_cache;
}; // class FontProperties


/** ****************************************************************************
*** \brief A class encompassing all properties that define a text style
***
*** The TextSupervisor class maintains a TextStyle object that serves as the
*** default style. The various constructors for the TextStyle class will use the
*** properties of the default text style when they are not provided with the
*** information to initialize all of the class members.
*** ***************************************************************************/
class TextStyle {
public:
	//! \brief No-arg constructor will set all members to the same value as the current default text style
	TextStyle();

	//! \brief Constructor requiring a font name only
	TextStyle(std::string fnt);

	//! \brief Constructor requiring a text color only
	TextStyle(Color c);

	//! \brief Constructor requiring a shadow style only
	TextStyle(TEXT_SHADOW_STYLE style);

	//! \brief Constructor requiring a font name and text color
	TextStyle(std::string fnt, Color c);

	//! \brief Constructor requiring a font name and shadow style
	TextStyle(std::string fnt, TEXT_SHADOW_STYLE style);

	//! \brief Constructor requiring a text color and shadow style
	TextStyle(Color c, TEXT_SHADOW_STYLE style);

	//! \brief Constructor requiring a font name, color, and shadow style
	TextStyle(std::string fnt, Color c, TEXT_SHADOW_STYLE style);

	//! \brief Full constructor requiring initialization data arguments for all class members
	TextStyle(std::string fnt, Color c, TEXT_SHADOW_STYLE style, int32 shadow_x, int32 shadow_y);

	// ---------- Public members

	//! \brief The string font name
	std::string font;

	//! \brief The color of the text
	Color color;

	//! \brief The enum representing the shadow style
	TEXT_SHADOW_STYLE shadow_style;

	//! \brief The x and y offsets of the shadow
	int32 shadow_offset_x, shadow_offset_y;
}; // class TextStyle

namespace private_video {

/** ****************************************************************************
*** \brief Represents an image of rendered text stored in a texture sheet
***
*** A text specific class derived from the BaseImage class, it contains a
*** unicode string and text style needed to render a piece of text.
*** ***************************************************************************/
class TextTexture : public private_video::BaseTexture {
public:
	/** \brief Constructor defaults image as the first one in a texture sheet.
	*** \note The actual sheet where the image is located will be determined later.
	**/
	TextTexture(const hoa_utils::ustring& string_, const TextStyle& style_);

	~TextTexture();

	// ---------- Public members

	//! \brief The string represented
	hoa_utils::ustring string;

	//! \brief The text style of the rendered string
	TextStyle style;

	// ---------- Public methods

	//! \brief Generate a text texture and add to a texture sheet
	bool Regenerate();

	//! \brief Reload texture to an already assigned texture sheet
	bool Reload();

private:
	TextTexture(const TextTexture& copy);
	TextTexture& operator=(const TextTexture& copy);
}; // class TextTexture : public private_video::BaseImage


/** ****************************************************************************
*** \brief An element used as a portion of a full rendered block of text.
***
*** This class represents a block (usually a single line) of rendered text that
*** is stored by the TextImage class as a portion, or element, that makes up
*** a rendered block of text. This class managed a pointer to a TextTexture
*** object that refers to the rendered line of text as it is stored in texture
*** memory.
*** ***************************************************************************/
class TextElement : public ImageDescriptor {
public:
	TextElement();

	TextElement(TextTexture* texture);

	~TextElement();

	// ---------- Public members

	//! \brief The image that is being referenced by this object.
	TextTexture* text_texture;

	// ---------- Public methods

	void Clear();

	void Draw() const;

	void Draw(const Color& draw_color) const;

	void EnableGrayScale()
		{}

	void DisableGrayScale()
		{}

	/** \brief Sets the texture used by the class and modifies the width and height members
	*** \param texture A pointer to the TextTexture object that this class object should manage
	***
	*** Because it is rare that the user needs to define a custom width/height for the text (since
	*** scaled text looks so poor), this function also automatically sets the _width and _height members
	*** to the translated value of the width and height of the TextTexture in the current coordinate system
	*** (current context) in which this function call is made. Passing a NULL argument will deallocate any
	*** texture resources and set the width/height to zero.
	***
	*** \note This function will invoke the AddReference() method for the argument object if it is not NULL.
	**/
	void SetTexture(TextTexture* texture);

	void SetStatic(bool is_static)
		{ _is_static = is_static; }

	void SetWidth(float width)
		{ _width = width; }

	void SetHeight(float height)
		{ _height = height; }

	void SetDimensions(float width, float height)
		{ SetWidth(width); SetHeight(height); }
}; // class TextElement : public ImageDescriptor

} // namespace private_video

/** ****************************************************************************
*** \brief Represents a rendered text string
*** TextImage is a compound image containing each line of a text string.
*** ***************************************************************************/
class TextImage : public ImageDescriptor {
	friend class VideoEngine;
public:
	//! \brief Construct empty text object
	TextImage();

	//! \brief Constructs rendered string of specified ustring
	TextImage(const hoa_utils::ustring& string, TextStyle style = TextStyle());

	//! \brief Constructs rendered string of specified std::string
	TextImage(const std::string& string, TextStyle style = TextStyle());

	//! \brief Destructs TextImage, lowering reference counts on all contained timages.
	~TextImage()
		{ Clear(); }

	TextImage(const TextImage& copy);
	TextImage& operator=(const TextImage& copy);

	// ---------- Public methods

	//! \brief Clears the image by resetting its properties
	void Clear();

	//! \brief Draws the rendered text to the screen
	void Draw() const;

	/** \brief Draws the rendered text to the screen with a color modulation
	*** \param draw_color The color to modulate the text by
	**/
	void Draw(const Color& draw_color) const;

	//! \brief Dervied from ImageDescriptor, this method is not used by TextImage
	void EnableGrayScale()
		{}

	//! \brief Dervied from ImageDescriptor, this method is not used by TextImage
	void DisableGrayScale()
		{}

	//! \brief Sets image to static/animated
	virtual void SetStatic(bool is_static)
		{ _is_static = is_static; }

	//! \brief Sets width of the image
	virtual void SetWidth(float width)
		{ _width = width; }

	//! \brief Sets height of the image
	virtual void SetHeight(float height)
		{ _height = height; }

	//! \brief Sets the dimensions (width + height) of the image.
	virtual void SetDimensions(float width, float height)
		{ _width  = width; _height = height; }

	//! \brief Dervied from ImageDescriptor, this method is not used by TextImage
	void SetUVCoordinates(float u1, float v1, float u2, float v2)
		{}

	//! \brief Sets the color for the image (for all four verteces).
	void SetColor(const Color &color)
		{ _color[0] = _color[1] = _color[2] = _color[3] = color; }

	/** \brief Sets individual vertex colors in the image.
	*** \param tl top left vertex color
	*** \param tr top right vertex color
	*** \param bl bottom left vertex color
	*** \param br bottom right vertex color
	**/
	void SetVertexColors(const Color &tl, const Color &tr, const Color &bl, const Color &br)
		{ _color[0] = tl; _color[1] = tr; _color[2] = bl; _color[3] = br; }

	//! \brief Sets the text contained
	void SetText(const hoa_utils::ustring &string)
		{ _string = string; _Regenerate(); }

	//! \brief Sets the text (std::string version)
	void SetText(const std::string &string)
		{ SetText(hoa_utils::MakeUnicodeString(string)); _Regenerate(); }

	//! \brief Sets the texts style - regenerating text if present.
	void SetStyle(TextStyle style)
		{ _style = style; _Regenerate(); }

	//! \name Class Member Access Functions
	//@{
	hoa_utils::ustring GetString() const
		{ return _string; }

	TextStyle GetStyle() const
		{ return _style; }
	//@}

private:
	//! \brief The unicode string of the text to render
	hoa_utils::ustring _string;

	//! \brief The style to render the text in
	TextStyle _style;

	//! \brief The TextTexture elements representing rendered text portions, usually lines.
	std::vector<private_video::TextElement*> _text_sections;

	// ---------- Private methods

	//! \brief Regenerates the texture images for the text
	void _Regenerate();
}; // class TextImage : public ImageDescriptor


/** ****************************************************************************
*** \brief A helper class to the video engine to manage all text rendering
***
*** This class is a singleton and it is both created and destroyed by the VideoEngine
*** class. TextSupervisor is essentially an extension of the VideoEngine singleton
*** class which handles all font and text related operations.
***
*** \note The singleton name of this class is "TextManager"
***
*** \note When the API user needs to access methods of this class, the recommended
*** way for doing so is to call "VideoManager->Text()->MethodName()".
*** VideoManager->Text() returns the singleton pointer to this class.
*** ***************************************************************************/
class TextSupervisor : public hoa_utils::Singleton<TextSupervisor> {
	friend class hoa_utils::Singleton<TextSupervisor>;
	friend class VideoEngine;
	friend class TextureController;
	friend class private_video::TextTexture;
	friend class TextImage;

public:
	~TextSupervisor();

	/** \brief Initialzies the SDL_ttf library and loads a debug_font
	*** \return True if all initializations were successful, or false if there was an error
	**/
	bool SingletonInitialize();

	//! \name Font manipulation methods
	//@{
	/** \brief Loads a font file from disk with a specific size and name
	*** \param font_filename The filename of the font file to load
	*** \param font_name The name which to refer to the font after it is loaded
	*** \param size The point size to set the font after it is loaded
	*** \param make_default If set to true, this font will be made the default font if it is loaded successfully (default value = false)
	*** \return True if the font was successfully loaded, or false if there was an error
	**/
	bool LoadFont(const std::string& filename, const std::string& font_name, uint32 size, bool make_default = false);

	/** \brief Removes a loaded font from memory and frees up associated resources
	*** \param font_name The reference name of the font to unload
	***
	*** If the argument name is invalid (i.e. no font with that reference name exists), this method will do
	*** nothing more than print out a warning message if running in debug mode.
	***
	*** \todo Implement this function. Its not available yet because of potential problems with lingering references to the
	*** font (in TextStyle objects, or elswhere)
	**/
	void FreeFont(const std::string& font_name);

	/** \brief Returns true if a font of a certain reference name exists
	*** \param font_name The reference name of the font to check
	*** \return True if font name is valid, false if it is not.
	**/
	bool IsFontValid(const std::string& font_name)
		{ return (_font_map.find(font_name) != _font_map.end()); }

	/** \brief Get the font properties for a loaded font
	*** \param font_name The name reference of the loaded font
	*** \return A pointer to the FontProperties object with the requested data, or NULL if the properties could not be fetched
	**/
	FontProperties* GetFontProperties(const std::string& font_name);
	//@}

	//! \name Text methods
	//@{
	/** \brief Renders and draws a unicode string of text to the screen in the default text style
	*** \param text The text string to draw in unicode format
	**/
	void Draw(const hoa_utils::ustring& text)
		{ Draw(text, _default_style); }

	/** \brief Renders and draws a string of text to the screen in a desired text style
	*** \param text The text string to draw in unicode format
	*** \param style A reference to the TextStyle to use for drawing the string
	**/
	void Draw(const hoa_utils::ustring& text, const TextStyle& style);

	/** \brief Renders and draws a standard string of text to the screen in the default text style
	*** \param text The text string to draw in standard format
	**/
	void Draw(const std::string& text)
		{ Draw(hoa_utils::MakeUnicodeString(text)); }

	/** \brief Renders and draws a standard string of text to the screen in a desired text style
	*** \param text The text string to draw in standard format
	*** \param style A reference to the TextStyle to use for drawing the string
	**/
	void Draw(const std::string& text, const TextStyle& style)
		{ Draw(hoa_utils::MakeUnicodeString(text), style); }

	/** \brief Calculates what the width would be for a unicode string of text if it were rendered
	*** \param font_name The reference name of the font to use for the calculation
	*** \param text The text string in unicode format
	*** \return The width of the text as it would be rendered, or -1 if there was an error
	**/
	int32 CalculateTextWidth(const std::string& font_name, const hoa_utils::ustring& text);

	/** \brief Calculates what the width would be for a standard string of text if it were rendered
	*** \param font_name The reference name of the font to use for the calculation
	*** \param text The text string in standard format
	*** \return The width of the text as it would be rendered, or -1 if there was an error
	**/
	int32 CalculateTextWidth(const std::string& font_name, const std::string& text);
	//@}

	//! \name Class member access methods
	//@{
	const TextStyle& GetDefaultStyle() const
		{ return _default_style; }

	void SetDefaultStyle(TextStyle style)
		{ _default_style = style; }
	//@}

private:
	TextSupervisor();

	// ---------- Private members

	//! \brief The default text style
	TextStyle _default_style;

	/** \brief A container for properties for each font which has been loaded
	*** The key to the map is the font name.
	**/
	std::map<std::string, FontProperties*> _font_map;

	// ---------- Private methods

	/** \brief Retrieves the color for a shadow based on the current text color and a shadow style
	*** \param style The text style that would be used to generate the shadow for the text
	*** \return The color of the shadow
	**/
	Color _GetTextShadowColor(const TextStyle& style) const;

	/** \brief Caches glyph information and textures for rendering
	*** \param text A pointer to the unicode string holding the characters (glyphs) to cache
	*** \param fp A pointer to the FontProperties representing the font being used in rendering the font
	**/
	void _CacheGlyphs(const uint16* text, FontProperties* fp);

	/** \brief Draws text to the screen using OpenGL commands
	*** \param text A pointer to a unicode string holding the text to draw
	*** \param fp A pointer to the properties of the font to use in drawing the text
	*** \param text_color The color to render the text in
	***
	*** This class assists the public Draw methods. This method is intended for drawing only
	*** a single line of text in a single color (it does not account for shadows).
	**/
	void _DrawTextHelper(const uint16* const text, FontProperties* fp, Color text_color);

	/** \brief Renders a unicode string with a given TextStyle to a pixel array
	*** \param string The unicdoe string to render
	*** \param style The text style to render the string in
	*** \param buffer A reference to the pixel array where to place the rendered string to
	*** \return True if the string was rendered successfully, or false if it was not
	**/
	bool _RenderText(hoa_utils::ustring& string, TextStyle& style, private_video::ImageMemory& buffer);
}; // class TextSupervisor : public hoa_utils::Singleton

}  // namespace hoa_video

#endif // __TEXT_HEADER__
