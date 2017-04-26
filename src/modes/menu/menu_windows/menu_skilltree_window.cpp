///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_windows/menu_skilltree_window.h"

#include "modes/menu/menu_mode.h"

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/system.h"

#include "common/global/skill_tree/skill_tree.h"

#include <limits>

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

// Area where on can draw the skill tree nodes
const float SKILL_TREE_AREA_WIDTH = 950.0f;
const float SKILL_TREE_AREA_HEIGHT = 550.0f;

SkillTreeWindow::SkillTreeWindow() :
    _selected_character_id(std::numeric_limits<uint32_t>::max()), // Invalid id
    _current_x_offset(-1.0f), // Invalid view
    _current_y_offset(-1.0f),
    _selected_node_index(std::numeric_limits<uint32_t>::max()), // Invalid index
    _active(false)
{
    // TODO
    _location_marker.SetStatic(true);
    if(!_location_marker.LoadFromAnimationScript("data/gui/menus/rotating_crystal_grey.lua"))
        PRINT_ERROR << "Could not load marker image!" << std::endl;

    _location_pointer.SetStatic(true);
    if(!_location_pointer.Load("data/gui/menus/hand_down.png"))
        PRINT_ERROR << "Could not load pointer image!" << std::endl;
}

void SkillTreeWindow::Activate(bool new_state)
{
    _active = new_state;

    // TODO: Set the selection node to where the character was last located.
    // If the character is unset, set the default node
    if (_selected_character_id == std::numeric_limits<uint32_t>::max())
        _selected_node_index = 0;

    // Set view on node
    _ResetSkillTreeView();
}

void SkillTreeWindow::Update()
{
    if (!_active)
        return;

    // TODO
}

void SkillTreeWindow::Draw()
{
    if(!_active)
        return;

    // TODO: Add background window
    //MenuWindow::Draw();
    //float window_position_x, window_position_y;
    //GetPosition(window_position_x, window_position_y);
    //VideoManager->Move(window_position_x + _current_x_offset, window_position_y + _current_y_offset);

    // Draw the visible lines
    for (NodeLine node_line : _displayed_node_links) {
        vt_video::VideoManager->DrawLine(node_line.x1, node_line.y1, 2,
                                         node_line.x1, node_line.y2, 2,
                                         vt_video::Color::white);
    }

    // Draw the visible skill nodes
    for (SkillNode* skill_node : _displayed_skill_nodes) {
        vt_video::VideoManager->Move(skill_node->GetXLocation() - _current_x_offset,
                                     skill_node->GetYLocation() - _current_y_offset);
        skill_node->GetIconImage().Draw();
    }
}

void SkillTreeWindow::SetCharacter(vt_global::GlobalCharacter& character)
{
    // Set base data
    _selected_character_id = character.GetID();
    _character_icon = character.GetStaminaIcon();

    // FIXME: Load character current position or start position for now
    // e.g: _selected_node_index = character.GetLatestNodeLocationId();
    _selected_node_index = 0;

    // Set view on node
    _ResetSkillTreeView();
}

void SkillTreeWindow::_ResetSkillTreeView()
{
    // Set current offset based on the currently selected node
    SkillTree& skill_tree = vt_global::GlobalManager->GetSkillTree();
    SkillNode* current_skill_node = skill_tree.GetSkillNode(_selected_node_index);
    // If the node is invalid, try the default one.
    if (current_skill_node == nullptr) {
        current_skill_node = skill_tree.GetSkillNode(0);
        _selected_node_index = 0;
    }

    // If the default one fails, set an empty view
    if (current_skill_node == nullptr) {
        _current_x_offset = -1.0f;
        _current_y_offset = -1.0f;
        _selected_node_index = std::numeric_limits<uint32_t>::max();
        return;
    }

    _current_x_offset = current_skill_node->GetXLocation();
    _current_y_offset = current_skill_node->GetYLocation();

    _UpdateSkillTreeView();
}

void SkillTreeWindow::_UpdateSkillTreeView()
{
    // Check to prevent invalid updates
    if (_selected_node_index == std::numeric_limits<uint32_t>::max())
        return;

    float min_x_view = _current_x_offset - (SKILL_TREE_AREA_WIDTH / 2.0f);
    float max_x_view = _current_x_offset + (SKILL_TREE_AREA_WIDTH / 2.0f);
    float min_y_view = _current_y_offset - (SKILL_TREE_AREA_HEIGHT / 2.0f);
    float max_y_view = _current_y_offset + (SKILL_TREE_AREA_HEIGHT / 2.0f);

    // Based on current offset, reload visible nodes
    _displayed_skill_nodes.clear();
    SkillTree& skill_tree = vt_global::GlobalManager->GetSkillTree();
    auto skill_nodes = skill_tree.GetSkillNodes();
    for (SkillNode& skill_node : skill_nodes) {
        float node_x = skill_node.GetXLocation();
        float node_y = skill_node.GetYLocation();
        if (node_x > min_x_view && node_x < max_x_view
            && node_y > min_y_view && node_y < max_y_view) {
            _displayed_skill_nodes.push_back(&skill_node);
        }
    }

    // Prepare lines coordinates for draw time
    _displayed_node_links.clear();
    // default ones (white)
    for (SkillNode* skill_node : _displayed_skill_nodes) {
        auto node_links = skill_node->GetNodeLinks();
        // Don't load anything if there are no links
        if (node_links.empty())
            continue;

        // Load line start
        NodeLine node_line;
        node_line.x1 = skill_node->GetXLocation() - _current_x_offset;
        node_line.y1 = skill_node->GetYLocation() - _current_y_offset;

        // For each link, add a line end
        for (uint32_t link_id : node_links) {
            SkillNode* linked_node = skill_tree.GetSkillNode(link_id);
            if (!linked_node)
                continue;

            node_line.x2 = linked_node->GetXLocation() - _current_x_offset;
            node_line.y2 = linked_node->GetYLocation() - _current_y_offset;

            _displayed_node_links.push_back(node_line);
        }
    }

    // TODO colored lines per characters for paths done by them.
}

} // namespace private_menu

} // namespace vt_menu
