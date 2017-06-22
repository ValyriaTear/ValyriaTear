///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "skill_node_bottom_info.h"

#include "engine/video/video.h"

#include "common/global/global_skills.h"

using namespace vt_video;
using namespace vt_global;
using namespace vt_utils;
using namespace vt_system;

SkillNodeBottomInfo::SkillNodeBottomInfo()
{
    // Cost
    _node_cost.SetStyle(TextStyle("text18"));
    _items_cost.SetTextStyle(TextStyle("text14"));
    _items_cost.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _items_cost.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _items_cost.SetSelectMode(vt_gui::VIDEO_SELECT_SINGLE);
    _items_cost.SetCursorState(vt_gui::VIDEO_CURSOR_STATE_HIDDEN);
    _items_cost.SetCursorOffset(-58.0f, -18.0f);

    _skill_name.SetStyle(TextStyle("text20"));
    _skill_sp_cost.SetStyle(TextStyle("text18"));
    _skill_description.SetStyle(TextStyle("text14"));
    _skill_description.SetWordWrapWidth(300);
}

void SkillNodeBottomInfo::Clear()
{
    _skill_name.Clear();
    _skill_description.Clear();
    _skill_icon.Clear();
}

void SkillNodeBottomInfo::SetNode(const vt_global::SkillNode& node)
{
    _SetCostInfo(node.GetExperiencePointsNeeded(),
                 node.GetItemsNeeded());
    _SetSkillInfo(node.GetSkillIdLearned());

    auto stats = node.GetStatsUpgrades();
    //if (stats.empty())
    //    return;
}

void SkillNodeBottomInfo::Draw(float x_left, float y_top)
{
    VideoManager->PushState();

    VideoManager->Move(x_left, y_top);
    // Draw the cost block if not unlocked
    // TODO: Handle the unlocked case
    _cost_title.Draw();
    VideoManager->MoveRelative(2.0f, 30.0f);
    _node_cost.Draw();
    VideoManager->MoveRelative(0.0f, 30.0f);
    _items_cost.SetPosition(x_left, y_top + 55.0f);
    _items_cost.Draw();

    // Draw skill block if present
    if (!_skill_learned_text.GetString().empty()) {
        VideoManager->MoveRelative(220.0f, -60.0f);
        _skill_learned_text.Draw();
        VideoManager->MoveRelative(5.0f, 30.0f);
        _skill_icon.Draw();
        VideoManager->MoveRelative(_skill_icon.GetWidth() + 5.0f, 0.0f);
        _skill_name.Draw();
        VideoManager->MoveRelative(0.0f, 20.0f);
        _skill_sp_cost.Draw();
        VideoManager->MoveRelative(-_skill_icon.GetWidth() - 5.0f, 30.0f);
        _skill_description.Draw();
    }

    VideoManager->PopState();
}

void SkillNodeBottomInfo::_SetCostInfo(uint32_t exp_points_needed,
                                       const std::vector<std::pair<uint32_t, uint32_t> >& items_needed)
{
    // Done this way to ensure the right translation
    _cost_title.SetText(vt_system::UTranslate("To Unlock:"));

    if (exp_points_needed > 0)
        _node_cost.SetText(vt_system::VTranslate("XP Needed: %d", exp_points_needed));
    else
        _node_cost.SetText(vt_system::UTranslate("No XP Needed"));

    // Update list dimension according to the numberof items
    _items_cost.ClearOptions();
    uint32_t items_nb = items_needed.size();
    _items_cost.SetDimensions(100.0f, 30.0f * items_nb, 1, items_nb, 1, items_nb);
    size_t list_index = 0;
    for (auto item_info : items_needed) {
        uint32_t item_id = item_info.first;
        uint32_t item_number = item_info.second;

        // Create a global object to get info from.
        std::shared_ptr<GlobalObject> item = GlobalCreateNewObject(item_id, 1);
        if (!item) {
            PRINT_WARNING << "Invalid Skill node (item id: "
                          << item_id << ")" << std::endl;
            ++list_index;
            continue;
        }

        ustring item_nb_text = MakeUnicodeString(vt_system::VTranslate(" x %d", item_number));
        if (item->GetIconImage().GetFilename().empty()) {
            _items_cost.AddOption(MakeUnicodeString("<30>")
                                  + item->GetName()
                                  + item_nb_text);
        }
        else {
            _items_cost.AddOption(MakeUnicodeString("<" + item->GetIconImage().GetFilename() + "><30>")
                                  + item->GetName()
                                  + item_nb_text);
        }

        StillImage* img = _items_cost.GetEmbeddedImage(list_index);
        if (img)
            img->SetDimensions(30.0f, 30.0f);

        ++list_index;
    }
}

void SkillNodeBottomInfo::_SetSkillInfo(int32_t skill_id)
{
    // Don't draw anything for empty skills
    std::unique_ptr<GlobalSkill> skill(new GlobalSkill(skill_id));
    if (!skill->IsValid()) {
        _skill_learned_text.Clear();
        _skill_name.Clear();
        _skill_description.Clear();
        _skill_icon.Clear();
        _skill_sp_cost.Clear();
        return;
    }

    // Update current skill data
    _skill_name.SetText(skill->GetName());
    _skill_description.SetText(skill->GetDescription());
    if (_skill_icon.Load(skill->GetIconFilename()))
        _skill_icon.SetWidthKeepRatio(50.0f);
    _skill_sp_cost.SetText(vt_system::VTranslate("%d SP", skill->GetSPRequired()));

    // Set title. This is done thqt way to ensure correct translation
    _skill_learned_text.SetText(vt_system::UTranslate("Skill learned:"));
}
