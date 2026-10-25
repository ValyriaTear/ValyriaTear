// Exercise the production SDL-surface copy with a checked upload stub, or with
// an actual OpenGL texture/readback when IMAGE_MEMORY_TEST_OPENGL is defined.
#include <SDL.h>
#include <SDL_ttf.h>
#include "engine/video/image_base.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
std::vector<uint8_t> uploaded;
#ifndef IMAGE_MEMORY_TEST_OPENGL
GLenum uploaded_format;
int uploaded_width, uploaded_height;
GLint unpack_alignment = 4;
int upload_count;
#endif

void CheckSurface(SDL_Surface* surface, const char* name)
{
    if (!surface)
        throw std::runtime_error(SDL_GetError());
    const size_t row_bytes = surface->w * surface->format->BytesPerPixel;
    std::vector<uint8_t> expected;
    for (int y = 0; row_bytes != 0 && y < surface->h; ++y) {
        const uint8_t* row = static_cast<const uint8_t*>(surface->pixels) + y * surface->pitch;
        expected.insert(expected.end(), row, row + row_bytes);
    }
    std::vector<uint8_t> original;
    if (surface->h * surface->pitch != 0) {
        const uint8_t* pixels = static_cast<const uint8_t*>(surface->pixels);
        original.assign(pixels, pixels + surface->h * surface->pitch);
    }
    vt_video::private_video::ImageMemory image(surface);
    if (image.GetWidth() != static_cast<size_t>(surface->w) ||
        image.GetHeight() != static_cast<size_t>(surface->h) ||
        image.GetBytesPerPixel() != surface->format->BytesPerPixel)
        throw std::runtime_error(std::string(name) + ": image dimensions or format differ");

    for (GLint alignment : {4, 8, 2, 1}) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        uploaded.clear();
#ifdef IMAGE_MEMORY_TEST_OPENGL
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surface->w ? surface->w : 1,
                     surface->h ? surface->h : 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
#else
        upload_count = 0;
#endif
        image.GlTexSubImage(0, 0);
        GLint restored_alignment;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &restored_alignment);
        if (restored_alignment != alignment)
            throw std::runtime_error(std::string(name) + ": unpack alignment was not restored");
#ifdef IMAGE_MEMORY_TEST_OPENGL
        if (!expected.empty()) {
            uploaded.resize(expected.size());
            GLint pack_alignment;
            glGetIntegerv(GL_PACK_ALIGNMENT, &pack_alignment);
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glGetTexImage(GL_TEXTURE_2D, 0, image.GetBytesPerPixel() == 3 ? GL_RGB : GL_RGBA,
                          GL_UNSIGNED_BYTE, uploaded.data());
            glPixelStorei(GL_PACK_ALIGNMENT, pack_alignment);
        }
        glDeleteTextures(1, &texture);
        if (glGetError() != GL_NO_ERROR)
            throw std::runtime_error(std::string(name) + ": OpenGL error");
#else
        if (expected.empty()) {
            if (upload_count != 0)
                throw std::runtime_error(std::string(name) + ": empty image attempted an upload");
        } else if (upload_count != 1 || uploaded_width != surface->w ||
                   uploaded_height != surface->h ||
                   uploaded_format != (surface->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA)) {
            throw std::runtime_error(std::string(name) + ": upload dimensions or format differ");
        }
#endif
        if (uploaded != expected)
            throw std::runtime_error(std::string(name) + ": pixel bytes differ at unpack alignment " + std::to_string(alignment));
    }
    if (!original.empty() && std::memcmp(original.data(), surface->pixels, original.size()) != 0)
        throw std::runtime_error(std::string(name) + ": input surface changed");
    std::cout << name << ": " << surface->w << 'x' << surface->h
              << " bpp=" << unsigned(surface->format->BytesPerPixel)
              << " pitch=" << surface->pitch << " row=" << row_bytes
              << " unpack=4,8,2,1 PASS\n";
}

void CheckSynthetic(int width, int height, int pitch, Uint32 format, const char* name)
{
    std::vector<uint8_t> pixels(height * pitch, 0xED);
    const int bpp = SDL_BYTESPERPIXEL(format);
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width * bpp; ++x)
            pixels[y * pitch + x] = static_cast<uint8_t>((y * 41 + x * 17) % 251);
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels.data(), width, height,
                                                             bpp * 8, pitch, format);
    CheckSurface(surface, name);
    SDL_FreeSurface(surface);
}
}

#ifndef IMAGE_MEMORY_TEST_OPENGL
extern "C" void GLAPIENTRY glGetIntegerv(GLenum name, GLint* value)
{
    if (name != GL_UNPACK_ALIGNMENT)
        throw std::runtime_error("unexpected pixel-store query");
    *value = unpack_alignment;
}

extern "C" void GLAPIENTRY glPixelStorei(GLenum name, GLint value)
{
    if (name != GL_UNPACK_ALIGNMENT || (value != 1 && value != 2 && value != 4 && value != 8))
        throw std::runtime_error("unexpected pixel-store setting");
    unpack_alignment = value;
}

extern "C" void GLAPIENTRY glTexSubImage2D(GLenum, GLint, GLint, GLint, GLsizei width,
                                           GLsizei height, GLenum format, GLenum type,
                                           const void* pixels)
{
    if (type != GL_UNSIGNED_BYTE)
        throw std::runtime_error("unexpected upload type");
    uploaded_width = width;
    uploaded_height = height;
    uploaded_format = format;
    ++upload_count;
    const size_t row_bytes = width * (format == GL_RGB ? 3 : 4);
    if (row_bytes % unpack_alignment != 0)
        throw std::runtime_error("packed upload has padded OpenGL row stride");
    const uint8_t* bytes = static_cast<const uint8_t*>(pixels);
    uploaded.assign(bytes, bytes + width * height * (format == GL_RGB ? 3 : 4));
}
#endif

int main(int argc, char** argv)
{
    try {
#ifdef IMAGE_MEMORY_TEST_OPENGL
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
            throw std::runtime_error(SDL_GetError());
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_Window* window = SDL_CreateWindow("ImageMemory regression", 0, 0, 32, 32,
                                              SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        if (!window)
            throw std::runtime_error(SDL_GetError());
        SDL_GLContext context = SDL_GL_CreateContext(window);
        if (!context)
            throw std::runtime_error(SDL_GetError());
        std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER)
                  << "; version: " << glGetString(GL_VERSION) << '\n';
#endif
        const bool empty_only = argc == 2 && std::strcmp(argv[1], "--empty-only") == 0;
        const bool rgb_only = argc == 2 && std::strcmp(argv[1], "--rgb-only") == 0;
        if (!empty_only && !rgb_only) {
            CheckSynthetic(3, 5, 32, SDL_PIXELFORMAT_RGBA32, "padded RGBA distinct channels and alpha");
            CheckSynthetic(4, 3, 16, SDL_PIXELFORMAT_RGBA32, "tight RGBA");
        }
        if (!empty_only) {
            CheckSynthetic(3, 5, 12, SDL_PIXELFORMAT_RGB24, "padded RGB");
            CheckSynthetic(4, 3, 12, SDL_PIXELFORMAT_RGB24, "tight RGB");
        }
        for (int dimension = 0; !rgb_only && dimension < 3; ++dimension) {
            SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, dimension == 1 ? 3 : 0,
                                                                 dimension == 0 ? 5 : 0,
                                                                 32, SDL_PIXELFORMAT_RGBA32);
            CheckSurface(surface, "empty RGBA");
            SDL_FreeSurface(surface);
        }
        if (TTF_Init() != 0)
            throw std::runtime_error(TTF_GetError());
        const Uint16 samples[][32] = {{'i', 'l', '1', 0}, {'W', 'W', 'M', ' ', '8', '8', 0},
                                     {'D', 'E', 'U', 'T', 'S', 'C', 'H', 0},
                                     {'R', 0x00e9, 's', 'u', 'm', 0x00e9, ' ', 0x03a9, 0}};
        for (int f = 1; !empty_only && !rgb_only && f < argc; ++f) {
            for (int size : {18, 22, 30}) {
                TTF_Font* font = TTF_OpenFont(argv[f], size);
                if (!font)
                    throw std::runtime_error(TTF_GetError());
                for (const auto& sample : samples) {
                    SDL_Surface* surface = TTF_RenderUNICODE_Blended(font, sample, SDL_Color{255, 255, 255, 255});
                    CheckSurface(surface, argv[f]);
                    SDL_FreeSurface(surface);
                }
                TTF_CloseFont(font);
            }
        }
        TTF_Quit();
#ifdef IMAGE_MEMORY_TEST_OPENGL
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
#endif
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
