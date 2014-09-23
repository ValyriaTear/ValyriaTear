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
*** \file    image_base.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for image base classes
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "image_base.h"

#include "video.h"

using namespace vt_utils;

namespace vt_video
{

namespace private_video
{

// -----------------------------------------------------------------------------
// ImageMemory class
// -----------------------------------------------------------------------------

ImageMemory::ImageMemory() :
    width(0),
    height(0),
    pixels(NULL),
    rgb_format(false)
{}



ImageMemory::~ImageMemory()
{
// Winter Knight - I commented this out because it was causing double free
// segfaults when ImageMemory objects were copied via copy constructor.
    if(pixels != NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "pixels member was not NULL upon object destruction" << std::endl;
//		free(pixels);
//		pixels = NULL;
    }
}



bool ImageMemory::LoadImage(const std::string &filename)
{
    if(pixels != NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "pixels member was not NULL upon function invocation" << std::endl;
        free(pixels);
        pixels = NULL;
    }

    SDL_Surface* temp_surf = IMG_Load(filename.c_str());
    if(temp_surf == NULL) {
        PRINT_ERROR << "Couldn't load image file: " << filename << std::endl;
        return false;
    }

    SDL_Surface* alpha_surf = SDL_ConvertSurfaceFormat(temp_surf, SDL_PIXELFORMAT_ARGB8888, 0);

    // Tells whether the alpha image will be used
    bool alpha_format = true;
    if(alpha_surf == NULL) {
        // use the default image in that case
        alpha_surf = temp_surf;
        alpha_format = false;
    } else {
        SDL_FreeSurface(temp_surf);
        temp_surf = NULL;
    }

    // Now allocate the pixel values
    width = alpha_surf->w;
    height = alpha_surf->h;
    pixels = malloc(width * height * 4);
    rgb_format = false;

    // convert the data so that it works in our format
    uint8 *img_pixel = NULL;
    uint8 *dst_pixel = NULL;

    for(uint32 y = 0; y < height; ++y) {
        for(uint32 x = 0; x < width; ++x) {
            img_pixel = (uint8 *)alpha_surf->pixels + y * alpha_surf->pitch + x * alpha_surf->format->BytesPerPixel;
            dst_pixel = ((uint8 *)pixels) + ((y * width) + x) * 4;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            if(alpha_format) {
                dst_pixel[0] = img_pixel[0];
                dst_pixel[1] = img_pixel[1];
                dst_pixel[2] = img_pixel[2];
                dst_pixel[3] = img_pixel[3];
            } else {
                dst_pixel[2] = img_pixel[0];
                dst_pixel[1] = img_pixel[1];
                dst_pixel[0] = img_pixel[2];
                dst_pixel[3] = img_pixel[3];
            }
#else
            if(alpha_format) { // ARGB8888
#ifdef __APPLE__
                dst_pixel[3] = img_pixel[0];
                dst_pixel[0] = img_pixel[1];
                dst_pixel[1] = img_pixel[2];
                dst_pixel[2] = img_pixel[3];
#else
                dst_pixel[2] = img_pixel[0];
                dst_pixel[1] = img_pixel[1];
                dst_pixel[0] = img_pixel[2];
                dst_pixel[3] = img_pixel[3];
#endif
            } else {
                dst_pixel[0] = img_pixel[0];
                dst_pixel[1] = img_pixel[1];
                dst_pixel[2] = img_pixel[2];
                dst_pixel[3] = img_pixel[3];
            }
#endif
            // GL_LINEAR white artifact removal
            // Make the r,g,b values black to prevent OpenGL to make linear average with
            // another color when smoothing.
            // This is removing the white edges often seen on sprites.
            if(dst_pixel[3] == 0) {
                dst_pixel[0] = 0;
                dst_pixel[1] = 0;
                dst_pixel[2] = 0;
            }
        }
    }

    SDL_FreeSurface(alpha_surf);
    return true;
}

bool ImageMemory::SaveImage(const std::string &filename)
{
    if(pixels == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "pixels member was NULL upon function invocation for file: " << filename << std::endl;
        return false;
    }

    // open up the file for writing
    FILE *fp = fopen(filename.c_str(), "wb");

    if(fp == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "could not open file: " << filename << std::endl;
        return false;
    }

    // grab a write structure
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);

    if(!png_ptr) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "png_create_write_struct() failed for file: " << filename << std::endl;
        fclose(fp);
        return false;
    }

    // and a place to store the metadata
    png_infop info_ptr = png_create_info_struct(png_ptr);

    if(!info_ptr) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "png_create_info_struct() failed for file: " << filename << std::endl;
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        fclose(fp);
        return false;
    }

    // prepare for error handling!
    if(setjmp(png_jmpbuf(png_ptr))) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "setjmp returned non-zero for file: " << filename << std::endl;
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        fclose(fp);
        return false;
    }

    // tell it where to look
    png_init_io(png_ptr, fp);

    // write the header
    int32 color_type = rgb_format ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGBA;
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, color_type,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);


    png_write_info(png_ptr, info_ptr);

    png_set_packing(png_ptr);

    // get the row array from our data
    png_bytep *row_pointers = new png_bytep[height];
    if(!row_pointers) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "Couldn't allocate png row_pointers for: " << filename << std::endl;
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        fclose(fp);
        return false;
    }

    int32 bytes_per_row = rgb_format ? width * 3 : width * 4;
    for(uint32 i = 0; i < height; ++i) {
        row_pointers[i] = (png_bytep)pixels + bytes_per_row * i;
    }

    // tell it what the rows are
    png_set_rows(png_ptr, info_ptr, row_pointers);
    // and write the PNG
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_write_image(png_ptr, row_pointers);
    // clean up
    png_write_end(png_ptr, info_ptr);

    fclose(fp);

    // free the memory
    delete[] row_pointers;
    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);

    return true;
} // bool ImageMemory::SaveImage(const std::string& filename)

void ImageMemory::ConvertToGrayscale()
{
    if(width <= 0 || height <= 0) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "width and/or height members were invalid (<= 0)" << std::endl;
        return;
    }

    if(pixels == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "no image data (pixels == NULL)" << std::endl;
        return;
    }

    uint8 format_bytes = (rgb_format ? 3 : 4);
    uint8 *end_position = static_cast<uint8 *>(pixels) + (width * height * format_bytes);

    for(uint8 *i = static_cast<uint8 *>(pixels); i < end_position; i += format_bytes) {
        // Compute the grayscale value for this pixel based on RGB values: 0.30R + 0.59G + 0.11B
        uint8 value = static_cast<uint8>((30 * *(i) + 59 * *(i + 1) + 11 * *(i + 2)) * 0.01f);
        *i = value;
        *(i + 1) = value;
        *(i + 2) = value;
        // *(i + 3) for RGBA is the alpha value and is left unmodified
    }
}


void ImageMemory::RGBAToRGB()
{
    if(width <= 0 || height <= 0) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "width and/or height members were invalid (<= 0)" << std::endl;
        return;
    }

    if(pixels == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "no image data (pixels == NULL)" << std::endl;
        return;
    }

    if(rgb_format == true) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "image data was said to already be in RGB format" << std::endl;
        return;
    }

    uint8 *pixel_index = static_cast<uint8 *>(pixels);
    uint8 *pixel_source = pixel_index;

    for(uint32 i = 0; i < height * width; ++i, pixel_index += 4) {
        int32 index = 3 * i;
        pixel_source[index] = *pixel_index;
        pixel_source[index + 1] = *(pixel_index + 1);
        pixel_source[index + 2] = *(pixel_index + 2);
    }

    // Reduce the memory consumed by 1/4 since we no longer need to contain alpha data
    void *new_pixels = realloc(pixels, width * height * 3);
    if(new_pixels != NULL)
        pixels = new_pixels;
    rgb_format = true;
}



void ImageMemory::CopyFromTexture(TexSheet *texture)
{
    if(pixels != NULL)
        free(pixels);
    pixels = NULL;

    // Get the texture as a buffer
    height = texture->height;
    width = texture->width;
    pixels = malloc(height * width * (rgb_format ? 3 : 4));
    if(pixels == NULL) {
        PRINT_ERROR << "failed to malloc enough memory to copy the texture" << std::endl;
    }

    TextureManager->_BindTexture(texture->tex_id);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}



void ImageMemory::CopyFromImage(BaseTexture *img)
{
    // First copy the image's entire texture sheet to memory
    CopyFromTexture(img->texture_sheet);

    // Check that the image to copy is smaller than its texture sheet (usually true).
    // If so, then copy over only the sub-rectangle area of the image from its texture
    if(height > img->height || width > img->width) {
        uint8 format_bytes = (rgb_format ? 3 : 4);
        uint32 src_bytes = width * format_bytes;
        uint32 dst_bytes = img->width * format_bytes;
        uint32 src_offset = img->y * width * format_bytes + img->x * format_bytes;
        void *img_pixels = malloc(img->width * img->height * format_bytes);
        if(img_pixels == NULL) {
            PRINT_ERROR << "failed to malloc enough memory to copy the image" << std::endl;
            return;
        }

        for(uint32 i = 0; i < img->height; ++i) {
            memcpy((uint8 *)img_pixels + i * dst_bytes, (uint8 *)pixels + i * src_bytes + src_offset, dst_bytes);
        }

        // Delete the memory used for the texture sheet and replace it with the memory for the image
        if(pixels)
            free(pixels);

        height = img->height;
        width = img->width;
        pixels = img_pixels;
    }
}

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



BaseTexture::BaseTexture(uint32 width_, uint32 height_) :
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



BaseTexture::BaseTexture(TexSheet *texture_sheet_, uint32 width_, uint32 height_) :
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



BaseTexture::~BaseTexture()
{
    if(ref_count > 0) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "destructor invoked when the object had a reference count greater than zero: " << ref_count << std::endl;
    }
}



bool BaseTexture::RemoveReference()
{
    ref_count--;

    if(ref_count < 0) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "texture ref_count member is now negative: " << ref_count << std::endl;
        return true;
    } else if(ref_count == 0)
        return true;
    else
        return false;
}

// -----------------------------------------------------------------------------
// ImageTexture class
// -----------------------------------------------------------------------------

ImageTexture::ImageTexture(const std::string &filename_, const std::string &tags_, int32 width_, int32 height_) :
    BaseTexture(width_, height_),
    filename(filename_),
    tags(tags_)
{
    if(VIDEO_DEBUG) {
        if(TextureManager->_IsImageTextureRegistered(filename + tags))
            PRINT_WARNING << "constructor invoked when ImageTexture was already referenced for: " << filename << tags << std::endl;
    }

    TextureManager->_RegisterImageTexture(this);
}



ImageTexture::ImageTexture(TexSheet *texture_sheet_, const std::string &filename_, const std::string &tags_, int32 width_, int32 height_) :
    BaseTexture(texture_sheet_, width_, height_),
    filename(filename_),
    tags(tags_)
{
    if(VIDEO_DEBUG) {
        if(TextureManager->_IsImageTextureRegistered(filename + tags))
            PRINT_WARNING << "constructor invoked when ImageTexture was already referenced for: " << filename << tags << std::endl;
    }

    TextureManager->_RegisterImageTexture(this);
}



ImageTexture::~ImageTexture()
{
    // Remove this instance from the texture manager
    TextureManager->_UnregisterImageTexture(this);
}

} // namespace private_video

} // namespace vt_video
