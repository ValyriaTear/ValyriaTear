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
*** \file    video.cpp
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for video engine interface.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "engine/video/video.h"

#include "engine/mode_manager.h"
#include "engine/script/script_read.h"
#include "engine/system.h"

#include "utils/utils_strings.h"

using namespace vt_utils;
using namespace vt_video::private_video;

namespace vt_video
{

VideoEngine *VideoManager = NULL;
bool VIDEO_DEBUG = false;

//-----------------------------------------------------------------------------
// Static variable for the Color class
//-----------------------------------------------------------------------------

const Color Color::clear(0.0f, 0.0f, 0.0f, 0.0f);
const Color Color::white(1.0f, 1.0f, 1.0f, 1.0f);
const Color Color::gray(0.5f, 0.5f, 0.5f, 1.0f);
const Color Color::black(0.0f, 0.0f, 0.0f, 1.0f);
const Color Color::red(1.0f, 0.0f, 0.0f, 1.0f);
const Color Color::orange(1.0f, 0.4f, 0.0f, 1.0f);
const Color Color::yellow(1.0f, 1.0f, 0.0f, 1.0f);
const Color Color::green(0.0f, 1.0f, 0.0f, 1.0f);
const Color Color::aqua(0.0f, 1.0f, 1.0f, 1.0f);
const Color Color::blue(0.0f, 0.0f, 1.0f, 1.0f);
const Color Color::violet(0.0f, 0.0f, 1.0f, 1.0f);
const Color Color::brown(0.6f, 0.3f, 0.1f, 1.0f);

void RotatePoint(float &x, float &y, float angle)
{
    float original_x = x;
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);

    x = x * cos_angle - y * sin_angle;
    y = y * cos_angle + original_x * sin_angle;
}


//-----------------------------------------------------------------------------
// VideoEngine class
//-----------------------------------------------------------------------------

VideoEngine::VideoEngine():
    _fps_display(false),
    _fps_sum(0),
    _current_sample(0),
    _number_samples(0),
    _FPS_textimage(NULL),
    _gl_error_code(GL_NO_ERROR),
    _gl_blend_is_active(false),
    _gl_texture_2d_is_active(false),
    _gl_alpha_test_is_active(false),
    _gl_stencil_test_is_active(false),
    _gl_scissor_test_is_active(false),
    _gl_vertex_array_is_activated(false),
    _gl_color_array_is_activated(false),
    _gl_texture_coord_array_is_activated(false),
    _viewport_x_offset(0),
    _viewport_y_offset(0),
    _viewport_width(0),
    _viewport_height(0),
    _screen_width(0),
    _screen_height(0),
    _fullscreen(false),
    _x_cursor(0),
    _y_cursor(0),
    _debug_info(false),
    _x_shake(0),
    _y_shake(0),
    _brightness_value(1.0f),
    _temp_fullscreen(false),
    _temp_width(0),
    _temp_height(0),
    _smooth_pixel_art(true),
    _transformed_vertex_array_ptr(NULL),
    _vertex_array_ptr(NULL),
    _vertex_array_stride(0),
    _vertex_array_size(0),
    _initialized(false)
{
    _current_context.blend = 0;
    _current_context.x_align = -1;
    _current_context.y_align = -1;
    _current_context.x_flip = 0;
    _current_context.y_flip = 0;
    _current_context.coordinate_system = CoordSys(0.0f, VIDEO_STANDARD_RES_WIDTH,
                                         0.0f, VIDEO_STANDARD_RES_HEIGHT);
    _current_context.viewport = ScreenRect(0, 0, VIDEO_STANDARD_RES_WIDTH, VIDEO_STANDARD_RES_HEIGHT);
    _current_context.scissor_rectangle = ScreenRect(0, 0, VIDEO_STANDARD_RES_WIDTH,
                                         VIDEO_STANDARD_RES_HEIGHT);
    _current_context.scissoring_enabled = false;

    _transform_stack.push(Transform2D());

    for(uint32 sample = 0; sample < FPS_SAMPLES; sample++)
        _fps_samples[sample] = 0;
}

void VideoEngine::_UpdateFPS()
{
    if(!_fps_display)
        return;

    // We only create the text image when needed, to permit getting the text style correctly.
    if (!_FPS_textimage)
        _FPS_textimage = new TextImage("FPS: ", TextStyle("text20", Color::white));

    //! \brief Maximum milliseconds that the current frame time and our averaged frame time must vary
    //! before we begin trying to catch up
    const uint32 MAX_FTIME_DIFF = 5;

    //! \brief The number of samples to take if we need to play catchup with the current FPS
    const uint32 FPS_CATCHUP = 20;

    uint32 frame_time = vt_system::SystemManager->GetUpdateTime();

    // Calculate the FPS for the current frame
    uint32 current_fps = 1000;
    if(frame_time)
        current_fps /= frame_time;

    // The number of times to insert the current FPS sample into the fps_samples array
    uint32 number_insertions;

    if(_number_samples == 0) {
        // If the FPS display is uninitialized, set the entire FPS array to the current FPS
        _number_samples = FPS_SAMPLES;
        number_insertions = FPS_SAMPLES;
    } else if(current_fps >= 500) {
        // If the game is going at 500 fps or faster, 1 insertion is enough
        number_insertions = 1;
    } else {
        // Find if there's a discrepancy between the current frame time and the averaged one.
        // If there's a large difference, add extra samples so the FPS display "catches up" more quickly.
        float avg_frame_time = 1000.0f * FPS_SAMPLES / _fps_sum;
        int32 time_difference = static_cast<int32>(avg_frame_time) - static_cast<int32>(frame_time);

        if(time_difference < 0)
            time_difference = -time_difference;

        if(time_difference <= static_cast<int32>(MAX_FTIME_DIFF))
            number_insertions = 1;
        else
            number_insertions = FPS_CATCHUP; // Take more samples to catch up to the current FPS
    }

    // Insert the current_fps samples into the fps_samples array for the number of times specified
    for(uint32 j = 0; j < number_insertions; j++) {
        _fps_sum -= _fps_samples[_current_sample];
        _fps_sum += current_fps;
        _fps_samples[_current_sample] = current_fps;
        _current_sample = (_current_sample + 1) % FPS_SAMPLES;
    }

    uint32 avg_fps = _fps_sum / FPS_SAMPLES;

    // The text to display to the screen
    _FPS_textimage->SetText("FPS: " + NumberToString(avg_fps));
}

void VideoEngine::_DrawFPS()
{
    if(!_fps_display || !_FPS_textimage)
        return;

    PushState();
    SetStandardCoordSys();
    SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_X_NOFLIP, VIDEO_Y_NOFLIP, VIDEO_BLEND, 0);
    Move(930.0f, 40.0f); // Upper right hand corner of the screen
    _FPS_textimage->Draw();
    PopState();
} // void GUISystem::_DrawFPS()

VideoEngine::~VideoEngine()
{
    TextManager->SingletonDestroy();

    _default_menu_cursor.Clear();
    _rectangle_image.Clear();
    delete _FPS_textimage;

    TextureManager->SingletonDestroy();
}

bool VideoEngine::SingletonInitialize()
{
    // check to see if the singleton is already initialized
    if(_initialized)
        return true;

    if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
        PRINT_ERROR << "SDL video initialization failed" << std::endl;
        return false;
    }



    return true;
} // bool VideoEngine::SingletonInitialize()

bool VideoEngine::FinalizeInitialization()
{
    // Create instances of the various sub-systems
    TextureManager = TextureController::SingletonCreate();
    TextManager = TextSupervisor::SingletonCreate();

    // Initialize all sub-systems
    if(TextureManager->SingletonInitialize() == false) {
        PRINT_ERROR << "could not initialize texture manager" << std::endl;
        return false;
    }

    if(TextManager->SingletonInitialize() == false) {
        PRINT_ERROR << "could not initialize text manager" << std::endl;
        return false;
    }

    // Prepare the screen for rendering
    Clear();

    // Empty image used to draw colored rectangles.
    if(_rectangle_image.Load("") == false) {
        PRINT_ERROR << "_rectangle_image could not be created" << std::endl;
        return false;
    }

    _initialized = true;
    return true;
}

void VideoEngine::SetInitialResolution(int32 width, int32 height)
{
    // Get the current system color depth and resolution
    const SDL_VideoInfo *video_info(0);
    video_info = SDL_GetVideoInfo();

    if(video_info) {
        // Set the resolution to be the highest possible (lower than the user one)
        if(video_info->current_w >= width && video_info->current_h >= height) {
            SetResolution(width, height);
        } else if(video_info->current_w >= 1024 && video_info->current_h >= 768) {
            SetResolution(1024, 768);
        } else if(video_info->current_w >= 800 && video_info->current_h >= 600) {
            SetResolution(800, 600);
        } else {
            SetResolution(640, 480);
        }
    } else {
        // Default resoltion if we could not retrieve the resolution of the user
        SetResolution(width, height);
    }
}

//-----------------------------------------------------------------------------
// VideoEngine class - General methods
//-----------------------------------------------------------------------------

void VideoEngine::SetDrawFlags(int32 first_flag, ...)
{
    int32 flag = first_flag;
    va_list args;

    va_start(args, first_flag);
    while(flag != 0) {
        switch(flag) {
        case VIDEO_X_LEFT:
            _current_context.x_align = -1;
            break;
        case VIDEO_X_CENTER:
            _current_context.x_align = 0;
            break;
        case VIDEO_X_RIGHT:
            _current_context.x_align = 1;
            break;

        case VIDEO_Y_TOP:
            _current_context.y_align = 1;
            break;
        case VIDEO_Y_CENTER:
            _current_context.y_align = 0;
            break;
        case VIDEO_Y_BOTTOM:
            _current_context.y_align = -1;
            break;

        case VIDEO_X_NOFLIP:
            _current_context.x_flip = 0;
            break;
        case VIDEO_X_FLIP:
            _current_context.x_flip = 1;
            break;

        case VIDEO_Y_NOFLIP:
            _current_context.y_flip = 0;
            break;
        case VIDEO_Y_FLIP:
            _current_context.y_flip = 1;
            break;

        case VIDEO_NO_BLEND:
            _current_context.blend = 0;
            break;
        case VIDEO_BLEND:
            _current_context.blend = 1;
            break;
        case VIDEO_BLEND_ADD:
            _current_context.blend = 2;
            break;

        default:
            IF_PRINT_WARNING(VIDEO_DEBUG) << "Unknown flag in argument list: " << flag << std::endl;
            break;
        }
        flag = va_arg(args, int32);
    }
    va_end(args);
}



void VideoEngine::Clear()
{
    //! \todo glClearColor is a state change operation. It should only be called when the clear color changes
    Clear(Color::black);
}



void VideoEngine::Clear(const Color& c)
{
    _current_context.viewport = ScreenRect(_viewport_x_offset, _viewport_y_offset, _viewport_width, _viewport_height);
    glViewport(_viewport_x_offset, _viewport_y_offset, _viewport_width, _viewport_height);
    glClearColor(c[0], c[1], c[2], c[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    TextureManager->_debug_num_tex_switches = 0;
}


void VideoEngine::Update()
{
    uint32 frame_time = vt_system::SystemManager->GetUpdateTime();

    _screen_fader.Update(frame_time);

    if (_fps_display)
        _UpdateFPS();
}

void VideoEngine::DrawDebugInfo()
{
    if(TextureManager->debug_current_sheet >= 0)
        TextureManager->DEBUG_ShowTexSheet();

    if (_fps_display)
        _DrawFPS();
} // void VideoEngine::Draw()

bool VideoEngine::CheckGLError() {
    if(!VIDEO_DEBUG)
        return false;

    _gl_error_code = glGetError();
    return (_gl_error_code != GL_NO_ERROR);
}

const std::string VideoEngine::CreateGLErrorString()
{
    const GLubyte *error_string = gluErrorString(_gl_error_code);

    if(error_string == NULL)
        return ("Unknown GL error code: " + NumberToString(_gl_error_code));
    else
        return (char *)error_string;
}

//-----------------------------------------------------------------------------
// VideoEngine class - Screen size and resolution methods
//-----------------------------------------------------------------------------

void VideoEngine::GetPixelSize(float &x, float &y)
{
    x = fabs(_current_context.coordinate_system.GetRight() - _current_context.coordinate_system.GetLeft()) / _viewport_width;
    y = fabs(_current_context.coordinate_system.GetTop() - _current_context.coordinate_system.GetBottom()) / _viewport_height;
}



bool VideoEngine::ApplySettings()
{
    // Losing GL context, so unload images first
    if(!TextureManager || !TextureManager->UnloadTextures())
        IF_PRINT_WARNING(VIDEO_DEBUG) << "failed to delete OpenGL textures during a context change" << std::endl;

    int32 flags = SDL_OPENGL;

    if(_temp_fullscreen)
        flags |= SDL_FULLSCREEN;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 2);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

    if(SDL_SetVideoMode(_temp_width, _temp_height, 0, flags) == false) {
        // RGB values of 1 for each and 8 for depth seemed to be sufficient.
        // 565 and 16 here because it works with them on this computer.
        // NOTE from prophile: this ought to be changed to 5558
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
        SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

        if(SDL_SetVideoMode(_temp_width, _temp_height, 0, flags) == false) {
            IF_PRINT_WARNING(VIDEO_DEBUG) << "SDL_SetVideoMode() failed with error: " << SDL_GetError() << std::endl;

            _temp_fullscreen = _fullscreen;
            _temp_width = _screen_width;
            _temp_height = _screen_height;

            _UpdateViewportMetrics();

            // Test to see if we already had a valid video mode
            if(TextureManager && _screen_width > 0)
                TextureManager->ReloadTextures();

            return false;
        }
    }

    // Clear GL state, after SDL_SetVideoMode() for OSX compatibility
    DisableBlending();
    DisableTexture2D();
    DisableAlphaTest();
    DisableStencilTest();
    DisableScissoring();
    DisableVertexArray();
    DisableColorArray();
    DisableTextureCoordArray();

    // Turn off writing to the depth buffer
    glDepthMask(GL_FALSE);

    _screen_width = _temp_width;
    _screen_height = _temp_height;
    _fullscreen = _temp_fullscreen;

    _UpdateViewportMetrics();

    if(TextureManager)
        TextureManager->ReloadTextures();

    return true;
} // bool VideoEngine::ApplySettings()

void VideoEngine::_UpdateViewportMetrics()
{
    // Test the desired resolution and adds the necessary offsets if it's not a 4:3 one
    float width = _screen_width;
    float height = _screen_height;
    float scr_ratio = height > 0.2f ? width / height : 1.33f;
    if (vt_utils::IsFloatEqual(scr_ratio, 1.33f, 0.2f)) { // 1.33f == 4:3
        // 4:3: No offsets
        _viewport_x_offset = 0;
        _viewport_y_offset = 0;
        _viewport_width = _screen_width;
        _viewport_height = _screen_height;
        return;
    }

    // Handle non 4:3 cases
    if (width >= height) {
        float ideal_width = height / 3.0f * 4.0f;
        _viewport_width = ideal_width;
        _viewport_height = _screen_height;
        _viewport_x_offset = (int32)((width - ideal_width) / 2.0f);
        _viewport_y_offset = 0;
    }
    else {
        float ideal_height = width / 3.0f * 4.0f;
        _viewport_height = ideal_height;
        _viewport_width = _screen_width;
        _viewport_x_offset = 0;
        _viewport_y_offset = (int32)((height - ideal_height) / 2.0f);
    }
}

//-----------------------------------------------------------------------------
// VideoEngine class - Coordinate system and viewport methods
//-----------------------------------------------------------------------------

void VideoEngine::SetCoordSys(const CoordSys &coordinate_system)
{
    _current_context.coordinate_system = coordinate_system;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(_current_context.coordinate_system.GetLeft(), _current_context.coordinate_system.GetRight(),
            _current_context.coordinate_system.GetBottom(), _current_context.coordinate_system.GetTop(), -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void VideoEngine::GetCurrentViewport(float &x, float &y, float &width, float &height)
{
    static GLint viewport_dimensions[4] = {(GLint)0};
    glGetIntegerv(GL_VIEWPORT, viewport_dimensions);
    x = (float) viewport_dimensions[0];
    y = (float) viewport_dimensions[1];
    width = (float) viewport_dimensions[2];
    height = (float) viewport_dimensions[3];
}

void VideoEngine::SetViewport(float x, float y, float width, float height)
{
    if(width <= 0 || height <= 0)
    {
        PRINT_WARNING << "attempted to set an invalid viewport size: " << x << "," << y
            << " at " << width << ":" << height << std::endl;
        return;
    }

    _viewport_x_offset = x;
    _viewport_y_offset = y;
    _viewport_width = width;
    _viewport_height = height;
    glViewport(_viewport_x_offset, _viewport_y_offset, _viewport_width, _viewport_height);
}

void VideoEngine::EnableAlphaTest()
{
    if(!_gl_alpha_test_is_active) {
        glEnable(GL_ALPHA_TEST);
        _gl_alpha_test_is_active = true;
    }
}

void VideoEngine::DisableAlphaTest()
{
    if(_gl_alpha_test_is_active) {
        glDisable(GL_ALPHA_TEST);
        _gl_alpha_test_is_active = false;
    }
}

void VideoEngine::EnableBlending()
{
    if(!_gl_blend_is_active) {
        glEnable(GL_BLEND);
        _gl_blend_is_active = true;
    }
}

void VideoEngine::DisableBlending()
{
    if(_gl_blend_is_active) {
        glDisable(GL_BLEND);
        _gl_blend_is_active = false;
    }
}

void VideoEngine::EnableStencilTest()
{
    if(!_gl_stencil_test_is_active) {
        glEnable(GL_STENCIL_TEST);
        _gl_stencil_test_is_active = true;
    }
}

void VideoEngine::DisableStencilTest()
{
    if(_gl_stencil_test_is_active) {
        glDisable(GL_STENCIL_TEST);
        _gl_stencil_test_is_active = false;
    }
}

void VideoEngine::EnableTexture2D()
{
    if(!_gl_texture_2d_is_active) {
        glEnable(GL_TEXTURE_2D);
        _gl_texture_2d_is_active = true;
    }
}

void VideoEngine::DisableTexture2D()
{
    if(_gl_texture_2d_is_active) {
        glDisable(GL_TEXTURE_2D);
        _gl_texture_2d_is_active = false;
    }
}

void VideoEngine::EnableColorArray()
{
    if(!_gl_color_array_is_activated) {
        glEnableClientState(GL_COLOR_ARRAY);
        _gl_color_array_is_activated = true;
    }
}

void VideoEngine::DisableColorArray()
{
    if(_gl_color_array_is_activated) {
        glDisableClientState(GL_COLOR_ARRAY);
        _gl_color_array_is_activated = false;
    }
}

void VideoEngine::EnableVertexArray()
{
    if(!_gl_vertex_array_is_activated) {
        glEnableClientState(GL_VERTEX_ARRAY);
        _gl_vertex_array_is_activated = true;
    }
}

void VideoEngine::DisableVertexArray()
{
    if(_gl_vertex_array_is_activated) {
        glDisableClientState(GL_VERTEX_ARRAY);
        _gl_vertex_array_is_activated = false;
    }
}

void VideoEngine::EnableTextureCoordArray()
{
    if(!_gl_texture_coord_array_is_activated) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        _gl_texture_coord_array_is_activated = true;
    }
}

void VideoEngine::DisableTextureCoordArray()
{
    if(_gl_texture_coord_array_is_activated) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        _gl_texture_coord_array_is_activated = false;
    }
}

void VideoEngine::SetVertexPointer(GLint size, GLsizei stride, const float *ptr)
{
    assert(size > 0);
    assert(stride >= 0);
    assert(ptr);

    if (stride == 0) {
        stride = size * sizeof(float);
    }

    _vertex_array_ptr = ptr;
    _vertex_array_stride = stride;
    _vertex_array_size = size;
}

void VideoEngine::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    assert(_vertex_array_ptr);
    assert(first >= 0);
    assert(count > 0);

    int stride_float = _vertex_array_stride / sizeof(float);

    // Resize transformed_vertex array if needed.
    _transformed_vertex_array.resize(stride_float * count);
    if (_transformed_vertex_array_ptr != &_transformed_vertex_array[0]) {
        _transformed_vertex_array_ptr = &_transformed_vertex_array[0];
        glVertexPointer(_vertex_array_size, GL_FLOAT, _vertex_array_stride, _transformed_vertex_array_ptr);
    }

    // Apply the transform.
    _transform_stack.top().Apply(_vertex_array_ptr + first * stride_float,
                                 _transformed_vertex_array_ptr,
                                 count,
                                 _vertex_array_stride);

    glDrawArrays(mode, 0, count);
}

void VideoEngine::EnableScissoring()
{
    _current_context.scissoring_enabled = true;
    if(!_gl_scissor_test_is_active) {
        glEnable(GL_SCISSOR_TEST);
        _gl_scissor_test_is_active = true;
    }
}

void VideoEngine::DisableScissoring()
{
    _current_context.scissoring_enabled = false;
    if(_gl_scissor_test_is_active) {
        glDisable(GL_SCISSOR_TEST);
        _gl_scissor_test_is_active = false;
    }
}

void VideoEngine::SetScissorRect(float left, float right, float bottom, float top)
{
    _current_context.scissor_rectangle = CalculateScreenRect(left, right, bottom, top);

    glScissor(static_cast<GLint>((_current_context.scissor_rectangle.left / static_cast<float>(VIDEO_STANDARD_RES_WIDTH)) * _current_context.viewport.width),
              static_cast<GLint>((_current_context.scissor_rectangle.top / static_cast<float>(VIDEO_STANDARD_RES_HEIGHT)) * _current_context.viewport.height),
              static_cast<GLsizei>((_current_context.scissor_rectangle.width / static_cast<float>(VIDEO_STANDARD_RES_WIDTH)) * _current_context.viewport.width),
              static_cast<GLsizei>((_current_context.scissor_rectangle.height / static_cast<float>(VIDEO_STANDARD_RES_HEIGHT)) * _current_context.viewport.height));
}

void VideoEngine::SetScissorRect(const ScreenRect &rect)
{
    _current_context.scissor_rectangle = rect;

    glScissor(static_cast<GLint>((_current_context.scissor_rectangle.left / static_cast<float>(VIDEO_STANDARD_RES_WIDTH)) * _current_context.viewport.width),
              static_cast<GLint>((_current_context.scissor_rectangle.top / static_cast<float>(VIDEO_STANDARD_RES_HEIGHT)) * _current_context.viewport.height),
              static_cast<GLsizei>((_current_context.scissor_rectangle.width / static_cast<float>(VIDEO_STANDARD_RES_WIDTH)) * _current_context.viewport.width),
              static_cast<GLsizei>((_current_context.scissor_rectangle.height / static_cast<float>(VIDEO_STANDARD_RES_HEIGHT)) * _current_context.viewport.height)
             );
}



ScreenRect VideoEngine::CalculateScreenRect(float left, float right, float bottom, float top)
{
    ScreenRect rect;

    int32 scr_left = _ScreenCoordX(left);
    int32 scr_right = _ScreenCoordX(right);
    int32 scr_bottom = _ScreenCoordY(bottom);
    int32 scr_top = _ScreenCoordY(top);

    int32 temp;
    if(scr_left > scr_right) {
        temp = scr_left;
        scr_left = scr_right;
        scr_right = temp;
    }

    if(scr_top > scr_bottom) {
        temp = scr_top;
        scr_top = scr_bottom;
        scr_bottom = temp;
    }

    rect.top = scr_top;
    rect.left = scr_left;
    rect.width = scr_right - scr_left;
    rect.height = scr_bottom - scr_top;

    return rect;
}

//-----------------------------------------------------------------------------
// VideoEngine class - Transformation methods
//-----------------------------------------------------------------------------

void VideoEngine::Move(float x, float y)
{
    _transform_stack.top().Reset();
    _transform_stack.top().Translate(x, y);

    _x_cursor = x;
    _y_cursor = y;
}

void VideoEngine::MoveRelative(float x, float y)
{
    _transform_stack.top().Translate(x, y);

    _x_cursor += x;
    _y_cursor += y;
}

void VideoEngine::PushMatrix()
{
    _transform_stack.push(_transform_stack.top());
}

void VideoEngine::PopMatrix()
{
    // Sanity.
    if (!_transform_stack.empty()) {
        _transform_stack.pop();
    }

    // Sanity.
    if (_transform_stack.empty()) {
        _transform_stack.push(Transform2D());
    }
}

void VideoEngine::PushState()
{
    PushMatrix();

    _context_stack.push(_current_context);
}

void VideoEngine::PopState()
{
    // Restore the most recent context information and pop it from stack
    if(_context_stack.empty()) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "no video states were saved on the stack" << std::endl;
        return;
    }

    _current_context = _context_stack.top();
    _context_stack.pop();

    PopMatrix();

    glViewport(_current_context.viewport.left, _current_context.viewport.top, _current_context.viewport.width, _current_context.viewport.height);

    if(_current_context.scissoring_enabled) {
        EnableScissoring();
        glScissor(static_cast<GLint>((_current_context.scissor_rectangle.left / static_cast<float>(VIDEO_STANDARD_RES_WIDTH)) * _current_context.viewport.width),
                  static_cast<GLint>((_current_context.scissor_rectangle.top / static_cast<float>(VIDEO_STANDARD_RES_HEIGHT)) * _current_context.viewport.height),
                  static_cast<GLsizei>((_current_context.scissor_rectangle.width / static_cast<float>(VIDEO_STANDARD_RES_WIDTH)) * _current_context.viewport.width),
                  static_cast<GLsizei>((_current_context.scissor_rectangle.height / static_cast<float>(VIDEO_STANDARD_RES_HEIGHT)) * _current_context.viewport.height));
    } else {
        DisableScissoring();
    }
}

void VideoEngine::Rotate(float angle)
{
    _transform_stack.top().Rotate(angle);
}

void VideoEngine::Scale(float x, float y)
{
    _transform_stack.top().Scale(x, y);
}

void VideoEngine::DrawFadeEffect()
{
    _screen_fader.Draw();
}

void VideoEngine::DisableFadeEffect()
{
    // Disable potential game fades as it is just another light effect.
    // Transitional effects are done by the mode manager and shouldn't
    // be interrupted.
    if(IsFading() && !IsLastFadeTransitional())
        FadeIn(0);
}

StillImage VideoEngine::CaptureScreen() throw(Exception)
{
    // Static variable used to make sure the capture has a unique name in the texture image map
    static uint32 capture_id = 0;

    StillImage screen_image;

    // Retrieve width/height of the viewport. viewport_dimensions[2] is the width, [3] is the height
    GLint viewport_dimensions[4];
    glGetIntegerv(GL_VIEWPORT, viewport_dimensions);
    screen_image.SetDimensions((float)viewport_dimensions[2], (float)viewport_dimensions[3]);

    // Set up the screen rectangle to copy
    ScreenRect screen_rect(viewport_dimensions[0], viewport_dimensions[1], viewport_dimensions[2], viewport_dimensions[3]);

    // Create a new ImageTexture with a unique filename for this newly captured screen
    ImageTexture *new_image = new ImageTexture("capture_screen" + NumberToString(capture_id), "<T>", viewport_dimensions[2], viewport_dimensions[3]);
    new_image->AddReference();

    // Create a texture sheet of an appropriate size that can retain the capture
    TexSheet *temp_sheet = TextureManager->_CreateTexSheet(RoundUpPow2(viewport_dimensions[2]), RoundUpPow2(viewport_dimensions[3]), VIDEO_TEXSHEET_ANY, false);
    VariableTexSheet *sheet = dynamic_cast<VariableTexSheet *>(temp_sheet);

    // Ensure that texture sheet creation succeeded, insert the texture image into the sheet, and copy the screen into the sheet
    if(sheet == NULL) {
        delete new_image;
        throw Exception("could not create texture sheet to store captured screen", __FILE__, __LINE__, __FUNCTION__);
    }
    if(sheet->InsertTexture(new_image) == false) {
        TextureManager->_RemoveSheet(sheet);
        delete new_image;
        throw Exception("could not insert captured screen image into texture sheet", __FILE__, __LINE__, __FUNCTION__);
    }
    if(sheet->CopyScreenRect(0, 0, screen_rect) == false) {
        TextureManager->_RemoveSheet(sheet);
        delete new_image;
        throw Exception("call to TexSheet::CopyScreenRect() failed", __FILE__, __LINE__, __FUNCTION__);
    }

    // Store the image element to the saved image (with a flipped y axis)
    screen_image._image_texture = new_image;
    screen_image._texture = new_image;

    // Vertically flip the texture image by swapping the v coordinates, since OpenGL returns the image upside down in the CopyScreenRect call
    float temp = new_image->v1;
    new_image->v1 = new_image->v2;
    new_image->v2 = temp;

    ++capture_id;
    return screen_image;
}

StillImage VideoEngine::CreateImage(ImageMemory *raw_image, const std::string &image_name, bool delete_on_exist) throw(Exception)
{
    //the returning image
    StillImage still_image;

    //check if the raw_image pointer is valid
    if(!raw_image)
    {
        throw Exception("raw_image is NULL, cannot create a StillImage", __FILE__, __LINE__, __FUNCTION__);
    }

    still_image.SetDimensions(raw_image->width, raw_image->height);

    //Check to see if the image_name exists
    if(TextureManager->_IsImageTextureRegistered(image_name))
    {
        //if we are allowed to delete, then we remove the texture
        if(delete_on_exist)
        {
            ImageTexture* old = TextureManager->_GetImageTexture(image_name);
            TextureManager->_UnregisterImageTexture(old);
            if(old->RemoveReference())
                delete old;
        }
        else
        {
            throw Exception("image already exists in texture manager", __FILE__, __LINE__, __FUNCTION__);
        }
    }

    //create a new texture image. the next few steps are similar to CaptureImage, so in the future
    // we may want to do a code-cleanup
    ImageTexture *new_image = new ImageTexture(image_name, "<T>", raw_image->width, raw_image->height);
    new_image->AddReference();
    // Create a texture sheet of an appropriate size that can retain the capture
    TexSheet *temp_sheet = TextureManager->_CreateTexSheet(RoundUpPow2(raw_image->width), RoundUpPow2(raw_image->height), VIDEO_TEXSHEET_ANY, false);
    VariableTexSheet *sheet = dynamic_cast<VariableTexSheet *>(temp_sheet);

    // Ensure that texture sheet creation succeeded, insert the texture image into the sheet, and copy the screen into the sheet
    if(sheet == NULL) {
        delete new_image;
        throw Exception("could not create texture sheet to store still image", __FILE__, __LINE__, __FUNCTION__);
    }

    if(sheet->InsertTexture(new_image) == false)
    {
        TextureManager->_RemoveSheet(sheet);
        delete new_image;
        throw Exception("could not insert raw image into texture sheet", __FILE__, __LINE__, __FUNCTION__);
    }

    if(sheet->CopyRect(0, 0, *raw_image) == false)
    {
        TextureManager->_RemoveSheet(sheet);
        delete new_image;
        throw Exception("call to TexSheet::CopyRect() failed", __FILE__, __LINE__, __FUNCTION__);
    }

    // Store the image element to the saved image (with a flipped y axis)
    still_image._image_texture = new_image;
    still_image._texture = new_image;
    return still_image;
}

bool VideoEngine::IsScreenShaking()
{
    vt_mode_manager::GameMode *gm = vt_mode_manager::ModeManager->GetTop();

    if (!gm)
        return false;

    vt_mode_manager::EffectSupervisor &effects = gm->GetEffectSupervisor();
    if (!effects.IsScreenShaking())
        return false;

    // update the shaking offsets before returning
    effects.GetShakingOffsets(_x_shake, _y_shake);
    return true;
}

void VideoEngine::SetBrightness(float value)
{
    _brightness_value = value;

    // Limit min/max brightness
    if(_brightness_value > 2.0f) {
        _brightness_value = 2.0f;
    } else if(_brightness_value < 0.0f) {
        _brightness_value = 0.0f;
    }

    // Note: To replace with: SDL_SetWindowBrightness() in SDL 2
    SDL_SetGamma(_brightness_value, _brightness_value, _brightness_value);
}

void VideoEngine::MakeScreenshot(const std::string &filename)
{
    private_video::ImageMemory buffer;

    // Retrieve the width and height of the viewport.
    GLint viewport_dimensions[4]; // viewport_dimensions[2] is the width, [3] is the height
    glGetIntegerv(GL_VIEWPORT, viewport_dimensions);

    // Buffer to store the image before it is flipped
    buffer.width = viewport_dimensions[2];
    buffer.height = viewport_dimensions[3];
    buffer.pixels = malloc(buffer.width * buffer.height * 3);
    buffer.rgb_format = true;

    // Read the viewport pixel data
    glReadPixels(viewport_dimensions[0], viewport_dimensions[1],
                 buffer.width, buffer.height, GL_RGB, GL_UNSIGNED_BYTE, buffer.pixels);

    if(CheckGLError() == true) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "an OpenGL error occured: " << CreateGLErrorString() << std::endl;

        free(buffer.pixels);
        buffer.pixels = NULL;
        return;
    }

    // Vertically flip the image, then swap the flipped and original images
    void *buffer_temp = malloc(buffer.width * buffer.height * 3);
    for(uint32 i = 0; i < buffer.height; ++i) {
        memcpy((uint8 *)buffer_temp + i * buffer.width * 3,
               (uint8 *)buffer.pixels + (buffer.height - i - 1) * buffer.width * 3, buffer.width * 3);
    }
    void *temp = buffer.pixels;
    buffer.pixels = buffer_temp;
    buffer_temp = temp;

    buffer.SaveImage(filename);

    free(buffer_temp);
    free(buffer.pixels);
    buffer.pixels = NULL;
}

int32 VideoEngine::_ConvertYAlign(int32 y_align)
{
    switch(y_align) {
    case VIDEO_Y_BOTTOM:
        return -1;
    case VIDEO_Y_CENTER:
        return 0;
    case VIDEO_Y_TOP:
        return 1;
    default:
        IF_PRINT_WARNING(VIDEO_DEBUG) << "unknown value for argument flag: " << y_align << std::endl;
        return 0;
    }
}

int32 VideoEngine::_ConvertXAlign(int32 x_align)
{
    switch(x_align) {
    case VIDEO_X_LEFT:
        return -1;
    case VIDEO_X_CENTER:
        return 0;
    case VIDEO_X_RIGHT:
        return 1;
    default:
        IF_PRINT_WARNING(VIDEO_DEBUG) << "unknown value for argument flag: " << x_align << std::endl;
        return 0;
    }
}

bool VideoEngine::SetDefaultCursor(const std::string &cursor_image_filename)
{
    return _default_menu_cursor.Load(cursor_image_filename);
}

StillImage *VideoEngine::GetDefaultCursor()
{
    if(_default_menu_cursor.GetWidth() != 0.0f)   // cheap test if image is valid
        return &_default_menu_cursor;
    else
        return NULL;
}

int32 VideoEngine::_ScreenCoordX(float x)
{
    float percent;
    if(_current_context.coordinate_system.GetLeft() < _current_context.coordinate_system.GetRight())
        percent = (x - _current_context.coordinate_system.GetLeft()) /
                  (_current_context.coordinate_system.GetRight() - _current_context.coordinate_system.GetLeft());
    else
        percent = (x - _current_context.coordinate_system.GetRight()) /
                  (_current_context.coordinate_system.GetLeft() - _current_context.coordinate_system.GetRight());

    return static_cast<int32>(percent * static_cast<float>(_viewport_width));
}

int32 VideoEngine::_ScreenCoordY(float y)
{
    float percent;
    if(_current_context.coordinate_system.GetTop() < _current_context.coordinate_system.GetBottom())
        percent = (y - _current_context.coordinate_system.GetTop()) /
                  (_current_context.coordinate_system.GetBottom() - _current_context.coordinate_system.GetTop());
    else
        percent = (y - _current_context.coordinate_system.GetBottom()) /
                  (_current_context.coordinate_system.GetTop() - _current_context.coordinate_system.GetBottom());

    return static_cast<int32>(percent * static_cast<float>(_viewport_height));
}

void VideoEngine::DrawLine(float x1, float y1, float x2, float y2, float width, const Color& color)
{
    EnableBlending();
    DisableTexture2D();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Normal blending
    glPushAttrib(GL_LINE_WIDTH);
    glLineWidth(width);

    EnableVertexArray();
    DisableColorArray();
    DisableTextureCoordArray();

    glColor4fv((GLfloat *)color.GetColors());

    GLfloat vert_coords[] = { x1, y1, x2, y2 };
    _transformed_vertex_array_ptr = vert_coords;
    SetVertexPointer(2, 0, vert_coords);
    DrawArrays(GL_LINES, 0, 2);

    glPopAttrib(); // GL_LINE_WIDTH
}

void VideoEngine::DrawGrid(float x, float y, float x_step, float y_step, const Color &c)
{
    float x_max = _current_context.coordinate_system.GetRight();
    float y_max = _current_context.coordinate_system.GetBottom();

    int32 num_vertices = 0;
    std::vector<GLfloat> vertices;
    for(; x <= x_max; x += x_step) {
        vertices.push_back(x);
        vertices.push_back(_current_context.coordinate_system.GetBottom());
        vertices.push_back(x);
        vertices.push_back(_current_context.coordinate_system.GetTop());
        num_vertices += 2;
    }

    for(; y < y_max; y += y_step) {
        vertices.push_back(_current_context.coordinate_system.GetLeft());
        vertices.push_back(y);
        vertices.push_back(_current_context.coordinate_system.GetRight());
        vertices.push_back(y);
        num_vertices += 2;
    }

    glColor4fv(&c[0]);
    DisableTexture2D();
    EnableVertexArray();

    SetVertexPointer(2, 0, &(vertices[0]));
    DrawArrays(GL_LINES, 0, num_vertices);
}

void VideoEngine::DrawRectangle(float width, float height, const Color &color)
{
    _rectangle_image._width = width;
    _rectangle_image._height = height;
    _rectangle_image._color[0] = color;

    _rectangle_image.Draw(color);
}

void VideoEngine::DrawRectangleOutline(float left, float right, float bottom, float top, float width, const Color &color)
{
    DrawLine(left, bottom, right, bottom, width, color);
    DrawLine(left, top, right, top, width, color);
    DrawLine(left, bottom, left, top, width, color);
    DrawLine(right, bottom, right, top, width, color);
}

void VideoEngine::DrawHalo(const ImageDescriptor &id, const Color &color)
{
    char old_blend_mode = _current_context.blend;
    _current_context.blend = VIDEO_BLEND_ADD;
    id.Draw(color);
    _current_context.blend = old_blend_mode;
}

}  // namespace vt_video
