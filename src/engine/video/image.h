///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    image.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for image classes
***
*** This file contains several classes that represent images loaded into the
*** engine. The public classes used outside of the video engine are:
***
*** - <b>ImageDescriptor</b> is an abstract base class for other images. This
*** type is used to allow containers to create multiple derived image classes.
*** It also contains a number of static functions for general image operation
*** code.
***
*** - <b>StillImage</b> is a single non-animated image and is what the user
*** will utilize most of the time.
***
*** - <b>AnimatedImage</b> is an animated image that contains multiple frames,
*** where a frame is a StillImage object along with timing information.
***
*** - <b>AnimationFrame</b> is a single frame of animation. It consists of a
*** StillImage, and how long the frame should be displayed in the animation.
*** This class is contained in the private_video namespace and is not a part
*** of the image manipulation API.
***
*** - <b>CompositeImage</b> is an image formed by piecing together multiple
*** StillImage objects. The image elements in a composite image may or may
*** not overlap one another
***
*** \note A <b>multi image</b> is nothing more than a single image file which
*** contains multiple adjacent sub-images within it. For example, a tileset file
*** contains numerous tile images. All sub-images are required to have the same
*** dimensions in a multi image file. Multi images are never explicitly contained
*** in any way by the video engine -- rather, these multi images are loaded into
*** memory and then the sub-images are split up into separate StillImage objects.
*** ***************************************************************************/

#ifndef __IMAGE_HEADER__
#define __IMAGE_HEADER__

#include "defs.h"
#include "utils.h"

#include "image_base.h"
#include "color.h"
#include "texture.h"

namespace hoa_video {

/** ****************************************************************************
*** \brief An abstract base class for all public API image classes
***
*** This class also contains several static functions for performing image
*** operations which do not operate on a single instance of a derived
*** ImageDescriptor object. For example, functions which load a single image
*** file into multiple StillImage objects.
***
*** \note A <b>multi image</b> is defined as a single image file which contains
*** several images in a grid format that we wish to extract as seperate image
*** entities. This is used, for example, to hold multiple tile images in a large
*** tileset file, or multiple animation frames stored in a single image file.
*** All individual image elements in a multi image are required to be the same
*** size.
***
*** \note The copy constructor and copy assignement operator ensure that the
*** reference to the texture being used is updated appropriately. The
*** derived classes should not require the implementation of a copy constructor
*** or assignment operator, at least not for the purposes of proper reference
*** counting.
***
*** \note If the destructor finds that the image texture has no more references
*** after it decrements the reference counter, it will mark it as free on the
*** image texture's texture sheet. However it will <b>not</b> attempt to remove
*** it from any container in the TextureManager class, so that is up to the
*** destructors of the derived classes to implement.
*** ***************************************************************************/
class ImageDescriptor {
	friend class VideoEngine;

public:
	ImageDescriptor();

	virtual ~ImageDescriptor();

	ImageDescriptor(const ImageDescriptor& copy);

	ImageDescriptor& operator=(const ImageDescriptor& copy);

	//! \brief Clears all data retained by the object (color, width, height, etc.)
	virtual void Clear() = 0;

	/** \brief Draws the image to the display buffer
	*** The location and orientation of the drawn image is dependent upon the current cursor position
	*** and context (draw flags) set in the VideoEngine class.
	**/
	virtual void Draw() const = 0;

	/** \brief Draws a color modulated version of the image to the display buffer
	*** \param draw_color The color to modulate the image by
	**/
	virtual void Draw(const Color& draw_color) const = 0;

	//! \name Class Member Access Functions
	//@{
	//! \brief Returns the image width
	virtual float GetWidth() const
		{ return _width; }

	//! \brief Returns image height
	virtual float GetHeight() const
		{ return _height; }

	//! \brief Returns true if the image is grayscale.
	bool IsGrayScale() const
		{ return _grayscale; }

	virtual void EnableGrayScale() = 0;

	virtual void DisableGrayScale() = 0;

	/** \brief Enables or disables the image's static property
	*** \param is_static If true, the image will be made static
	**/
	virtual void SetStatic(bool is_static) = 0;

	/** \brief Sets the image's width, expressed as coordinate system units
	*** \param width The desired width of the image
	**/
	virtual void SetWidth(float width) = 0;

	/** \brief Sets the image's height, expressed as coordinate system units
	*** \param height The desired height of the image
	**/
	virtual void SetHeight(float height) = 0;

	/** \brief Sets the image's dimensions, expressed as coordinate system units
	*** \param width The desired width of the image
	*** \param height The desired height of the image
	**/
	virtual void SetDimensions(float width, float height) = 0;

	/** \brief Sets the UV coordinates for the image
	*** \param u1 First u coordinate
	*** \param v1 First v coordinate
	*** \param u2 Second u coordinate
	*** \param v2 Second v coordinate
	***
	*** This method rarely needs to be called, as the default UV coordinates suffice
	*** for nearly all images.
	**/
	virtual void SetUVCoordinates(float u1, float v1, float u2, float v2)
		{ _u1 = u1; _v1 = v1; _u2 = u2; _v2 = v2; }

	/** \brief Sets the image's four vertices to a single color
	*** \param color The desired color of all image vertices
	**/
	virtual void SetColor(const Color& color);

	/** \brief Sets the image's vertex colors
	*** \param tl The top left vertex color
	*** \param tr The top right vertex color
	*** \param bl The bottom left vertex color
	*** \param br The bottom right vertex color
	**/
	virtual void SetVertexColors(const Color& tl, const Color& tr, const Color& bl, const Color& br);
	//@}

	/** \name Static Image Manipulation Functions
	*** This series of static functions provide additional image mainpulation that does not operate
	*** on a single instance of a derived image type.
	**/
	//@{
	/** \brief Retrieves various properties about an image file
	*** \param filename The name of the image file (.png or .jpg) to retrieve the properties of
	*** \param rows The number of rows of pixels in the image
	*** \param cols The number of columns of pixels in the image
	*** \param bpp The number of bits per pixel of the image
	*** \throw Exception If any of the properties are not retrieved successfully
	**/
	static void GetImageInfo(const std::string& filename, uint32& rows, uint32& cols, uint32& bpp) throw(hoa_utils::Exception);

	/** \brief Loads a multi image into a vector of StillImage objects
	*** \param images Reference to the vector of StillImages to be loaded with elements from the multi image
	*** \param filename The name of the multi image file to load the image data from
	*** \param elem_width The width of each sub-image element, in pixels
	*** \param elem_height The  height of each sub-image element, in pixels
	*** \return True upon successful loading, false if there was an error
	***
	*** This function determines the image elements to extract from the multi image by the width and height
	*** of each element (in pixels) specified in the function arguments. Upon success, the size of the images
	*** reference vector will always be equal to the area of the multi image divided by the area of each
	*** element image.
	*** \note All image elements within the multi image should be of the same size
	 */
	static bool LoadMultiImageFromElementSize(std::vector<StillImage>& images, const std::string& filename,
		const uint32 elem_width, const uint32 elem_height);

	/** \brief Loads a multi image into a vector of StillImage objects
	*** \param images Reference to the vector of StillImages to be loaded with elements from the multi image
	*** \param filename The name of the multi image file to load the image data from
	*** \param grid_rows The number of rows of image elements contained in the multi image
	*** \param grid_cols The number of columns of image elements contained in the multi image
	*** \return True upon successful loading, false if there was an error
	***
	*** This function determines the image elements to extract from dividing the multi image into a number
	*** of rows and columns, as given through the function's arguments. Upon success, the size of the images
	*** reference vector will always be equal to grid_rows * grid_cols.
	*** \note All image elements within the multi image should be of the same size
	**/
	static bool LoadMultiImageFromElementGrid(std::vector<StillImage>& images, const std::string& filename,
		const uint32 grid_rows, const uint32 grid_cols);

	/** \brief Saves a vector of images into a single image file (a multi image)
	*** \param images A reference to the vector of StillImage pointers to save into a multi image
	*** \param filename The name of the multi image file to write (.png of .jpg extension required)
	*** \param grid_rows The number of rows of sub-images in the MultiImage.
	*** \param grid_cols Number of columns of sub-images in the MultiImage.
	*** \return True upon successful loading, false if there was an error
	*** \note All images within the images vector should be of the same size
	**/
	static bool SaveMultiImage(const std::vector<StillImage*>& images, const std::string& filename,
		const uint32 grid_rows, const uint32 grid_cols);
	//@}

	//! \brief A debug function which prints the image's information to the screen
	void DEBUG_PrintInfo();

protected:
	/** \brief A pointer to the texture used by the image
	*** The purpose of this member is for the ImageDescriptor class to be able to manage
	**/
	private_video::BaseTexture* _texture;

	//! \brief The width and height of the image, in coordinate system units.
	float _width, _height;

	/** \brief The texture coordinates for the image
	*** (u1, v1) represents the upper-left corner while (u2, v2) represents the bottom-right corner. These coordinates
	*** are typically (0.0f, 0.0f), (1.0f, 1.0f) and do not need to be modified except in special cases.
	**/
	float _u1, _v1, _u2, _v2;

    //! \brief Holds the color of the upper left, upper right, lower left, and lower right vertices, respectively.
	Color _color[4];

	//! \brief True indicates to perform blending with this image
	bool _blend;

	//! \brief Set to true if all vertices are the same color
	bool _unichrome_vertices;

	//! \brief Indicates whether the image being loaded should be loaded into a non-volatile area of texture memory.
	bool  _is_static;

	//! \brief True if this image is grayscale.
	bool _grayscale;

	/** \brief Removes a reference to _texture, and frees or deletes it if it has no remaining references
	***
	*** This method will set _texture to NULL before returning. If your derived class has a duplicate texture
	*** pointer (ie, ImageTexture pointer for StillImage class), you should make sure to set that member to
	*** NULL as well.
	**/
	void _RemoveTextureReference();

	/** \brief A draw helper function which adjusts the draw orientation (translation and scaling)
	***
	*** \note This method modifies the draw cursor position and does not restore it before finishing. Therefore
	*** under most circumstances, you will want to call VideoManager->PushState()/PopState(), or
	*** glPushMatrix()/glPopMatrix() before and after calling this function. The latter is preferred due to the
	*** lower cost of the call, but some circumstances may require using the former when more state information
	*** needs to be retained.
	**/
	void _DrawOrientation() const;

	/** \brief Draws the OpenGL texture referred to by the object on the screen
	*** \param draw_color A non-NULL pointer to an array of four valid Color objects
	***
	*** This method is typically a helper method to other draw calls in some way. It assumes that
	*** all of the appropriate transformation, scaling, and other image property opertaions have been
	*** completed prior to the calling of this function. The draw_color argument is usually nothing
	*** more than a pointer to the _color member of this very class, but in certain cases like during
	*** a screen fade these colors may differ.
	**/
	void _DrawTexture(const Color* draw_color) const;

private:
	/** \brief Retrieves various properties about a PNG image file
	*** \param filename The name of the PNG image file to retrieve the properties of
	*** \param rows The number of rows of pixels in the image
	*** \param cols The number of columns of pixels in the image
	*** \param bpp The number of bits per pixel of the image
	*** \throw Exception If any of the properties are not retrieved successfully
	**/
	static void _GetPngImageInfo(const std::string& filename, uint32& rows, uint32& cols, uint32& bpp) throw(hoa_utils::Exception);

	/** \brief Retrieves various properties about a JPG image file
	*** \param filename The name of the JPG image file to retrieve the properties of
	*** \param rows The number of rows of pixels in the image
	*** \param cols The number of columns of pixels in the image
	*** \param bpp The number of bits per pixel of the image
	*** \throw Exception If any of the properties are not retrieved successfully
	**/
	static void _GetJpgImageInfo(const std::string& filename, uint32& rows, uint32& cols, uint32& bpp) throw(hoa_utils::Exception);

	/** \brief A helper function to the public LoadMultiImage* calls
	*** \param images Reference to the vector of StillImages to be loaded
	*** \param filename The name of the multi image file to read
	*** \param grid_rows The number of rows of image elements in the multi image
	*** \param grid_cols The number of columns of image elements in the multi image
	*** \return True if the image file was loaded and parsed successfully, false if there was an error.
	**/
	static bool _LoadMultiImage(std::vector<StillImage>& images, const std::string& filename,
		const uint32 grid_rows, const uint32 grid_cols);
}; // class ImageDescriptor


/** ****************************************************************************
*** \brief Represents a simple still image
***
*** This is the most frequently used image construct of the video engine. Objects
*** of this class are used in the construction of some of the more advanced image
*** classes.
*** ***************************************************************************/
class StillImage : public ImageDescriptor {
	friend class VideoEngine;
	friend class ImageDescriptor;
	friend class AnimatedImage;
	friend class CompositeImage;
	friend class TextureController;
	friend class private_video::ParticleSystem;

public:
	//! \brief Supply the constructor with "true" if you want this to represent a grayscale image
	StillImage(const bool grayscale = false);

	~StillImage();

	//! \brief Resets the image's properties and removes any references to image data that it maintains
	void Clear();

	/** \brief Loads a single image file to be represented by the class object
	*** \param filename The filename of the image to load (should have a .png or .jpg extension)
	*** \return True if the image was successfully loaded and is now represented by this object
	***
	*** \note Invoking this function will clear all image elements currently used by this class.
	*** \note Passing in an emptry string for filename is a special case, and will construct
	*** a colored quad for the image procedurally. It is not an error to pass an empty string to
	*** the function.
	**/
	bool Load(const std::string& filename);

	bool Load(const std::string& filename, float width, float height)
		{ SetDimensions(width, height); return Load(filename); }

	//! \brief Draws the image to the screen
	void Draw() const;

	/** \brief Draws a color-modulated version of the image
	*** \param draw_color The color to modulate the image by
	**/
	void Draw(const Color& draw_color) const;

	/** \brief Saves the image to a file
	*** \param filename The filename of the image to save (should have a .png or .jpg extension)
	*** \return True if the image was successfully saved to a file
	***
	*** \note The image being saved should contain only one image element. Support for saving of
	*** composite (multi-element) images is not yet supported
	**/
	bool Save(const std::string& filename) const;

	//! \brief Enables grayscaling for the image then reloads it
	void EnableGrayScale();

	//! \brief Disables grayscaling for the image then reloads it
	void DisableGrayScale();

	//! \name Class Member Access Functions
	//@{
	//! \brief Returns the filename string for the image
	const std::string& GetFilename() const
		{ return _filename; }

	/** \brief Returns the color of a particular vertex
	*** \param c The Color object to place the color in.
	*** \param index The vertex index of the color to fetch
	*** \note If an invalid index value is used, the function will return with no warning.
	**/
	void GetVertexColor(Color& c, uint8 index)
		{ if (index > 3) return; else c = _color[index]; }

	/** \brief Sets width of the image
	*** \param width Width of the image
	**/
	void SetWidth(float width)
		{ _width = width; }

	/** \brief Sets height of the image
	*** \param height Height of the image
	**/
	void SetHeight(float height)
		{ _height = height; }

	/** \brief Sets the dimensions of the image for a desired coordinate system
	*** \param width The width of the image
	*** \param height The height of the image
	**/
	void SetDimensions(float width, float height)
		{ SetWidth(width); SetHeight(height); }

	/** \brief Sets image to static/animated
	***	\param is_static Flag indicating whether the image should be made static or not
	**/
	void SetStatic(bool is_static)
		{ _is_static = is_static; }
	//@}

protected:
	/** \brief The name of the image file from which this image was created
	*** This member is only valid for StillImage objects which had their Load() function
	*** invoked successfully and have no additional elements. This member will be set to
	*** the empty string otherwise.
	**/
	std::string _filename;

	//! \brief The texture image that is referenced by this element
	private_video::ImageTexture* _image_texture;
}; // class StillImage : public ImageDescriptor



namespace private_video {

/** ****************************************************************************
*** \brief Represents a single frame in an animation
*** ***************************************************************************/
class AnimationFrame {
public:
	//! \brief The amount of time to display this frame, in milliseconds
	uint32 frame_time;

	//! \brief The StillImage used for this frame in the animation
	StillImage image;
}; // class AnimationFrame


/** ****************************************************************************
*** \brief Represents a single element in a composite image
*** ***************************************************************************/
class ImageElement {
public:
	ImageElement() :
		x_offset(0.0f), y_offset(0.0f) {}

	ImageElement(const StillImage& img, float x, float y) :
		image(img), x_offset(x), y_offset(y) {}

	//! \brief The singular image that represents this element
	StillImage image;

	//! \brief X and y draw position offsets of this element
	float x_offset, y_offset;
}; // class ImageElement

} // namespace private_video


/** ****************************************************************************
*** \brief Represents an animated image with both frames and timing information
***
*** Animated images are really nothing more than a series of individual still
*** images and timing information for each frame. This class assumes that
*** all frame images are the same size, so you should not attempt to use
*** this class if your frames are of different sizes. If you wish to use different
*** sized frame images in an animation, you'll need to implement the code
*** to do so yourself.
*** ***************************************************************************/
class AnimatedImage : public ImageDescriptor {
	friend class VideoEngine;

public:
	//! \brief Supply the constructor with "true" if you want this to represent a grayscale image.
	AnimatedImage(bool grayscale = false);

	//! \brief A constructor which also sets the image's dimensions
	AnimatedImage(float width, float height, bool grayscale = false);

	//! \brief Resets the image's properties and removes any references to image data that it maintains
	void Clear();

	/** \brief Loads an AnimatedImage by opening a multi image file
	*** \param filename The name of the file to load, which should end in a .png or .jpg extension
	*** \param timings A vector reference which holds the timing information for each animation frame
	*** \param frame_width The width (in pixels) of each frame in the multi image file
	*** \param frame_height The height (in pixels) of each frame in the multi image file
	*** \param trim The number of frame images to "ignore" from the multi image (default == 0)
	*** \return True if the animation was successfully constructed from the loaded multi image
	***
	*** The trim factor must be less than the total number of frames that are stored in the multi image.
	*** The size of the timings vector must be at least (# of frames in multi image - trim). It may
	*** be larger than this, but the rest of the elements beyond the minimum size will be ignored.
	**/
	bool LoadFromFrameSize(const std::string& filename, const std::vector<uint32>& timings, const uint32 frame_width, const uint32 frame_height, const uint32 trim = 0);

	/** \brief Loads an AnimatedImage from a multi image file
	*** \param filename The name of the file to load, which should end in a .png or .jpg extension
	*** \param timings A vector reference which holds the timing information for each animation frame
	*** \param frame_rows The number of rows of frame images in the image file
	*** \param frame_cols The number of columns of frame images in the image file
	*** \param trim The number of frame images to "ignore" from the multi image (default == 0)
	*** \return True if the animation was successfully constructed from the loaded multi image
	***
	*** The trim factor is useful for indicating if any of the final frames in a multi image
	*** contain no relevant image data that we are interested in. For example, if we have a
	*** multi image with 2 rows and 4 columns of frames, but only the first 6 frames (the entire
	*** top row, and the left-most two frames in the bottom row) are valid, we would set the trim
	*** factor to two. Obviously, trim must be less than frame_rows * frame_cols, otherwise we
	*** can't load even a single frame.
	***
	*** The timings vector must have a minimum size of (frame_rows * frame_cols - trim) so that each
	*** frame we will add has a timing value associated with it. The timings vector may be larger
	*** than this minimum size, but only the first (frame_rows * frame_cols - trim) elements will
	*** be used, and the rest of the vector ignored.
	**/
	bool LoadFromFrameGrid(const std::string& filename, const std::vector<uint32>& timings, const uint32 frame_rows, const uint32 frame_cols, const uint32 trim = 0);

	//! \brief Draws the current frame image to the screen
	void Draw() const;

	/** \brief Draws the current frame image which is modulated by a color
	*** \param draw_color The color to modulate the image by
	**/
	void Draw(const Color& draw_color) const;

	/** \brief Saves all frame images into a single file (a multi image file)
	*** \param filename The filename of the image to save (should have a .png or .jpg extension)
	*** \param grid_rows The number of grid rows to save in the multi image
	*** \param grid_cols The number of grid columns to save in the multi image
	*** \return True if all frames were successfully saved to a file
	***
	*** This function
	***
	*** \note No frame images should contain more than one image element. Support for saving of
	*** composite (multi-element) images is not yet supported.
	**/
	bool Save(const std::string& filename, const uint32 grid_rows = 0, const uint32 grid_cols = 0) const;

	//! \brief Enables grayscale for all image frames
	void EnableGrayScale();

	//! \brief Disables grayscale for all image frames
	void DisableGrayScale();

	//! \brief Resets the animation's frame, counter, and looping.
	void ResetAnimation()
		{ _frame_index = 0; _frame_counter = 0; _loop_counter = 0; _loops_finished = false; }

	/** \brief Called every frame to update the animation's current frame
	*** This will automatically synchronize the animation to VIDEO_ANIMATION_FRAME_PERIOD,
	*** i.e. 30 frames per second. If you want to update the frames yourself using some custom
	*** algorithm, then use the SetFrame() method instead of calling this function
	***
	*** \note This method will do nothing if there are no frames contained in the animation,
	*** or if the _loops_finished member is set to true.
	**/
	void Update();

	/** \brief Adds an animation frame using the filename of the image to add.
	*** \param frame The filename of the frame image to add.
	*** \param frame_time The number of milliseconds that this animation should last for
	*** \return True on success, false on failure.
	***
	*** This is perhaps a more convenient way to add frames, <b>but</b> this makes it impossible
	*** to control the image properties such as vertex colors, and size. If you use this function,
	*** the width and height will be the pixel width/height of the image itself. This is not what
	*** you always will want. For example, if your coordinate system is in terms of 32x32 pixel
	*** tiles, then a tile image would have a width and height of 1, not 32.
	**/
	bool AddFrame(const std::string& frame, uint32 frame_time);

	/** \brief Adds an animation frame by using an existing static image.
	*** \param frame The still image to use as the frame image.
	*** \param frame_time The amount of millseconds to display the frame.
	*** \return True on success, false on failure.
	***
	*** The frame argument should have at least one element prepared. Passing a StillImage
	*** that does not contain any image data will result in failure for this call.
	**/
	bool AddFrame(const StillImage& frame, uint32 frame_time);

	//! \name Class Member Access Functions
	//@{
	//! \brief Returns the number of frames in this animation
	uint32 GetNumFrames() const
		{ return _frames.size(); }

	//! \brief Retuns a pointer to the StillImage representing the current frame
	StillImage* GetCurrentFrame() const
		{ return const_cast<StillImage*>(&(_frames[_frame_index].image)); }

	//! \brief Returns the index number of the current frame in the animation.
	uint32 GetCurrentFrameIndex() const
		{ return _frame_index; }

	/** \brief Returns a pointer to the StillImage at a specified frame.
	*** \param index index of the frame you want
	*** \return A pointer to the image at that index, or NULL if the index parameter was invalid
	***
	*** Using this function is dangerous since it provides direct access to an image frame.
	*** If you find yourself in constant need of using this function, think twice about
	*** what you are doing.
	**/
	StillImage* GetFrame(uint32 index) const
		{ if (index >= _frames.size()) return NULL; else return const_cast<StillImage*>(&(_frames[index].image)); }

	//! \brief Returns the number of milliseconds that the current frame has been shown for.
	uint32 GetTimeProgress() const
		{ return _frame_counter; }

	/** \brief Returns the percentage of timing complete for the current frame being shown.
	*** \return A float from 0.0f to 1.0f, indicate how much of its allotted time this frame has spent
	*** \note The divide by 0.0f case is not checked for here, so this function could potentially throw
	*** a divide by zero exception at run-time.
	**/
	float GetPercentProgress() const
		{ return static_cast<float>(_frame_counter) / _frames[_frame_index].frame_time; }

	//! \brief Returns true if the loops have finished, false otherwise
	bool IsLoopsFinished() const
		{ return _loops_finished; }

	/** \brief Sets all animation frames to be a certain width
	*** \param width Width to set each frame (in coordinate system units)
	**/
	void SetWidth(float width);

	/** \brief Sets all animation frames to be a certain height
	*** \param height Height to set each frame (in coordinate system units)
	**/
	void SetHeight(float height);

	/** \brief Sets all animation frames to be a certain width and height
	*** \param width Width to set each frame (in coordinate system units)
	*** \param height Height to set each frame (in coordinate system units)
	**/
	void SetDimensions(float width, float height);

	void SetUVCoordinates(float u1, float v1, float u2, float v2)
		{}

	/** \brief Sets the static member for all animation frame images
	*** \param is_static Flag indicating whether the image will be static or not.
	*** \note If the frames are already loaded, it doesn't bother to try to unload them
	*** and then reload them again statically.
	**/
	void SetStatic(bool is_static)
		{ _is_static = is_static; }

	/** \brief sets All frames to be of a certain color (all vertices are set to the same color)
	*** \param color Color of the 4 vertices
	**/
	void SetColor(const Color &color);

	/** \brief sets all frames to have the specified vertex colors
	*** \param tl The top left vertex color
	*** \param tr The top right vertex color
	*** \param bl The bottom left vertex color
	*** \param br The bottom right vertex color
	**/
	void SetVertexColors(const Color &tl, const Color &tr, const Color &bl, const Color &br);

	/** \brief Sets the current frame index of the animation.
	*** \param index The index of the frame to access
	*** \note Passing in an invalid value for the index will not change the current frame
	**/
	void SetFrameIndex(const uint32 index)
		{ if (index > _frames.size()) return; _frame_index = index; _frame_counter = 0; }

	/** \brief Sets the number of milliseconds that the current frame has been shown for.
	*** \param time The time to set the frame counter
	*** \note This does not set the frame timer for the current frame
	**/
	void SetTimeProgress(uint32 time)
		{ _frame_counter = time; }

	/** \brief Set the number of loops for the animation.
	*** A value less than zero indicates to loop forever. Zero indicates do not loop: just run the
	*** animation from beginning to end and stop.
	***	\param loops Number of loops for the animation
	**/
	void SetNumberLoops(int32 loops)
		{ _number_loops = loops; if (_loop_counter >= _number_loops && _number_loops >= 0) _loops_finished = true; }

	/** \brief Set the current number of loops that the animation has completed.
	*** \param loops The urrent loop count
	**/
	void SetLoopCounter(int32 loops)
		{ _loop_counter = loops; if (_loop_counter >= _number_loops && _number_loops >= 0) _loops_finished = true; }

	/** \brief Effectively stops the animation in its track if this member is set to true.
	*** \param loops True to stop the looping process. Setting it to false will restart the loop counter
	**/
	void SetLoopsFinished(bool loops)
		{ _loops_finished = loops; if (loops == false) _loop_counter = 0; }
	//@}

private:
	//! \brief The index of which animation frame to display.
	uint32 _frame_index;

    //! \brief Counts how long each frame has been shown for.
	uint32 _frame_counter;

	/** \brief The number of times to loop the animation frames.
	*** A negative value indicates to loop forever, which is the default.
	**/
	int32 _number_loops;

	//! \brief Counts the number of loops remaining for the animation.
	int32 _loop_counter;

	/** \brief Set to true when the loop counter has expired.
	*** This member will remain eternally false if the looping is set to infinite mode.
	**/
	bool _loops_finished;

	//! \brief The vector of animation frames (contains both images and timing)
	std::vector<private_video::AnimationFrame> _frames;
}; // class AnimatedImage : public ImageDescriptor


/** ****************************************************************************
*** \brief Represents a composite image created from multiple image elements
***
*** A composite image is created by taking multiple StillImage objects and
*** "stitching" them together to represent a singular image object. Each image
*** element has x and y offsets that determine where the image is located in
*** the composite image. A good example usage of this class can be found in the
*** MenuWindow class, where a window is represented as a composite image and
*** created by attaching multiple border images together to create the window.
***
*** \note Because this class references other StillImage objects, it's _texture
*** member is always NULL, since the class itself does not make use of any
*** textures.
*** ***************************************************************************/
class CompositeImage : public ImageDescriptor {
public:
	CompositeImage()
		{}

	~CompositeImage()
		{}

	//! \brief Removes all image elements held by this class
	void Clear();

	/** \brief Draws the image to the display buffer
	*** The location and orientation of the drawn image is dependent upon the current cursor position
	*** and context (draw flags) set in the VideoEngine class.
	**/
	void Draw() const;

	/** \brief Draws a color modulated version of the image to the display buffer
	*** \param draw_color The color to modulate the image by
	**/
	void Draw(const Color& draw_color) const;

	/** \brief Sets the static member for all future element images
	*** \param is_static Flag indicating whether the image will be static or not
	*** \note If the elements are already loaded, it doesn't bother to try to unload them
	*** and then reload them again statically.
	**/
	void SetStatic(bool is_static)
		{ _is_static = is_static; }

	/** \brief Sets the image's width, expressed as coordinate system units
	*** \param width The desired width of the image
	**/
	void SetWidth(float width);

	/** \brief Sets the image's height, expressed as coordinate system units
	*** \param height The desired height of the image
	**/
	void SetHeight(float height);

	/** \brief Sets the image's dimensions, expressed as coordinate system units
	*** \param width The desired width of the image
	*** \param height The desired height of the image
	**/
	void SetDimensions(float width, float height)
		{ SetWidth(width); SetHeight(height); }

	void EnableGrayScale()
		{}

	void DisableGrayScale()
		{}

	void SetUVCoordinates(float u1, float v1, float u2, float v2)
		{}

	/** \brief Sets the image's four vertices to a single color
	*** \param color The desired color of all image vertices
	**/
	void SetColor(const Color& color);

	/** \brief Sets the image's vertex colors
	*** \param tl The top left vertex color
	*** \param tr The top right vertex color
	*** \param bl The bottom left vertex color
	*** \param br The bottom right vertex color
	**/
	void SetVertexColors(const Color& tl, const Color& tr, const Color& bl, const Color& br);

	/** \brief Adds a new image element to the composite image
	*** \param img The image to add to the composite image.
	*** \param x_offset The x offset of the composite image.
	*** \param y_offset The y offset of the composite image.
	*** \param u1 The upper-left u coordinate for the image. The default is 0.0f.
	*** \param v1 The upper-left v coordinate for the image. The default is 0.0f.
	*** \param u2 The lower-right u coordinate for the image. The default is 1.0f.
	*** \param v2 The lower-right v coordinate for the image. The default is 1.0f.
	***
	*** Starting with a newly created StillImage, call AddImage(), for all of the images you wish
	*** to add, along with the x and y offsets that they should be positioned at. The u1, v1, u2, v2
	*** coordinates tell which portion of the image to use (usually 0.0f, 0.0f, 1.0f, 1.0f)
	**/
	void AddImage(const StillImage& img, float x_offset, float y_offset, float u1 = 0.0f, float v1 = 0.0f,
		float u2 = 1.0f, float v2 = 1.0f);

	/** \brief Creates a single composite image from a 2D array of like-sized images
	*** \param tiles A 1D vector of StillImage objects that will be used to construct the composite image
	*** \param indeces A 2D vector in row-column order (e.g. indices[y][x]) with indeces into the tiles vector
	***
	*** This method is useful for constructing variable-sized objects within a map from multiple smaller tile
	*** images. The StillImage object that this method is invoked upon will be cleared prior to constructing
	*** the composite image.
	***
	*** \note This should be obvious, but don't include "this" StillImage object inside the tiles argument vector
	*** \note All StillImages in the tiles vector should have the same dimensions
	*** \note Every vector row in indeces must be the same size
	*** \note Every index element (indices[y][x]) should range from 0 to tiles.size() - 1
	**/
// 	void ConstructCompositeImage(const std::vector<ImageElement>& tiles, const std::vector<std::vector<uint32> >& indeces);

private:
	//! \brief A container for each element in the composite image
	std::vector<private_video::ImageElement> _elements;
}; // class CompositeImage : public ImageDescriptor

}  // namespace hoa_video

#endif // __IMAGE_HEADER__
