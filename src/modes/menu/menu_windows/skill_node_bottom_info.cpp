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

SkillNodeBottomInfo::SkillNodeBottomInfo()
{
    // Cost
    _node_cost.SetStyle(TextStyle("text18"));
    _items_cost.SetTextStyle(TextStyle("text20"));
    _items_cost.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _items_cost.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _items_cost.SetSelectMode(vt_gui::VIDEO_SELECT_SINGLE);
    _items_cost.SetCursorState(vt_gui::VIDEO_CURSOR_STATE_HIDDEN);
    _items_cost.SetCursorOffset(-58.0f, -18.0f);

    _skill_name.SetStyle(TextStyle("text20"));
    _skill_sp_cost.SetStyle(TextStyle("text18"));
    _skill_description.SetStyle(TextStyle("text18"));
    _skill_description.SetWordWrapWidth(350);
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
    VideoManager->MoveRelative(5.0f, 30.0f);
    _node_cost.Draw();
    VideoManager->MoveRelative(0.0f, 30.0f);
    _items_cost.SetPosition(x_left + 5.0f, y_top + 60.0f);
    _items_cost.Draw();

    // Draw skill block if present
    VideoManager->MoveRelative(150.0f, -60.0f);
    if (!_skill_learned_text.GetString().empty()) {
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
    _items_cost.SetDimensions(200.0f, 10.0f * items_nb, 1, items_nb, 1, items_nb);
    for (auto pair : items_needed) {
        _items_cost.AddOption(vt_utils::MakeUnicodeString(vt_system::VTranslate("id: %d x %d", pair.first, pair.second)));
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
