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
*** \file    video.h
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Daniel Steuernol, steu@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for video engine interface.
***
*** This code provides a comprehensive API for managing all drawing, rendering,
*** GUI, and other related graphical functions.
***
*** In addition for its use in the game, the video engine is also actively used
*** by the map editor GUI as a QT widget.
***
*** \note This code uses the OpenGL library for graphics rendering.
*** \note This code uses the SDL_Image library to load images.
*** \note This code uses the SDL_ttf 2.0 library for font rendering.
*** ***************************************************************************/

#ifndef __VIDEO_HEADER__
#define __VIDEO_HEADER__

#include "engine/video/color.h"
#include "engine/video/context.h"
#include "engine/video/coord_sys.h"
#include "engine/video/fade.h"
#include "engine/video/image.h"
#include "engine/video/screen_rect.h"
#include "engine/video/text.h"
#include "engine/video/texture_controller.h"
#include "engine/video/transform2d.h"

namespace vt_gui {
class TextBox;
class OptionBox;
class GUISystem;
class MenuWindow;
namespace private_gui {
class GUIElement;
}
}

namespace vt_map {
namespace private_map {
class MapTransitionEvent;
}
}

namespace vt_mode_manager {
class ModeEngine;
}

//! \brief All calls to the video engine are wrapped in this namespace.
namespace vt_video
{
class VideoEngine;

//! \brief The singleton pointer for the engine, responsible for all video operations.
extern VideoEngine *VideoManager;

//! \brief Determines whether the code in the vt_video namespace should print
extern bool VIDEO_DEBUG;

//! \brief The number of FPS samples to retain across frames
const uint32 FPS_SAMPLES = 250;

//! \brief Draw flags to control x and y alignment, flipping, and texture blending.
enum VIDEO_DRAW_FLAGS {
    VIDEO_DRAW_FLAGS_INVALID = -1,

    //! X draw alignment flags
    //@{
    VIDEO_X_LEFT = 1,
    VIDEO_X_CENTER = 2,
    VIDEO_X_RIGHT = 3,
    //@}

    //! Y draw alignment flags
    //@{
    VIDEO_Y_TOP = 4,
    VIDEO_Y_CENTER = 5,
    VIDEO_Y_BOTTOM = 6,
    //@}

    //! X flip flags
    //@{
    VIDEO_X_FLIP = 7,
    VIDEO_X_NOFLIP = 8,
    //@}

    //! Y flip flags
    //@{
    VIDEO_Y_FLIP = 9,
    VIDEO_Y_NOFLIP = 10,
    //@}

    //! Texture blending flags
    //@{
    VIDEO_NO_BLEND = 11,
    VIDEO_BLEND = 12,
    VIDEO_BLEND_ADD = 13,
    //@}

    VIDEO_DRAW_FLAGS_TOTAL = 14
};

//! \brief The standard screen resolution
const float	VIDEO_STANDARD_RES_WIDTH  = 1024.0f;
const float	VIDEO_STANDARD_RES_HEIGHT = 768.0f;

/** \brief Rotates a point (x,y) around the origin (0,0), by angle radians
*** \param x x coordinate of point to rotate
*** \param y y coordinate of point to rotate
*** \param angle amount to rotate by (in radians)
**/
void RotatePoint(float &x, float &y, float angle);


/** ****************************************************************************
*** \brief Manages all the video operations and serves as the API to the video engine.
***
*** This is one of the largest classes in the engine. Because it is so
*** large, the implementation of many of the methods for this class are split up
*** into multiple .cpp source files in the video code directory.
*** *****************************************************************************/
class VideoEngine : public vt_utils::Singleton<VideoEngine>
{
    friend class vt_utils::Singleton<VideoEngine>;

    friend class TextureController;
    friend class TextSupervisor;
    friend class vt_gui::GUISystem;

    friend class vt_gui::TextBox;
    friend class vt_gui::OptionBox;
    friend class vt_gui::MenuWindow;

    friend class vt_gui::private_gui::GUIElement;
    friend class private_video::TexSheet;
    friend class private_video::FixedTexSheet;
    friend class private_video::VariableTexSheet;

    friend class ImageDescriptor;
    friend class CompositeImage;
    friend class private_video::TextElement;
    friend class TextImage;

public:
    ~VideoEngine();

    /** \brief Initialzies all video engine libraries and sub-systems
    *** \return True if all initializations were successful, false if there was an error.
    **/
    bool SingletonInitialize();

    //! \brief Delayed setup calls, that require data from the settings file.
    //@{
    void SetInitialResolution(int32 width, int32 height);
    bool FinalizeInitialization();
    //@}

    // ---------- General methods

    /** \brief Sets one to multiple flags which control drawing orientation (flip, align, blending, etc). Simply pass
    *** \param first_flag The first (and possibly only) draw flag to set
    *** \param ... Additional draw flags. The list must terminate with a 0.
    *** \note Refer to the VIDEO_DRAW_FLAGS enum for a list of valid flags that this function will accept
    **/
    void SetDrawFlags(int32 first_flag, ...);

    /** \brief Clears the contents of the screen
    *** This method should be called at the beginning of every frame, before any draw operations
    *** are performed. Note that it only clears the color buffer, not any of the other OpenGL buffers.
    **/
    void Clear();

    /** \brief Clears the contents of the screen to a specific color
    *** \param background_color The color to set the cleared screen to
    **/
    void Clear(const Color &background_color);

    /** \brief Updates every main game sub-engines.
    **/
    void Update();

    //! \brief Displays potential debug information (FPS and textures).
    void DrawDebugInfo();

    /** \brief Retrieves the OpenGL error code and retains it in the _gl_error_code member
    *** \return True if an OpenGL error has been detected, false if no errors were detected
    *** \note This function only produces a meaningful result if the VIDEO_DEBUG variable is set to true. This is done
    *** because the call to glGetError() requires a round trip to the GPU and a flush of the rendering pipeline; a fairly
    *** expensive operation. If VIDEO_DEBUG is false, the function will always return false immediately.
    **/
    bool CheckGLError();

    //! \brief Returns the value of the most recently fetched OpenGL error code
    GLenum GetGLError() {
        return _gl_error_code;
    }

    ///! \brief Returns a string representation of the most recently fetched OpenGL error code
    const std::string CreateGLErrorString();

    // ---------- Screen size and resolution methods

    //! \brief Returns the width of the screen, in pixels
    int32 GetScreenWidth() const {
        return _screen_width;
    }

    //! \brief Returns the height of the screen, in pixels
    int32 GetScreenHeight() const {
        return _screen_height;
    }

    //! \brief Returns the viewport current offsets/width/height
    int32 GetViewportXOffset() const {
        return _viewport_x_offset;
    }
    int32 GetViewportYOffset() const {
        return _viewport_y_offset;
    }
    int32 GetViewportWidth() const {
        return _viewport_width;
    }
    int32 GetViewportHeight() const {
        return _viewport_height;
    }

    bool IsInitialized() const {
        return _initialized;
    }

    //! \brief Returns true if game is in fullscreen mode, false if it is in windowed mode
    bool IsFullscreen() const {
        return _fullscreen;
    }

    /** \brief sets the current resolution to the given width and height
    *** \param width new screen width
    *** \param height new screen height
    ***
    *** \note  you must call ApplySettings() to actually apply the change
    **/
    void SetResolution(uint32 width, uint32 height) {
        _temp_width = width;
        _temp_height = height;
    }

    /** \brief sets the game to fullscreen or windowed depending on whether
     *         true or false is passed
     *  \param fullscreen set to true if you want fullscreen, false for windowed.
     *  \note  you must call ApplySettings() to actually apply the change
     */
    void SetFullscreen(bool fullscreen) {
        _temp_fullscreen = fullscreen;
    }

    /** \brief Toggles fullscreen mode on or off
     *  \note  you must call ApplySettings() to actually apply the change
     */
    void ToggleFullscreen() {
        SetFullscreen(!_temp_fullscreen);
    }

    //! \brief Will make the pixel art related images smoothed (only used for map tiles at the moment)
    void SetPixelArtSmoothed(bool smooth) {
        _smooth_pixel_art = smooth;
    }

    //! \brief Returns true if pixel art textures should be smoothed (only used for map tiles at the moment)
    bool ShouldSmoothPixelArt() {
        return _smooth_pixel_art;
    }

    //! \brief Returns a reference to the current coordinate system
    const CoordSys &GetCoordSys() const {
        return _current_context.coordinate_system;
    }

    /** \brief Returns the pixel size expressed in coordinate system units
    *** \param x A reference of where to store the horizontal resolution
    *** \param x A reference of where to store the vertical resolution
    **/
    void GetPixelSize(float &x, float &y);

    /** \brief applies any changes to video settings like resolution and
     *         fullscreen. If the changes fail, then this function returns
     *         false, and the video settings are reset to whatever the last
     *         working setting was.
     * \return True if the video settings were successfully applied, false if they could not be applied
     */
    bool ApplySettings();

    // ---------- Coordinate system and viewport methods

    /** \brief Sets the coordinate system to use.
    *** \param left The coordinate for the left border of screen
    *** \param right The coordinate for the right border of screen
    *** \param top The coordinate for the top border of screen
    *** \param bottom The coordinate for the bottom border of screen
    *** \note The default coordinate system for the video engine is
    *** (0.0f, 1024.0f, 768.0f, 0.0f)
    **/
    void SetCoordSys(float left, float right, float bottom, float top) {
        SetCoordSys(CoordSys(left, right, bottom, top));
    }
    void SetStandardCoordSys() {
        SetCoordSys(CoordSys(0.0f, VIDEO_STANDARD_RES_WIDTH, VIDEO_STANDARD_RES_HEIGHT, 0.0f));
    }

    /** \brief Sets the coordinate system to use.
    *** \param coordinate_system The coordinate system to set the screen to use
    **/
    void SetCoordSys(const CoordSys &coordinate_system);

    /** \brief get the current viewport information
    *** \param x the current x location as a float
    *** \param y the current y location as a float
    *** \param width current width as a float
    *** \param height current height as a float
    **/
    void GetCurrentViewport(float &x, float &y, float &width, float &height);

    /** \brief assigns the viewport for open gl to draw into
    *** \param x the x start location
    *** \param y the y start location
    *** \param width the x width
    *** \param height the y height
    **/
    void SetViewport(float x, float y, float width, float height);

    //! Perform the OpenGL corresponding calls, but only if necessary.
    void EnableAlphaTest();
    void DisableAlphaTest();
    void EnableBlending();
    void DisableBlending();
    void EnableStencilTest();
    void DisableStencilTest();
    void EnableTexture2D();
    void DisableTexture2D();
    void EnableColorArray();
    void DisableColorArray();
    void EnableVertexArray();
    void DisableVertexArray();
    void EnableTextureCoordArray();
    void DisableTextureCoordArray();

    //!  \brief glVertexPointer wrapper, vertex type is restricted to float.
    void SetVertexPointer(GLint size, GLsizei stride, const float *ptr);

    //! \brief glDrawArrays wrapper.
    void DrawArrays(GLenum mode, GLint first, GLsizei count);

    /** \brief Enables the scissoring effect in the video engine
    *** Scissoring is where you can specify a rectangle of the screen which is affected
    *** by rendering operations (and hence, specify what area is not affected). Make sure
    *** to disable scissoring as soon as you're done using the effect, or all subsequent
    *** draw calls will get messed up.
    **/
    void EnableScissoring();

    //! \brief Disables the scissoring effect
    void DisableScissoring();

    //! \brief Returns true if scissoring is enabled, or false if it is not
    bool IsScissoringEnabled() {
        return _current_context.scissoring_enabled;
    }

    //! \brief Retrieves the current scissoring rectangle
    ScreenRect GetScissorRect() {
        return _current_context.scissor_rectangle;
    }

    /** \brief Sets the rectangle area to use for scissorring
    *** \param left Coordinate for left side of scissoring rectangle
    *** \param right Coordinate for right side of scissoring rectangle
    *** \param bottom Coordinate for bottom side of scissoring rectangle
    *** \param top Coordinate for top side of scissoring rectangle
    *** \note The coordinate arguments are based on the current coordinate system,
    *** not the screen coordinates
    **/
    void SetScissorRect(float left, float right, float bottom, float top);

    /** \brief Sets the rectangle area to use for scissorring
    *** \param rect The rectangle to set the scissoring rectangle to
    *** In this function the rect coordinates should have already been transformed to
    *** integer values (pixel unit) with (0,0) as the upper left and (w-1, h-1) as the
    *** lower right, where w and h are the current screen dimensions. Do <b>not</b>
    *** incorrectly assume that rect should contain coordinates based on the current
    *** coordinate system.
    **/
    void SetScissorRect(const ScreenRect &rect);

    /** \brief Converts coordinates from the current coordinate system into screen coordinates
    *** \return A ScreenRect object that contains the translated screen coordinates.
    *** Screen coordinates are in pixel units with (0,0) as the top left and (w-1, h-1)
    *** as the lower-right, where w and h are the dimensions of the screen.
    **/
    ScreenRect CalculateScreenRect(float left, float right, float bottom, float top);

    // ----------  Transformation methods

    /** \brief Gets the location of the draw cursor
    * \param x stores x position of the cursor
    * \param y stores y position of the cursor
    */
    void GetDrawPosition(float &x, float &y) const {
        x = _x_cursor;
        y = _y_cursor;
    }

    /** \brief Moves the draw cursor position to (x,y)
    *** \param x The x coordinate to move the draw cursor to
    *** \param y The y coordinate to move the draw cursor to
    **/
    void Move(float x, float y);

    /** \brief Moves the draw position relative to its current location by (x, y)
    *** \param x How far to move the draw cursor in the x direction
    *** \param y How far to move the draw cursor in the y direction
    **/
    void MoveRelative(float x, float y);

    /** \brief Saves the current modelview transformation on to the stack
    *** What this means is that it save the combined result of all transformation
    *** calls (Move/MoveRelative/Scale/Rotate)
    **/
    void PushMatrix();

    //! \brief Pops the modelview transformation from the stack
    void PopMatrix();

    /** \brief Saves relevant state of the video engine on to an internal stack
    *** The contents saved include the modelview transformation and the current
    *** video engine context.
    ***
    *** \note This is a very expensive function call. If you only need to push
    *** the current transformation, you should use PushMatrix() and PopMatrix().
    ***
    *** \note The size of the stack is small (around 32 entries), so you should
    *** try and limit the maximum number of pushed state entries so that this
    *** limit is not exceeded
    **/
    void PushState();

    //! \brief Restores the most recently pushed video engine state
    void PopState();

    /** \brief Rotates images counterclockwise by the specified number of radians
    *** \param angle How many radians to perform the rotation by
    *** \note You should understand how transformation matrices work in OpenGL
    *** prior to using this function.
    **/
    void Rotate(float angle);

    /** \brief Scales all subsequent image drawing calls in the horizontal and vertical direction
    *** \param x The amount of horizontal scaling to perform (0.5 for half, 1.0 for normal, 2.0 for double, etc)
    *** \param y The amount of vertical scaling to perform (0.5 for half, 1.0 for normal, 2.0 for double, etc)
    *** \note You should understand how transformation matrices work in OpenGL
    *** prior to using this function.
    **/
    void Scale(float x, float y);

    // ----------  Image operation methods

    /** \brief Captures the contents of the screen and saves it as an image texture
    *** \return An initialized StillImage object used to draw/manipulate the captured screen
    *** \throw Exception If the new captured screen could not be created
    ***
    *** When this function is called, it will generate an image using the contents that are
    *** being displayed on the current screen. This means that you can have multiple screen
    *** captures in memory at the same time. You should be careful not to have too many
    *** screen captures existing at one time, because each image capture requires a relatively
    *** large amount of texutre memory (roughly 3GB for a 1024x768 screen).
    **/
    StillImage CaptureScreen() throw(vt_utils::Exception);

    /** \brief Creates an image based on the raw image information passed in. This
    *** image can be rendered or used as a texture by the rendering system
    *** \param raw_image a pointer to a valid ImageMemory. It is assumed all the parameters such as width and height are set,
    *** and that the size of a pixel is 4-bytes wide
    *** \param image_name The unique image name that we will use to create this image. Note that if it is not unique we throw an Exception
    *** if delete_on_exist is not set to true (default)
    *** \param delete_on_exist Flag that indicates whether or not to destroy the image from the TextureManager if the image_name exists.
    *** Default true
    *** \return a valid StillImage that is created from the input parameter
    *** \throw Exception if the new image cannot be created
    **/
    StillImage CreateImage(private_video::ImageMemory *raw_image, const std::string &image_name, bool delete_on_exist = true) throw(vt_utils::Exception);

    //-- Overlays: Lighting, Lightning  -----------------------------------------------------

    /** \brief draws a halo at the current draw cursor position
     *
     *  \param id    image descriptor for the halo image
     *  \param color color of halo
     */
    void DrawHalo(const ImageDescriptor &id, const Color &color = Color::white);

    //-- Fading ---------------------------------------------------------------

    //! \brief call after all map images are drawn to apply a fade effect.
    void DrawFadeEffect();

    //! \brief disables all the active fade effects.
    void DisableFadeEffect();

    /** \brief Begins a game-wise screen fade.
    *** \param color The color to fade the screen to
    *** \param time The fading process will take this number of milliseconds
    **/
    void FadeScreen(const Color &color, uint32 time) {
        _screen_fader.BeginFade(color, time);
    }

    //! \brief Returns true if a screen fade is currently in progress
    bool IsFading() {
        return _screen_fader.IsFading();
    }

    //! \brief A shortcut function used to make a fade in more explicitely.
    void FadeIn(uint32 time) {
        _screen_fader.FadeIn(time);
    }

    //! \brief tells whether the last fade effect was transitional (done by the system).
    bool IsLastFadeTransitional() const {
        return _screen_fader.IsLastFadeTransitional();
    }

    //-- Screen shaking -------------------------------------------------------
    //! \brief Returns true if the screen is shaking
    //! \note The function acts as a wrapper for the current game mode effect supervisor
    //! and check for active shaking
    bool IsScreenShaking();

    //-- Miscellaneous --------------------------------------------------------

    /** \brief Sets a new brightness value
    *** \param value Brightness value [0.0f - 2.0f]. 1.0f is the default value
    **/
    void SetBrightness(float value);

    /** \brief Returns the current brightness value [0.0f - 2.0f]
    *** \return the brightness value
    **/
    float GetBrightness() const {
        return _brightness_value;
    }

    /** \brief Draws a colored line between two points
    *** \param x1 The x coordinate of the first point
    *** \param y1 The y coordinate of the first point
    *** \param x2 The x coordinate of the second point
    *** \param y2 The y coordinate of the second point
    *** \param width The width/thickness of the line to draw, in pixels
    *** \param color The color to draw the line in
    *** \todo Width argument should be an unsigned, non-zero integer
    **/
    void DrawLine(float x1, float y1, float x2, float y2, float width, const Color &color);

    /** \brief draws a line grid. Used by map editor to draw a grid over all
     *         the tiles. This function will start at (x,y), and go to
     *         (xMax, yMax), with horizontal cell spacing of xstep and
     *         vertical cell spacing of ystep. The final parameter is just the
     *         color the lines should be drawn
     *
     *  \note  xMax and yMax are not inputs to the function- they are taken
     *         from the current coord sys
     * \param x x coordinate to start grid at
     * \param y y coordinate to start grid at
     * \param x_step width of grid squares
     * \param y_step height of grid squares
     * \param c color of the grid
     */
    void DrawGrid(float x, float y, float x_step, float y_step, const Color &c);

    /** \brief Draws a solid rectangle of a given color.
    *** Draws a solid rectangle of a given color. For that, the lower-left corner
    *** of the rectangle has to be specified, and also its size. The parameters depends
    *** on the current Coordinate System.
    *** \param width Width of the rectangle.
    *** \param height Height of the rectangle.
    *** \param color Color to paint the rectangle.
    **/
    void DrawRectangle(float width, float height, const Color &color);

    /** \brief Draws an outline of a rectangle that is not filled in
    *** \param left The x coordinate corresponding to the left side of the rectangle
    *** \param right The x coordinate corresponding to the right side of the rectangle
    *** \param bottom The y coordinate corresponding to the bottom side of the rectangle
    *** \param top The y coordinate corresponding to the top side of the rectangle
    *** \param width The width/thickness of the outline to draw, in pixels
    *** \param color The color to draw the outline in
    *** \todo Width argument should be an unsigned, non-zero integer
    **/
    void DrawRectangleOutline(float x1, float y1, float x2, float y2, float width, const Color &color);

    /** \brief Takes a screenshot and saves the image to a file
    *** \param filename The name of the file, if any, to save the screenshot as. Default is "screenshot.png"
    **/
    void MakeScreenshot(const std::string &filename = "screenshot.png");

    /** \brief toggles debug information display.
    *** currently used for debugging game modes, and more especially the map mode.
     */
    void ToggleDebugInfo() {
        _debug_info = !_debug_info;
    }

    /**
    *** Tells whether the advanced (or debug) display should be drawn.
    **/
    bool DebugInfoOn() {
        return _debug_info;
    }

    /** \brief sets the default cursor to the image in the given filename
    * \param cursor_image_filename file containing the cursor image
    */
    bool SetDefaultCursor(const std::string &cursor_image_filename);

    /** \brief returns the cursor image
    * \return the cursor image
    */
    StillImage *GetDefaultCursor();

    /** Retrieves current set text style for rendering
     */
    TextStyle GetTextStyle();

    //! \brief toggles the FPS display
    void ToggleFPS() {
        _fps_display = !_fps_display;
    }
private:
    VideoEngine();

    //-- System fades. Only usable by the mode manager
    // and the MapTransition MapEvent.
    friend class vt_mode_manager::ModeEngine;
    friend class vt_map::private_map::MapTransitionEvent;
    void _StartTransitionFadeOut(const Color &final, uint32 time) {
        _screen_fader.StartTransitionFadeOut(final, time);
    }

    void _TransitionalFadeIn(uint32 time) {
        _screen_fader.TransitionalFadeIn(time);
    }

    //-- Private variables ----------------------------------------------------

    //! fps display flag. If true, FPS is displayed
    bool _fps_display;

    //! \brief A circular array of FPS samples used for calculating average FPS
    uint32 _fps_samples[FPS_SAMPLES];

    /** \brief Keeps track of the sum of FPS values over the last VIDEO_FPS_SAMPLES frames
    *** This is used to simplify the calculation of average frames per second.
    **/
    uint32 _fps_sum;

    //! \brief An index variable to keep track of the start of the circular fps_samples array.
    uint32 _current_sample;

    /** \brief The number of FPS samples currently recorded.
    *** This value should always be VIDEO_FPS_SAMPLES, unless the game has just started, in which
    *** case it could be anywhere from 0 to VIDEO_FPS_SAMPLES depending on how many frames have
    *** been displayed.
    **/
    uint32 _number_samples;

    //! The FPS text
    TextImage* _FPS_textimage;

    //! \brief Holds the most recently fetched OpenGL error code
    GLenum _gl_error_code;

    //! \brief those members are used to activate/deactivate the GL state only when needed.
    //! \brief Holds whether the GL_BLEND state is activated. Used to optimize the drawing logic
    bool _gl_blend_is_active;
    //! \brief Holds whether the GL_TEXTURE_2D state is activated. Used to optimize the drawing logic
    bool _gl_texture_2d_is_active;
    //! \brief Holds whether the GL_ALPHA_TEST state is activated. Used to optimize the drawing logic
    bool _gl_alpha_test_is_active;
    //! \brief Holds whether the GL_STENCIL_STATE state is activated. Used to optimize the drawing logic
    bool _gl_stencil_test_is_active;
    //! \brief Holds whether the GL_SCISSOR_TEST state is activated. Used to optimize the drawing logic
    bool _gl_scissor_test_is_active;
    //! \brief Holds whether the GL_VERTEX_ARRAY state is activated. Used to optimize the drawing logic
    bool _gl_vertex_array_is_activated;
    //! \brief Holds whether the GL_VERTEX_ARRAY state is activated. Used to optimize the drawing logic
    bool _gl_color_array_is_activated;
    //! \brief Holds whether the GL_VERTEX_ARRAY state is activated. Used to optimize the drawing logic
    bool _gl_texture_coord_array_is_activated;

    //! \brief The x/y offsets, width and height of the current viewport (the drawn part), in pixels
    //! \note the viewport is different from the screen size when in non-4:3 modes.
    int32 _viewport_x_offset;
    int32 _viewport_y_offset;
    int32 _viewport_width;
    int32 _viewport_height;

    //! \brief The width and height of the current screen (window), in pixels
    int32 _screen_width;
    int32 _screen_height;

    //! \brief True if the game is currently running fullscreen
    bool _fullscreen;

    //! \brief The x and y coordinates of the current draw cursor position
    float _x_cursor, _y_cursor;

    //! \brief Contains information about the current video engine's context, such as draw flags, the coordinate system, etc.
    private_video::Context _current_context;

    //! \brief Manages the current screen fading effect when fading is activated
    private_video::ScreenFader _screen_fader;

    //! Keeps whether debug info about the current game mode should be drawn.
    bool _debug_info;

    // Shaking effects

    //! X offset to shake the screen by (if any)
    float _x_shake;

    //! Y offset to shake the screen by (if any)
    float _y_shake;

    //! Current brightness value between 0.0f and 2.0f. 1.0f is the original brightness value.
    float _brightness_value;

    // changing the video settings does not actually do anything until
    // you call ApplySettings(). Up til that point, store them in temp
    // variables so if the new settings are invalid, we can roll back.

    //! holds the desired fullscreen status (true=fullscreen, false=windowed). Not actually applied until ApplySettings() is called
    bool _temp_fullscreen;

    //! holds the desired screen width. Not actually applied until ApplySettings() is called
    int32 _temp_width;

    //! holds the desired screen height. Not actually applied until ApplySettings() is called
    int32 _temp_height;

    //! \brief Tells whether pixel art sprites should be smoothed.
    bool _smooth_pixel_art;

    //! image which is to be used as the cursor
    StillImage _default_menu_cursor;

    //! Image used for rendering rectangles
    StillImage _rectangle_image;

    //! stack containing context, i.e. draw flags plus coord sys. Context is pushed and popped by any VideoEngine functions that clobber these settings
    std::stack<private_video::Context> _context_stack;

    //! stack containing 2D transforms. Pushed and popped by PushMatrix/PopMatrix.
    std::stack<vt_video::Transform2D> _transform_stack;

    //! cache vertex array data to be able to apply transform before drawing.
    std::vector<float> _transformed_vertex_array;
    float *_transformed_vertex_array_ptr;
    const float *_vertex_array_ptr;
    GLsizei _vertex_array_stride;
    GLint _vertex_array_size;

    //! check to see if the VideoManager has already been setup.
    bool _initialized;

    //-- Private methods ------------------------------------------------------

    /** \brief converts VIDEO_DRAW_LEFT or VIDEO_DRAW_RIGHT flags to a numerical offset
    * \param xalign the draw flag
    * \return the numerical offset
    */
    int32 _ConvertXAlign(int32 xalign);

    /** \brief converts VIDEO_DRAW_TOP or VIDEO_DRAW_BOTTOM flags to a numerical offset
    * \param yalign the draw flag
    * \return the numerical offset
    */
    int32 _ConvertYAlign(int32 yalign);

    /**
    * \brief takes an x value and converts it into screen coordinates
    * \return the converted value
    */
    int32 _ScreenCoordX(float x);

    /**
    * \brief takes an x value and converts it into screen coordinates
    * \return the converted value
    */
    int32 _ScreenCoordY(float y);

    //! \brief Updates the viewport metrics according to the current screen width/height.
    //! \note it also centers the viewport when the resolution isn't a 4:3 one.
    void _UpdateViewportMetrics();

    // Debug info
    //! \brief Updates the FPS counter.
    void _UpdateFPS();
    //! \brief Draws the current average FPS to the screen.
    void _DrawFPS();
}; // class VideoEngine : public vt_utils::Singleton<VideoEngine>

}  // namespace vt_video

#endif // __VIDEO_HEADER__
