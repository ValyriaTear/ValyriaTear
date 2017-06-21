///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_NODE_BOTTOM_INFO__
#define __SKILL_NODE_BOTTOM_INFO__

#include "common/global/skill_tree/skill_node.h"

#include "engine/video/text.h"
#include "common/gui/option.h"

//! \brief Contains all the necessary info to display
//! bottom panel data related to a given node.
class SkillNodeBottomInfo {

public:
    SkillNodeBottomInfo();

    SkillNodeBottomInfo(const vt_global::SkillNode& node) {
        SetNode(node);
    }

    //! \brief Clear displayed info
    void Clear();

    //! \brief Set info corresponding to the given skill node.
    void SetNode(const vt_global::SkillNode& node);

    //! \brief Draw the panel info using the given topleft coordinates.
    void Draw(float x_left, float y_top);

private:
    /// Cost
    //! \brief cost title
    vt_video::TextImage _cost_title;

    //! \brief experience cost text
    vt_video::TextImage _node_cost;

    //! \brief The items needed to unlock the node
    vt_gui::OptionBox _items_cost;

    /// Skill obtained
    //! \brief Skill learned title
    vt_video::TextImage _skill_learned_text;

    //! \brief The skill icon
    vt_video::StillImage _skill_icon;

    //! \brief The translated skill name
    vt_video::TextImage _skill_name;

    //! \brief The translated skill name
    vt_video::TextImage _skill_description;

    //! \brief The skill sp cost text
    vt_video::TextImage _skill_sp_cost;

    //! \brief Updates cost info
    void _SetCostInfo(uint32_t exp_points_needed,
                      const std::vector<std::pair<uint32_t, uint32_t> >& items_needed);

    //! \brief Updates skill info
    void _SetSkillInfo(int32_t new_skill_id);

    /// Stats
    //! \brief The stats upgrade list.
    vt_gui::OptionBox _stats_upgrade;
};

#endif // __SKILL_NODE_BOTTOM_INFO__
