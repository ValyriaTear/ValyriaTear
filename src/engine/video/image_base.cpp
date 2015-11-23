///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
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
    _width(0),
    _height(0),
    _rgb_format(false)
{
}

ImageMemory::ImageMemory(const SDL_Surface* surface) :
    _width(surface->w),
    _height(surface->h),
    _rgb_format(surface->format->BytesPerPixel == 3)
{
    _pixels.reserve(_width * _height * GetBytesPerPixel());
    for(size_t i = 0; i < _width * _height * GetBytesPerPixel(); ++i) {
        _pixels.push_back(static_cast<const uint8_t*>(surface->pixels)[i]);
    }
}

void ImageMemory::Resize(size_t width, size_t height, bool is_rgb)
{
    _rgb_format = is_rgb;
    _width = width;
    _height = height;

    _pixels.clear();
    _pixels.resize(_width * _height * GetBytesPerPixel());
}

bool ImageMemory::LoadImage(const std::string& filename)
{
    assert(_pixels.empty());
    if (!_pixels.empty()) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "_pixels member was not empty upon function invocation" << std::endl;
    }

    SDL_Surface* temp_surf = IMG_Load(filename.c_str());
    if (temp_surf == nullptr) {
        PRINT_ERROR << "Couldn't load image file: " << filename << std::endl;
        return false;
    }

    SDL_Surface* alpha_surf = SDL_ConvertSurfaceFormat(temp_surf, SDL_PIXELFORMAT_ARGB8888, 0);

    // Tells whether the alpha image will be used
    bool alpha_format = true;
    if (alpha_surf == nullptr) {
        // use the default image in that case
        alpha_surf = temp_surf;
        temp_surf = nullptr;
        alpha_format = false;
    }

    // Now allocate the pixel values
    Resize(alpha_surf->w, alpha_surf->h, 3 == alpha_surf->format->BytesPerPixel);

    // convert the data so that it works in our format
    uint8_t* img_pixel = nullptr;
    uint8_t* dst_pixel = nullptr;

    for (uint32_t y = 0; y < _height; ++y) {
        for (uint32_t x = 0; x < _width; ++x) {
            img_pixel = static_cast<uint8_t *>(alpha_surf->pixels) + y * alpha_surf->pitch + x * alpha_surf->format->BytesPerPixel;
            dst_pixel = &_pixels[(y * _width + x) * GetBytesPerPixel()];
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            if (alpha_format) {
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
            if (alpha_format) { // ARGB8888
#ifdef __APPLE__
                dst_pixel[3] = img_pixel[3];
                dst_pixel[0] = img_pixel[2];
                dst_pixel[1] = img_pixel[1];
                dst_pixel[2] = img_pixel[0];
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
            if (dst_pixel[3] == 0) {
                dst_pixel[0] = 0;
                dst_pixel[1] = 0;
                dst_pixel[2] = 0;
            }
        }
    }

    if (temp_surf != nullptr) {
        SDL_FreeSurface(temp_surf);
        temp_surf = nullptr;
    }

    if (alpha_surf != nullptr) {
        SDL_FreeSurface(alpha_surf);
        alpha_surf = nullptr;
    }

    return true;
}

bool ImageMemory::SaveImage(const std::string& filename)
{
    assert(!_pixels.empty());
    if (_pixels.empty()) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "The _pixels member was empty upon function invocation for file: " << filename << std::endl;
        return false;
    }

    // Define all variables which require clean up.
    FILE* fp = nullptr;
    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;
    png_bytep* row_pointers = nullptr;

    // Define a clean up function.
    auto CleanUp = [&]()
    {
        if (row_pointers != nullptr) {
            delete[] row_pointers;
            row_pointers = nullptr;
        }

        // Assuming an 'info_ptr' without a 'png_ptr' is impossible by design.
        if (png_ptr == nullptr) {
            assert(info_ptr == nullptr);
        }

        if (png_ptr != nullptr && info_ptr != nullptr) {
            png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
        }

        if (png_ptr != nullptr) {
            // Note: A second parameter of 'nullptr' is valid for this function.
            png_destroy_write_struct(&png_ptr, &info_ptr);
            png_ptr = nullptr;
            info_ptr = nullptr;
        }

        if (fp != nullptr) {
            fclose(fp);
            fp = nullptr;
        }
    };

    // Open a file for writing.
    assert(fp == nullptr);
    fp = fopen(filename.c_str(), "wb");
    if (fp == nullptr) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "Could not open file: " << filename << std::endl;
        CleanUp();
        return false;
    }

    // Create a write structure.
    assert(png_ptr == nullptr);
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "png_create_write_struct() failed for file: " << filename << std::endl;
        CleanUp();
        return false;
    }

    // Create a place to store meta data.
    assert(info_ptr == nullptr);
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "png_create_info_struct() failed for file: " << filename << std::endl;
        CleanUp();
        return false;
    }

    // Setup error handling.
    if (setjmp(png_jmpbuf(png_ptr))) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "setjmp returned non-zero for file: " << filename << std::endl;
        CleanUp();
        return false;
    }

    // Set the file pointer.
    png_init_io(png_ptr, fp);

    // Write the header.
    int32_t color_type = _rgb_format ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGBA;
    png_set_IHDR(png_ptr, info_ptr, _width, _height, 8, color_type,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);


    png_write_info(png_ptr, info_ptr);
    png_set_packing(png_ptr);

    // Create the row array.
    assert(row_pointers == nullptr);
    row_pointers = new png_bytep[_height];
    if (row_pointers == nullptr) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "Couldn't allocate row_pointers for: " << filename << std::endl;
        CleanUp();
        return false;
    }

    // Initialize the row array.
    int32_t bytes_per_row = _width * GetBytesPerPixel();
    for (uint32_t i = 0; i < _height; ++i) {
        row_pointers[i] = static_cast<png_bytep>(&_pixels[bytes_per_row * i]);
    }

    // Define the rows.
    png_set_rows(png_ptr, info_ptr, row_pointers);

    // Write the image.
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);
    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, info_ptr);

    CleanUp();

    return true;
}

void ImageMemory::ConvertToGrayscale()
{
    if(_pixels.empty()) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "No image data (_pixels is empty)" << std::endl;
        return;
    }

    uint8_t format_bytes = GetBytesPerPixel();
    uint8_t *end_position = &_pixels[_width * _height * format_bytes];

    for(uint8_t *i = &_pixels[0]; i < end_position; i += format_bytes) {
        // Compute the grayscale value for this pixel based on RGB values: 0.30R + 0.59G + 0.11B
        uint8_t value = static_cast<uint8_t>((30 * *(i) + 59 * *(i + 1) + 11 * *(i + 2)) * 0.01f);
        *i = value;
        *(i + 1) = value;
        *(i + 2) = value;
        // *(i + 3) for RGBA is the alpha value and is left unmodified
    }
}

void ImageMemory::RGBAToRGB()
{
    if(_pixels.empty()) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "No image data (pixels is empty)" << std::endl;
        return;
    }

    if(_rgb_format) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "Image data was said to already be in RGB format" << std::endl;
        return;
    }

    uint8_t* pixel_index = &_pixels[0];
    uint8_t* pixel_source = pixel_index;

    for(uint32_t i = 0; i < _height * _width; ++i, pixel_index += 4) {
        int32_t index = 3 * i;
        pixel_source[index] = *pixel_index;
        pixel_source[index + 1] = *(pixel_index + 1);
        pixel_source[index + 2] = *(pixel_index + 2);
    }

    // Reduce the memory consumed by 1/4 since we no longer need to contain alpha data
    _pixels.resize(_width * _height * GetBytesPerPixel());
    std::vector<uint8_t> new_pixels(_pixels);
    std::swap(_pixels, new_pixels);
    _rgb_format = true;
}

void ImageMemory::CopyFromTexture(TexSheet *texture)
{
    assert(texture != nullptr);

    Resize(texture->width, texture->height, false);

    if (_pixels.empty()) {
        PRINT_ERROR << "Failed to malloc enough memory to copy the texture." << std::endl;
    }

    TextureManager->_BindTexture(texture->tex_id);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &_pixels[0]);
}

void ImageMemory::CopyFromImage(BaseTexture *img)
{
    assert(img != nullptr);

    // First copy the image's entire texture sheet to memory
    CopyFromTexture(img->texture_sheet);

    // Check that the image to copy is smaller than its texture sheet (usually true).
    // If so, then copy over only the sub-rectangle area of the image from its texture.
    if (_height <= img->height && _width <= img->width)
        return;

    uint32_t src_bytes = _width * GetBytesPerPixel();
    uint32_t dst_bytes = img->width * GetBytesPerPixel();
    uint32_t src_offset = (img->y * _width + img->x) * GetBytesPerPixel();

    std::vector<uint8_t> img_pixels;
    try {
        img_pixels.reserve(img->width * img->height * GetBytesPerPixel());
    }
    catch (std::exception&) {
        PRINT_ERROR << "Failed to malloc enough memory to copy the image" << std::endl;
        return;
    }

    for (uint32_t i = 0; i < img->height; ++i) {
        std::vector<uint8_t>::const_iterator start = _pixels.begin() + i * src_bytes + src_offset;
        std::vector<uint8_t>::const_iterator end = start + dst_bytes;
        img_pixels.insert(img_pixels.end(), start, end);
    }

    _height = img->height;
    _width = img->width;

    std::swap(_pixels, img_pixels);
}

void ImageMemory::CopyFrom(const ImageMemory& src, 
                           uint32_t src_offset,
                           uint32_t dst_bytes,
                           uint32_t dst_offset)
{
    size_t src_bytes = src.GetWidth() * GetBytesPerPixel();
    dst_bytes *= GetBytesPerPixel();
    src_offset *= GetBytesPerPixel();
    dst_offset *= GetBytesPerPixel();
    uint32_t bytes = _width * GetBytesPerPixel();

    for(size_t line = 0; line < _height; ++line) {
        memcpy(&_pixels[0] + line * dst_bytes + dst_offset,
               &src._pixels[0] + line * src_bytes + src_offset,
               bytes);
    }
}

void ImageMemory::CopyFrom(const ImageMemory& src, uint32_t src_offset)
{
    size_t src_bytes = src.GetWidth() * GetBytesPerPixel();
    size_t dst_bytes = GetWidth() * GetBytesPerPixel();
    src_offset *= GetBytesPerPixel();
    uint32_t bytes = _width * GetBytesPerPixel();

    for(size_t line = 0; line < _height; ++line) {
        memcpy(&_pixels[0] + line * dst_bytes,
               &src._pixels[0] + line * src_bytes + src_offset,
               bytes);
    }
}

void ImageMemory::GlGetTexImage()
{
    glGetTexImage(GL_TEXTURE_2D, 0, _rgb_format ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, &_pixels[0]);
}

void ImageMemory::GlTexSubImage(int32_t x, int32_t y)
{
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, _width, _height,
                    _rgb_format ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, &_pixels[0]);
}

void ImageMemory::GlReadPixels(int32_t x, int32_t y)
{
    glReadPixels(x, y, _width, _height,
                 _rgb_format ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, &_pixels[0]);
}

void ImageMemory::VerticalFlip()
{
    std::vector<uint8_t> flipped;
    flipped.reserve(_pixels.size());

    for (uint32_t i = 1; i <= _height; ++i) {
        std::vector<uint8_t>::const_iterator start = _pixels.end() - (i * _width * GetBytesPerPixel());
        std::vector<uint8_t>::const_iterator end = start + _width * GetBytesPerPixel();
        flipped.insert(flipped.end(), start, end);
    }
    std::swap(flipped, _pixels);
}

// -----------------------------------------------------------------------------
// BaseTexture class
// -----------------------------------------------------------------------------

BaseTexture::BaseTexture() :
    texture_sheet(nullptr),
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



BaseTexture::BaseTexture(uint32_t width_, uint32_t height_) :
    texture_sheet(nullptr),
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



BaseTexture::BaseTexture(TexSheet *texture_sheet_, uint32_t width_, uint32_t height_) :
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

ImageTexture::ImageTexture(const std::string &filename_, const std::string &tags_, int32_t width_, int32_t height_) :
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



ImageTexture::ImageTexture(TexSheet *texture_sheet_, const std::string &filename_, const std::string &tags_, int32_t width_, int32_t height_) :
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
