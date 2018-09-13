////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "modes/battle/command/skill_command.h"

#include "engine/video/video.h"
#include "engine/system.h"
#include "engine/input.h"

#include "common/global/global.h"

using namespace vt_gui;
using namespace vt_global;
using namespace vt_input;
using namespace vt_system;
using namespace vt_video;

namespace vt_battle
{

namespace private_battle
{

const float HEADER_POSITION_X = 140.0f;
const float HEADER_POSITION_Y = -12.0f;
const float HEADER_SIZE_X = 350.0f;
const float HEADER_SIZE_Y = 30.0f;

SkillCommand::SkillCommand(MenuWindow& window) :
    _skills(nullptr),
    _skill_list(nullptr),
    _target_n_cost_list(nullptr)
{
    _skill_header.SetOwner(&window);
    _skill_header.SetPosition(HEADER_POSITION_X, HEADER_POSITION_Y);
    _skill_header.SetDimensions(HEADER_SIZE_X, HEADER_SIZE_Y, 1, 1, 1, 1);
    _skill_header.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _skill_header.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _skill_header.SetTextStyle(TextStyle("title22"));
    _skill_header.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _skill_header.AddOption(UTranslate("Skill<R>Type SP"));
}

void SkillCommand::Initialize(std::vector<GlobalSkill *>* skills,
                              OptionBox *skill_list,
                              OptionBox *target_n_cost_list)
{
    if(skills == nullptr) {
        PRINT_WARNING << "function received nullptr skills argument" << std::endl;
        return;
    }
    if(skill_list == nullptr) {
        PRINT_WARNING << "function received nullptr skill_list argument" << std::endl;
        return;
    }

    _skills = skills;
    _skill_list = skill_list;
    _target_n_cost_list = target_n_cost_list;
}

GlobalSkill *SkillCommand::GetSelectedSkill() const
{
    if((_skills == nullptr) || (_skill_list == nullptr))
        return nullptr;

    uint32_t selection = _skill_list->GetSelection();
    // The skills object needs to be returned even if not enabled due to low SP
    // in order to print information of both, enabled and disabled skills.
    return _skills->at(selection);
}

bool SkillCommand::GetSelectedSkillEnabled()
{
    if((_skills == nullptr) || (_skill_list == nullptr))
        return false;

    uint32_t selection = _skill_list->GetSelection();
    return _skill_list->IsOptionEnabled(selection);
}

void SkillCommand::UpdateList()
{
    if(_skill_list == nullptr)
        return;

    _skill_list->Update();
    _target_n_cost_list->Update();

    if(InputManager->UpPress()) {
        _skill_list->InputUp();
        _target_n_cost_list->InputUp();
        GlobalManager->Media().PlaySound("bump");
    } else if(InputManager->DownPress()) {
        _skill_list->InputDown();
        _target_n_cost_list->InputDown();
        GlobalManager->Media().PlaySound("bump");
    }
}

void SkillCommand::DrawList()
{
    if(_skill_list == nullptr)
        return;

    _skill_header.Draw();
    _skill_list->Draw();
    _target_n_cost_list->Draw();
}

} // namespace private_battle

} // namespace vt_battle
