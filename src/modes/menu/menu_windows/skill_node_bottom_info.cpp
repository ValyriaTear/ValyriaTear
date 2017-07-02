///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "skill_node_bottom_info.h"

#include "engine/video/video.h"

#include "common/global/global.h"

using namespace vt_video;
using namespace vt_global;
using namespace vt_utils;
using namespace vt_system;

SkillNodeBottomInfo::SkillNodeBottomInfo() :
    _x_pos(0.0f),
    _y_pos(0.0f)
{
    // Cost
    _node_cost.SetStyle(TextStyle("text18"));
    _items_cost.SetTextStyle(TextStyle("text14"));
    _items_cost.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _items_cost.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _items_cost.SetSelectMode(vt_gui::VIDEO_SELECT_SINGLE);
    _items_cost.SetCursorState(vt_gui::VIDEO_CURSOR_STATE_HIDDEN);
    _items_cost.SetCursorOffset(-58.0f, -18.0f);

    // Skill
    _skill_name.SetStyle(TextStyle("text20"));
    _skill_sp_cost.SetStyle(TextStyle("text18"));
    _skill_description.SetStyle(TextStyle("text14"));
    _skill_description.SetWordWrapWidth(300);

    // Stats
    _stats_upgrade.SetTextStyle(TextStyle("text18"));
    _stats_upgrade.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _stats_upgrade.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _stats_upgrade.SetSelectMode(vt_gui::VIDEO_SELECT_SINGLE);
    _stats_upgrade.SetCursorState(vt_gui::VIDEO_CURSOR_STATE_HIDDEN);
    _stats_upgrade.SetCursorOffset(-58.0f, -18.0f);
}

void SkillNodeBottomInfo::SetNode(const vt_global::SkillNode& node)
{
    _SetCostInfo(node.GetExperiencePointsNeeded(),
                 node.GetItemsNeeded());

    _SetSkillInfo(node.GetSkillIdLearned());

    _SetStatsInfo(node.GetStatsUpgrades());
}

void SkillNodeBottomInfo::SetPosition(float x_left, float y_top)
{
    _x_pos = x_left;
    _y_pos = y_top;

    // Update cost item list position
    _items_cost.SetPosition(x_left, y_top + 55.0f);
}

void SkillNodeBottomInfo::Draw()
{
    VideoManager->PushState();

    VideoManager->Move(_x_pos, _y_pos);
    // Draw the cost block if not unlocked
    // TODO: Handle the unlocked case
    _cost_title.Draw();
    VideoManager->MoveRelative(2.0f, 30.0f);
    _node_cost.Draw();
    VideoManager->MoveRelative(0.0f, 30.0f);
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

        // Prepare statistics location
        VideoManager->MoveRelative(320.0f, -80.0f);
    }
    else {
        VideoManager->MoveRelative(220.0f, -60.0f);
    }

    if (_stats_upgrade.GetNumberOptions() > 0) {
        _stats_title.Draw();
        VideoManager->MoveRelative(0.0f, 30.0f);
        _stats_upgrade.Draw();
    }

    VideoManager->PopState();
}

void SkillNodeBottomInfo::_SetCostInfo(uint32_t exp_points_needed,
                                       const std::vector<std::pair<uint32_t, uint32_t> >& items_needed)
{
    // Done this way to ensure the right translation
    _cost_title.SetText(vt_system::UTranslate("To Unlock:"));

    if (exp_points_needed > 0)
        _node_cost.SetText(vt_system::VTranslate("XP Needed: %d",
                                                 exp_points_needed));
    else
        _node_cost.SetText(vt_system::UTranslate("No XP Needed"));

    // Update list dimension according to the number of items
    _items_cost.ClearOptions();
    uint32_t items_nb = items_needed.size();
    _items_cost.SetDimensions(100.0f, 30.0f * items_nb,
                              1, items_nb, 1, items_nb);
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

        // Update stats list position
        _stats_upgrade.SetPosition(_x_pos + 220.0f, _y_pos + 30.0f);
        return;
    }

    // Update current skill data
    _skill_name.SetText(skill->GetName());
    _skill_description.SetText(skill->GetDescription());
    if (_skill_icon.Load(skill->GetIconFilename()))
        _skill_icon.SetWidthKeepRatio(50.0f);
    _skill_sp_cost.SetText(vt_system::VTranslate("%d SP",
                                                 skill->GetSPRequired()));

    // Set title. This is done thqt way to ensure correct translation
    _skill_learned_text.SetText(vt_system::UTranslate("Skill learned:"));

    // Update stats list position
    _stats_upgrade.SetPosition(_x_pos + 540.0f, _y_pos + 30.0f);
}

void SkillNodeBottomInfo::_SetStatsInfo(const std::vector<std::pair<uint32_t, uint32_t> >& stats_upgrades)
{
    // Done this way to ensure the right translation
    _stats_title.SetText(vt_system::UTranslate("Statistics:"));

    // Update list dimension according to the numberof items
    _stats_upgrade.ClearOptions();
    uint32_t stat_nb = stats_upgrades.size();
    _stats_upgrade.SetDimensions(280.0f, 30.0f * stat_nb,
                                 2, stat_nb, 2, stat_nb);

    size_t list_index = 0;
    for (auto stat_info : stats_upgrades) {
        uint32_t stat_id = stat_info.first;
        uint32_t stat_upgrade = stat_info.second;

        ustring stat_text;
        switch (stat_id) {
            default:
                PRINT_WARNING << "Invalid stat id: " << stat_id << std::endl;
                ++list_index;
                continue;
                break;
            case GLOBAL_STATUS_PHYS_ATK:
                stat_text = MakeUnicodeString(VTranslate("Phys. Atk +%d",
                                                         stat_upgrade));
                break;
            case GLOBAL_STATUS_MAG_ATK:
                stat_text = MakeUnicodeString(VTranslate("Mag. Atk +%d",
                                                         stat_upgrade));
                break;
            case GLOBAL_STATUS_PHYS_DEF:
                stat_text = MakeUnicodeString(VTranslate("Phys. Def +%d",
                                                         stat_upgrade));
                break;
            case GLOBAL_STATUS_MAG_DEF:
                stat_text = MakeUnicodeString(VTranslate("Mag. Def +%d",
                                                         stat_upgrade));
                break;
            case GLOBAL_STATUS_STAMINA:
                stat_text = MakeUnicodeString(VTranslate("Stamina +%d",
                                                         stat_upgrade));
                break;
            case GLOBAL_STATUS_EVADE:
                stat_text = MakeUnicodeString(VTranslate("Evade +%.1f%%",
                                                         static_cast<float>(stat_upgrade) / 10.0f));
                break;
            case GLOBAL_STATUS_HP:
                stat_text = MakeUnicodeString(VTranslate("HP +%d",
                                                         stat_upgrade));
                break;
            case GLOBAL_STATUS_SP:
                stat_text = MakeUnicodeString(VTranslate("SP +%d",
                                                         stat_upgrade));
                break;
        }

        _stats_upgrade.AddOption();

        // Set the image
        GlobalMedia& media = GlobalManager->Media();
        StillImage* img =
            media.GetStatusIcon(static_cast<GLOBAL_STATUS>(stat_id),
                                GLOBAL_INTENSITY_NEUTRAL);
        if (img)
            _stats_upgrade.AddOptionElementImage(list_index, img);

        // Get back the image copy
        img = _stats_upgrade.GetEmbeddedImage(list_index);
        if (img)
            img->SetDimensions(30.0f, 30.0f);

        _stats_upgrade.AddOptionElementPosition(list_index, 30);
        _stats_upgrade.AddOptionElementText(list_index, stat_text);

        ++list_index;
    }
}
