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
*** \file    text.h
*** \author  Lindsay Roberts, linds@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for text rendering
***
*** This code makes use of the SDL_ttf font library for representing fonts,
*** font glyphs, and text.
*** ***************************************************************************/

#ifndef __TEXT_HEADER__
#define __TEXT_HEADER__

#include "engine/video/image.h"

#include "utils/singleton.h"
#include "utils/ustring.h"

typedef struct _TTF_Font TTF_Font;

namespace vt_video
{

const std::string _LANGUAGE_FILE = "dat/config/languages.lua";

class TextSupervisor;

//! \brief The singleton pointer for the instance of the text supervisor
extern TextSupervisor *TextManager;


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
class FontGlyph
{
public:
    FontGlyph() :
        _texture(0),
        _width(0),
        _height(0),
        _min_x(0),
        _min_y(0),
        _tx(0.0f),
        _ty(0.0f),
        _advance(0)
    {
    }

    ~FontGlyph()
    {
        // Clean up the OpenGL texture.
        if (_texture != 0) {
            GLuint textures[] = { _texture };
            glDeleteTextures(1, textures);
            _texture = 0;
        }
    }

    //! \brief The index of the GL texture for this glyph.
    GLuint _texture;

    //! \brief The width and height of the glyph in pixels.
    int32 _width, _height;

    //! \brief The mininum x and y pixel coordinates of the glyph in texture space (refer to TTF_GlyphMetrics).
    int _min_x, _min_y;

    //! \brief The maximum x and y texture coordinates of the glyph. (The minimum texture coordinates are assumed to be zero.)
    float _tx, _ty;

    //! \brief The amount of space between glyphs.
    int32 _advance;

private:
    FontGlyph(const FontGlyph&) {}
    FontGlyph& operator=(const FontGlyph&) { return *this; }
};

/** ****************************************************************************
*** \brief A structure which holds properties about fonts
*** ***************************************************************************/
class FontProperties
{
public:
    FontProperties():
        height(0),
        line_skip(0),
        ascent(0),
        descent(0),
        ttf_font(NULL),
        font_size(0),
        glyph_cache(NULL)
    {
    }

    ~FontProperties()
    {
        ClearFont();
    }

    //! \brief Clears out a font object plus its glyph cache.
    //! Useful when changing a TextStyle font without deleting
    //! the font properties object.
    void ClearFont() {
        // Free the font.
        if (ttf_font)
            TTF_CloseFont(ttf_font);

        // Clears the glyph cache and delete it.
        if (glyph_cache) {
            std::vector<vt_video::FontGlyph *>::const_iterator it_end = glyph_cache->end();
            for(std::vector<FontGlyph *>::iterator j = glyph_cache->begin(); j != it_end; ++j) {
                delete *j;
            }
            delete glyph_cache;
        }
    }

    //! \brief The maximum height of all of the glyphs for this font.
    int32 height;

    //! \brief SDL_ttf's recommended amount of spacing between lines.
    int32 line_skip;

    //! \brief The height above and below baseline of font
    int32 ascent, descent;

    //! \brief A pointer to SDL_TTF's font structure.
    TTF_Font* ttf_font;

    //! \brief Used to know the font currently used.
    std::string font_filename;

    //! \brief Used to know the font size currently used.
    uint32 font_size;

    //! \brief A pointer to a cache which holds all of the glyphs used in this font.
    std::vector<FontGlyph*>* glyph_cache;

private:
    FontProperties(const FontProperties&) {}
    FontProperties& operator=(const FontProperties&) { return *this; }
};

/** ****************************************************************************
*** \brief A class encompassing all properties that define a text style
***
*** The TextSupervisor class maintains a TextStyle object that serves as the
*** default style. The various constructors for the TextStyle class will use the
*** properties of the default text style when they are not provided with the
*** information to initialize all of the class members.
*** ***************************************************************************/
class TextStyle
{
public:
    //! \brief No-arg constructor will set all members to the same value as the current default text style.
    //! Special empty TextStyle to permit a default initialization in the TextSupervisor.
    TextStyle():
        _shadow_style(VIDEO_TEXT_SHADOW_NONE),
        _shadow_offset_x(0),
        _shadow_offset_y(0),
        _font_property(NULL)
    {
    }

    //! \brief Constructor requiring a font name only.
    TextStyle(const std::string& font);

    //! \brief Constructor requiring a text color only.
    TextStyle(const Color& color);

    //! \brief Constructor requiring a shadow style only.
    TextStyle(TEXT_SHADOW_STYLE style);

    //! \brief Constructor requiring a font name and text color.
    TextStyle(const std::string& font, const Color& color);

    //! \brief Constructor requiring a font name and shadow style.
    TextStyle(const std::string& font, TEXT_SHADOW_STYLE style);

    //! \brief Constructor requiring a text color and shadow style.
    TextStyle(const Color& color, TEXT_SHADOW_STYLE style);

    //! \brief Constructor requiring a font name, color, and shadow style.
    TextStyle(const std::string& font, const Color& color, TEXT_SHADOW_STYLE style);

    //! \brief Full constructor requiring initialization data arguments for all class members.
    TextStyle(const std::string& font, const Color& color, TEXT_SHADOW_STYLE style, int32 shadow_x, int32 shadow_y);

    //! Sets the font name and updates the font properties.
    void SetFont(const std::string& font);

    //! Sets the font name and updates the font properties.
    void SetColor(const Color& color) {
        _color = color;
        _UpdateTextShadowColor();
    }

    //! Sets the shadow style.
    void SetShadowStyle(TEXT_SHADOW_STYLE shadow_style) {
        _shadow_style = shadow_style;
        _UpdateTextShadowColor();
    }

    //! Sets the shadow offsets from the given text.
    void SetShadowOffsets(int32 xoffset, int32 yoffset) {
        _shadow_offset_x = xoffset;
        _shadow_offset_y = yoffset;
    }

    //! \brief Returns the font property pointer value.
    FontProperties* GetFontProperties() const {
        return _font_property;
    }

    const std::string& GetFontName() const {
        return _font;
    }

    const Color& GetColor() const {
        return _color;
    }

    const Color& GetShadowColor() const {
        return _shadow_color;
    }

    TEXT_SHADOW_STYLE GetShadowStyle() const {
        return _shadow_style;
    }

    int32 GetShadowOffsetX() const {
        return _shadow_offset_x;
    }

    int32 GetShadowOffsetY() const {
        return _shadow_offset_y;
    }

private:
    //! \brief The string font name
    std::string _font;

    //! \brief The color of the text
    Color _color;

    //! \brief The enum representing the shadow style.
    TEXT_SHADOW_STYLE _shadow_style;
    //! \brief The shadow color corresponding to the current color and shadow style.
    Color _shadow_color;

    //! \brief The x and y offsets of the shadow.
    int32 _shadow_offset_x;
    int32 _shadow_offset_y;

    //! \brief A pointer to the FontProperty object.
    //! This acts as reference cache, thus it must not be deleted here!
    FontProperties* _font_property;

    //! \brief Updates the shadow color to correspond to the current color and shadow style.
    void _UpdateTextShadowColor();
};

namespace private_video
{

/** ****************************************************************************
*** \brief Represents an image of rendered text stored in a texture sheet
***
*** A text specific class derived from the BaseImage class, it contains a
*** unicode string and text style needed to render a piece of text.
*** ***************************************************************************/
class TextTexture : public private_video::BaseTexture
{
public:
    /** \brief Constructor defaults image as the first one in a texture sheet.
    *** \note The actual sheet where the image is located will be determined later.
    **/
    TextTexture(const vt_utils::ustring &string_, const TextStyle &style_);

    ~TextTexture();

    // ---------- Public members

    //! \brief The string represented
    vt_utils::ustring string;

    //! \brief The text style of the rendered string
    TextStyle style;

    // ---------- Public methods

    //! \brief Generate a text texture and add to a texture sheet
    bool Regenerate();

    //! \brief Reload texture to an already assigned texture sheet
    bool Reload();

private:
    TextTexture(const TextTexture &copy);
    TextTexture &operator=(const TextTexture &copy);
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
class TextElement : public ImageDescriptor
{
public:
    TextElement();

    TextElement(TextTexture *texture);

    ~TextElement();

    // ---------- Public members

    //! \brief The image that is being referenced by this object.
    TextTexture *text_texture;

    // ---------- Public methods

    void Clear();

    void Draw() const;

    void Draw(const Color &draw_color) const;

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
    void SetTexture(TextTexture *texture);

    void SetStatic(bool is_static) {
        _is_static = is_static;
    }

    void SetWidth(float width) {
        _width = width;
    }

    void SetHeight(float height) {
        _height = height;
    }

    void SetDimensions(float width, float height) {
        SetWidth(width);
        SetHeight(height);
    }
}; // class TextElement : public ImageDescriptor

} // namespace private_video

/** ****************************************************************************
*** \brief Represents a rendered text string
*** TextImage is a compound image containing each line of a text string.
*** ***************************************************************************/
class TextImage : public ImageDescriptor
{
    friend class VideoEngine;
public:
    //! \brief Construct empty text object
    TextImage();

    //! \brief Constructs rendered string of specified ustring
    TextImage(const vt_utils::ustring& text, const TextStyle& style = TextStyle());

    //! \brief Constructs rendered string of specified std::string
    TextImage(const std::string& text, const TextStyle& style = TextStyle());

    //! \brief Destructs TextImage, lowering reference counts on all contained timages.
    ~TextImage() {
        Clear();
    }

    TextImage(const TextImage &copy);
    TextImage &operator=(const TextImage &copy);

    // ---------- Public methods

    //! \brief Clears the image by resetting its properties
    void Clear();

    /** \brief Draws the rendered text to the screen with a color modulation
    *** \param draw_color The color to modulate the text by
    **/
    void Draw(const Color &draw_color = vt_video::Color::white) const;

    //! \brief Dervied from ImageDescriptor, this method is not used by TextImage
    void EnableGrayScale()
    {}

    //! \brief Dervied from ImageDescriptor, this method is not used by TextImage
    void DisableGrayScale()
    {}

    //! \brief Sets image to static/animated
    virtual void SetStatic(bool is_static) {
        _is_static = is_static;
    }

    //! \brief Sets width of the image
    virtual void SetWidth(float width) {
        _width = width;
    }

    //! \brief Sets height of the image
    virtual void SetHeight(float height) {
        _height = height;
    }

    //! \brief Sets the dimensions (width + height) of the image.
    virtual void SetDimensions(float width, float height) {
        _width  = width;
        _height = height;
    }

    //! \brief Sets the color for the image (for all four verteces).
    void SetColor(const Color &color) {
        _color[0] = _color[1] = _color[2] = _color[3] = color;
    }

    /** \brief Sets individual vertex colors in the image.
    *** \param tl top left vertex color
    *** \param tr top right vertex color
    *** \param bl bottom left vertex color
    *** \param br bottom right vertex color
    **/
    void SetVertexColors(const Color &tl, const Color &tr, const Color &bl, const Color &br) {
        _color[0] = tl;
        _color[1] = tr;
        _color[2] = bl;
        _color[3] = br;
    }

    //! \brief Sets the text contained
    void SetText(const vt_utils::ustring &text) {
        // Don't do anything if it's the same text
        if (_text == text)
            return;

        _text = text;
        _Regenerate();
    }

    void SetText(const vt_utils::ustring &text, const TextStyle& text_style) {
        _text = text;
        _style = text_style;
        _Regenerate();
    }

    //! \brief Sets the text (std::string version)
    void SetText(const std::string &text) {
        SetText(vt_utils::MakeUnicodeString(text));
    }

    //! \brief Sets the texts style - regenerating text if present.
    void SetStyle(const TextStyle& style) {
        _style = style;
        _Regenerate();
    }

    void SetText(const std::string &text, const TextStyle& text_style) {
        SetText(vt_utils::MakeUnicodeString(text), text_style);
    }

    void SetWordWrapWidth(uint32 width) {
        _max_width = width;
    }

    //! \name Class Member Access Functions
    //@{
    const vt_utils::ustring& GetString() const {
        return _text;
    }

    TextStyle GetStyle() const {
        return _style;
    }

    virtual float GetWidth() const {
        return _width;
    }

    uint32 GetWordWrapWidth() const {
        return _max_width;
    }
    //@}

private:
    //! \brief The unicode string of the text to render
    vt_utils::ustring _text;

    //! \brief The style to render the text in
    TextStyle _style;

    //! \brief The text max width, used for word wrapping
    uint32 _max_width;

    //! \brief The TextTexture elements representing rendered text portions, usually lines.
    std::vector<private_video::TextElement *> _text_sections;

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
class TextSupervisor : public vt_utils::Singleton<TextSupervisor>
{
    friend class vt_utils::Singleton<TextSupervisor>;
    friend class VideoEngine;
    friend class TextureController;
    friend class private_video::TextTexture;
    friend class TextImage;
    friend class TextStyle;

public:
    ~TextSupervisor();

    /** \brief Initializes the SDL_ttf library
    *** \return True if all initializations were successful, or false if there was an error
    **/
    bool SingletonInitialize();

    /** \brief Loads or reloads font needed by the given locale (ex: fr, it, ru, ...)
    *** using the font script filename: "dat/config/fonts.lua"
    *** \return false in case of an error.
    **/
    bool LoadFonts(const std::string& locale_name);
    //@}

    //! \name Text methods
    //@{
    /** \brief Renders and draws a unicode string of text to the screen in the default text style
    *** \param text The text string to draw in unicode format
    **/
    inline void Draw(const vt_utils::ustring &text) {
        Draw(text, _default_style);
    }

    /** \brief Renders and draws a string of text to the screen in a desired text style
    *** \param text The text string to draw in unicode format
    *** \param style A reference to the TextStyle to use for drawing the string
    **/
    void Draw(const vt_utils::ustring &text, const TextStyle &style);

    /** \brief Renders and draws a standard string of text to the screen in the default text style
    *** \param text The text string to draw in standard format
    **/
    inline void Draw(const std::string &text) {
        Draw(vt_utils::MakeUnicodeString(text));
    }

    /** \brief Renders and draws a standard string of text to the screen in a desired text style
    *** \param text The text string to draw in standard format
    *** \param style A reference to the TextStyle to use for drawing the string
    **/
    inline void Draw(const std::string &text, const TextStyle &style) {
        Draw(vt_utils::MakeUnicodeString(text), style);
    }

    /** \brief Calculates what the width would be for a unicode string of text if it were rendered
    *** \param ttf_font The True Type SDL font object
    *** \param text The text string in unicode format
    *** \return The width of the text as it would be rendered, or -1 if there was an error
    **/
    int32 CalculateTextWidth(TTF_Font* ttf_font, const vt_utils::ustring& text);

    /** \brief Calculates what the width would be for a standard string of text if it were rendered
    *** \param ttf_font The True Type SDL font object
    *** \param text The text string in standard format
    *** \return The width of the text as it would be rendered, or -1 if there was an error
    **/
    int32 CalculateTextWidth(TTF_Font* ttf_font, const std::string& text);

    /** \brief Returns the text as a vector of lines which text width is inferior or equal to the given pixel max width.
    *** \param text The ustring text
    *** \param ttf_font The True Type SDL font object
    **/
    std::vector<vt_utils::ustring> WrapText(const vt_utils::ustring& text, TTF_Font* ttf_font, uint32 max_width);
    //@}

    //! \name Class member access methods
    //@{
    const TextStyle &GetDefaultStyle() const {
        return _default_style;
    }

    void SetDefaultStyle(const TextStyle& style) {
        _default_style = style;
    }
    //@}

private:
    TextSupervisor();

    // ---------- Private members

    //! \brief The default text style
    TextStyle _default_style;

    /** \brief A container for properties for each font which has been loaded
    *** The key to the map is the font name.
    **/
    std::map<std::string, FontProperties *> _font_map;

    // ---------- Private methods

    /** \brief Loads or Reloads a font file from disk with a specific size and name
    *** \param Text style name The name which to refer to the text style after it is loaded
    *** \param font_filename The filename of the TTF font filename to load
    *** \param size The point size to set the font after it is loaded
    *** \return True if the font was successfully loaded, or false if there was an error
    **/
    bool _LoadFont(const std::string& textstyle_name, const std::string& font_filename, uint32 size);

    /** \brief Removes a loaded font from memory and frees up associated resources
    *** \param font_name The reference name of the font to unload
    ***
    *** If the argument name is invalid (i.e. no font with that reference name exists), this method will do
    *** nothing more than print out a warning message if running in debug mode.
    ***
    *** \todo Implement this function. Its not available yet because of potential problems with lingering references to the
    *** font (in TextStyle objects, or elswhere)
    **/
    void _FreeFont(const std::string &font_name);

    /** \brief Caches glyph information and textures for rendering
    *** \param text A pointer to the unicode string holding the characters (glyphs) to cache
    *** \param fp A pointer to the FontProperties representing the font being used in rendering the font
    **/
    void _CacheGlyphs(const uint16 *text, FontProperties *fp);

    /** \brief Renders a unicode string to the screen.
    *** \param text A pointer to a unicode string to draw.
    *** \param font_properties A pointer to the properties of the font to use in drawing the text.
    *** \param color The color to render the text in.
    ***
    *** This method is intended for drawing only a single line of text in a single color. It does not account for shadows.
    **/
    void _RenderText(const uint16* const text, FontProperties* font_properties, const Color& color);

    /** \brief Renders a unicode string to a pixel array.
    *** \param text The unicdoe string to render.
    *** \param style The text style to render the string in.
    *** \param buffer A reference to the pixel array where to place the rendered string into.
    *** \return True if the string was rendered successfully, or false if it was not.
    **/
    bool _RenderText(const vt_utils::ustring& text, TextStyle& style, private_video::ImageMemory& buffer);

    /** \brief Returns true if a font of a certain reference name exists
    *** \param font_name The reference name of the font to check
    *** \return True if font name is valid, false if it is not.
    **/
    bool _IsFontValid(const std::string& font_name) {
        return (_font_map.find(font_name) != _font_map.end());
    }

    /** \brief Get the font properties for a loaded font
    *** \param font_name The name reference of the loaded font
    *** \return A pointer to the FontProperties object with the requested data, or NULL if the properties could not be fetched
    **/
    FontProperties* _GetFontProperties(const std::string& font_name);
}; // class TextSupervisor : public vt_utils::Singleton

}  // namespace vt_video

#endif // __TEXT_HEADER__
