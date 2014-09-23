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
*** \file    texture_controller.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for texture management code
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "texture_controller.h"
#include "utils/utils_files.h"

#include "engine/mode_manager.h"
#include "engine/video/video.h"

using namespace vt_utils;
using namespace vt_video::private_video;

namespace vt_video
{

TextureController *TextureManager = NULL;



TextureController::TextureController() :
    debug_current_sheet(-1),
    _last_tex_id(INVALID_TEXTURE_ID),
    _debug_num_tex_switches(0)
{}



TextureController::~TextureController()
{
    IF_PRINT_DEBUG(VIDEO_DEBUG) << "Deleting all remaining ImageTextures, a total of: " << _images.size() << std::endl;

    // Invoking the ImageTexture destructor will erase the entry in the _images map that corresponds to that object
    // Thus the map will decrement in size by one on every iteration through this loop
    while(_images.empty() == false) {
        ImageTexture *img = (*_images.begin()).second;
        img->texture_sheet->RemoveTexture(img);
        delete img;
    }

    IF_PRINT_DEBUG(VIDEO_DEBUG) << "Deleting all remaining texture sheets, a total of: " << _tex_sheets.size() << std::endl;
    for(std::vector<TexSheet *>::iterator i = _tex_sheets.begin(); i != _tex_sheets.end(); ++i) {
        delete *i;
    }
}



bool TextureController::SingletonInitialize()
{
    // Create a default set of texture sheets
    if(_CreateTexSheet(512, 512, VIDEO_TEXSHEET_32x32, false) == NULL) {
        PRINT_ERROR << "could not create default 32x32 texture sheet" << std::endl;
        return false;
    }
    if(_CreateTexSheet(512, 512, VIDEO_TEXSHEET_32x64, false) == NULL) {
        PRINT_ERROR << "could not create default 32x64 texture sheet" << std::endl;
        return false;
    }
    if(_CreateTexSheet(512, 512, VIDEO_TEXSHEET_64x64, false) == NULL) {
        PRINT_ERROR << "could not create default 64x64 texture sheet" << std::endl;
        return false;
    }
    if(_CreateTexSheet(512, 512, VIDEO_TEXSHEET_ANY, true) == NULL) {
        PRINT_ERROR << "could not create default static variable sized texture sheet" << std::endl;
        return false;
    }
    if(_CreateTexSheet(512, 512, VIDEO_TEXSHEET_ANY, false) == NULL) {
        PRINT_ERROR << "could not create default variable sized tex sheet" << std::endl;
        return false;
    }

    return true;
}



bool TextureController::UnloadTextures()
{
    bool success = true;

    // Save temporary textures to disk, in other words textures which were not
    // loaded from a file. This way when we recreate the GL context we will
    // be able to load them again.
    if(_SaveTempTextures() == false) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "call to _SaveTempTextures() failed" << std::endl;
        success = false;
    }

    // Unload all texture sheets
    std::vector<TexSheet *>::iterator i = _tex_sheets.begin();
    while(i != _tex_sheets.end()) {
        if(*i != NULL) {
            if((*i)->Unload() == false) {
                IF_PRINT_WARNING(VIDEO_DEBUG) << "a TextureSheet::Unload() call failed" << std::endl;
                success = false;
            }
        } else {
            IF_PRINT_WARNING(VIDEO_DEBUG) << "a NULL TextureSheet was found in the _tex_sheets container" << std::endl;
            success = false;
        }

        ++i;
    }

    // Clear all font caches
    std::map<std::string, FontProperties *>::iterator j = TextManager->_font_map.begin();
    std::map<std::string, FontProperties *>::const_iterator j_end = TextManager->_font_map.end();
    while(j != j_end) {
        std::vector<FontGlyph *>* glyph_cache = j->second->glyph_cache;

        if(glyph_cache) {
            std::vector<vt_video::FontGlyph *>::iterator it_end = glyph_cache->end();
            for(std::vector<FontGlyph *>::iterator k = glyph_cache->begin();
                    k != it_end; ++k) {
                if(*k)
                    _DeleteTexture((*k)->texture);
                delete *k;
            }

            glyph_cache->clear();
        }

        ++j;
    }

    return success;
} // bool TextureController::UnloadTextures()



bool TextureController::ReloadTextures()
{
    bool success = true;
    std::vector<TexSheet *>::iterator i = _tex_sheets.begin();

    while(i != _tex_sheets.end()) {
        if(*i != NULL) {
            if((*i)->Reload() == false) {
                IF_PRINT_WARNING(VIDEO_DEBUG) << "a TextureSheet::Reload() call failed" << std::endl;
                success = false;
            }
        } else {
            IF_PRINT_WARNING(VIDEO_DEBUG) << "a NULL TextureSheet was found in the _tex_sheets container" << std::endl;
            success = false;
        }

        ++i;
    }

    _DeleteTempTextures();

    return success;
}



void TextureController::DEBUG_NextTexSheet()
{
    debug_current_sheet++;

    if(debug_current_sheet >= static_cast<int32>(_tex_sheets.size()))
        debug_current_sheet = -1;  // Disables texture sheet display
}



void TextureController::DEBUG_PrevTexSheet()
{
    debug_current_sheet--;

    if(debug_current_sheet < -1)
        debug_current_sheet = static_cast<int32>(_tex_sheets.size()) - 1;
}



void TextureController::DEBUG_ShowTexSheet()
{
    // Value less than zero means we shouldn't show any texture sheets
    if(debug_current_sheet < 0) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "function was called when debug_current_sheet was not a positive value" << std::endl;
        return;
    }

    if(_tex_sheets.empty()) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "there were no texture sheets available to show" << std::endl;
        return;
    }

    // If we were viewing a particular texture sheet and it happened to get deleted, we change
    // to look at a different sheet
    int32 num_sheets = static_cast<uint32>(_tex_sheets.size());

    if(debug_current_sheet >= num_sheets) {
        debug_current_sheet = num_sheets - 1;
    }

    TexSheet *sheet = _tex_sheets[debug_current_sheet];
    if(sheet == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "discovered a NULL texture sheet in _tex_sheets container" << std::endl;
        return;
    }

    VideoManager->PushState();
    VideoManager->SetDrawFlags(VIDEO_NO_BLEND, VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
    VideoManager->SetStandardCoordSys();

    VideoManager->PushMatrix();
    VideoManager->Move(0.0f, 368.0f);
    VideoManager->Scale(sheet->width / 2.0f, sheet->height / 2.0f);

    sheet->DEBUG_Draw();

    VideoManager->PopMatrix();

    char buf[200];

    VideoManager->Move(20, 60);
    TextManager->Draw("Current Texture sheet:");

    sprintf(buf, "  Sheet:   %d", debug_current_sheet);
    VideoManager->MoveRelative(0, 20);
    TextManager->Draw(buf);

    VideoManager->MoveRelative(0, 20);
    sprintf(buf, "  Size:    %dx%d", sheet->width, sheet->height);
    TextManager->Draw(buf);

    if(sheet->type == VIDEO_TEXSHEET_32x32)
        sprintf(buf, "  Type:    32x32");
    else if(sheet->type == VIDEO_TEXSHEET_32x64)
        sprintf(buf, "  Type:    32x64");
    else if(sheet->type == VIDEO_TEXSHEET_64x64)
        sprintf(buf, "  Type:    64x64");
    else if(sheet->type == VIDEO_TEXSHEET_ANY)
        sprintf(buf, "  Type:    Any size");
    else
        sprintf(buf, "  Type:    Unknown");

    VideoManager->MoveRelative(0, 20);
    TextManager->Draw(buf);

    sprintf(buf, "  Static:  %d", sheet->is_static);
    VideoManager->MoveRelative(0, 20);
    TextManager->Draw(buf);

    sprintf(buf, "  TexID:   %d", sheet->tex_id);
    VideoManager->MoveRelative(0, 20);
    TextManager->Draw(buf);

    VideoManager->PopState();
} // void TextureController::DEBUG_ShowTexSheet()



GLuint TextureController::_CreateBlankGLTexture(int32 width, int32 height)
{
    GLuint tex_id;
    glGenTextures(1, &tex_id);

    if(VideoManager->CheckGLError()) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "an OpenGL error was detected: " << VideoManager->CreateGLErrorString() << std::endl;
        _DeleteTexture(tex_id);
        return INVALID_TEXTURE_ID;
    }

    _BindTexture(tex_id);

    // If the binding was successful, initialize the texture with glTexImage2D()
    if(VideoManager->GetGLError() == GL_NO_ERROR) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    if(VideoManager->CheckGLError()) {
        PRINT_ERROR << "failed to create new texture. OpenGL reported the following error: " << VideoManager->CreateGLErrorString() << std::endl;
        _DeleteTexture(tex_id);
        return INVALID_TEXTURE_ID;
    }

    // Set linear texture interpolation based on the smooth option.
    GLenum filtering_type = VideoManager->ShouldSmoothPixelArt() ? GL_LINEAR : GL_NEAREST;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    return tex_id;
}



void TextureController::_BindTexture(GLuint tex_id)
{
    // Return if this texture ID is already bound
    if(tex_id == _last_tex_id)
        return;

    _last_tex_id = tex_id;
    glBindTexture(GL_TEXTURE_2D, tex_id);
    ++_debug_num_tex_switches;
}



void TextureController::_DeleteTexture(GLuint tex_id)
{
    glDeleteTextures(1, &tex_id);

    if(_last_tex_id == tex_id)
        _last_tex_id = INVALID_TEXTURE_ID;
}



bool TextureController::_SaveTempTextures()
{
    bool success = true;

    for(std::map<std::string, ImageTexture *>::iterator i = _images.begin(); i != _images.end(); ++i) {
        ImageTexture *image = i->second;

        // Check that this is a temporary texture and if so, save it to disk as a .png file
        if(image->tags.find("<T>") != std::string::npos) {
            IF_PRINT_DEBUG(VIDEO_DEBUG) << " saving temporary texture " << image->filename << std::endl;
            ImageMemory buffer;
            buffer.CopyFromImage(image);
            std::string path = GetUserDataPath();
            if(buffer.SaveImage(path + image->filename + ".png") == false) {
                success = false;
                IF_PRINT_WARNING(VIDEO_DEBUG) << "call to ImageMemory::SaveImage() failed" << std::endl;
            }
        }
    }
    return success;
}



bool TextureController::_DeleteTempTextures()
{
    return vt_utils::CleanDirectory("img/temp");
}



TexSheet *TextureController::_CreateTexSheet(int32 width, int32 height, TexSheetType type, bool is_static)
{
    // Validate that the function arguments are appropriate values
    if(!IsPowerOfTwo(width) || !IsPowerOfTwo(height)) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "non power-of-two width and/or height argument" << std::endl;
        return NULL;
    }

    if(type <= VIDEO_TEXSHEET_INVALID || type >= VIDEO_TEXSHEET_TOTAL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid TexSheetType argument" << std::endl;
        return NULL;
    }

    // Create a blank texture for the sheet to use
    GLuint tex_id = _CreateBlankGLTexture(width, height);
    if(tex_id == INVALID_TEXTURE_ID) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "failed to create a new blank OpenGL texture" << std::endl;
        return NULL;
    }

    TexSheet *sheet = NULL;
    if(type == VIDEO_TEXSHEET_32x32)
        sheet = new FixedTexSheet(width, height, tex_id, type, is_static, 32, 32);
    else if(type == VIDEO_TEXSHEET_32x64)
        sheet = new FixedTexSheet(width, height, tex_id, type, is_static, 32, 64);
    else if(type == VIDEO_TEXSHEET_64x64)
        sheet = new FixedTexSheet(width, height, tex_id, type, is_static, 64, 64);
    else
        sheet = new VariableTexSheet(width, height, tex_id, type, is_static);

    _tex_sheets.push_back(sheet);
    return sheet;
}



void TextureController::_RemoveSheet(TexSheet *sheet)
{
    if(sheet == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "NULL argument passed to function" << std::endl;
        return;
    }

    if(_tex_sheets.empty()) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "no texture sheets were loaded when function was called" << std::endl;
        return;
    }

    std::vector<TexSheet *>::iterator i = _tex_sheets.begin();

    while(i != _tex_sheets.end()) {
        if(*i == sheet) {
            delete sheet;
            _tex_sheets.erase(i);
            return;
        }
        ++i;
    }

    IF_PRINT_WARNING(VIDEO_DEBUG) << "could not find texture sheet to delete" << std::endl;
}



TexSheet *TextureController::_InsertImageInTexSheet(BaseTexture *image, ImageMemory &load_info, bool is_static)
{
    // Image sizes larger than 512 in either dimension require their own texture sheet
    if(load_info.width > 512 || load_info.height > 512) {
        int32 round_width = RoundUpPow2(load_info.width);
        int32 round_height = RoundUpPow2(load_info.height);
        TexSheet *sheet = _CreateTexSheet(round_width, round_height, VIDEO_TEXSHEET_ANY, false);

        // Ran out of memory!
        if(sheet == NULL) {
            IF_PRINT_WARNING(VIDEO_DEBUG) << "could not create new texture sheet for image" << std::endl;
            return NULL;
        }

        if(sheet->AddTexture(image, load_info) == true)
            return sheet;
        else {
            IF_PRINT_WARNING(VIDEO_DEBUG) << "TexSheet::AddTexture returned false when trying to insert a large image" << std::endl;
            return NULL;
        }
    }

    // Determine the type of texture sheet that should hold this image
    TexSheetType type;

    if(load_info.width == 32 && load_info.height == 32)
        type = VIDEO_TEXSHEET_32x32;
    else if(load_info.width == 32 && load_info.height == 64)
        type = VIDEO_TEXSHEET_32x64;
    else if(load_info.width == 64 && load_info.height == 64)
        type = VIDEO_TEXSHEET_64x64;
    else
        type = VIDEO_TEXSHEET_ANY;

    // Look through all existing texture sheets and see if the image will fit in any of the ones which
    // match the type and static status that we are looking for
    for(uint32 i = 0; i < _tex_sheets.size(); i++) {
        TexSheet *sheet = _tex_sheets[i];
        if(sheet == NULL) {
            IF_PRINT_WARNING(VIDEO_DEBUG) << "found a NULL texture sheet in the _tex_sheets container" << std::endl;
            continue;
        }

        if(sheet->type == type && sheet->is_static == is_static) {
            if(sheet->AddTexture(image, load_info) == true) {
                return sheet;
            }
        }
    }

    // We couldn't add it to any existing sheets, so we must create a new one for it
    TexSheet *sheet = _CreateTexSheet(512, 512, type, is_static);
    if(sheet == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "failed to create a new texture sheet for image" << std::endl;
        return NULL;
    }

    // AddTexture should always work here. If not, there is a serious problem
    if(sheet->AddTexture(image, load_info)) {
        return sheet;
    } else {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "all attempts to add image to a texture sheet have failed" << std::endl;
        return NULL;
    }
} // TexSheet* TextureController::_InsertImageInTexSheet(BaseImage *image, ImageMemory& load_info, bool is_static)



bool TextureController::_ReloadImagesToSheet(TexSheet *sheet)
{
    // Delete images
    std::map<std::string, std::pair<ImageMemory, ImageMemory> > multi_image_info;

    bool success = true;
    for(std::map<std::string, ImageTexture *>::iterator i = _images.begin(); i != _images.end(); ++i) {
        // Only operate on images which belong to the requested TexSheet
        if(i->second->texture_sheet != sheet) {
            continue;
        }

        ImageTexture *img = i->second;
        ImageMemory load_info;
        bool is_multi_image = (img->tags.find("<X", 0) != img->filename.npos);

        // Multi Images require a different reloading process
        if(is_multi_image) {
            ImageMemory image;

            if(multi_image_info.find(img->filename) == multi_image_info.end()) {
                // Load the image
                if(load_info.LoadImage(img->filename) == false) {
                    IF_PRINT_WARNING(VIDEO_DEBUG) << "call to _LoadRawImage() failed" << std::endl;
                    success = false;
                    continue;
                }

                // Copy the part of the image in a buffer
                image.height = img->height;
                image.width = img->width;
                image.pixels = malloc(image.height * image.width * 4);

                if(image.pixels == NULL) {
                    IF_PRINT_WARNING(VIDEO_DEBUG) << "call to malloc returned NULL" << std::endl;
                    success = false;
                    continue;
                }

                multi_image_info[img->filename] = std::make_pair(load_info, image);
            } else {
                load_info = multi_image_info[img->filename].first;
                image = multi_image_info[img->filename].second;
            }

            uint16 pos0, pos1; // Used to find the start and end positions of a sub-string
            uint32 x, y; //
            uint32 rows, cols;

            pos0 = img->tags.find("<X", 0);
            pos1 = img->tags.find('_', pos0);
            x = atoi(img->tags.substr(pos0 + 2, pos1).c_str());

            pos0 = img->tags.find("<Y", 0);
            pos1 = img->tags.find('_', pos0);
            y = atoi(img->tags.substr(pos0 + 2, pos1).c_str());

            rows = load_info.height / image.height;
            cols = load_info.width / image.width;

            for(uint32 row = 0; row < image.height; ++row) {
                memcpy((uint8 *)image.pixels + 4 * image.width * row, (uint8 *)load_info.pixels + (((x * load_info.height / rows) + row)
                        * load_info.width + y * load_info.width / cols) * 4, 4 * image.width);
            }

            // Convert to grayscale if needed
            if(img->tags.find("<G>", 0) != img->filename.npos)
                image.ConvertToGrayscale();

            // Copy the image into the texture sheet
            if(sheet->CopyRect(img->x, img->y, image) == false) {
                IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TexSheet::CopyRect() failed" << std::endl;
                success = false;
            }
        } // if (is_multi_image)

        // Reload a normal image file
        else {
            std::string fname = img->filename;
            IF_PRINT_DEBUG(VIDEO_DEBUG) << " Reloading image " << fname << std::endl;

            // Check if it is a temporary image, and if so retrieve it from the img/temp directory
            if(img->tags.find("<T>", 0) != img->tags.npos) {
                fname = "img/temp/" + fname + ".png";
            }

            if(load_info.LoadImage(fname) == false) {
                IF_PRINT_WARNING(VIDEO_DEBUG) << "call to _LoadRawImage() failed" << std::endl;
                success = false;
            }

            // Convert to grayscale if needed
            if(img->tags.find("<G>", 0) != img->filename.npos)
                load_info.ConvertToGrayscale();

            if(sheet->CopyRect(img->x, img->y, load_info) == false) {
                IF_PRINT_WARNING(VIDEO_DEBUG) << "call to TexSheet::CopyRect() failed" << std::endl;
                success = false;
            }

            if(load_info.pixels) {
                free(load_info.pixels);
                load_info.pixels = NULL;
            }
        }
    } // for (std::map<string, ImageTexture*>::iterator i = _images.begin(); i != _images.end(); i++)

    for(std::map<std::string, std::pair<ImageMemory, ImageMemory> >::iterator i = multi_image_info.begin();
            i != multi_image_info.end(); ++i) {
        free(i->second.first.pixels);
        i->second.first.pixels = NULL;
        free(i->second.second.pixels);
        i->second.second.pixels = NULL;
    }

    // Regenerate all font textures
    for(std::set<TextTexture *>::iterator i = _text_images.begin(); i != _text_images.end(); ++i) {
        if((*i)->texture_sheet == sheet) {
            if((*i)->Reload() == false) {
                IF_PRINT_WARNING(VIDEO_DEBUG) << "failed to reload a TextTexture" << std::endl;
                success = false;
            }
        }
    }

    return success;
} // bool TextureController::_ReloadImagesToSheet(TexSheet* sheet)



void TextureController::_RegisterImageTexture(ImageTexture *img)
{
    if(img == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "NULL argument passed to function" << std::endl;
        return;
    }

    std::string nametag = img->filename + img->tags;
    if(_IsImageTextureRegistered(nametag) == true) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "this ImageTexture was already registered: " << nametag << std::endl;
        return;
    }

    _images[nametag] = img;
}



void TextureController::_UnregisterImageTexture(ImageTexture *img)
{
    if(img == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "NULL argument passed to function" << std::endl;
        return;
    }

    std::string nametag = img->filename + img->tags;
    std::map<std::string, private_video::ImageTexture *>::iterator img_iter = _images.find(nametag);
    if(img_iter == _images.end()) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "this ImageTexture was not registered: " << nametag << std::endl;
        return;
    }
    _images.erase(img_iter);
}



void TextureController::_RegisterTextTexture(TextTexture *tex)
{
    if(tex == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "NULL argument passed to function" << std::endl;
        return;
    }

    if(_IsTextTextureRegistered(tex) == true) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "attempted to register an already registered TextTexture" << std::endl;
        return;
    }

    _text_images.insert(tex);
}



void TextureController::_UnregisterTextTexture(TextTexture *tex)
{
    if(tex == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "NULL argument passed to function" << std::endl;
        return;
    }

    std::set<private_video::TextTexture *>::iterator tex_iter = _text_images.find(tex);
    if(tex_iter == _text_images.end()) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "TextTexture was not registered" << std::endl;
        return;
    }
    _text_images.erase(tex_iter);
}


}  // namespace vt_video
