///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    video.h
*** \author  Raj Sharma, roos@allacrost.org
***          Daniel Steuernol, steu@allacrost.org
*** \brief   Header file for video engine interface.
***
*** This code provides a comprehensive API for managing all drawing, rendering,
*** GUI, and other related graphical functions. This video engine is very large,
*** so the API user is advised to seek the documentation for it available on the
*** Allacrost wiki (http://wiki.allacrost.org).
***
*** In addition for its use in the game, the video engine is also actively used
*** by the Allacrost map editor GUI as a QT widget.
***
*** \note This code uses the OpenGL library for graphics rendering.
*** \note This code uses the libpng and libjpeg libraries for loading images.
*** \note This code uses the SDL_ttf 2.0 library for font rendering.
*** ***************************************************************************/

#ifndef __VIDEO_HEADER__
#define __VIDEO_HEADER__

// required for Code::Blocks and VS
#ifdef _WIN32
	#include <windows.h> // needs to be included before gl.h
#endif

// just required for VS
#ifdef _VS
	#include <GL/glew.h>
#endif

#ifdef __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#include <png.h>
extern "C" {
	#include <jpeglib.h>
}

#ifdef __APPLE__
	#include <SDL_ttf/SDL_ttf.h>
#else
	#include <SDL/SDL_ttf.h>
#endif

#include "defs.h"
#include "utils.h"

#include "context.h"
#include "color.h"
#include "coord_sys.h"
#include "fade.h"
#include "image.h"
#include "interpolator.h"
#include "shake.h"
#include "screen_rect.h"
#include "texture_controller.h"
#include "text.h"
#include "particle_manager.h"
#include "particle_effect.h"

//! \brief All calls to the video engine are wrapped in this namespace.
namespace hoa_video {

//! \brief The singleton pointer for the engine, responsible for all video operations.
extern VideoEngine* VideoManager;

//! \brief Determines whether the code in the hoa_video namespace should print
extern bool VIDEO_DEBUG;


//! animation frame period: how many milliseconds 1 frame of animation lasts for
const int32 VIDEO_ANIMATION_FRAME_PERIOD = 10;

namespace private_video {
//! \brief The number of FPS samples to retain across frames
const uint32 FPS_SAMPLES = 250;

//! \brief Maximum milliseconds that the current frame time and our averaged frame time must vary before we begin trying to catch up
const uint32 MAX_FTIME_DIFF = 5;

//! \brief The number of samples to take if we need to play catchup with the current FPS
const uint32 FPS_CATCHUP = 20;
}

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


//! \brief Specifies the target window environement where the video engine will run
enum VIDEO_TARGET {
	VIDEO_TARGET_INVALID = -1,

	//! Represents a SDL window
	VIDEO_TARGET_SDL_WINDOW = 0,

	//! Represents a QT widget
	VIDEO_TARGET_QT_WIDGET  = 1,

	VIDEO_TARGET_TOTAL = 2
};


//! \brief Specifies the stencil operation to use and describes how the stencil buffer is modified
enum VIDEO_STENCIL_OP {
	VIDEO_STENCIL_OP_INVALID = -1,

	//! Set the stencil value to one
	VIDEO_STENCIL_OP_ZERO = 0,

	//! Set the stencil value to zero
	VIDEO_STENCIL_OP_ONE = 1,

	//! Increase the stencil value
	VIDEO_STENCIL_OP_INCREASE = 2,

	//! Decrease the stencil value
	VIDEO_STENCIL_OP_DECREASE = 3,

	VIDEO_STENCIL_OP_TOTAL = 4
};

//! \brief The standard screen resolution for Allacrost
enum {
	VIDEO_STANDARD_RES_WIDTH  = 1024,
	VIDEO_STANDARD_RES_HEIGHT = 768
};


/** \brief Linearly interpolates a value which is (alpha * 100) percent between initial and final
*** \param alpha Determines where inbetween initial (0.0f) and final (1.0f) the interpolation should be
*** \param initial The initial value
*** \param final The final value
*** \return the linear interpolated value
**/
float Lerp(float alpha, float initial, float final);


/** \brief Rotates a point (x,y) around the origin (0,0), by angle radians
*** \param x x coordinate of point to rotate
*** \param y y coordinate of point to rotate
*** \param angle amount to rotate by (in radians)
**/
void RotatePoint(float &x, float &y, float angle);


/** ****************************************************************************
*** \brief Manages all the video operations and serves as the API to the video engine.
***
*** This is one of the largest classes in the Allacrost engine. Because it is so
*** large, the implementation of many of the methods for this class are split up
*** into multiple .cpp source files in the video code directory.
*** *****************************************************************************/
class VideoEngine : public hoa_utils::Singleton<VideoEngine> {
	friend class hoa_utils::Singleton<VideoEngine>;

	friend class TextureController;
	friend class TextSupervisor;
	friend class hoa_gui::GUISystem;

	friend class hoa_gui::TextBox;
	friend class hoa_gui::OptionBox;
	friend class hoa_gui::MenuWindow;

	friend class hoa_gui::private_gui::GUIElement;
	friend class private_video::TexSheet;
	friend class private_video::FixedTexSheet;
	friend class private_video::VariableTexSheet;
	friend class private_video::ParticleSystem;

	friend class ImageDescriptor;
	friend class StillImage;
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

	/** \brief Sets the target window environment where the video engine will be used
	*** \param target The window target, which can be VIDEO_TARGET_SDL_WINDOW or VIDEO_TARGET_QT_WIDGET
	*** \note The video engien's default target is a SDL window, so if that's what you desire then this
	*** function does not need to be called.
	*** \note You must set the target before calling the SingletonInitialize() function. Any invocations
	*** of the SetTarget function after SingletonInitialize has been called will result in no effect.
	**/
	void SetTarget(VIDEO_TARGET target);

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
	void Clear(const Color& background_color);

	/** \brief Swaps the frame buffers and displays the newly drawn contents onto the screen
	*** \param frame_time The number of milliseconds that have expired since the last frame was drawn.
	**/
	void Display(uint32 frame_time);

	/** \brief Retrieves the OpenGL error code and retains it in the _gl_error_code member
	*** \return True if an OpenGL error has been detected, false if no errors were detected
	*** \note This function only produces a meaningful result if the VIDEO_DEBUG variable is set to true. This is done
	*** because the call to glGetError() requires a round trip to the GPU and a flush of the rendering pipeline; a fairly
	*** expensive operation. If VIDEO_DEBUG is false, the function will always return false immediately.
	**/
	bool CheckGLError()
		{ if (VIDEO_DEBUG == false) return false; _gl_error_code = glGetError(); return (_gl_error_code != GL_NO_ERROR); }

	//! \brief Returns the value of the most recently fetched OpenGL error code
	GLenum GetGLError()
		{ return _gl_error_code; }

	///! \brief Returns a string representation of the most recently fetched OpenGL error code
	const std::string CreateGLErrorString();

	// ---------- Screen size and resolution methods

	//! \brief Returns the width of the screen, in pixels
	int32 GetScreenWidth() const
		{ return _screen_width; }

	//! \brief Returns the height of the screen, in pixels
	int32 GetScreenHeight() const
		{ return _screen_height; }

	bool IsInitialized() const
		{ return _initialized; }

	//! \brief Returns true if game is in fullscreen mode, false if it is in windowed mode
	bool IsFullscreen() const
		{ return _fullscreen; }

	/** \brief sets the current resolution to the given width and height
	*** \param width new screen width
	*** \param height new screen height
	***
	*** \note  you must call ApplySettings() to actually apply the change
	**/
	void SetResolution(uint32 width, uint32 height)
		{ _temp_width = width; _temp_height = height; }

	/** \brief sets the game to fullscreen or windowed depending on whether
	 *         true or false is passed
	 *  \param fullscreen set to true if you want fullscreen, false for windowed.
	 *  \note  you must call ApplySettings() to actually apply the change
	 */
	void SetFullscreen(bool fullscreen)
		{ _temp_fullscreen = fullscreen; }

	/** \brief Toggles fullscreen mode on or off
	 *  \note  you must call ApplySettings() to actually apply the change
	 */
	void ToggleFullscreen()
		{ SetFullscreen(!_temp_fullscreen); }

	//! \brief Returns a reference to the current coordinate system
	const CoordSys& GetCoordSys() const
		{ return _current_context.coordinate_system; }

	/** \brief Returns the pixel size expressed in coordinate system units
	*** \param x A reference of where to store the horizontal resolution
	*** \param x A reference of where to store the vertical resolution
	**/
	void GetPixelSize(float& x, float& y);

	/** \brief applies any changes to video settings like resolution and
	 *         fullscreen. If the changes fail, then this function returns
	 *         false, and the video settings are reset to whatever the last
	 *         working setting was.
	 * \return True if the video settings were successfully applied, false if they could not be applied
	 */
	bool ApplySettings();

	// ---------- Coordinate system and viewport methods

	/** \brief Sets the viewport (the area of the screen that gets drawn)
	*** \param left Left border of screen
	*** \param right Right border of screen
	*** \param top Top border of screen
	*** \param bottom Bottom border of screen
	***
	*** The arguments are percentages from 0.0f to 100.0f. The default viewport is
	*** (0, 100, 0, 100), which covers the entire screen.
	**/
	void SetViewport(float left, float right, float bottom, float top);

	/** \brief Sets the coordinate system to use.
	*** \param left The coordinate for the left border of screen
	*** \param right The coordinate for the right border of screen
	*** \param top The coordinate for the top border of screen
	*** \param bottom The coordinate for the bottom border of screen
	*** \note The default coordinate system for the video engine is
	*** (0.0f, 1024.0f, 0.0f, 768.0f)
	**/
	void SetCoordSys(float left, float right, float bottom, float top)
		{ SetCoordSys(CoordSys(left, right, bottom, top)); }

	/** \brief Sets the coordinate system to use.
	*** \param coordinate_system The coordinate system to set the screen to use
	**/
	void SetCoordSys(const CoordSys& coordinate_system);

	/** \brief Enables the scissoring effect in the video engine
	*** Scisorring is where you can specify a rectangle of the screen which is affected
	*** by rendering operations (and hence, specify what area is not affected). Make sure
	*** to disable scissoring as soon as you're done using the effect, or all subsequent
	*** draw calls will get messed up.
	**/
	void EnableScissoring();

	//! \brief Disables the scissoring effect
	void DisableScissoring();

	//! \brief Returns true if scissoring is enabled, or false if it is not
	bool IsScissoringEnabled()
		{ return _current_context.scissoring_enabled; }

	//! \brief Retrieves the current scissoring rectangle
	ScreenRect GetScissorRect()
		{ return _current_context.scissor_rectangle; }

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
	void SetScissorRect(const ScreenRect& rect);

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
	void GetDrawPosition(float &x, float &y) const
		{ x = _x_cursor; y = _y_cursor; }

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
	void PushMatrix()
		{ glPushMatrix(); }

	//! \brief Pops the modelview transformation from the stack
	void PopMatrix()
		{ glPopMatrix(); }

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
	void PopState ();

	/** \brief Rotates images counterclockwise by the specified number of radians
	*** \param angle How many radians to perform the rotation by
	*** \note You should understand how transformation matrices work in OpenGL
	*** prior to using this function.
	**/
	void Rotate(float angle)
		{ glRotatef(angle, 0, 0, 1); }

	/** \brief Scales all subsequent image drawing calls in the horizontal and vertical direction
	*** \param x The amount of horizontal scaling to perform (0.5 for half, 1.0 for normal, 2.0 for double, etc)
	*** \param y The amount of vertical scaling to perform (0.5 for half, 1.0 for normal, 2.0 for double, etc)
	*** \note You should understand how transformation matrices work in OpenGL
	*** prior to using this function.
	**/
	void Scale(float x, float y)
		{ glScalef(x, y, 1.0f); }

	/** \brief Sets the OpenGL transform to the contents of 4x4 matrix
	*** \param matrix A pointer to an array of 16 float values that form a 4x4 transformation matrix
	**/
	void SetTransform(float matrix[16]);

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
	StillImage CaptureScreen() throw(hoa_utils::Exception);

	/** \brief returns the amount of animation frames that have passed since the last
	 *         call to VideoEngine::Display(). This number is based on VIDEO_ANIMATION_FRAME_PERIOD,
	 *         and is used so that AnimatedImages know how many frames to increment themselves by.
	 * \return the number of nimations frames passed since last VideoEngine::Display() call
	 */
	int32 GetFrameChange()
		{ return _current_frame_diff; }

	/** \brief Returns a pointer to the GUIManager singleton object
	*** This method allows the user to perform text operations. For example, to load a
	*** font, the user may utilize this method like so:
	*** `if (VideoManager->Text()->LoadFont(...) == true) { cout << "Success" << endl; }`
	***
	*** \note See text.h for the public methods available from the TextSupervisor class
	*** \note This function is guaranteed to return a valid pointer so long as the VideoEngine class
	*** has been properly initialized
	**/
	TextSupervisor* Text()
		{ return TextManager; }

	/** \brief Returns a pointer to the TextureManager singleton object
	*** This method allows the user to perform texture management operations. For example, to reload
	*** all textures, the user may utilize this method like so:
	*** `if (VideoManager->Textures()->ReloadTextures() == true) { cout << "Success" << endl; }`
	***
	*** \note See texture_controller.h for the public methods available from the GUISystem class
	*** \note This function is guaranteed to return a valid pointer so long as the VideoEngine class
	*** has been properly initialized
	**/
	TextureController* Textures()
		{ return TextureManager; }

	//-- Lighting -----------------------------------------------------

	/** \brief turn on the ligt color for the scene
	 * \param color the light color to use
	 */
	void EnableSceneLighting(const Color& color);

	/** \brief disables scene lighting
	*/
	void DisableSceneLighting();

	/** \brief returns the scene lighting color
	 * \return the light color used in the scene
	 */
	const Color& GetSceneLightingColor()
		{ return _light_color; }

	/** \brief draws a halo at the given spot
	 *
	 *  \param id    image descriptor for the halo image
	 *  \param x     x coordinate of halo
	 *  \param y     y coordinate of halo
	 *  \param color color of halo
	 */
	void DrawHalo(const ImageDescriptor &id, float x, float y, const Color &color = Color(1.0f, 1.0f, 1.0f, 1.0f));

	/** \brief draws a real light at the given spot
	 *
	 *  \param radius light radius
	 *  \param x      x coordinate of light
	 *  \param y      y coordinate of light
	 *  \param color  color of light
	 */
	void DrawLight(float radius, float x, float y, const Color &color = Color(1.0f, 1.0f, 1.0f, 1.0f));

	/** \brief call after all map images are drawn to apply lighting. All
	 *         menu and text rendering should occur AFTER this call, so that
	 *         they are not affected by lighting.
	 */
	void ApplyLightingOverlay();

	//-- Overlays / lightning -------------------------------------------------------

	/** \brief draws a full screen overlay of the given color
	 *  \note  This is very slow, so use sparingly!
	 */
	void DrawFullscreenOverlay(const Color& color);

	/** \brief call to create lightning effect
	 *  \param lit_file a .lit file which contains lightning intensities stored
	 *                 as bytes (0-255).
	 * \return success/failure
	 */
	bool MakeLightning(const std::string& lit_file);

	/** \brief call this every frame to draw any lightning effects. You should make
	 *         sure to place this call in an appropriate spot. In particular, you should
	 *         draw the lightning before drawing the GUI. The lightning is drawn by
	 *         using a fullscreen overlay.
	 */
	void DrawLightning();

	//-- Fading ---------------------------------------------------------------

	/** \brief Begins a screen fade.
	*** \param color The color to fade the screen to
	*** \param time The fading process will take this number of milliseconds
	**/
	void FadeScreen(const Color& color, uint32 time)
		{ _screen_fader.BeginFade(color, time); }

	//! \brief Returns true if a screen fade is currently in progress
	bool IsFading()
		{ return _screen_fader.IsFading(); }

	//-- Screen shaking -------------------------------------------------------

	/** \brief Adds a new shaking effect to the screen
	***
	*** \param force The initial force of the shake
	*** \param falloff_time The number of milliseconds that the effect should last for. 0 indicates infinite time.
	*** \param falloff_method Specifies the method of falloff. The default is VIDEO_FALLOFF_NONE.
	*** \note If you want to manually control when the shaking stops, set the falloff_time to zero
	*** and the falloff_method to VIDEO_FALLOFF_NONE.
	**/
	void ShakeScreen(float force, uint32 falloff_time, ShakeFalloff falloff_method = VIDEO_FALLOFF_NONE);

	//! \brief Terminates all current screen shake effects
	void StopShaking()
		{ _shake_forces.clear(); _x_shake = 0.0f; _y_shake = 0.0f; }

	//! \brief Returns true if the screen is shaking
	bool IsShaking()
		{ return (_shake_forces.empty() == false); }

	// ----------  Particle effect methods

	/** \brief add a particle effect at the given point x and y
	 *  \param particle_effect_filename - file containing the particle effect definition
	 *  \param x - X coordinate of the effect
	 *  \param y - Y coordinate of the effect
	 *  \param reload - reload the effect from file if it already exists
	 *  \return id corresponding to the loaded particle effect
	 *  \note  set the reload parameter to true to reload the effect definition file
	 *         every time the effect is played. This is useful if you are working on an
	 *         effect and want to see how it looks. When we actually release the game,
	 *         reload should be false since it adds some cost to the loading
	 */
	ParticleEffectID AddParticleEffect(const std::string &particle_effect_filename, float x, float y, bool reload=false);

	/** \brief draws all active particle effects
	 * \return success/failure
	 */
	bool DrawParticleEffects();

	/** \brief stops all active particle effects
	 *  \param kill_immediate  If this is true, then the particle effects die out immediately
	 *                         If it is false, then they don't immediately die, but new particles
	 *                         stop spawning
	 */
	void StopAllParticleEffects(bool kill_immediate = false);

	/** \brief get pointer to an effect given its ID
	 * \return the particle effect with the given ID
	 */
	ParticleEffect *GetParticleEffect(ParticleEffectID id);

	/** \brief get number of live particles
	 * \return the number of live particles in the system
	 */
	int32 GetNumParticles();

	//-- Miscellaneous --------------------------------------------------------

	/** \brief Sets a new gamma value using SDL_SetGamma()
	 *
	 *  \param value        Gamma value of 1.0f is the default value
	 */
	void SetGamma(float value);

	/** \brief Returns the gamma value
	 * \return the gamma value
	 */
	float GetGamma() const
		{ return _gamma_value; }

	/** \brief Draws a colored line between two points
	*** \param x1 The x coordinate of the first point
	*** \param y1 The y coordinate of the first point
	*** \param x2 The x coordinate of the second point
	*** \param y2 The y coordinate of the second point
	*** \param width The width/thickness of the line to draw, in pixels
	*** \param color The color to draw the line in
	*** \todo Width argument should be an unsigned, non-zero integer
	**/
	void DrawLine(float x1, float y1, float x2, float y2, float width, const Color& color);

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
	void DrawRectangle(float width, float height, const Color& color);

	/** \brief Draws an outline of a rectangle that is not filled in
	*** \param left The x coordinate corresponding to the left side of the rectangle
	*** \param right The x coordinate corresponding to the right side of the rectangle
	*** \param bottom The y coordinate corresponding to the bottom side of the rectangle
	*** \param top The y coordinate corresponding to the top side of the rectangle
	*** \param width The width/thickness of the outline to draw, in pixels
	*** \param color The color to draw the outline in
	*** \todo Width argument should be an unsigned, non-zero integer
	**/
	void DrawRectangleOutline(float x1, float y1, float x2, float y2, float width, const Color& color);

	/** \brief Takes a screenshot and saves the image to a file
	*** \param filename The name of the file, if any, to save the screenshot as. Default is "screenshot.jpg"
	**/
	void MakeScreenshot(const std::string& filename = "screenshot.jpg");

	/** \brief toggles advanced information display for video engine, shows
	 *         things like number of texture switches per frame, etc.
	 */
	void ToggleAdvancedDisplay()
		{ _advanced_display = !_advanced_display; }

	/** \brief sets the default cursor to the image in the given filename
	* \param cursor_image_filename file containing the cursor image
	*/
	bool SetDefaultCursor(const std::string& cursor_image_filename);

	/** \brief returns the cursor image
	* \return the cursor image
	*/
	StillImage *GetDefaultCursor();

	/** Retrieves current set text style for rendering
	 */
	TextStyle GetTextStyle();

	/** \brief Updates the FPS counter and draws the current average FPS to the screen
	*** The number of milliseconds that have expired since the last frame was drawn
	**/
	void DrawFPS(uint32 frame_time);

	/** \brief toggles the FPS display (on by default)
	 */
	void ToggleFPS()
		{ _fps_display = !_fps_display; }
private:
	VideoEngine();

	//-- Private variables ----------------------------------------------------

	//! fps display flag. If true, FPS is displayed
	bool _fps_display;
	
	//! \brief A circular array of FPS samples used for calculating average FPS
	uint32 _fps_samples[private_video::FPS_SAMPLES];

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

	//! \brief Holds the most recently fetched OpenGL error code
	GLenum _gl_error_code;

	//! \brief The type of window target that the video manager will operate on (SDL window or QT widget)
	VIDEO_TARGET _target;

	//! \brief The width and height of the current screen, in pixels
	int32  _screen_width, _screen_height;

    //! \brief True if the game is currently running fullscreen
	bool _fullscreen;

	//! \brief The x and y coordinates of the current draw cursor position
	float _x_cursor, _y_cursor;

	//! \brief Contains information about the current video engine's context, such as draw flags, the coordinate system, etc.
	private_video::Context _current_context;

	//! \brief Manages the current screen fading effect when fading is activated
	private_video::ScreenFader _screen_fader;

	//! eight character name for temp files that increments every time you create a new one so they are always unique
	char _next_temp_file[9];

	//! advanced display flag. If true, info about the video engine is shown on screen
	bool _advanced_display;

	//! keep track of number of draw calls per frame
	int32 _num_draw_calls;

	//! true if real lights are enabled
	bool _uses_lights;

	//! lighting overlay texture
	GLuint _light_overlay;

	//! lighting overlay FBO
	GLuint _light_overlay_fbo;

	//! X offset to shake the screen by (if any)
	float  _x_shake;

	//! Y offset to shake the screen by (if any)
	float _y_shake;

	//! Current gamma value
	float _gamma_value;

	//! current shake forces affecting screen
	std::list<private_video::ShakeForce> _shake_forces;

	//! particle manager, does dirty work of managing particle effects
	private_video::ParticleManager _particle_manager;

	// changing the video settings does not actually do anything until
	// you call ApplySettings(). Up til that point, store them in temp
	// variables so if the new settings are invalid, we can roll back.

	//! holds the desired fullscreen status (true=fullscreen, false=windowed). Not actually applied until ApplySettings() is called
	bool _temp_fullscreen;

	//! holds the desired screen width. Not actually applied until ApplySettings() is called
	int32 _temp_width;

	//! holds the desired screen height. Not actually applied until ApplySettings() is called
	int32 _temp_height;

	//! image which is to be used as the cursor
	StillImage _default_menu_cursor;

	//! Image used for rendering rectangles
	StillImage _rectangle_image;

	//! current scene lighting color (essentially just modulates vertex colors of all the images)
	Color _light_color;

	//! true if a lightning effect is active
	bool _lightning_active;

	//! current time of lightning effect (time since it started)
	int32 _lightning_current_time;

	//! how many milliseconds to do the lightning effect for
	int32 _lightning_end_time;

	//! intensity data for lightning effect
	std::vector <float> _lightning_data;

	//! counter to keep track of milliseconds since game started for animations
	int32 _animation_counter;

	//! keeps track of the number of frames animations should increment by for the current frame
	int32 _current_frame_diff;

	//! STL map containing all loaded particle effect definitions
	std::map<std::string, ParticleEffectDef*> _particle_effect_defs;

	//! stack containing context, i.e. draw flags plus coord sys. Context is pushed and popped by any VideoEngine functions that clobber these settings
	std::stack<private_video::Context> _context_stack;

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

	/** \brief returns a filename like TEMP_abcd1234.ext, and each time you call it, it increments the
	 *         alphanumeric part of the filename. This way, during any particular run
	 *         of the game, each temp filename is guaranteed to be unique.
	 *         Assuming you create a new temp file every second, it would take 100,000 years to get
	 *         from TEMP_00000000 to TEMP_zzzzzzzz
	 *
	 *  \param extension   The extension for the temp file. Although we could just save temp files
	 *                     without an extension, that might cause stupid bugs like DevIL refusing
	 *                     to load an image because it doesn't end with .png.
	 * \return name of the generated temp file
	 */
	std::string _CreateTempFilename(const std::string &extension);

	/** \brief Rounds a force value to the nearest integer based on probability.
	*** \param force  The force to round
	*** \return the rounded force value
	*** \note For example, a force value of 2.85 has an 85% chance of rounding to 3 and a 15% chance of rounding to 2. This rounding
	*** methodology is necessary because for force values less than 1 (e.g. 0.5f), the shake force would always round down to zero
	*** even though there is positive force.
	**/
	float _RoundForce(float force);

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

	/** \brief Updates all active shaking effects
	*** \param frame_time The number of milliseconds that have elapsed for the current rendering frame
	**/
	void _UpdateShake(uint32 frame_time);

	//! \brief Returns true if textures should be smoothed (used for non natural screen resolutions)
	bool _ShouldSmooth()
		{ return ( _screen_width != VIDEO_STANDARD_RES_WIDTH || _screen_height != VIDEO_STANDARD_RES_HEIGHT); }

	/** \brief Shows graphical statistics useful for performance tweaking
	*** This includes, for instance, the number of texture switches made during a frame.
	**/
	void _DEBUG_ShowAdvancedStats();
}; // class VideoEngine : public hoa_utils::Singleton<VideoEngine>

}  // namespace hoa_video

#endif // __VIDEO_HEADER__
