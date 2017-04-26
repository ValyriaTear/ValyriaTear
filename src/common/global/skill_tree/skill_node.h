////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_NODE_HEADER__
#define __SKILL_NODE_HEADER__

#include "engine/video/image.h"

namespace vt_global {

/** *****************************************************************************
*** \brief Skill node data
*** Handles related data management to a given skill node.
*** *****************************************************************************/
class SkillNode
{
friend class SkillTree;

public:
    SkillNode(uint32_t id,
              float x_location,
              float y_location,
              const std::string& icon_path,
              uint32_t skill_points_needed,
              uint32_t skill_id_learned);

    //! \brief Gets skill node id
    uint32_t GetId() const {
        return _id;
    }

    float GetXLocation() const {
        return _x_location;
    }

    float GetYLocation() const {
        return _y_location;
    }

    const vt_video::StillImage& GetIconImage() const {
        return _icon_image;
    }

    //! \brief Gets skill id learned, or -1 if none.
    uint32_t GetSkillIdLearned() const {
        return _skill_id_learned;
    }

    uint32_t GetSkillPointsNeeded() const {
        return _skill_points_needed;
    }

    const std::vector<std::pair<uint32_t, uint32_t> >& GetItemsNeeded() const {
        return _items_needed;
    }

    const std::vector<std::pair<uint32_t, uint32_t> >& GetStatsUpgrades() const {
        return _items_needed;
    }

    const std::vector<uint32_t>& GetNodeLinks() const {
        return _nodes_links;
    }

    void AddNeededItem(uint32_t item_id, uint32_t item_number);

    void AddStatUpgrade(uint32_t stat, uint32_t upgrade);

    void AddNodeLink(uint32_t node_id);

private:
    //! \brief The Skill Node Id
    uint32_t _id;

    //! \brief Location in the skill tree
    float _x_location;
    float _y_location;

    //! \brief Icon used to represent this node.
    vt_video::StillImage _icon_image;

    //! \brief Skill points needed to reach this node
    uint32_t _skill_points_needed;

    //! \brief Skill learned, or -1 if none
    uint32_t _skill_id_learned;

    //! \brief The list of items needs to reach the node
    std::vector<std::pair<uint32_t, uint32_t> > _items_needed;

    //! \brief The list of stats upgrade the character gets when reaching the node
    std::vector<std::pair<uint32_t, uint32_t> > _stats_upgrades;

    //! \brief Permited links to other nodes
    std::vector<uint32_t> _nodes_links;
};

} // namespace vt_global

#endif // __SKILL_NODE_HEADER__
