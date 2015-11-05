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

using namespace vt_video::private_video;

namespace vt_video
{

//! \brief The temporary directory.
const std::string DIRECTORY_TEMPORARY = "temporary/";

//! \brief The temporary texture directory.
const std::string DIRECTORY_TEMPORARY_TEXTURE = DIRECTORY_TEMPORARY + "texture/";


//! \brief A pointer to the texture controller.
TextureController* TextureManager = nullptr;

TextureController::TextureController() :
    _last_tex_id(INVALID_TEXTURE_ID),
    _debug_current_sheet(-1),
    _debug_num_tex_switches(0)
{
}

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
    if(_CreateTexSheet(512, 512, VIDEO_TEXSHEET_32x32, false) == nullptr) {
        PRINT_ERROR << "could not create default 32x32 texture sheet" << std::endl;
        return false;
    }
    if(_CreateTexSheet(512, 512, VIDEO_TEXSHEET_32x64, false) == nullptr) {
        PRINT_ERROR << "could not create default 32x64 texture sheet" << std::endl;
        return false;
    }
    if(_CreateTexSheet(512, 512, VIDEO_TEXSHEET_64x64, false) == nullptr) {
        PRINT_ERROR << "could not create default 64x64 texture sheet" << std::endl;
        return false;
    }
    if(_CreateTexSheet(512, 512, VIDEO_TEXSHEET_ANY, true) == nullptr) {
        PRINT_ERROR << "could not create default static variable sized texture sheet" << std::endl;
        return false;
    }
    if(_CreateTexSheet(512, 512, VIDEO_TEXSHEET_ANY, false) == nullptr) {
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
        if(*i != nullptr) {
            if((*i)->Unload() == false) {
                IF_PRINT_WARNING(VIDEO_DEBUG) << "a TextureSheet::Unload() call failed" << std::endl;
                success = false;
            }
        } else {
            IF_PRINT_WARNING(VIDEO_DEBUG) << "a nullptr TextureSheet was found in the _tex_sheets container" << std::endl;
            success = false;
        }

        ++i;
    }

    return success;
}

bool TextureController::ReloadTextures()
{
    bool success = true;
    std::vector<TexSheet *>::iterator i = _tex_sheets.begin();

    while(i != _tex_sheets.end()) {
        if(*i != nullptr) {
            if((*i)->Reload() == false) {
                IF_PRINT_WARNING(VIDEO_DEBUG) << "a TextureSheet::Reload() call failed" << std::endl;
                success = false;
            }
        } else {
            IF_PRINT_WARNING(VIDEO_DEBUG) << "a nullptr TextureSheet was found in the _tex_sheets container" << std::endl;
            success = false;
        }

        ++i;
    }

    _DeleteTempTextures();

    return success;
}

void TextureController::DEBUG_NextTexSheet()
{
    _debug_current_sheet++;

    if(_debug_current_sheet >= static_cast<int32_t>(_tex_sheets.size()))
        _debug_current_sheet = -1;  // Disables texture sheet display
}

void TextureController::DEBUG_PrevTexSheet()
{
    _debug_current_sheet--;

    if(_debug_current_sheet < -1)
        _debug_current_sheet = static_cast<int32_t>(_tex_sheets.size()) - 1;
}

void TextureController::DEBUG_ShowTexSheet()
{
    // Value less than zero means we shouldn't show any texture sheets
    if(_debug_current_sheet < 0) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "function was called when debug_current_sheet was not a positive value" << std::endl;
        return;
    }

    if(_tex_sheets.empty()) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "there were no texture sheets available to show" << std::endl;
        return;
    }

    // If we were viewing a particular texture sheet and it happened to get deleted, we change
    // to look at a different sheet
    int32_t num_sheets = static_cast<uint32_t>(_tex_sheets.size());

    if(_debug_current_sheet >= num_sheets) {
        _debug_current_sheet = num_sheets - 1;
    }

    TexSheet *sheet = _tex_sheets[_debug_current_sheet];
    if(sheet == nullptr) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "discovered a nullptr texture sheet in _tex_sheets container" << std::endl;
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

    sprintf(buf, "  Sheet:   %d", _debug_current_sheet);
    VideoManager->MoveRelative(0, 20);
    TextManager->Draw(buf);

    VideoManager->MoveRelative(0, 20);
    sprintf(buf, "  Size:    %dx%d", sheet->width, sheet->height);
    TextManager->Draw(buf);

    if (sheet->type == VIDEO_TEXSHEET_32x32)
        sprintf(buf, "  Type:    32x32");
    else if (sheet->type == VIDEO_TEXSHEET_32x64)
        sprintf(buf, "  Type:    32x64");
    else if (sheet->type == VIDEO_TEXSHEET_64x64)
        sprintf(buf, "  Type:    64x64");
    else if (sheet->type == VIDEO_TEXSHEET_ANY)
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
}

GLuint TextureController::_CreateBlankGLTexture(int32_t width, int32_t height)
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }

    if(VideoManager->CheckGLError()) {
        PRINT_ERROR << "failed to create new texture. OpenGL reported the following error: " << VideoManager->CreateGLErrorString() << std::endl;
        _DeleteTexture(tex_id);
        return INVALID_TEXTURE_ID;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    return tex_id;
}

void TextureController::_BindTexture(GLuint tex_id)
{
    // Return if this texture ID is already bound.
    if (tex_id == _last_tex_id)
        return;

    _last_tex_id = tex_id;
    glBindTexture(GL_TEXTURE_2D, tex_id);
    ++_debug_num_tex_switches;
}

void TextureController::_DeleteTexture(GLuint tex_id)
{
    if (tex_id != 0) {
        GLuint textures[] = { tex_id };
        glDeleteTextures(1, textures);
    }

    if (_last_tex_id == tex_id)
        _last_tex_id = INVALID_TEXTURE_ID;
}

bool TextureController::_SaveTempTextures()
{
    bool success = true;

    // Create the temporary directory.
    std::string path = vt_utils::GetUserDataPath() + DIRECTORY_TEMPORARY;
    if (!vt_utils::DoesFileExist(path)) {
        if (!vt_utils::MakeDirectory(path)) {
            success = false;
            IF_PRINT_WARNING(VIDEO_DEBUG) << "Unable to create the temporary directory: " << path << std::endl;
        }
    }

    // Create the temporary texture directory.
    path = vt_utils::GetUserDataPath() + DIRECTORY_TEMPORARY_TEXTURE;
    if (!vt_utils::DoesFileExist(path)) {
        if (!vt_utils::MakeDirectory(path)) {
            success = false;
            IF_PRINT_WARNING(VIDEO_DEBUG) << "Unable to create the temporary texture directory: " << path << std::endl;
        }
    }

    for (std::map<std::string, ImageTexture *>::iterator i = _images.begin(); i != _images.end(); ++i) {
        ImageTexture *image = i->second;

        // Check if the texture is temporary.
        // If it is, save the texture to disk.
        if (image->tags.find("<T>") != std::string::npos) {
            IF_PRINT_DEBUG(VIDEO_DEBUG) << "saving temporary texture " << image->filename << std::endl;

            ImageMemory buffer;
            buffer.CopyFromImage(image);

            std::string path = vt_utils::GetUserDataPath() + DIRECTORY_TEMPORARY_TEXTURE;
            if (buffer.SaveImage(path + image->filename + ".png") == false) {
                success = false;
                IF_PRINT_WARNING(VIDEO_DEBUG) << "call to ImageMemory::SaveImage() failed" << std::endl;
            }
        }
    }

    return success;
}

bool TextureController::_DeleteTempTextures()
{
    bool result = true;

    // Remove the temporary texture directory.
    std::string path = vt_utils::GetUserDataPath() + DIRECTORY_TEMPORARY_TEXTURE;
    if (vt_utils::DoesFileExist(path)) {
        if (!vt_utils::RemoveDirectory(path)) {
            result = false;
            IF_PRINT_WARNING(VIDEO_DEBUG) << "Unable to remove the temporary texture directory: " << path << std::endl;
        }
    }

    // Do not remove the temporary directory.  It is possible other
    // sections of the code could use it in the future.

    return result;
}

TexSheet *TextureController::_CreateTexSheet(int32_t width, int32_t height, TexSheetType type, bool is_static)
{
    // Validate that the function arguments are appropriate values
    if(!vt_utils::IsPowerOfTwo(width) || !vt_utils::IsPowerOfTwo(height)) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "non power-of-two width and/or height argument" << std::endl;
        return nullptr;
    }

    if(type <= VIDEO_TEXSHEET_INVALID || type >= VIDEO_TEXSHEET_TOTAL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid TexSheetType argument" << std::endl;
        return nullptr;
    }

    // Create a blank texture for the sheet to use
    GLuint tex_id = _CreateBlankGLTexture(width, height);
    if(tex_id == INVALID_TEXTURE_ID) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "failed to create a new blank OpenGL texture" << std::endl;
        return nullptr;
    }

    TexSheet *sheet = nullptr;
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
    if(sheet == nullptr) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "nullptr argument passed to function" << std::endl;
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
    if(load_info.GetWidth() > 512 || load_info.GetHeight() > 512) {
        int32_t round_width = vt_utils::RoundUpPow2(load_info.GetWidth());
        int32_t round_height = vt_utils::RoundUpPow2(load_info.GetHeight());
        TexSheet *sheet = _CreateTexSheet(round_width, round_height, VIDEO_TEXSHEET_ANY, false);

        // Ran out of memory!
        if(sheet == nullptr) {
            IF_PRINT_WARNING(VIDEO_DEBUG) << "could not create new texture sheet for image" << std::endl;
            return nullptr;
        }

        if(sheet->AddTexture(image, load_info) == true)
            return sheet;
        else {
            IF_PRINT_WARNING(VIDEO_DEBUG) << "TexSheet::AddTexture returned false when trying to insert a large image" << std::endl;
            return nullptr;
        }
    }

    // Determine the type of texture sheet that should hold this image
    TexSheetType type;

    if(load_info.GetWidth() == 32 && load_info.GetHeight() == 32)
        type = VIDEO_TEXSHEET_32x32;
    else if(load_info.GetWidth() == 32 && load_info.GetHeight() == 64)
        type = VIDEO_TEXSHEET_32x64;
    else if(load_info.GetWidth() == 64 && load_info.GetHeight() == 64)
        type = VIDEO_TEXSHEET_64x64;
    else
        type = VIDEO_TEXSHEET_ANY;

    // Look through all existing texture sheets and see if the image will fit in any of the ones which
    // match the type and static status that we are looking for
    for(uint32_t i = 0; i < _tex_sheets.size(); ++i) {
        TexSheet *sheet = _tex_sheets[i];
        if(sheet == nullptr) {
            IF_PRINT_WARNING(VIDEO_DEBUG) << "found a nullptr texture sheet in the _tex_sheets container" << std::endl;
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
    if(sheet == nullptr) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "failed to create a new texture sheet for image" << std::endl;
        return nullptr;
    }

    // AddTexture should always work here. If not, there is a serious problem
    if(sheet->AddTexture(image, load_info)) {
        return sheet;
    } else {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "all attempts to add image to a texture sheet have failed" << std::endl;
        return nullptr;
    }
}

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
                try {
                    image.Resize(img->width, img->height, false);
                }
                catch( std::exception &e ) {
                    IF_PRINT_WARNING(VIDEO_DEBUG) << "Resize failed." << std::endl;
                    success = false;
                    continue;
                }

                multi_image_info[img->filename] = std::make_pair(load_info, image);
            } else {
                load_info = multi_image_info[img->filename].first;
                image = multi_image_info[img->filename].second;
            }

            uint16_t pos0, pos1; // Used to find the start and end positions of a sub-string
            uint32_t x, y; //
            uint32_t rows, cols;

            pos0 = img->tags.find("<X", 0);
            pos1 = img->tags.find('_', pos0);
            x = atoi(img->tags.substr(pos0 + 2, pos1).c_str());

            pos0 = img->tags.find("<Y", 0);
            pos1 = img->tags.find('_', pos0);
            y = atoi(img->tags.substr(pos0 + 2, pos1).c_str());

            rows = load_info.GetHeight() / image.GetHeight();
            cols = load_info.GetWidth() / image.GetWidth();

            image.CopyFrom(load_info,
                           load_info.GetWidth() * (x * load_info.GetHeight() / rows)
                               + load_info.GetWidth() * y / cols);

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

            // Check if the image is temporary.
            // If so, retrieve the image from the temporary image directory.
            if (img->tags.find("<T>", 0) != img->tags.npos) {
                fname = vt_utils::GetUserDataPath() + DIRECTORY_TEMPORARY_TEXTURE + fname + ".png";
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
        }
    } // for (std::map<string, ImageTexture*>::iterator i = _images.begin(); i != _images.end(); i++)

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
    if(img == nullptr) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "nullptr argument passed to function" << std::endl;
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
    if(img == nullptr) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "nullptr argument passed to function" << std::endl;
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
    if(tex == nullptr) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "nullptr argument passed to function" << std::endl;
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
    if(tex == nullptr) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "nullptr argument passed to function" << std::endl;
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
