////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_GRAPH_HEADER__
#define __SKILL_GRAPH_HEADER__

#include "skill_node.h"

#include "engine/script/script_read.h"

namespace vt_global {

/** *****************************************************************************
*** \brief Skill graph manager
*** Handles related data management of skill nodes in the skill graph.
*** *****************************************************************************/
class SkillGraph
{
public:
    SkillGraph()
    {}

    ~SkillGraph() {
        Clear();
    }

    //! \brief Initialize the skill graph data.
    //! \returns true if everthing went fine.
    bool Initialize(const std::string& skill_graph_file);

    void Clear() {
        for (SkillNode* node : _skill_graph_data) {
            delete node;
        }
        _skill_graph_data.clear();
    }

    //! \brief Returns the skill node corresponding to the desired id,
    //! or nullptr is not found.
    SkillNode* GetSkillNode(uint32_t skill_node_id);

    //! \brief Returns the skill tree data
    const std::vector<SkillNode*>& GetSkillNodes() const {
        return _skill_graph_data;
    }

    //! \brief Gets the character's starting skill node id.
    //! \param character_id The given character id.
    //! \returns the given starting skill node id
    //! or uint32_t max if not found.
    uint32_t GetStartingSkillNodeId(uint32_t character_id) const;

private:
    //! \brief The vector of skill nodes.
    std::vector<SkillNode*> _skill_graph_data;

    //! \brief Contains the starting node ids per character ids.
    //! Contains <character_id, skill_node_id>
    std::map<uint32_t, uint32_t> _starting_node_ids;

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

#endif // __SKILL_GRAPH_HEADER__
