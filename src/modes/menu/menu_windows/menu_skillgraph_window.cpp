///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_windows/menu_skillgraph_window.h"

#include "modes/menu/menu_mode.h"

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/system.h"

#include "common/global/skill_graph/skill_graph.h"

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

//! \brief Area where on can draw the skill tree nodes
const float SKILL_GRAPH_AREA_WIDTH = 815.0f;
const float SKILL_GRAPH_AREA_HEIGHT = 415.0f;
const float WINDOW_BORDER_WIDTH = 18.0f;
const float NODES_DISPLAY_MARGIN = 100.0f;
const vt_video::Color grayed_path = vt_video::Color(0.5f, 0.5f, 0.5f, 0.2f);

//! \brief Top left bottom menu position
const float BOTTOM_MENU_X_POS = 90.0f;
const float BOTTOM_MENU_Y_POS = 565.0f;

SkillGraphWindow::SkillGraphWindow() :
    _skillgraph_state(SKILLGRAPH_STATE_NONE),
    _selected_character_id(std::numeric_limits<uint32_t>::max()), // Invalid id
    _current_x_offset(-1.0f), // Invalid view
    _current_y_offset(-1.0f),
    _view_x_position(0.0f),
    _view_y_position(0.0f),
    _selected_node_index(std::numeric_limits<uint32_t>::max()), // Invalid index
    _character_node_index(std::numeric_limits<uint32_t>::max()), // Invalid index
    _active(false)
{
    _location_pointer.SetStatic(true);
    if(!_location_pointer.Load("data/gui/menus/hand_down.png"))
        PRINT_ERROR << "Could not load pointer image!" << std::endl;

    _bottom_info.SetPosition(BOTTOM_MENU_X_POS, BOTTOM_MENU_Y_POS);

    _InitCharSelect();

    // We set them here so that they are re-translated when changing the language.
    _select_character_text.SetText(UTranslate("Choose a character."),
                                   TextStyle("text20"));
}

void SkillGraphWindow::SetActive(bool is_active_state)
{
    _active = is_active_state;

    // Activate window and first option box...or deactivate both
    if(_active) {
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
        _skillgraph_state = SKILLGRAPH_STATE_CHAR;
    } else {
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        _skillgraph_state = SKILLGRAPH_STATE_NONE;
        return;
    }
}

void SkillGraphWindow::Update()
{
    if (!_active)
        return;

    switch (_skillgraph_state) {
    // Do nothing in default state
    default:
    case SKILLGRAPH_STATE_NONE:
        return;
        break;
    case SKILLGRAPH_STATE_CHAR:
        _UpdateSkillCharacterSelectState();
        break;
    case SKILLGRAPH_STATE_LIST:
        _UpdateSkillGraphListState();
        break;
    }
}

void SkillGraphWindow::Draw()
{
    // Background window
    MenuWindow::Draw();

    switch (_skillgraph_state) {
    // Do nothing in default state
    default:
    case SKILLGRAPH_STATE_NONE:
        return;
        break;
    case SKILLGRAPH_STATE_CHAR:
        _DrawCharacterState();
        break;
    case SKILLGRAPH_STATE_LIST:
        _DrawSkillGraphState();
        break;
    }
}

void SkillGraphWindow::DrawBottomWindow()
{
    switch (_skillgraph_state) {
    // Do nothing in default state
    default:
    case SKILLGRAPH_STATE_NONE:
        return;
        break;
    case SKILLGRAPH_STATE_CHAR:
        VideoManager->Move(BOTTOM_MENU_X_POS, BOTTOM_MENU_Y_POS);
        _select_character_text.Draw();
        break;
    case SKILLGRAPH_STATE_LIST:
        _bottom_info.Draw();
        break;
    }
}

void SkillGraphWindow::SetCharacter(vt_global::GlobalCharacter& character)
{
    // Set base data
    _selected_character_id = character.GetID();
    _character_icon = character.GetStaminaIcon();

    // Set the selection node to where the character was last located.
    _selected_node_index = character.GetSkillNodeLocation();
    _character_node_index = _selected_node_index;
}

void SkillGraphWindow::_InitCharSelect()
{
    //character selection set up
    std::vector<ustring> options;
    uint32_t size = GlobalManager->GetActiveParty()->GetPartySize();

    _char_select.SetPosition(72.0f, 109.0f);
    _char_select.SetDimensions(360.0f, 432.0f, 1, 4, 1, 4);
    _char_select.SetCursorOffset(-50.0f, -6.0f);
    _char_select.SetTextStyle(TextStyle("text20"));
    _char_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _char_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _char_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

    //Use blank strings....won't be seen anyway
    for(uint32_t i = 0; i < size; i++) {
        options.push_back(MakeUnicodeString(" "));
    }

    //Set options, selection and cursor state
    _char_select.SetOptions(options);
    _char_select.SetSelection(0);
    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}

void SkillGraphWindow::_UpdateSkillCharacterSelectState()
{
    _char_select.Update();

    if(InputManager->CancelPress()) {
        SetActive(false);
        return;
    }
    if (InputManager->UpPress()) {
        _char_select.InputUp();
    }
    else if (InputManager->DownPress()) {
        _char_select.InputDown();
    }
    else if (InputManager->ConfirmPress()) {
        _char_select.InputConfirm();
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

        // If the character is unset, set the default node
        GlobalCharacter* character =
            GlobalManager->GetActiveParty()->GetCharacterAtIndex(_char_select.GetSelection());
        if (!character) {
            _selected_character_id = std::numeric_limits<uint32_t>::max();
            _selected_node_index = 0;
            return;
        }

        SetCharacter(*character);
        _skillgraph_state = SKILLGRAPH_STATE_LIST;

        // Set view on node
        _ResetSkillGraphView();
    }
}

void SkillGraphWindow::_UpdateSkillGraphListState()
{
    if(InputManager->CancelPress()) {
        _skillgraph_state = SKILLGRAPH_STATE_CHAR;
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
        return;
    }

    _UpdateSkillGraphView();

    // TODO: Handle the appropriate input events
    if (InputManager->LeftPress()) {
        if (_selected_node_index > 0) {
            --_selected_node_index;
        }
    }
    else if (InputManager->RightPress()) {
        ++_selected_node_index;
    }
    else {
        // Only update when necessary
        return;
    }

    SkillGraph& skill_graph = vt_global::GlobalManager->GetSkillGraph();
    SkillNode* current_skill_node = skill_graph.GetSkillNode(_selected_node_index);

    // Cancel right press if not possible
    if (!current_skill_node && InputManager->RightPress())
        --_selected_node_index;

    // Update bottom windows info
    if (current_skill_node)
        _bottom_info.SetNode(*current_skill_node);
}

void SkillGraphWindow::_DrawCharacterState()
{
    _char_select.Draw();
}

void SkillGraphWindow::_DrawSkillGraphState()
{
    // Scissor the view to cut the layout properly
    float left = GetXPosition() + WINDOW_BORDER_WIDTH;
    float top = GetYPosition() + WINDOW_BORDER_WIDTH;
    float width = SKILL_GRAPH_AREA_WIDTH;
    float height = SKILL_GRAPH_AREA_HEIGHT;

    VideoManager->PushScissoredRect(left,
                                    top,
                                    width,
                                    height);

    // Debug draw limits
//    VideoManager->DrawRectangleOutline(left,
//                                       left + SKILL_GRAPH_AREA_WIDTH,
//                                       top + SKILL_GRAPH_AREA_HEIGHT,
//                                       top,
//                                       2, Color::white);

    // Draw the visible lines
    for (NodeLine node_line : _displayed_node_links) {
        vt_video::VideoManager->DrawLine(node_line.x1,
                                         node_line.y1, 5,
                                         node_line.x2,
                                         node_line.y2, 5,
                                         grayed_path);
    }

    float pointer_x_location = -1.0f;
    float pointer_y_location = -1.0f;

    // Draw the visible skill nodes
    for (SkillNode* skill_node : _displayed_skill_nodes) {
        VideoManager->Move(_view_x_position, _view_y_position);
        VideoManager->MoveRelative(skill_node->GetXLocation(),
                                   skill_node->GetYLocation());
        // Center the image
        vt_video::StillImage& image = skill_node->GetIconImage();
        VideoManager->MoveRelative(-image.GetWidth() / 2.0f,
                                   -image.GetHeight() / 2.0f);
        image.Draw();

        // Setup the marker location to be on the currently selected node
        if (_selected_node_index == skill_node->GetId()) {
            pointer_x_location = _view_x_position + skill_node->GetXLocation()
                - _location_pointer.GetWidth() / 3.0f;
            pointer_y_location = _view_y_position + skill_node->GetYLocation()
                - image.GetHeight() - _location_pointer.GetHeight();
        }

        // Draw the character portrait if the character is on its latest learned skill.
        if (_character_node_index == skill_node->GetId()) {
            VideoManager->Move(_view_x_position, _view_y_position);
            VideoManager->MoveRelative(skill_node->GetXLocation(),
                                       skill_node->GetYLocation());
            // Center the image
            VideoManager->MoveRelative(-_character_icon.GetWidth() / 2.0f,
                                       -_character_icon.GetHeight() / 2.0f);
            _character_icon.Draw();
        }
    }

    // Draw the location pointer if the node was found
    if (pointer_x_location > 0.0f || pointer_y_location > 0.0f) {
        VideoManager->Move(pointer_x_location, pointer_y_location);
        _location_pointer.Draw();
    }

    VideoManager->PopScissoredRect();
}

void SkillGraphWindow::_ResetSkillGraphView()
{
    // Set current offset based on the currently selected node
    SkillGraph& skill_graph = vt_global::GlobalManager->GetSkillGraph();
    SkillNode* current_skill_node = skill_graph.GetSkillNode(_selected_node_index);

    // If the node is invalid, try the default one.
    if (current_skill_node == nullptr) {
        current_skill_node = skill_graph.GetSkillNode(0);
        _selected_node_index = 0;
    }

    // If the default one fails, set an empty view
    if (current_skill_node == nullptr) {
        _current_x_offset = -1.0f;
        _current_y_offset = -1.0f;
        _selected_node_index = std::numeric_limits<uint32_t>::max();
        PRINT_WARNING << "Empty Skill Graph View" << std::endl;
        return;
    }

    _UpdateSkillGraphView(false);
}

void SkillGraphWindow::_UpdateSkillGraphView(bool scroll)
{
    // Check to prevent invalid updates
    if (_selected_node_index == std::numeric_limits<uint32_t>::max())
        return;

    SkillGraph& skill_graph = vt_global::GlobalManager->GetSkillGraph();
    SkillNode* current_skill_node = skill_graph.GetSkillNode(_selected_node_index);

    _current_x_offset = current_skill_node->GetXLocation();
    _current_y_offset = current_skill_node->GetYLocation();

    // Get the current view offset
    float target_x_position, target_y_position;
    GetPosition(target_x_position, target_y_position);
    target_x_position = target_x_position
                       + (SKILL_GRAPH_AREA_WIDTH / 2.0f)
                       + WINDOW_BORDER_WIDTH
                       - _current_x_offset;
    target_y_position = target_y_position
                       + (SKILL_GRAPH_AREA_HEIGHT / 2.0f)
                       + WINDOW_BORDER_WIDTH
                       - _current_y_offset;

    // Don't update the view if it is already centered
    if (_view_x_position == target_x_position
            && _view_y_position == target_y_position) {
        return;
    }

    const float target_x_distance = target_x_position - _view_x_position;
    const float target_y_distance = target_y_position - _view_y_position;

    if (!scroll) {
        // Make it instant
        _view_x_position = target_x_position;
        _view_y_position = target_y_position;
    }
    else {
        // Smooth the view move
        const float max_speed = 30.0f;
        const float min_speed = 100.0f;
        const float pixel_move_x = (min_speed - std::abs(target_x_distance)) / 10.0f < max_speed / 10.0f ?
                                   max_speed / 10.0f : (min_speed - std::abs(target_x_distance)) / 10.0f;
        const float pixel_move_y = (min_speed - std::abs(target_y_distance)) / 10.0f < max_speed / 10.0f ?
                                   max_speed / 10.0f : (min_speed - std::abs(target_y_distance)) / 10.0f;
        const float update_time = static_cast<float>(vt_system::SystemManager->GetUpdateTime());
        const float update_move_x = update_time / pixel_move_x;
        const float update_move_y = update_time / pixel_move_y;

        // Make the view scroll
        if (_view_x_position < target_x_position) {
            _view_x_position += update_move_x;
            if (_view_x_position > target_x_position)
                _view_x_position = target_x_position;
        }
        else if (_view_x_position > target_x_position) {
            _view_x_position -= update_move_x;
            if (_view_x_position < target_x_position)
                _view_x_position = target_x_position;
        }

        if (_view_y_position < target_y_position) {
            _view_y_position += update_move_y;
            if (_view_y_position > target_y_position)
                _view_y_position = target_y_position;
        }
        else if (_view_y_position > target_y_position) {
            _view_y_position -= update_move_y;
            if (_view_y_position < target_y_position)
                _view_y_position = target_y_position;
        }
    }

    // Update the skill node displayed list
    const float area_width = SKILL_GRAPH_AREA_WIDTH / 2.0f + NODES_DISPLAY_MARGIN;
    const float area_height = SKILL_GRAPH_AREA_HEIGHT / 2.0f + NODES_DISPLAY_MARGIN;
    const float min_x_view = _current_x_offset - area_width + target_x_distance;
    const float max_x_view = _current_x_offset + area_width + target_x_distance;
    const float min_y_view = _current_y_offset - area_height + target_y_distance;
    const float max_y_view = _current_y_offset + area_height + target_y_distance;

    // Based on current offset, reload visible nodes
    _displayed_skill_nodes.clear();
    auto skill_nodes = skill_graph.GetSkillNodes();
    for (SkillNode* skill_node : skill_nodes) {
        float node_x = skill_node->GetXLocation();
        float node_y = skill_node->GetYLocation();
        if (node_x < min_x_view || node_x > max_x_view
                || node_y < min_y_view || node_y > max_y_view) {
            continue;
        }
        _displayed_skill_nodes.push_back(skill_node);
    }

    // Prepare lines coordinates for draw time
    _displayed_node_links.clear();
    // default ones (white, grayed out)
    for (SkillNode* skill_node : _displayed_skill_nodes) {
        auto node_links = skill_node->GetNodeLinks();
        // Don't load anything if there are no links
        if (node_links.empty())
            continue;

        // Load line start
        NodeLine node_line;
        node_line.x1 = skill_node->GetXLocation() + _view_x_position;
        node_line.y1 = skill_node->GetYLocation() + _view_y_position;

        // For each link, add a line end
        for (uint32_t link_id : node_links) {
            SkillNode* linked_node = skill_graph.GetSkillNode(link_id);
            if (!linked_node)
                continue;

            // Don't draw the line if it goes over the edge.
            float node_x = linked_node->GetXLocation();
            float node_y = linked_node->GetYLocation();

            node_line.x2 = node_x + _view_x_position;
            node_line.y2 = node_y + _view_y_position;

            _displayed_node_links.push_back(node_line);
        }
    }

    // TODO colored lines per characters for paths done by them.
}

} // namespace private_menu

} // namespace vt_menu
