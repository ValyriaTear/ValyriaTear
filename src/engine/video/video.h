///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
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

#include "engine/video/video_utils.h"

#include "engine/video/color.h"
#include "engine/video/context.h"
#include "engine/video/coord_sys.h"
#include "engine/video/fade.h"
#include "engine/video/gl/gl_shader_definitions.h"
#include "engine/video/gl/gl_shader_programs.h"
#include "engine/video/gl/gl_shaders.h"
#include "engine/video/gl/gl_transform.h"
#include "engine/video/image.h"
#include "engine/video/screen_rect.h"
#include "engine/video/text.h"
#include "engine/video/texture_controller.h"

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
namespace vt_video {

namespace gl {
class ParticleSystem;
class RenderTarget;
class Shader;
class ShaderProgram;
class Sprite;
}

class VideoEngine;

//! \brief The singleton pointer for the engine, responsible for all video operations.
extern VideoEngine *VideoManager;

//! \brief Determines whether the code in the vt_video namespace should print
extern bool VIDEO_DEBUG;

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
    bool SingletonInitialize()
    { return true; }

    //! \brief Delayed setup calls, that require data from the settings file.
    //@{
    bool FinalizeInitialization();
    //@}

    // ---------- General methods

    /** \brief Sets one to multiple flags which control drawing orientation (flip, align, blending, etc). Simply pass
    *** \param first_flag The first (and possibly only) draw flag to set
    *** \param ... Additional draw flags. The list must terminate with a 0.
    *** \note Refer to the VIDEO_DRAW_FLAGS enum for a list of valid flags that this function will accept
    **/
    void SetDrawFlags(int32_t first_flag, ...);

    /** \brief Clears the contents of the framebuffer.
    **/
    void Clear();

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
    int32_t GetScreenWidth() const {
        return _screen_width;
    }

    //! \brief Returns the height of the screen, in pixels
    int32_t GetScreenHeight() const {
        return _screen_height;
    }

    //! \brief Returns the viewport current offsets/width/height
    int32_t GetViewportXOffset() const {
        return _viewport_x_offset;
    }
    int32_t GetViewportYOffset() const {
        return _viewport_y_offset;
    }
    int32_t GetViewportWidth() const {
        return _viewport_width;
    }
    int32_t GetViewportHeight() const {
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
    void SetResolution(uint32_t width, uint32_t height) {
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

    //! \brief Sets VSync mode support.
    //! \param mode 0 or > 2: None, 1: VSync, 2: Swap Tearing
    void SetVSyncMode(uint32_t mode) {
        _vsync_mode = mode;
    }

    //! \brief Gets current VSync mode support.
    //! \return 0 or > 2: None, 1: VSync, 2: Swap Tearing
    inline uint32_t GetVSyncMode() const {
        return _vsync_mode;
    }

    //! \brief Sets the game main loop update mode.
    //! \param update_mode true for performance, false for the CPU-gentle loop.
    //! \note  It is always on performance when VSync is enabled.
    void SetGameUpdateMode(bool update_mode) {
        _game_update_mode = update_mode;
    }

    //! \brief Gets the game main loop update mode.
    //! \param update_mode true for performance, false for the CPU-gentle loop.
    //! \note  It is always on performance when VSync is enabled.
    inline bool GetGameUpdateMode() const {
        return _game_update_mode;
    }

    //! \brief Returns a reference to the current coordinate system
    const CoordSys& GetCoordSys() const {
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
    void EnableBlending();
    void DisableBlending();
    void EnableStencilTest();
    void DisableStencilTest();
    void EnableTexture2D();
    void DisableTexture2D();

    //! Enables the secondary render target.
    void EnableSecondaryRenderTarget();

    //! Disables the secondary render target.
    void DisableSecondaryRenderTarget();

    /** \brief Draws the secondary render target onto the primary render target.
    ***
    ***        This function automatically disables the secondary render target
    ***        before drawing its texture to the primary render target.
    **/
    void DrawSecondaryRenderTarget();

    //! \brief Loads a shader program.
    gl::ShaderProgram* LoadShaderProgram(const gl::shader_programs::ShaderPrograms& shader_program);

    //! \brief Unloads the currently loaded shader program.
    void UnloadShaderProgram();

    //! \brief Draws a particle system.
    void DrawParticleSystem(gl::ShaderProgram* shader_program,
                            float* vertex_positions,
                            float* vertex_texture_coordinates,
                            float* vertex_colors,
                            unsigned number_of_vertices);

    //! \brief Draws a sprite.
    void DrawSprite(gl::ShaderProgram* shader_program,
                    float* vertex_positions,
                    float* vertex_texture_coordinates,
                    float* vertex_colors,
                    const Color& color = ::vt_video::Color::white);

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
    *** \param x Coordinate for the X value of scissoring rectangle
    *** \param y Coordinate for the Y value of scissoring rectangle
    *** \param width Coordinate for the width of scissoring rectangle
    *** \param height Coordinate for the height of scissoring rectangle
    *** \note The arguments are based on screen coordinates
    **/
    void SetScissorRect(int32_t x, int32_t y, int32_t width, int32_t height);

    /** \brief Sets the rectangle area to use for scissorring
    *** \param rect The rectangle to set the scissoring rectangle to
    *** \note The arguments are based on screen coordinates
    **/
    void SetScissorRect(const ScreenRect& screen_rectangle);

    // ----------  Transformation methods

    /** \brief Gets the location of the draw cursor
    * \param x stores x position of the cursor
    * \param y stores y position of the cursor
    */
    void GetDrawPosition(float &x, float &y) const {
        x = _x_cursor;
        y = _y_cursor;
    }

    /** \brief Saves the current modelview transformation on to the stack
    *** What this means is that it save the combined result of all transformation
    *** calls (Move/MoveRelative/Scale/Rotate)
    **/
    void PushMatrix();

    //! \brief Pops the modelview transformation from the stack
    void PopMatrix();

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

    /** \brief Rotates images counterclockwise by the specified number of degrees.
    *** \param angle How many degrees to perform the rotation by.
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
    void FadeScreen(const Color &color, uint32_t time) {
        _screen_fader.BeginFade(color, time);
    }

    //! \brief Returns true if a screen fade is currently in progress
    bool IsFading() {
        return _screen_fader.IsFading();
    }

    //! \brief A shortcut function used to make a fade in more explicitely.
    void FadeIn(uint32_t time) {
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

    /** \brief Draws a colored line between two points.
    *** \param x1 The x coordinate of the first point.
    *** \param y1 The y coordinate of the first point.
    *** \param width1 The width/thickness of the first point.
    *** \param x2 The x coordinate of the second point.
    *** \param y2 The y coordinate of the second point.
    *** \param width2 The width/thickness of the second point.
    *** \param color The color to draw the line in.
    **/
    void DrawLine(float x1, float y1, unsigned width1, float x2, float y2, unsigned width2, const Color &color);

    /** \brief Draws a grid of lines grid. Used by map editor to draw a grid over all the tiles.
     *
     * \param left The left coordinate of the grid.
     * \param top The top coordinate of the grid.
     * \param right The right coordinate of the grid.
     * \param bottom The bottom coordinate of the grid.
     * \param width_cell_horizontal The horizontal width of the grid's cells.
     * \param width_cell_vertical The vertical width of the grid's cells.
     * \param width_line The width of the grid's lines.
     * \param color The color of the grid.
     */
    void DrawGrid(float left, float top, float right, float bottom, float width_cell_horizontal, float width_cell_vertical, unsigned width_line, const Color& color);

    /** \brief Draws a solid rectangle of a given color.
    *** Draws a solid rectangle of a given color. For that, the lower-left corner
    *** of the rectangle has to be specified, and also its size. The parameters depends
    *** on the current Coordinate System.
    *** \param width Width of the rectangle.
    *** \param height Height of the rectangle.
    *** \param color Color to paint the rectangle.
    **/
    void DrawRectangle(float width, float height, const Color &color);

    /** \brief Draws an outline of a rectangle that is not filled in.
    *** \param left The x coordinate corresponding to the left side of the rectangle.
    *** \param right The x coordinate corresponding to the right side of the rectangle.
    *** \param bottom The y coordinate corresponding to the bottom side of the rectangle.
    *** \param top The y coordinate corresponding to the top side of the rectangle.
    *** \param width The width/thickness of the outline to draw.
    *** \param color The color to draw the outline in.
    **/
    void DrawRectangleOutline(float x1, float y1, float x2, float y2, unsigned width, const Color &color);

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

    /** Retrieves current set text style for rendering
     */
    TextStyle GetTextStyle();

    //! \brief toggles the FPS display
    void ToggleFPS() {
        _fps_display = !_fps_display;
    }

    void SetWindowHandle(SDL_Window* window)
    { _sdl_window = window; }

    SDL_Window* GetWindowHandle()
    { return _sdl_window; }
private:
    VideoEngine();

    //-- System fades. Only usable by the mode manager
    // and the MapTransition MapEvent.
    friend class vt_mode_manager::ModeEngine;
    friend class vt_map::private_map::MapTransitionEvent;
    void _StartTransitionFadeOut(const Color &final, uint32_t time) {
        _screen_fader.StartTransitionFadeOut(final, time);
    }

    void _TransitionalFadeIn(uint32_t time) {
        _screen_fader.TransitionalFadeIn(time);
    }

    //-- Private variables ----------------------------------------------------

    //! The SDL2 Window handle
    SDL_Window* _sdl_window;

    //! The secondary render target.
    gl::RenderTarget* _secondary_render_target;

    //! The FPS display flag.  If true, FPS is displayed.
    bool _fps_display;

    //! \brief A circular array of FPS samples used for calculating average FPS
    uint32_t _fps_samples[FPS_SAMPLES];

    /** \brief Keeps track of the sum of FPS values over the last VIDEO_FPS_SAMPLES frames
    *** This is used to simplify the calculation of average frames per second.
    **/
    uint32_t _fps_sum;

    //! \brief An index variable to keep track of the start of the circular fps_samples array.
    uint32_t _current_sample;

    /** \brief The number of FPS samples currently recorded.
    *** This value should always be VIDEO_FPS_SAMPLES, unless the game has just started, in which
    *** case it could be anywhere from 0 to VIDEO_FPS_SAMPLES depending on how many frames have
    *** been displayed.
    **/
    uint32_t _number_samples;

    //! The FPS text
    TextImage* _FPS_textimage;

    //! \brief Holds the most recently fetched OpenGL error code
    GLenum _gl_error_code;

    //! \brief These members are used to activate/deactivate the GL state only when needed.

    //! \brief Holds whether the GL_BLEND state is activated. Used to optimize the drawing logic
    bool _gl_blend_is_active;

    //! \brief Holds whether the GL_TEXTURE_2D state is activated. Used to optimize the drawing logic
    bool _gl_texture_2d_is_active;

    //! \brief Holds whether the GL_STENCIL_STATE state is activated. Used to optimize the drawing logic
    bool _gl_stencil_test_is_active;

    //! \brief Holds whether the GL_SCISSOR_TEST state is activated. Used to optimize the drawing logic
    bool _gl_scissor_test_is_active;

    //! \brief The x/y offsets, width and height of the current viewport (the drawn part), in pixels
    //! \note the viewport is different from the screen size when in non-4:3 modes.
    int32_t _viewport_x_offset;
    int32_t _viewport_y_offset;
    int32_t _viewport_width;
    int32_t _viewport_height;

    //! \brief The width and height of the current screen (window), in pixels
    int32_t _screen_width;
    int32_t _screen_height;

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
    int32_t _temp_width;

    //! holds the desired screen height. Not actually applied until ApplySettings() is called
    int32_t _temp_height;

    //! \brief Stores the current vsync mode.
    uint32_t _vsync_mode;

    //! \brief The game main loop update mode.
    //! \note update_mode true for performance, false for the CPU-gentle loop.
    //! It is always on performance when VSync is enabled.
    bool _game_update_mode;

    //! Image used for rendering rectangles
    StillImage _rectangle_image;

    //! The stack containing contexts, i.e. draw flags plus coord sys. Context is pushed and popped by any VideoEngine functions that clobber these settings
    std::stack<private_video::Context> _context_stack;

    //! The projection matrix.
    gl::Transform _projection;

    //! The stack containing transforms. Pushed and popped by PushMatrix/PopMatrix.
    std::stack<gl::Transform> _transform_stack;

    //! The OpenGL buffers and objects to draw a sprite.
    gl::Sprite* _sprite;

    //! The OpenGL buffers and objects to draw a particle system.
    gl::ParticleSystem* _particle_system;

    //! The OpenGL shaders.
    std::map<gl::shaders::Shaders, gl::Shader*> _shaders;

    //! The OpenGL shader programs.
    std::map<gl::shader_programs::ShaderPrograms, gl::ShaderProgram*> _programs;

    //! Check to see if the VideoManager has already been setup.
    bool _initialized;

    //-- Private methods ------------------------------------------------------

    /** \brief converts VIDEO_DRAW_LEFT or VIDEO_DRAW_RIGHT flags to a numerical offset
    * \param xalign the draw flag
    * \return the numerical offset
    */
    int32_t _ConvertXAlign(int32_t xalign);

    /** \brief converts VIDEO_DRAW_TOP or VIDEO_DRAW_BOTTOM flags to a numerical offset
    * \param yalign the draw flag
    * \return the numerical offset
    */
    int32_t _ConvertYAlign(int32_t yalign);

    //! \brief Updates the viewport metrics according to the current screen width/height.
    //! \note it also centers the viewport when the resolution isn't a 4:3 one.
    void _UpdateViewportMetrics();

    // Debug info
    //! \brief Updates the FPS counter.
    void _UpdateFPS();

    //! \brief Draws the current average FPS to the screen.
    void _DrawFPS();
};

} // namespace vt_video

#endif // __VIDEO_HEADER__
