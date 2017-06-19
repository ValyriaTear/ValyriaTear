////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_TREE_HEADER__
#define __SKILL_TREE_HEADER__

#include "skill_node.h"

#include "engine/script/script_read.h"

namespace vt_global {

/** *****************************************************************************
*** \brief Skill tree manager
*** Handles related data management of skill nodes in the skill tree.
*** *****************************************************************************/
class SkillTree
{
public:
    SkillTree()
    {}

    ~SkillTree() {
        Clear();
    }

    //! \brief Initialize the skill tree data.
    //! \returns true if everthing went fine.
    bool Initialize(const std::string& skill_tree_file);

    void Clear() {
        for (SkillNode* node : _skill_tree_data) {
            delete node;
        }
        _skill_tree_data.clear();
    }

    //! \brief Returns the skill node corresponding to the desired id,
    //! or nullptr is not found.
    SkillNode* GetSkillNode(uint32_t skill_node_id);

    //! \brief Returns the skill tree data
    const std::vector<SkillNode*>& GetSkillNodes() const {
        return _skill_tree_data;
    }

private:
    //! \brief The vector of skill nodes.
    std::vector<SkillNode*> _skill_tree_data;

    //! \brief Read item data and add them in the skill node data
    void _ReadItemsNeeded(vt_script::ReadScriptDescriptor& script,
                          SkillNode* skill_node);

    //! \brief Read stat upgrades and add them in the skill node data
    void _ReadStatsUpgrades(vt_script::ReadScriptDescriptor& script,
                            SkillNode* skill_node);

    //! \brief Read node links and add them in the skill node data
    void _ReadNodeLinks(vt_script::ReadScriptDescriptor& script,
                        SkillNode* skill_node);
};

} // namespace vt_global

#endif // __SKILL_TREE_HEADER__
