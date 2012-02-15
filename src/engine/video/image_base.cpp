///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    image_base.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for image base classes
*** ***************************************************************************/

#include <cstdarg>
#include <math.h>

#include "image_base.h"
#include "video.h"

using namespace std;
using namespace hoa_utils;

namespace hoa_video {

namespace private_video {

// -----------------------------------------------------------------------------
// ImageMemory class
// -----------------------------------------------------------------------------

ImageMemory::ImageMemory() :
	width(0),
	height(0),
	pixels(NULL),
	rgb_format(false)
{}



ImageMemory::~ImageMemory() {
// Winter Knight - I commented this out because it was causing double free
// segfaults when ImageMemory objects were copied via copy constructor.
	if (pixels != NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "pixels member was not NULL upon object destruction" << endl;
//		free(pixels);
//		pixels = NULL;
	}
}



bool ImageMemory::LoadImage(const string& filename) {
	if (pixels != NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "pixels member was not NULL upon function invocation" << endl;
		free(pixels);
		pixels = NULL;
	}

	// Isolate the extension
	size_t ext_position = filename.rfind('.');

	if (ext_position == string::npos) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "could not decipher file extension for filename: " << filename << endl;
		return false;
	}

	string extension = string(filename, ext_position, filename.length() - ext_position);

	// NOTE: We could technically try uppercase forms of the file extension, or also include the .jpeg extension name,
	// but Allacrost's file standard states that only the .png and .jpg image file extensions are suppported.
	if (extension == ".png")
		return _LoadPngImage(filename);
	else if (extension == ".jpg")
		return _LoadJpgImage(filename);

	IF_PRINT_WARNING(VIDEO_DEBUG) << "unsupported file extension: \"" << extension << "\" for filename: " << filename << endl;
	return false;
}



bool ImageMemory::SaveImage(const string& filename, bool png_image) {
	if (pixels == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "pixels member was NULL upon function invocation for file: " << filename << endl;
		return false;
	}

	if (png_image) {
		return _SavePngImage(filename);
	}
	else {
		// JPG images don't have alpha information, so we must convert the data to RGB format first
		if (rgb_format == false)
			RGBAToRGB();
		return _SaveJpgImage(filename);
	}
}



void ImageMemory::ConvertToGrayscale() {
	if (width <= 0 || height <= 0) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "width and/or height members were invalid (<= 0)" << endl;
		return;
	}

	if (pixels == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "no image data (pixels == NULL)" << endl;
		return;
	}

	uint8 format_bytes = (rgb_format ? 3 : 4);
	uint8* end_position = static_cast<uint8*>(pixels) + (width * height * format_bytes);

	for (uint8* i = static_cast<uint8*>(pixels); i < end_position; i += format_bytes) {
		// Compute the grayscale value for this pixel based on RGB values: 0.30R + 0.59G + 0.11B
		uint8 value = static_cast<uint8>((30 * *(i) + 59 * *(i + 1) + 11 * *(i + 2)) * 0.01f);
		*i = value;
		*(i + 1) = value;
		*(i + 2) = value;
		// *(i + 3) for RGBA is the alpha value and is left unmodified
	}
}


void ImageMemory::RGBAToRGB() {
	if (width <= 0 || height <= 0) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "width and/or height members were invalid (<= 0)" << endl;
		return;
	}

	if (pixels == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "no image data (pixels == NULL)" << endl;
		return;
	}

	if (rgb_format == true) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "image data was said to already be in RGB format" << endl;
		return;
	}

	uint8* pixel_index = static_cast<uint8*>(pixels);
	uint8* pixel_source = pixel_index;

	for (int32 i = 0; i < height * width; i++, pixel_index += 4) {
		int32 index = 3 * i;
		pixel_source[index] = *pixel_index;
		pixel_source[index + 1] = *(pixel_index + 1);
		pixel_source[index + 2] = *(pixel_index + 2);
	}

	// Reduce the memory consumed by 1/4 since we no longer need to contain alpha data
	void* new_pixels = realloc(pixels, width * height * 3);
	if (new_pixels != NULL)
		pixels = new_pixels;
	rgb_format = true;
}



void ImageMemory::CopyFromTexture(TexSheet* texture) {
	if (pixels != NULL)
		free(pixels);
	pixels = NULL;

	// Get the texture as a buffer
	height = texture->height;
	width = texture->width;
	pixels = malloc(height * width * (rgb_format ? 3 : 4));
	if (pixels == NULL) {
		PRINT_ERROR << "failed to malloc enough memory to copy the texture" << endl;
	}

	TextureManager->_BindTexture(texture->tex_id);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}



void ImageMemory::CopyFromImage(BaseTexture* img) {
	// First copy the image's entire texture sheet to memory
	CopyFromTexture(img->texture_sheet);

	// Check that the image to copy is smaller than its texture sheet (usually true).
	// If so, then copy over only the sub-rectangle area of the image from its texture
	if (height > img->height || width > img->width) {
		uint8 format_bytes = (rgb_format ? 3 : 4);
		uint32 src_bytes = width * format_bytes;
		uint32 dst_bytes = img->width * format_bytes;
		uint32 src_offset = img->y * width * format_bytes + img->x * format_bytes;
		void* img_pixels = malloc(img->width * img->height * format_bytes);
		if (img_pixels == NULL) {
			PRINT_ERROR << "failed to malloc enough memory to copy the image" << endl;
			return;
		}

		for (int32 i = 0; i < img->height; i++) {
			memcpy((uint8*)img_pixels + i * dst_bytes, (uint8*)pixels + i * src_bytes + src_offset, dst_bytes);
		}

		// Delete the memory used for the texture sheet and replace it with the memory for the image
		if (pixels)
			free(pixels);

		height = img->height;
		width = img->width;
		pixels = img_pixels;
	}
}



bool ImageMemory::_LoadPngImage(const string& filename) {
	// open up the PNG
	FILE* fp = fopen(filename.c_str(), "rb");

	if (fp == NULL)
		return false;

	// check the signature to make sure it's a PNG
	uint8 test_buffer[8];

	fread(test_buffer, 1, 8, fp);
	if (png_sig_cmp(test_buffer, 0, 8)) {
		fclose(fp);
		return false;
	}

	// load the actual PNG
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);

	if (png_ptr == NULL) {
		fclose(fp);
		return false;
	}

	// get the info structure
	png_infop info_ptr = png_create_info_struct(png_ptr);

	if (info_ptr == NULL) {
		png_destroy_read_struct(&png_ptr, NULL, (png_infopp)NULL);
		fclose(fp);
		return false;
	}

	// error handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, NULL, (png_infopp)NULL);
		fclose(fp);
		return false;
	}

	// read the PNG
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	// and get an array of pointers, one for each row of the PNG
	uint8** row_pointers = png_get_rows(png_ptr, info_ptr);

	// copy metadata
	width = info_ptr->width;
	height = info_ptr->height;
	pixels = malloc(info_ptr->width * info_ptr->height * 4);

	// check that we were able to allocate enough memory for the PNG
	if (pixels == NULL) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		PRINT_ERROR << "failed to malloc sufficient memory for .png file: " << filename << endl;
		return false;
	}

	// convert the damn thing so that it works in our format
	// this is mostly just byteswapping and adding extra data - we want everything in four channels
	// for the moment, anyway
	uint32 bpp = info_ptr->channels;
	uint8* img_pixel = NULL;
	uint8* dst_pixel = NULL;

	if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE) {
		// colours come from a palette - for this colour type, we have to look up the colour from the palette
		png_color c;
		for (uint32 y = 0; y < info_ptr->height; y++) {
			for (uint32 x = 0; x < info_ptr->width; x++) {
				img_pixel = row_pointers[y] + (x * bpp);
				dst_pixel = ((uint8*)pixels) + ((y * info_ptr->width) + x) * 4;
				c = info_ptr->palette[img_pixel[0]];

				dst_pixel[0] = c.red;
				dst_pixel[1] = c.green;
				dst_pixel[2] = c.blue;
				dst_pixel[3] = 0xFF;
			}
		}
	}
	else if (bpp == 1) {
		for (uint32 y = 0; y < info_ptr->height; y++) {
			for (uint32 x = 0; x < info_ptr->width; x++) {
				img_pixel = row_pointers[y] + (x * bpp);
				dst_pixel = ((uint8*)pixels) + ((y * info_ptr->width) + x) * 4;
				dst_pixel[0] = img_pixel[0];
				dst_pixel[1] = img_pixel[0];
				dst_pixel[2] = img_pixel[0];
				dst_pixel[3] = 0xFF;
			}
		}
	}
	else if (bpp == 3) {
		for (uint32 y = 0; y < info_ptr->height; y++) {
			for (uint32 x = 0; x < info_ptr->width; x++) {
				img_pixel = row_pointers[y] + (x * bpp);
				dst_pixel = ((uint8*)pixels) + ((y * info_ptr->width) + x) * 4;
				dst_pixel[0] = img_pixel[0];
				dst_pixel[1] = img_pixel[1];
				dst_pixel[2] = img_pixel[2];
				dst_pixel[3] = 0xFF;
			}
		}
	}
	else if (bpp == 4) {
		for (uint32 y = 0; y < info_ptr->height; y++) {
			for (uint32 x = 0; x < info_ptr->width; x++) {
				img_pixel = row_pointers[y] + (x * bpp);
				dst_pixel = ((uint8*)pixels) + ((y * info_ptr->width) + x) * 4;
				dst_pixel[0] = img_pixel[0];
				dst_pixel[1] = img_pixel[1];
				dst_pixel[2] = img_pixel[2];
				dst_pixel[3] = img_pixel[3];
			}
		}
	}
	else {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		PRINT_ERROR << "failed to load .png file (bytes per pixel not supported): " << filename << endl;
		return false;
	}

	// clean everything up
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	fclose(fp);

	rgb_format = false;
	return true;
} // bool ImageMemory::_LoadPngImage(const string& filename)



bool ImageMemory::_LoadJpgImage(const string& filename) {
	// open the file
	FILE* fp;
	uint8** buffer;

	if ((fp = fopen(filename.c_str(), "rb")) == NULL)
		return false;

	// create the error-handing stuff and the main decompression object
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	// tell it where to read, and read the header
	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);

	// here we go, here we go, here we go!
	jpeg_start_decompress(&cinfo);

	// how much space does each row take up?
	JDIMENSION row_stride = cinfo.output_width * cinfo.output_components;

	// let's get us some MEMORY - but we use the jpeg library to do it so that it comes out of that memory pool
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	// metadata
	width = cinfo.output_width;
	height = cinfo.output_height;
	pixels = malloc(cinfo.output_width * cinfo.output_height * 3);

	// swizzle everything so it's in the format we want
	uint32 bpp = cinfo.output_components;
	uint8* img_pixel = NULL;
	uint8* dst_pixel = NULL;

	if (bpp == 3) {
		for (uint32 y = 0; y < cinfo.output_height; y++) {
			jpeg_read_scanlines(&cinfo, buffer, 1);

			for(uint32 x = 0; x < cinfo.output_width; x++) {
				img_pixel = buffer[0] + (x * bpp);
				dst_pixel = ((uint8 *)pixels) + ((y * cinfo.output_width) + x) * 3;

				dst_pixel[0] = img_pixel[0];
				dst_pixel[1] = img_pixel[1];
				dst_pixel[2] = img_pixel[2];
			}
		}
	}
	else if (bpp == 4) {
		for (uint32 y = 0; y < cinfo.output_height; y++) {
			jpeg_read_scanlines(&cinfo, buffer, 1);

			for (uint32 x = 0; x < cinfo.output_width; x++) {
				img_pixel = buffer[0] + (x * bpp);
				dst_pixel = ((uint8 *)pixels) + ((y * cinfo.output_width) + x) * 3;

				dst_pixel[0] = img_pixel[0];
				dst_pixel[1] = img_pixel[1];
				dst_pixel[2] = img_pixel[2];
			}
		}
	}
	else {
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		fclose(fp);
		PRINT_ERROR << "failed to load .jpg file (bytes per pixel not supported): " << filename << endl;
		return false;
	}

	// clean up
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(fp);
	rgb_format = true;
	return true;
} // bool ImageMemory::_LoadJpgImage(const string& filename)



bool ImageMemory::_SavePngImage(const std::string& filename) const {
	// open up the file for writing
	FILE* fp = fopen(filename.c_str(), "wb");

	if (fp == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "could not open file: " << filename << endl;
		return false;
	}

	// aah, RGB data! We can only handle RGBA at the moment
	if (rgb_format == true) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "attempting to save RGB format image data as a RGBA format PNG image" << endl;
	}

	// grab a write structure
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);

	if (!png_ptr) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "png_create_write_struct() failed for file: " << filename << endl;
		fclose(fp);
		return false;
	}

	// and a place to store the metadata
	png_infop info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "png_create_info_struct() failed for file: " << filename << endl;
		png_destroy_write_struct(&png_ptr, NULL);
		fclose(fp);
		return false;
	}

	// prepare for error handling!
	if (setjmp(png_jmpbuf(png_ptr))) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "setjmp returned non-zero for file: " << filename << endl;
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		return false;
	}

	// tell it where to look
	png_init_io(png_ptr, fp);

	// write the header
	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	// get the row array from our data
	png_byte** row_pointers = new png_byte*[height];
	int32 bytes_per_row = width * 4;
	for (int32 i = 0; i < height; i++) {
		row_pointers[i] = (png_byte*)pixels + bytes_per_row * i;
	}

	// tell it what the rows are
	png_set_rows(png_ptr, info_ptr, row_pointers);
	// and write the PNG
	png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
	png_write_image(png_ptr, row_pointers);
	// clean up
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);

	// free the memory we ate
	delete[] row_pointers;

	// peace and love for all
	return true;
} // bool ImageMemory::_SavePngImage(const std::string& filename) const



bool ImageMemory::_SaveJpgImage(const std::string& filename) const {
	FILE* fp = fopen(filename.c_str(), "wb");
	if (fp == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "could not open file: " << filename << endl;
		return false;
	}

	// we don't support RGBA because JPEGs don't support alpha
	if (rgb_format == false) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "attempting to save non-RGB format pixel data as a RGB format JPG image" << endl;
	}

	// compression object and error handling
	jpeg_compress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	// tell libjpeg how we do things in this town
	cinfo.in_color_space = JCS_RGB;
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;

	// everything else can be default
	jpeg_set_defaults(&cinfo);
	// tell it where to look
	jpeg_stdio_dest(&cinfo, fp);
	// compress it
	jpeg_start_compress(&cinfo, TRUE);

	JSAMPROW row_pointer; // A pointer to a single row
	uint32 row_stride = width * 3; // The physical row width in the buffer (RGB)

	// Note that the lines have to be stored from top to bottom
	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer = (uint8*)pixels + cinfo.next_scanline * row_stride;
	    jpeg_write_scanlines(&cinfo, &row_pointer, 1);
	}

	// compression is DONE, we are HAPPY
	// now finish it and clean up
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	fclose(fp);
	return true;
} // bool ImageMemory::_SaveJpgImage(const std::string& file_name) const

// -----------------------------------------------------------------------------
// BaseTexture class
// -----------------------------------------------------------------------------

BaseTexture::BaseTexture() :
	texture_sheet(NULL),
	width(0),
	height(0),
	x(0),
	y(0),
	u1(0.0f),
	v1(0.0f),
	u2(0.0f),
	v2(0.0f),
	smooth(false),
	ref_count(0)
{}



BaseTexture::BaseTexture(int32 width_, int32 height_) :
	texture_sheet(NULL),
	width(width_),
	height(height_),
	x(0),
	y(0),
	u1(0.0f),
	v1(0.0f),
	u2(0.0f),
	v2(0.0f),
	smooth(false),
	ref_count(0)
{}



BaseTexture::BaseTexture(TexSheet* texture_sheet_, int32 width_, int32 height_) :
	texture_sheet(texture_sheet_),
	width(width_),
	height(height_),
	x(0),
	y(0),
	u1(0.0f),
	v1(0.0f),
	u2(0.0f),
	v2(0.0f),
	smooth(false),
	ref_count(0)
{}



BaseTexture::~BaseTexture() {
	if (ref_count > 0) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "destructor invoked when the object had a reference count greater than zero: " << ref_count << endl;
	}
}



bool BaseTexture::RemoveReference() {
	ref_count--;

	if (ref_count < 0) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "texture ref_count member is now negative: " << ref_count << endl;
		return true;
	}
	else if (ref_count == 0)
		return true;
	else
		return false;
}

// -----------------------------------------------------------------------------
// ImageTexture class
// -----------------------------------------------------------------------------

ImageTexture::ImageTexture(const string& filename_, const string& tags_, int32 width_, int32 height_) :
	BaseTexture(width_, height_),
	filename(filename_),
	tags(tags_)
{
	if (VIDEO_DEBUG) {
		if (TextureManager->_IsImageTextureRegistered(filename + tags))
			PRINT_WARNING << "constructor invoked when ImageTexture was already referenced for: " << filename << tags << endl;
	}

	TextureManager->_RegisterImageTexture(this);
}



ImageTexture::ImageTexture(TexSheet* texture_sheet_, const string& filename_, const string& tags_, int32 width_, int32 height_) :
	BaseTexture(texture_sheet_, width_, height_),
	filename(filename_),
	tags(tags_)
{
	if (VIDEO_DEBUG) {
		if (TextureManager->_IsImageTextureRegistered(filename + tags))
			PRINT_WARNING << "constructor invoked when ImageTexture was already referenced for: " << filename << tags << endl;
	}

	TextureManager->_RegisterImageTexture(this);
}



ImageTexture::~ImageTexture() {
	// Remove this instance from the texture manager
	TextureManager->_UnregisterImageTexture(this);
}

} // namespace private_video

} // namespace hoa_video
