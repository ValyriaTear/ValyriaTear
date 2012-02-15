///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    image.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for image classes
*** ***************************************************************************/

#include <cstdarg>
#include <math.h>

#include "video.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_video::private_video;

namespace hoa_video {

// -----------------------------------------------------------------------------
// ImageDescriptor class
// -----------------------------------------------------------------------------

ImageDescriptor::ImageDescriptor() :
	_texture(NULL),
	_width(0.0f),
	_height(0.0f),
	_u1(0.0f),
	_v1(0.0f),
	_u2(1.0f),
	_v2(1.0f),
	_blend(false),
	_unichrome_vertices(true),
	_is_static(false),
	_grayscale(false)
{
	_color[0] = _color[1] = _color[2] = _color[3] = Color::white;
}



ImageDescriptor::~ImageDescriptor() {
	// The destructor for the inherited class should have disabled grayscale mode
	// If it didn't, the grayscale image might not have been properly dereferenced
	// and/or removed from texture memory
	if (_grayscale == true)
		IF_PRINT_WARNING(VIDEO_DEBUG) << "grayscale mode was still enabled when destructor was invoked -- possible memory leak" << endl;

	// Remove the reference to the original, colored texture
	if (_texture != NULL)
		_RemoveTextureReference();

	_texture = NULL;
}



ImageDescriptor::ImageDescriptor(const ImageDescriptor& copy) :
	_texture(copy._texture),
	_width(copy._width),
	_height(copy._height),
	_u1(copy._u1),
	_v1(copy._v1),
	_u2(copy._u2),
	_v2(copy._v2),
	_blend(copy._blend),
	_unichrome_vertices(copy._unichrome_vertices),
	_is_static(copy._is_static),
	_grayscale(copy._grayscale)
{
	_color[0] = copy._color[0];
	_color[1] = copy._color[1];
	_color[2] = copy._color[2];
	_color[3] = copy._color[3];

	if (_texture != NULL)
		_texture->AddReference();
}



ImageDescriptor& ImageDescriptor::operator=(const ImageDescriptor& copy) {
	// Handle the case were a dumbass assigns an object to itself
	if (this == &copy) {
		return *this;
	}

	_width = copy._width;
	_height = copy._height;
	_u1 = copy._u1;
	_v1 = copy._v1;
	_u2 = copy._u2;
	_v2 = copy._v2;
	_blend = copy._blend;
	_unichrome_vertices = copy._unichrome_vertices;
	_is_static = copy._is_static;
	_grayscale = copy._grayscale;
	_color[0] = copy._color[0];
	_color[1] = copy._color[1];
	_color[2] = copy._color[2];
	_color[3] = copy._color[3];


	// Update the reference to the previous image texturee
	if (_texture != NULL && copy._texture != _texture) {
		_RemoveTextureReference();
	}

	if (copy._texture != NULL) {
		 copy._texture->AddReference();
	}

	_texture = copy._texture;
	return *this;
}



void ImageDescriptor::Clear() {
	// This will also remove a reference to the grayscale version of the image
	// TODO
// 	if (_grayscale)
// 		DisableGrayScale();

	if (_texture != NULL)
		_RemoveTextureReference();

	_texture = NULL;
	_width = 0.0f;
	_height = 0.0f;
	_u1 = 0.0f;
	_v1 = 0.0f;
	_u2 = 1.0f;
	_v2 = 1.0f;
	_color[0] = _color[1] = _color[2] = _color[3] = Color::white;
	_blend = false;
	_unichrome_vertices = true;
	_is_static = false;
	_grayscale = false;
}




void ImageDescriptor::SetColor(const Color &color) {
	_color[0] = color;
	_color[1] = color;
	_color[2] = color;
	_color[3] = color;

	if (IsFloatEqual(color[3], 1.0f) == false)
		_blend = true;
	else
		_blend = false;

	_unichrome_vertices = true;
}



void ImageDescriptor::SetVertexColors(const Color &tl, const Color &tr, const Color &bl, const Color &br) {
	_color[0] = tl;
	_color[1] = tr;
	_color[2] = bl;
	_color[3] = br;

	if (IsFloatEqual(tl[3], 1.0f) && IsFloatEqual(tr[3], 1.0f) && IsFloatEqual(bl[3], 1.0f) && IsFloatEqual(br[3], 1.0f))
		_blend = false;
	else
		_blend = true;

	if (tl == tr && tl == bl && tl == br)
		_unichrome_vertices = true;
	else
		_unichrome_vertices = false;
}



void ImageDescriptor::GetImageInfo(const std::string& filename, uint32 &rows, uint32& cols, uint32& bpp) throw(Exception) {
	// Isolate the file extension
	size_t ext_position = filename.rfind('.');

	if (ext_position == string::npos) {
		throw Exception("could not decipher file extension for filename: " + filename, __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	std::string extension = std::string(filename, ext_position, filename.length() - ext_position);

	if (extension == ".png")
		_GetPngImageInfo(filename, rows, cols, bpp);
	else if (extension == ".jpg")
		_GetJpgImageInfo(filename, rows, cols, bpp);
	else
		throw Exception("unsupported image file extension \"" + extension + "\" for filename: " + filename, __FILE__, __LINE__, __FUNCTION__);
}



bool ImageDescriptor::LoadMultiImageFromElementSize(vector<StillImage>& images, const string& filename,
	const uint32 elem_width, const uint32 elem_height)
{
	// First retrieve the dimensions of the multi image (in pixels)
	uint32 img_height, img_width, bpp;
	try {
		GetImageInfo(filename, img_height, img_width, bpp);
	}
	catch (Exception e) {
		if (VIDEO_DEBUG)
			cerr << e.ToString() << endl;
		return false;
	}

	// Make sure that the element height and width divide evenly into the height and width of the multi image
	if ((img_height % elem_height) != 0 || (img_width % elem_width) != 0) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "multi image size not evenly divisible by element size for multi image file: " << filename << endl;
		return false;
	}

	// Determine the number of rows and columns of element images inside the multi image
	uint32 grid_rows = img_height / elem_height;
	uint32 grid_cols = img_width / elem_width;

	// If necessary, resize the images vector so that it is the same size as the number of element images which
	// we will soon extract from the multi image
	if (images.size() != grid_rows * grid_cols) {
		images.resize(grid_rows * grid_cols);
	}

	// If the width or height of the StillImages in the images vector were not specified (set to the default 0.0f),
	// then set those sizes to the element width and height arguments (which are in number of pixels)
	for (vector<StillImage>::iterator i = images.begin(); i < images.end(); i++) {
		if (IsFloatEqual(i->_height, 0.0f) == true)
			i->_height = static_cast<float>(elem_height);
		if (IsFloatEqual(i->_width, 0.0f) == true)
			i->_width = static_cast<float>(elem_width);
	}

	return _LoadMultiImage(images, filename, grid_rows, grid_cols);
} // bool ImageDescriptor::LoadMultiImageFromElementSize(...)



bool ImageDescriptor::LoadMultiImageFromElementGrid(vector<StillImage>& images, const string& filename,
		const uint32 grid_rows, const uint32 grid_cols)
{
	// First retrieve the dimensions of the multi image (in pixels)
	uint32 img_height, img_width, bpp;
	try {
		GetImageInfo(filename, img_height, img_width, bpp);
	}
	catch (Exception e) {
		if (VIDEO_DEBUG)
			cerr << e.ToString() << endl;
		return false;
	}

	// Make sure that the number of grid rows and columns divide evenly into the image size
	if ((img_height % grid_rows) != 0 || (img_width % grid_cols) != 0) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "multi image size not evenly divisible by grid rows or columns for multi image file: " << filename << endl;
		return false;
	}

	// If necessary, resize the images vector so that it is the same size as the number of element images which
	// we will soon extract from the multi image
	if (images.size() != grid_rows * grid_cols) {
		images.resize(grid_rows * grid_cols);
	}

	// If the width or height of the StillImages in the images vector were not specified (set to the default 0.0f),
	// then set those sizes to the element width and height arguments (which are in number of pixels)
	float elem_width = static_cast<float>(img_width) / static_cast<float>(grid_cols);
	float elem_height = static_cast<float>(img_height) / static_cast<float>(grid_rows);
	for (vector<StillImage>::iterator i = images.begin(); i < images.end(); i++) {
		if (IsFloatEqual(i->_height, 0.0f) == true)
			i->_height = static_cast<float>(elem_height);
		if (IsFloatEqual(i->_width, 0.0f) == true)
			i->_width = static_cast<float>(elem_width);
	}

	return _LoadMultiImage(images, filename, grid_rows, grid_cols);
} // bool ImageDescriptor::LoadMultiImageFromElementGrid(...)



bool ImageDescriptor::SaveMultiImage(const vector<StillImage*>& images, const string& filename,
	const uint32 grid_rows, const uint32 grid_columns)
{
	// Check there are elements to store
	if (images.empty()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "images vector argument was empty when saving file: " << filename << endl;
		return false;
	}

	// Check if the number of images is compatible with the number of rows and columns
	if (images.size() < grid_rows * grid_columns) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "images vector argument did not contain enough images to save for file: " << filename << endl;
		return false;
	}
	else if (images.size() > grid_rows * grid_columns) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "images vector argument had a size greater than the number of images to save for file: " << filename << endl;
		// NOTE: no return false for this case because we have enough images to continue
	}

	// Check that all the images are non-NULL and are of the same size
	float img_width = images[0]->_width;
	float img_height = images[0]->_height;
	for (uint32 i = 0; i < images.size(); i++) {
		if (images[i] == NULL || images[i]->_image_texture == NULL) {
			IF_PRINT_WARNING(VIDEO_DEBUG) << "NULL StillImage or ImageElement was present in images vector argument when saving file: " << filename << endl;
			return false;
		}
		if (IsFloatEqual(images[i]->_width, img_width) == false || IsFloatEqual(images[i]->_height, img_height)) {
			IF_PRINT_WARNING(VIDEO_DEBUG) << "images contained in vector argument did not share the same dimensions" << endl;
			return false;
		}
	}

	// Isolate the filename's extension and determine the type of image file we're saving
	bool is_png_image;
	size_t ext_position = filename.rfind('.');
	if (ext_position == string::npos) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "failed to decipher file extension for filename: " << filename << endl;
		return false;
	}

	string extension = string(filename, ext_position, filename.length() - ext_position);

	if (extension == ".png")
		is_png_image = true;
	else if (extension == ".jpg")
		is_png_image = false;
	else {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "unsupported file extension: \"" << extension << "\" for filename: " << filename << endl;
		return false;
	}

	// Structure for the image buffer to save
	ImageMemory save;

	save.height = static_cast<int32>(grid_rows * img_height);
	save.width = static_cast<int32>(grid_columns * img_width);
	save.pixels = malloc(save.width * save.height * 4);

	if (save.pixels == NULL) {
		PRINT_ERROR << "failed to malloc enough memory to save new image file: " << filename << endl;
		return false;
	}

	// Initially, we need to grab the Image pointer of the first StillImage, the texture ID of its TextureSheet owner,
	// and malloc enough memory for the entire sheet so that we can copy over the texture sheet from video memory to
	// system memory.
	ImageTexture* img = images[0]->_image_texture;
	GLuint tex_id = img->texture_sheet->tex_id;

	ImageMemory texture;
	texture.width = img->texture_sheet->width;
	texture.height = img->texture_sheet->height;
	texture.pixels = malloc(texture.width * texture.height * 4);

	if (texture.pixels == NULL) {
		PRINT_ERROR << "failed to malloc enough memory to save new image file: " << filename << endl;
		free(save.pixels);
		return false;
	}

	TextureManager->_BindTexture(tex_id);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.pixels);

	uint32 i = 0; // i is used to count through the images vector to get the image to save
	for (uint32 x = 0; x < grid_rows; x++) {
		for (uint32 y = 0; y < grid_columns; y++) {
			img = images[i]->_image_texture;

			// Check if this image has a different texture ID than the last. If it does, we need to re-grab the texture
			// memory for the texture sheet that the new image is contained within and store it in the texture.pixels
			// buffer, which is CPU system memory.
			if (tex_id != img->texture_sheet->tex_id) {
				// Get new texture ID
				TextureManager->_BindTexture(img->texture_sheet->tex_id);
				tex_id = img->texture_sheet->tex_id;

				// If the new texture is bigger, reallocate memory
				if (texture.height * texture.width < img->texture_sheet->height * img->texture_sheet->width) {
					free(texture.pixels);
					texture.width = img->texture_sheet->width;
					texture.height = img->texture_sheet->height;
					texture.pixels = realloc(texture.pixels, texture.width * texture.height * 4);
					if (texture.pixels == NULL) {
						PRINT_ERROR << "failed to malloc enough memory to save new image file: " << filename << endl;
						free(save.pixels);
						return false;
					}
				}
				glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.pixels);
			}

			// Determine the part of the texture that we are interested in (the part that contains the current image we're saving)
			uint32 src_offset = img->y * texture.width * 4 + img->x * 4;
			uint32 dst_offset = static_cast<uint32>(x * img_height * img_width * grid_columns * 4 + y * img_width * 4);
			uint32 src_bytes = texture.width * 4;
			uint32 dst_bytes = static_cast<uint32>(img_width * grid_columns * 4);
			uint32 copy_bytes = static_cast<uint32>(img_width * 4);

			// Copy each row of image pixels over the the save.pixels buffer one at a time
			for (int32 j = 0; j < img_height; j++) {
				memcpy((uint8*)save.pixels + j * dst_bytes + dst_offset, (uint8*)texture.pixels + j * src_bytes + src_offset, copy_bytes);
			}

			i++;
		} // for (uint32 y = 0; y < grid_columns; y++)
	} // for (uint32 x = 0; x < grid_rows; x++)

	// save.pixels now contains all the image data we wish to save, so write it out to the new image file
	bool success = true;
	success = save.SaveImage(filename, is_png_image);
	free(save.pixels);
	free(texture.pixels);

	return success;
} // bool ImageDescriptor::SaveMultiImage(...)



void ImageDescriptor::DEBUG_PrintInfo() {
	cout << "__ImageDescriptor Properties__" << endl;
	cout << "* width:                " << _width << endl;
	cout << "* height:               " << _height << endl;
	cout << "* UV coordinates:        (" << _u1 << ", " << _v1 << "), (" << _u2 << ", " << _v2 << ")" << endl;
	cout << "* colors, RGBA format:  " << endl;
	cout << "  * TL                  " << _color[0].GetRed() << ", " << _color[0].GetGreen() << ", " << _color[0].GetBlue() << ", " << _color[0].GetAlpha() << endl;
	cout << "  * TR                  " << _color[1].GetRed() << ", " << _color[1].GetGreen() << ", " << _color[1].GetBlue() << ", " << _color[1].GetAlpha() << endl;
	cout << "  * BL                  " << _color[2].GetRed() << ", " << _color[2].GetGreen() << ", " << _color[2].GetBlue() << ", " << _color[2].GetAlpha() << endl;
	cout << "  * BR:                 " << _color[3].GetRed() << ", " << _color[3].GetGreen() << ", " << _color[3].GetBlue() << ", " << _color[3].GetAlpha() << endl;
	cout << "* static:               " << (_is_static ? "true" : "false") << endl;
	cout << "* grayscale:            " << (_grayscale ? "true" : "false") << endl;
	cout << endl;
}



void ImageDescriptor::_RemoveTextureReference() {
	if (_texture == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "_texture member was NULL upon method invocation" << endl;
		return;
	}

	if (_texture->RemoveReference() == true) {
		_texture->texture_sheet->RemoveTexture(_texture);

		// If the image exceeds 512 in either width or height, it has an un-shared texture sheet, which we
		// should now delete that the image is being removed
		if (_texture->width > 512 || _texture->height > 512) {
			TextureManager->_RemoveSheet(_texture->texture_sheet);
		}
// 		else {
//
// 			// TODO: Otherise simply mark the image as free in the texture sheet
// // 			texture->texture_sheet->FreeTexture(texture);
// 		}
		delete _texture;
	}

	_texture = NULL;
}



void ImageDescriptor::_DrawOrientation() const {
	Context& current_context = VideoManager->_current_context;

	// Calculate x and y draw offsets due to any screen shaking effects
	float x_shake = VideoManager->_x_shake * (current_context.coordinate_system.GetRight() - current_context.coordinate_system.GetLeft()) / 1024.0f;
	float y_shake = VideoManager->_y_shake * (current_context.coordinate_system.GetTop() - current_context.coordinate_system.GetBottom()) / 768.0f;

	// TODO: I honestly have no idea what this is >_>
	float x_align_offset = ((current_context.x_align + 1) * _width) * 0.5f * -current_context.coordinate_system.GetHorizontalDirection();
	float y_align_offset = ((current_context.y_align + 1) * _height) * 0.5f * -current_context.coordinate_system.GetVerticalDirection();

	VideoManager->MoveRelative(x_align_offset, y_align_offset);

	// x/y draw offsets, which are a function of the flip draw flags, screen shaking, and the orientation of the current coordinate system
	float x_off = 0.0f, y_off = 0.0f;

	if (current_context.x_flip) {
		x_off = _width;
	}
	if (current_context.y_flip) {
		y_off = _height;
	}
	x_off += x_shake;
	y_off += y_shake;

	VideoManager->MoveRelative(x_off * current_context.coordinate_system.GetHorizontalDirection(), y_off * current_context.coordinate_system.GetVerticalDirection());

	// x/y scale degrees
	float x_scale = _width;
	float y_scale = _height;

	if (current_context.coordinate_system.GetHorizontalDirection() < 0.0f)
		x_scale = -x_scale;
	if (current_context.coordinate_system.GetVerticalDirection() < 0.0f)
		y_scale = -y_scale;
	glScalef(x_scale, y_scale, 1.0f);
}



void ImageDescriptor::_DrawTexture(const Color* draw_color) const {
	// Array of the four vertexes defined on the 2D plane for glDrawArrays()
	// This is no longer const, because when tiling the background for the menu's
	// sometimes you need to draw part of a texture
	float vert_coords[] = {
		_u1, _v1,
		_u2, _v1,
		_u2, _v2,
		_u1, _v2,
	};

	// If no color array was passed, use the image's own vertex colors
	if (draw_color == NULL)
		draw_color = _color;

	// Set blending parameters
	if (VideoManager->_current_context.blend) {
		glEnable(GL_BLEND);
		if (VideoManager->_current_context.blend == 1)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Normal blending
		else
			glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending
	}
	else if (_blend) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Normal blending
	}
	else {
		glDisable(GL_BLEND);
	}

	// If we have a valid image texture poiner, setup texture coordinates and the texture coordinate array for glDrawArrays()
	if (_texture != NULL) {
		// Set the texture coordinates
		float s0, s1, t0, t1;

		s0 = _texture->u1 + (_u1 * (_texture->u2 - _texture->u1));
		s1 = _texture->u1 + (_u2 * (_texture->u2 - _texture->u1));
		t0 = _texture->v1 + (_v1 * (_texture->v2 - _texture->v1));
		t1 = _texture->v1 + (_v2 * (_texture->v2 - _texture->v1));

		// Swap x texture coordinates if x flipping is enabled
		if (VideoManager->_current_context.x_flip) {
			float temp = s0;
			s0 = s1;
			s1 = temp;
		}

		// Swap y texture coordinates if y flipping is enabled
		if (VideoManager->_current_context.y_flip) {
			float temp = t0;
			t0 = t1;
			t1 = temp;
		}

		// Place the texture coordinates in a 4x2 array mirroring the structure of the vertex array for use in glDrawArrays().
		float tex_coords[] = {
			s0, t1,
			s1, t1,
			s1, t0,
			s0, t0,
		};

		// Enable texturing and bind texture
		glEnable(GL_TEXTURE_2D);
		TextureManager->_BindTexture(_texture->texture_sheet->tex_id);
		_texture->texture_sheet->Smooth(_texture->smooth);

		// Enable and setup the texture coordinate array
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, tex_coords);

		if (_unichrome_vertices == true) {
			glColor4fv((GLfloat*)draw_color[0].GetColors());
		}
		else {
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, 0, (GLfloat*)draw_color);
		}
	} // if (_texture != NULL)

	// Otherwise there is no image texture, so we're drawing pure color on the vertices
	else {
		// Use a single call to glColor for unichrome images, or a setup a gl color array for multiple colors
		if (_unichrome_vertices == true) {
			glColor4fv((GLfloat*)draw_color[0].GetColors());
			glDisableClientState(GL_COLOR_ARRAY);
		}
		else {
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, 0, (GLfloat*)draw_color);
		}

		// Disable texturing as we're using pure colour
		glDisable(GL_TEXTURE_2D);
	}

	// Use a vertex array to draw all of the vertices
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vert_coords);
	glDrawArrays(GL_QUADS, 0, 4);

	if (VideoManager->_current_context.blend || _blend == true)
		glDisable(GL_BLEND);

	if (VideoManager->CheckGLError() == true) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "an OpenGL error occurred: " << VideoManager->CreateGLErrorString() << endl;
	}
} // void ImageDescriptor::_DrawTexture(const Color* color_array) const



void ImageDescriptor::_GetPngImageInfo(const std::string& filename, uint32& rows, uint32& cols, uint32& bpp) throw(Exception) {
	// first, we start by reading from the file
	FILE* fp = fopen(filename.c_str(), "rb");

	if (fp == NULL) {
		throw Exception("failed to open file: " + filename, __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	// check the signature - make sure it is actually a PNG! otherwise BAD THINGS would happen
	uint8 test_buffer[8];

	fread(test_buffer, 1, 8, fp);
	if (png_sig_cmp(test_buffer, 0, 8)) {
		throw Exception("png_sig_cmp() failed for file: " + filename, __FILE__, __LINE__, __FUNCTION__);
		fclose(fp);
		return;
	}

	// open up our PNG file
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);

	if (png_ptr == NULL) {
		fclose(fp);
		return;
	}

	// grab the info structure
	png_infop info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, NULL, (png_infopp)NULL);
		fclose(fp);
		throw Exception("png_create_info_struct() returned NULL for file: " + filename, __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	// error checking
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, NULL, (png_infopp)NULL);
		fclose(fp);
		throw Exception("setjmp returned non-zero value for file: " + filename, __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	// open up the IO stuff and read the PNG
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	// grab the relevant data...
	cols = info_ptr->width;
	rows = info_ptr->height;
	bpp = info_ptr->channels * 8;

	// and clean up.
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	fclose(fp);
} // void ImageDescriptor::_GetPngImageInfo(const std::string& filename, uint32& rows, uint32& cols, uint32& bpp)



void ImageDescriptor::_GetJpgImageInfo(const std::string& filename, uint32& rows, uint32& cols, uint32& bpp) throw(Exception) {
	// open up the file (with C IO)
	FILE* fp = fopen(filename.c_str(), "rb");

	if (fp == NULL) {
		throw Exception("failed to open file: " + filename, __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	// do our magical setup: create a jpeg decompressor and the relevant error stuff
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	// tell jpeg where to look for the data...
	jpeg_stdio_src(&cinfo, fp);
	// and read the header
	jpeg_read_header(&cinfo, TRUE);

	// grab the relevant information from the header...
	cols = cinfo.output_width;
	rows = cinfo.output_height;
	bpp = cinfo.output_components;

	// clean up
	jpeg_destroy_decompress(&cinfo);

	fclose(fp);
} // void ImageDescriptor::_GetJpgImageInfo(const std::string& filename, uint32& rows, uint32& cols, uint32& bpp)



bool ImageDescriptor::_LoadMultiImage(vector<StillImage>& images, const string &filename,
	const uint32 grid_rows, const uint32 grid_cols)
{
	uint32 current_image;
	uint32 x, y;

	bool need_load = false;

	// 1D vectors storing info for each image element
	vector<std::string> tags;
	vector<bool> loaded;

	// Construct the tags for each image element and figure out which elements are not
	// already in texture memory and need to be loaded
	for (x = 0; x < grid_rows; x++) {
		for (y = 0; y < grid_cols; y++) {
			tags.push_back("<X" + NumberToString(x) + "_" + NumberToString(grid_rows) + ">" +
				"<Y" + NumberToString(y) + "_" + NumberToString(grid_cols) + ">");

			if (TextureManager->_IsImageTextureRegistered(filename + tags.back())) {
				loaded.push_back(true);
			}
			else {
				loaded.push_back(false);
				need_load = true;
			}
		}
	}

	// If the image elements are not all loaded, then load the multi image file
	// from disk and create enough memory to copy over individual sub-image elements from it
	ImageMemory multi_image;
	ImageMemory sub_image;
	if (need_load) {
		if (multi_image.LoadImage(filename) == false) {
			IF_PRINT_WARNING(VIDEO_DEBUG) << "failed to load multi image file: " << filename << endl;
			return false;
		}

		sub_image.width = multi_image.width / grid_cols;
		sub_image.height = multi_image.height / grid_rows;
		sub_image.pixels = malloc(sub_image.width * sub_image.height * 4);
		if (sub_image.pixels == NULL) {
			PRINT_ERROR << "failed to malloc memory for multi image file: " << filename << endl;
			free(multi_image.pixels);
			multi_image.pixels = NULL;
			return false;
		}
	}

	// One by one, get the subimages
	current_image = 0;
	for (x = 0; x < grid_rows; x++) {
		for (y = 0; y < grid_cols; y++) {
			ImageTexture* img;

			// If this image already exists in a texture sheet somewhere, add a reference to it
			// and add a new ImageElement to the current StillImage
			if (loaded[current_image] == true) {
				img = TextureManager->_GetImageTexture(filename + tags[current_image]);

				if (img == NULL) {
					IF_PRINT_WARNING(VIDEO_DEBUG) << "a NULL image was found in the TextureManager's _images container "
						<< "-- aborting multi image load operation" << endl;

					free(multi_image.pixels);
					free(sub_image.pixels);
					multi_image.pixels = NULL;
					sub_image.pixels = NULL;
					return false;
				}

				images.at(current_image)._filename = filename;
				images.at(current_image)._texture = img;
				images.at(current_image)._image_texture = img;
			}

			// We have to first extract this image from the larger multi image and add it to a texture sheet.
			// Then we can add the image data to the StillImage being constructed
			else {
				images.at(current_image)._filename = filename;

				for (int32 i = 0; i < sub_image.height; i++) {
					memcpy((uint8*)sub_image.pixels + 4 * sub_image.width * i, (uint8*)multi_image.pixels + (((x * multi_image.height / grid_rows) + i) *
						multi_image.width + y * multi_image.width / grid_cols) * 4, 4 * sub_image.width);
				}

				img = new ImageTexture(filename, tags[current_image], sub_image.width, sub_image.height);

				// Try to insert the image in a texture sheet
				TexSheet* sheet = TextureManager->_InsertImageInTexSheet(img, sub_image, images.at(current_image)._is_static);

				if (sheet == NULL) {
					IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TextureController::_InsertImageInTexSheet failed -- " <<
						"aborting multi image load operation" << endl;

					free(multi_image.pixels);
					free(sub_image.pixels);
					multi_image.pixels = NULL;
					sub_image.pixels = NULL;
					delete img;
					return false;
				}

				images.at(current_image)._texture = img;
				images.at(current_image)._image_texture = img;
			}

			img->AddReference();

			// Finally, do a grayscale conversion for the image if grayscale mode is enabled
			if (images.at(current_image)._grayscale) {
				// Set _grayscale to false so that the call doesn't think that the grayscale image is already loaded
				// It will be set back to true by the EnableGrayScale call
				images.at(current_image)._grayscale = false;
				images.at(current_image).EnableGrayScale();
			}

			current_image++;
		} // for (y = 0; y < grid_cols; y++)
	} // for (x = 0; x < grid_rows; x++)

	// Make sure to free all dynamically allocated memory
	if (multi_image.pixels) {
		free(multi_image.pixels);
		multi_image.pixels = NULL;
	}
	if (sub_image.pixels) {
		free(sub_image.pixels);
		sub_image.pixels = NULL;
 	}

	return true;
} // bool ImageDescriptor::_LoadMultiImage(...)

// -----------------------------------------------------------------------------
// StillImage class
// -----------------------------------------------------------------------------

StillImage::StillImage(const bool grayscale) :
	ImageDescriptor(),
	_filename(""),
	_image_texture(NULL)
{
	Clear();
	_grayscale = grayscale;
}



StillImage::~StillImage() {
	Clear();
}



void StillImage::Clear() {
	ImageDescriptor::Clear(); // This call will remove the texture reference for us
	_filename.clear();
	_image_texture = NULL;
}



bool StillImage::Load(const string& filename) {
	// Delete everything previously stored in here
	if (_image_texture != NULL) {
		_RemoveTextureReference();
		_image_texture = NULL;
		_width = 0.0f;
		_height = 0.0f;
	}

	_filename = filename;

	// TEMP: This is a temporary hack to support procedural images by using empty filenames. It should be removed later
	if (filename.empty() == true) {
		return true;
	}

	// 1. Check if an image with the same filename has already been loaded. If so, point to that and increment its reference
	if ((_image_texture = TextureManager->_GetImageTexture(_filename)) != NULL) {
		_texture = _image_texture;

		if (_image_texture == NULL) {
			IF_PRINT_WARNING(VIDEO_DEBUG) << "recovered a NULL image inside the TextureManager's image map: " << _filename << endl;
			return false;
		}

		// If the width or height of this object is 0.0, use the pixel width/height of the image texture
		if (IsFloatEqual(_width, 0.0f))
			_width = static_cast<float>(_image_texture->width);
		if (IsFloatEqual(_height, 0.0f))
			_height = static_cast<float>(_image_texture->height);

		_texture->AddReference();
		return true;
	}

	// 2. The image file needs to be loaded from disk
	ImageMemory img_data;
	if (img_data.LoadImage(_filename) == false) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "call to ImageMemory::LoadImage() failed for file: " << _filename << endl;
		return false;
	}

	// Create a new texture image and store it in a texture sheet. If the _grayscale member of this class is true,
	// we first load the color copy of the image to a texture sheet. Then we'll convert the image data to grayscale
	// and save that image data to texture memory as well
	_image_texture = new ImageTexture(_filename, "", img_data.width, img_data.height);
	_texture = _image_texture;

	if (TextureManager->_InsertImageInTexSheet(_image_texture, img_data, _is_static) == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TextureController::_InsertImageInTexSheet() failed for file: " << _filename << endl;
		delete _image_texture;
		_image_texture = NULL;
		_texture = NULL;
		free(img_data.pixels);
		img_data.pixels = NULL;
		return false;
	}

	_image_texture->AddReference();

	// If width or height members are zero, set them to the dimensions of the image data (which are in number of pixels)
	if (IsFloatEqual(_width, 0.0f) == true)
		_width = static_cast<float>(img_data.width);

	if (IsFloatEqual(_height, 0.0f) == true)
		_height = static_cast<float>(img_data.height);

	// If we don't need to create a grayscale version, we finished successfully
	if (_grayscale == false) {
		free(img_data.pixels);
		img_data.pixels = NULL;
		return true;
	}

	// 3. If we reached this point, we must now create a grayscale version of this image
	img_data.ConvertToGrayscale();
	ImageTexture* gray_image = new ImageTexture(_filename, "<G>", img_data.width, img_data.height);
	if (TextureManager->_InsertImageInTexSheet(gray_image, img_data, _is_static) == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TextureController::_InsertImageInTexSheet() failed for file: " << _filename << endl;

		TextureManager->_UnregisterImageTexture(gray_image);
		delete gray_image;
		_RemoveTextureReference(); // sets _texture to NULL
		_image_texture = NULL;
		free(img_data.pixels);
		img_data.pixels = NULL;
		return false;
	}

	_image_texture = gray_image;
	_texture = _image_texture;
	_image_texture->AddReference();

	free(img_data.pixels);
	img_data.pixels = NULL;
	return true;
} // bool StillImage::Load(const string& filename)



void StillImage::Draw() const {
	// If real lighting is enabled, draw images normally since the light overlay
	// will take care of the modulation. If not, (i.e. no overlay is being used)
	// then pass the light color so the vertex colors can do the modulation
	if (VideoManager->_uses_lights == false && (VideoManager->_light_color != Color::white))
		Draw(VideoManager->_light_color);
	else
		Draw(Color::white);
}



void StillImage::Draw(const Color& draw_color) const {
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
} // void StillImage::Draw(const Color& draw_color) const



bool StillImage::Save(const string& filename) const {
	if (_image_texture == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "attempted to save an image that had no texture reference" << endl;
		return false;
	}

	// Isolate the file extension
	size_t ext_position = filename.rfind('.');
	bool is_png_image;

	if (ext_position == string::npos) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "could not decipher file extension for file: " << filename << endl;
		return false;
	}

	string extension = string(filename, ext_position, filename.length() - ext_position);

	if (extension == ".png")
		is_png_image = true;
	else if (extension == ".jpg")
		is_png_image = false;
	else {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "unsupported file extension \"" << extension << "\" for file: " << filename << endl;
		return false;
	}

	ImageMemory buffer;
	buffer.CopyFromImage(_image_texture);
	return buffer.SaveImage(filename, is_png_image);
} // bool StillImage::Save(const string& filename)



void StillImage::EnableGrayScale() {
	if (_grayscale) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "grayscale mode was already enabled" << endl;
		return;
	}

	_grayscale = true;

	// 1. If no image texture is available we are done here (when Load() is next called, grayscale will automatically be enabled)
	if (_image_texture == NULL) {
		return;
	}

	// 2. Check if a grayscale version of this image already exists in texture memory and if so, update the ImageTexture pointer and reference
	string search_key = _filename + _image_texture->tags + "<G>";
	string tags = _image_texture->tags;
	ImageTexture *temp_texture = _image_texture;
	if ((_image_texture = TextureManager->_GetImageTexture(search_key)) != NULL) {
		// NOTE: We do not decrement the reference to the colored image, because we want to guarantee that
		// it remains referenced in texture memory while its grayscale counterpart is being used
		_texture = _image_texture;
		_image_texture->AddReference();
		return;
	}

	// 3. If no grayscale version exists, create a copy of the image, convert it to grayscale, and add the gray copy to texture memory
	ImageMemory gray_img;
	gray_img.CopyFromImage(temp_texture);
	gray_img.ConvertToGrayscale();

	ImageTexture* new_img = new ImageTexture(_filename, tags + "<G>", gray_img.width, gray_img.height);

	if (TextureManager->_InsertImageInTexSheet(new_img, gray_img, _is_static) == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "failed to insert new grayscale image into texture sheet" << endl;
		delete new_img;

		if (gray_img.pixels) {
			free(gray_img.pixels);
			gray_img.pixels = NULL;
		}

		return;
	}

	_image_texture = new_img;
	_texture = _image_texture;
	_image_texture->AddReference();

	if (gray_img.pixels) {
		free(gray_img.pixels);
		gray_img.pixels = NULL;
	}
} // void StillImage::EnableGrayScale()


void StillImage::DisableGrayScale() {
	if (_grayscale == false) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "grayscale mode was already disabled" << endl;
		return;
	}

	_grayscale = false;

	// If no image data is loaded, we're finished
	if (_image_texture == NULL) {
		return;
	}

	string search_key = _image_texture->filename + _image_texture->tags.substr(0, _image_texture->tags.length() - 3);
	if ((_image_texture = TextureManager->_GetImageTexture(search_key)) == NULL) {
		PRINT_WARNING << "non-grayscale version of image was not found in texture memory" << endl;
		return;
	}

	// Remove the reference to the grayscale version and grab the reference to the original color image
	_RemoveTextureReference();

	// No reference change is needed for the color image, since the color texture did not have a reference
	// decrement when the grayscale version was enabled
	_texture = _image_texture;
} // void StillImage::DisableGrayScale()

// -----------------------------------------------------------------------------
// AnimatedImage class
// -----------------------------------------------------------------------------

AnimatedImage::AnimatedImage(const bool grayscale) {
	Clear();
	_grayscale = grayscale;
}



AnimatedImage::AnimatedImage(float width, float height, bool grayscale) {
	Clear();
	_width = width;
	_height = height;
	_grayscale = grayscale;
}



void AnimatedImage::Clear() {
	ImageDescriptor::Clear();
	_frame_index = 0;
	_frame_counter = 0;
	// clear all animation frame images
	for (vector<AnimationFrame>::iterator i = _frames.begin(); i != _frames.end(); ++i)
		(*i).image.Clear();
	_frames.clear();
	_number_loops = -1;
	_loop_counter = 0;
	_loops_finished = false;
}



bool AnimatedImage::LoadFromFrameSize(const string& filename, const vector<uint32>& timings, const uint32 frame_width, const uint32 frame_height, const uint32 trim) {
	// Make the multi image call
	// TODO: Handle the case where the _grayscale member is true so all frames are loaded in grayscale format
	vector<StillImage> image_frames;
	if (ImageDescriptor::LoadMultiImageFromElementSize(image_frames, filename, frame_width, frame_height) == false) {
		return false;
	}

	if (trim >= image_frames.size()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "attempt to trim away more frames than requested to load for file: " << filename << endl;
		return false;
	}

	if (timings.size() < (image_frames.size() - trim)) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "not enough timing data to fill frames grid when loading file: " << filename << endl;
		return false;
	}

	_frames.clear();
	ResetAnimation();

	// Add the loaded frame image and timing information
	for (uint32 i = 0; i < image_frames.size() - trim; i++) {
		_frames.push_back(AnimationFrame());
		image_frames[i].SetDimensions(_width, _height);
		_frames.back().image = image_frames[i];
		_frames.back().frame_time = timings[i];
		if (timings[i] == 0) {
			IF_PRINT_WARNING(VIDEO_DEBUG) << "added a frame time value of zero when loading file: " << filename << endl;
		}
	}

	return true;
} // bool AnimatedImage::LoadFromFrameSize(...)



bool AnimatedImage::LoadFromFrameGrid(const string& filename, const vector<uint32>& timings, const uint32 frame_rows, const uint32 frame_cols, const uint32 trim) {
	if (trim >= frame_rows * frame_cols) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "attempt to trim away more frames than requested to load for file: " << filename << endl;
		return false;
	}

	if (timings.size() < (frame_rows * frame_cols - trim)) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "not enough timing data to fill frames grid when loading file: " << filename << endl;
		return false;
	}

	_frames.clear();
	ResetAnimation();

	// Make the multi image call
	// TODO: Handle the case where the _grayscale member is true so all frames are loaded in grayscale format
	vector<StillImage> image_frames;
	if (ImageDescriptor::LoadMultiImageFromElementGrid(image_frames, filename, frame_rows, frame_cols) == false) {
		return false;
	}

	// Add the loaded frame image and timing information
	for (uint32 i = 0; i < frame_rows * frame_cols - trim; i++) {
		_frames.push_back(AnimationFrame());
		image_frames[i].SetDimensions(_width, _height);
		_frames.back().image = image_frames[i];
		_frames.back().frame_time = timings[i];
		if (timings[i] == 0) {
			IF_PRINT_WARNING(VIDEO_DEBUG) << "added zero frame time for an image frame when loading file: " << filename << endl;
		}
	}

	return true;
} // bool AnimatedImage::LoadFromFrameGrid(...)



void AnimatedImage::Draw() const {
	if (_frames.empty()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "no frames were loaded into the AnimatedImage object" << endl;
		return;
	}

	_frames[_frame_index].image.Draw();
}



void AnimatedImage::Draw(const Color& draw_color) const {
	if (_frames.empty()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "no frames were loaded into the AnimatedImage object" << endl;
		return;
	}

	_frames[_frame_index].image.Draw(draw_color);
}



bool AnimatedImage::Save(const std::string& filename, uint32 grid_rows, uint32 grid_cols) const {
	vector<StillImage*> image_frames;
	for (uint32 i = 0; i < _frames.size(); i++) {
		image_frames.push_back(const_cast<StillImage*>(&(_frames[i].image)));
	}

	if (grid_rows == 0 || grid_cols == 0) {
		return ImageDescriptor::SaveMultiImage(image_frames, filename, 1, _frames.size());
	}
	else {
		return ImageDescriptor::SaveMultiImage(image_frames, filename, grid_rows, grid_cols);
	}
}



void AnimatedImage::EnableGrayScale() {
	if (_grayscale == true) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "grayscale mode was already enabled when function was invoked" << endl;
		return;
	}

	_grayscale = true;
	for (uint32 i = 0; i < _frames.size(); i++) {
		_frames[i].image.EnableGrayScale();
	}
}



void AnimatedImage::DisableGrayScale() {
	if (_grayscale == false) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "grayscale mode was already disabled when function was invoked" << endl;
		return;
	}

	_grayscale = false;
	for (uint32 i = 0; i < _frames.size(); i++) {
		_frames[i].image.DisableGrayScale();
	}
}



void AnimatedImage::Update() {
	if (_frames.size() <= 1)
		return;

	if (_loops_finished)
		return;

	// Get the amount of time that expired since the last frame
	uint32 frame_change = VideoManager->GetFrameChange();
	_frame_counter += frame_change;

	// If the frame time has expired, update the frame index and counter.
	while (_frame_counter >= _frames[_frame_index].frame_time) {
		frame_change = _frame_counter - _frames[_frame_index].frame_time;
		_frame_index++;
		if (_frame_index >= _frames.size()) {
				// Check if the animation has looping enabled and if so, increment the loop counter
				// and cease the ani_loop_countermation if the number of animation loops have finished
			if (_number_loops >= 0 && ++_loop_counter >= _number_loops) {
				_loops_finished = true;
				_frame_counter = 0;
				_frame_index--;
				return;
			}
			_frame_index = 0;
		}
		_frame_counter = frame_change;
	}
} // void AnimatedImage::Update()



bool AnimatedImage::AddFrame(const string& frame, uint32 frame_time) {
	StillImage img;
	img.SetStatic(_is_static);
	img.SetVertexColors(_color[0], _color[1], _color[2], _color[3]);
	if (img.Load(frame, _width, _height) == false) {
		return false;
	}

	AnimationFrame new_frame;
	new_frame.frame_time = frame_time;
	new_frame.image = img;
	_frames.push_back(new_frame);
	return true;
}



bool AnimatedImage::AddFrame(const StillImage& frame, uint32 frame_time) {
	if (frame._image_texture == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "StillImage argument did not contain any image elements" << endl;
		return false;
	}

	AnimationFrame new_frame;
	new_frame.image = frame;
	new_frame.frame_time = frame_time;

	_frames.push_back(new_frame);
	return true;
}



void AnimatedImage::SetWidth(float width) {
	_width = width;

	for (uint32 i = 0; i < _frames.size(); ++i) {
		_frames[i].image.SetWidth(width);
	}
}



void AnimatedImage::SetHeight(float height) {
	_height = height;

	for (uint32 i = 0; i < _frames.size(); i++) {
		_frames[i].image.SetHeight(height);
	}
}



void AnimatedImage::SetDimensions(float width, float height) {
	_width = width;
	_height = height;

	for (uint32 i = 0; i < _frames.size(); i++) {
		_frames[i].image.SetDimensions(width, height);
	}
}



void AnimatedImage::SetColor(const Color &color) {
	ImageDescriptor::SetColor(color);

	for (uint32 i = 0; i < _frames.size(); i++) {
		_frames[i].image.SetColor(color);
	}
}



void AnimatedImage::SetVertexColors(const Color &tl, const Color &tr, const Color &bl, const Color &br) {
	ImageDescriptor::SetVertexColors(tl, tr, bl, br);

	for (uint32 i = 0; i < _frames.size(); i++) {
		_frames[i].image.SetVertexColors(tl, tr, bl, br);
	}
}

// -----------------------------------------------------------------------------
// CompositeImage class
// -----------------------------------------------------------------------------

void CompositeImage::Clear() {
	ImageDescriptor::Clear();
	_elements.clear();
}



void CompositeImage::Draw() const {
	// If real lighting is enabled, draw images normally since the light overlay
	// will take care of the modulation. If not, (i.e. no overlay is being used)
	// then pass the light color so the vertex colors can do the modulation
	if (VideoManager->_uses_lights == false && (VideoManager->_light_color != Color::white))
		Draw(VideoManager->_light_color);
	else
		Draw(Color::white);
}



void CompositeImage::Draw(const Color& draw_color) const {
	// Don't draw anything if this image is completely transparent (invisible)
	if (IsFloatEqual(draw_color[3], 0.0f) == true) {
		return;
	}

	float modulation = VideoManager->_screen_fader.GetFadeModulation();
	Color fade_color(modulation, modulation, modulation, 1.0f);

	CoordSys coord_sys = VideoManager->_current_context.coordinate_system;

	float x_shake = VideoManager->_x_shake * (coord_sys.GetRight() - coord_sys.GetLeft()) / 1024.0f;
	float y_shake = VideoManager->_y_shake * (coord_sys.GetTop() - coord_sys.GetBottom()) / 768.0f;

	float x_align_offset = ((VideoManager->_current_context.x_align + 1) * _width) * 0.5f * -
		coord_sys.GetHorizontalDirection();
	float y_align_offset = ((VideoManager->_current_context.y_align + 1) * _height) * 0.5f * -
		coord_sys.GetVerticalDirection();

	// Save the draw cursor position as we move to draw each element
	glPushMatrix();

	VideoManager->MoveRelative(x_align_offset, y_align_offset);

	bool skip_modulation = (draw_color == Color::white && IsFloatEqual(modulation, 1.0f));

	// If we're modulating, calculate the fading color now
	if (skip_modulation == false)
		fade_color = draw_color * fade_color;

	for (uint32 i = 0; i < _elements.size(); ++i) {
		float x_off, y_off;

		if (VideoManager->_current_context.x_flip) {
			x_off = _width - _elements[i].x_offset - _elements[i].image.GetWidth();
		}
		else {
			x_off = _elements[i].x_offset;
		}

		if (VideoManager->_current_context.y_flip) {
			y_off = _height - _elements[i].y_offset - _elements[i].image.GetHeight();
		}
		else {
			y_off = _elements[i].y_offset;
		}

		x_off += x_shake;
		y_off += y_shake;

		glPushMatrix();
		VideoManager->MoveRelative(x_off * coord_sys.GetHorizontalDirection(),
			y_off * coord_sys.GetVerticalDirection());

		float x_scale = _elements[i].image.GetWidth();
		float y_scale = _elements[i].image.GetHeight();

		if (coord_sys.GetHorizontalDirection() < 0.0f)
			x_scale = -x_scale;
		if (coord_sys.GetVerticalDirection() < 0.0f)
			y_scale = -y_scale;

		glScalef(x_scale, y_scale, 1.0f);

		if (skip_modulation)
			_elements[i].image._DrawTexture(_color);
		else {
			Color modulated_colors[4];
			modulated_colors[0] = _color[0] * fade_color;
			modulated_colors[1] = _color[1] * fade_color;
			modulated_colors[2] = _color[2] * fade_color;
			modulated_colors[3] = _color[3] * fade_color;
			_elements[i].image._DrawTexture(modulated_colors);
		}
		glPopMatrix();
	}
	glPopMatrix();
} // void CompositeImage::Draw(const Color& draw_color) const



void CompositeImage::SetWidth(float width) {
	// Case 1: No image elements loaded, just change the internal width
	if (_elements.empty() == true) {
		_width = width;
		return;
	}

	// Case 2: we only have one image element to change its width
	if (_elements.size() == 1) {
		_width = width;
		_elements[0].image.SetWidth(width);
		return;
	}

	// Case 3: We must set the width of each element appropriately. That is,
	// scale its width relative to the width of the composite image
	if (IsFloatEqual(_width, 0.0f) == true) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "internal width was 0.0f when trying to re-size multiple image elements" << endl;
		return;
	}

	for (vector<ImageElement>::iterator i = _elements.begin(); i < _elements.end(); i++) {
		if (IsFloatEqual(i->image.GetWidth(), 0.0f) == false)
			i->image.SetWidth(width * (_width / i->image.GetWidth()));
	}
	_width = width;
}



void CompositeImage::SetHeight(float height) {
	// Case 1: No image elements loaded, just change the internal height
	if (_elements.empty() == true) {
		_height = height;
		return;
	}

	// Case 2: we only have one image element to change its height
	if (_elements.size() == 1) {
		_height = height;
		_elements[0].image.SetHeight(height);
		return;
	}

	// Case 3: We must set the height of each element appropriately. That is,
	// scale its height relative to the height of the composite image
	if (IsFloatEqual(_height, 0.0f) == true) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "internal height was 0.0f when trying to re-size multiple image elements" << endl;
		return;
	}

	for (vector<ImageElement>::iterator i = _elements.begin(); i < _elements.end(); i++) {
		if (IsFloatEqual(i->image.GetHeight(), 0.0f) == false)
			i->image.SetHeight(height * (_height / i->image.GetHeight()));
	}
	_height = height;
}



void CompositeImage::SetColor(const Color &color) {
	ImageDescriptor::SetColor(color);

	for (uint32 i = 0; i < _elements.size(); i++) {
		_elements[i].image.SetColor(color);
	}
}



void CompositeImage::SetVertexColors(const Color &tl, const Color &tr, const Color &bl, const Color &br) {
	ImageDescriptor::SetVertexColors(tl, tr, bl, br);

	for (uint32 i = 0; i < _elements.size(); i++) {
		_elements[i].image.SetVertexColors(tl, tr, bl, br);
	}
}



void CompositeImage::AddImage(const StillImage& img, float x_offset, float y_offset, float u1, float v1, float u2, float v2) {
	if (x_offset < 0.0f || y_offset < 0.0f) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "negative x or y offset passed to function" << endl;
		return;
	}

	_elements.push_back(ImageElement(img, x_offset, y_offset));

	StillImage& new_image = _elements.back().image;

	new_image.SetUVCoordinates(u1, v1, u2, v2);
	new_image.SetDimensions(img.GetWidth(), img.GetHeight());

	// Determine if the width or height of the composite image has grown from adding this new element
	float max_x = x_offset + new_image.GetWidth() * u2;
	if (max_x > _width)
		_width = max_x;

	float max_y = y_offset + new_image.GetHeight() * v2;
	if (max_y > _height)
		_height = max_y;
} // void CompositeImage::AddImage(const StillImage& img, float x_offset, float y_offset, float u1, float v1, float u2, float v2)



// void CompositeImage::ConstructCompositeImage(const std::vector<StillImage>& tiles, const std::vector<std::vector<uint32> >& indeces) {
// 	if (tiles.empty() == true || indeces.empty() == true) {
// 		IF_PRINT_WARNING(VIDEO_DEBUG) << "either the tiles or indeces vector function arguments were empty" << endl;
// 		return;
// 	}
//
// 	for (uint32 i = 1; i < tiles.size(); i++) {
// 		if (tiles[0]._width != tiles[i]._width || tiles[0]._height != tiles[i]._height) {
// 			IF_PRINT_WARNING(VIDEO_DEBUG) << "images within the tiles argument had unequal dimensions" << endl;
// 			return;
// 		}
// 	}
//
// 	for (uint32 i = 1; i < indeces.size(); i++) {
// 		if (indeces[0].size() != indeces[i].size()) {
// 			IF_PRINT_WARNING(VIDEO_DEBUG) << "the row sizes in the indices 2D vector argument did not match" << endl;
// 			return;
// 		}
// 	}
//
// 	Clear();
//
// 	// Set the members of the composite image that we are about to construct
// 	_width  = static_cast<float>(indeces[0].size()) * tiles[0]._width;
// 	_height = static_cast<float>(indeces.size()) * tiles[0]._height;
// 	_is_static = tiles[0]._is_static;
//
// 	// Add each tile at the image at the appropriate offset
// 	for (uint32 y = 0; y < indeces.size(); ++y) {
// 		for (uint32 x = 0; x < indeces[0].size(); ++x) {
// 			// NOTE: we did not check that all the entries in indeces were within the
// 			// correct range for the size of the tiles vector, so this may cause a out-of-bounds
// 			// run-time error.
// 			AddImage(tiles[indeces[y][x]], x * tiles[0]._width, y * tiles[0]._height);
// 		}
// 	}
// } // void CompositeImage::ConstructCompositeImage(const std::vector<StillImage>& tiles, const std::vector<std::vector<uint32> >& indeces)


}  // namespace hoa_video
