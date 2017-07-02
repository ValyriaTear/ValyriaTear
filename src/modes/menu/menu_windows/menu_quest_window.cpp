///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_windows/menu_quest_window.h"

#include "modes/menu/menu_mode.h"

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/system.h"

using namespace vt_menu::private_menu;
using namespace vt_utils;
using namespace vt_audio;
using namespace vt_video;
using namespace vt_gui;
using namespace vt_global;
using namespace vt_input;
using namespace vt_system;

namespace vt_menu
{

namespace private_menu
{

QuestWindow::QuestWindow():
    _location_image(nullptr),
    _location_subimage(nullptr)
{
    _quest_description.SetPosition(445, 130);
    _quest_description.SetDimensions(455, 200);
    _quest_description.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _quest_description.SetTextStyle(TextStyle("text20"));
    _quest_description.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);

    // preferably, we want the completion description
    // to show underneath the description text last line.
    // Unfortunatly, it seems CalculateTextHeight() doesn't work right
    // so we fix the position for now
    _quest_completion_description.SetPosition(445, 350);
    _quest_completion_description.SetDimensions(455, 200);
    _quest_completion_description.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _quest_completion_description.SetTextStyle(TextStyle("text20", Color::aqua));
    _quest_completion_description.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);

    _location_name.SetPosition(102, 556);
    _location_name.SetDimensions(500.0f, 150.0f);
    _location_name.SetTextStyle(TextStyle("text22"));
    _location_name.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _location_name.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);

    _location_subname.SetPosition(500, 556);
    _location_subname.SetDimensions(500.0f, 150.0f);
    _location_subname.SetTextStyle(TextStyle("text22"));
    _location_subname.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _location_subname.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
}

void QuestWindow::Draw()
{
    MenuWindow::Draw();
    Update();
    if(MenuMode::CurrentInstance()->_quest_list_window.IsActive()) {
        _quest_description.Draw();
        _quest_completion_description.Draw();
    }
}

void QuestWindow::DrawBottom()
{
    // Display Location name information
    _location_name.Draw();
    _location_subname.Draw();

    //check location image and draw
    if(_location_image != nullptr && !_location_image->GetFilename().empty()) {
        VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_BOTTOM, 0);
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
        VideoManager->Move(102, 685);
        _location_image->Draw();
    }

    //check location subimage and draw
    if(_location_subimage != nullptr && !_location_subimage->GetFilename().empty()) {
        VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_BOTTOM, 0);
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
        VideoManager->Move(500, 685);
        _location_subimage->Draw();
    }
}

void QuestWindow::Update()
{
    MenuWindow::Update();

    // Check to see if the id is empty or if the quest doesn't exist.
    // if so, draw an empty space
    if(_viewing_quest_id.empty()) {
        _quest_description.ClearText();
        _location_name.ClearText();
        _location_subname.ClearText();
        _location_image = nullptr;
        _location_subimage = nullptr;
        return;
    }

    // otherwise, put the text description for the quest in
    // Not calling ClearText each time will permit to set up the textbox text only when necessary
    const QuestLogInfo& info = GlobalManager->GetQuestInfo(_viewing_quest_id);
    if(!info._description.empty())
    {
        _quest_description.SetDisplayText(info._description);
        _location_name.SetDisplayText(info._location_name);
        _location_subname.SetDisplayText(info._location_subname);
        _location_image = &info._location_image;
        _location_subimage = &info._location_subimage;

        //set the completion description only if the quest is completed
        if(GlobalManager->IsQuestCompleted(_viewing_quest_id))
            _quest_completion_description.SetDisplayText(info._completion_description);
        else
            _quest_completion_description.ClearText();
    }
}

} // namespace private_menu

} // namespace vt_menu
