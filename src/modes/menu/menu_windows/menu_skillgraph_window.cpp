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
using namespace vt_common;

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
    _current_offset(-1.0f, -1.0f), // Invalid view
    _view_position(0.0f, 0.0f),
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

    // Only update animation when necessary
    if (!_Navigate())
        return;

    SkillGraph& skill_graph = vt_global::GlobalManager->GetSkillGraph();
    SkillNode* current_skill_node = skill_graph.GetSkillNode(_selected_node_index);

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
    for (Line2D node_line : _displayed_node_links) {
        vt_video::VideoManager->DrawLine(node_line.begin.x,
                                         node_line.begin.y, 5,
                                         node_line.end.x,
                                         node_line.end.y, 5,
                                         grayed_path);
    }

    Position2D pointer_location(-1.0f, -1.0f);

    // Draw the visible skill nodes
    for (SkillNode* skill_node : _displayed_skill_nodes) {
        VideoManager->Move(_view_position.x, _view_position.y);
        VideoManager->MoveRelative(skill_node->GetXPosition(),
                                   skill_node->GetYPosition());
        // Center the image
        vt_video::AnimatedImage& image = skill_node->GetIconImage();
        VideoManager->MoveRelative(-image.GetWidth() / 2.0f,
                                   -image.GetHeight() / 2.0f);
        image.Draw();

        // Setup the marker location to be on the currently selected node
        if (_selected_node_index == skill_node->GetId()) {
            pointer_location.x = _view_position.x + skill_node->GetXPosition()
                - _location_pointer.GetWidth() / 3.0f;
            pointer_location.y = _view_position.y + skill_node->GetYPosition()
                - image.GetHeight() - _location_pointer.GetHeight();
        }

        // Draw the character portrait if the character is on its latest learned skill.
        if (_character_node_index == skill_node->GetId()) {
            VideoManager->Move(_view_position.x, _view_position.y);
            VideoManager->MoveRelative(skill_node->GetXPosition(),
                                       skill_node->GetYPosition());
            // Center the image
            VideoManager->MoveRelative(-_character_icon.GetWidth() / 2.0f,
                                       -_character_icon.GetHeight() / 2.0f);
            _character_icon.Draw();
        }
    }

    // Draw the location pointer if the node was found
    if (pointer_location.x > 0.0f || pointer_location.y > 0.0f) {
        VideoManager->Move(pointer_location.x, pointer_location.y);
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
        _current_offset.x = -1.0f;
        _current_offset.y = -1.0f;
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

    _current_offset = current_skill_node->GetPosition();

    // Get the current view offset
    Position2D target_position = GetPosition();
    target_position.x = target_position.x
                       + (SKILL_GRAPH_AREA_WIDTH / 2.0f)
                       + WINDOW_BORDER_WIDTH
                       - _current_offset.x;
    target_position.y = target_position.y
                       + (SKILL_GRAPH_AREA_HEIGHT / 2.0f)
                       + WINDOW_BORDER_WIDTH
                       - _current_offset.y;

    // Don't update the view if it is already centered
    if (_view_position == target_position) {
        return;
    }

    const Vector2D target_distance(target_position.x - _view_position.x,
                                   target_position.y - _view_position.y);

    if (!scroll) {
        // Make it instant
        _view_position = target_position;
    }
    else {
        // Smooth the view move
        const float max_speed = 30.0f;
        const float min_speed = 100.0f;
        const float pixel_move_x = (min_speed - std::abs(target_distance.x)) / 10.0f < max_speed / 10.0f ?
                                   max_speed / 10.0f : (min_speed - std::abs(target_distance.x)) / 10.0f;
        const float pixel_move_y = (min_speed - std::abs(target_distance.y)) / 10.0f < max_speed / 10.0f ?
                                   max_speed / 10.0f : (min_speed - std::abs(target_distance.y)) / 10.0f;
        const float update_time = static_cast<float>(vt_system::SystemManager->GetUpdateTime());
        const Position2D update_move(update_time / pixel_move_x,
                                     update_time / pixel_move_y);

        // Make the view scroll
        if (_view_position.x < target_position.x) {
            _view_position.x += update_move.x;
            if (_view_position.x > target_position.x)
                _view_position.x = target_position.x;
        }
        else if (_view_position.x > target_position.x) {
            _view_position.x -= update_move.x;
            if (_view_position.x < target_position.x)
                _view_position.x = target_position.x;
        }

        if (_view_position.y < target_position.y) {
            _view_position.y += update_move.y;
            if (_view_position.y > target_position.y)
                _view_position.y = target_position.y;
        }
        else if (_view_position.y > target_position.y) {
            _view_position.y -= update_move.y;
            if (_view_position.y < target_position.y)
                _view_position.y = target_position.y;
        }
    }

    // Update the skill node displayed list
    const float area_width = SKILL_GRAPH_AREA_WIDTH / 2.0f + NODES_DISPLAY_MARGIN;
    const float area_height = SKILL_GRAPH_AREA_HEIGHT / 2.0f + NODES_DISPLAY_MARGIN;
    const Position2D min_view(_current_offset.x - area_width + target_distance.x,
                              _current_offset.y - area_height + target_distance.y);
    const Position2D max_view(_current_offset.x + area_width + target_distance.x,
                              _current_offset.y + area_height + target_distance.y);
    Rectangle2D nodes_rect(min_view.x, max_view.x,
                           min_view.y, max_view.y);

    // Do not reload visible nodes more than necessary
    static uint32_t update_timer = 0;
    update_timer += vt_system::SystemManager->GetUpdateTime();
    if (_view_position == target_position || update_timer >= 200) {
        update_timer = 0;
        // Based on current offset, reload visible nodes
        _displayed_skill_nodes.clear();
        auto skill_nodes = skill_graph.GetSkillNodes();
        for (SkillNode* skill_node : skill_nodes) {
            if (!nodes_rect.Contains(skill_node->GetPosition())) {
                continue;
            }
            _displayed_skill_nodes.push_back(skill_node);
        }
    }

    // Prepare lines coordinates for draw time
    _displayed_node_links.clear();
    // default ones (white, grayed out)
    for (SkillNode* skill_node : _displayed_skill_nodes) {
        auto node_links = skill_node->GetChildrenNodeLinks();
        // Don't load anything if there are no links
        if (node_links.empty())
            continue;

        // Load line start
        Line2D node_line;
        node_line.begin.x = skill_node->GetXPosition() + _view_position.x;
        node_line.begin.y = skill_node->GetYPosition() + _view_position.y;

        // For each link, add a line end
        for (uint32_t link_id : node_links) {
            SkillNode* linked_node = skill_graph.GetSkillNode(link_id);
            if (!linked_node)
                continue;

            // Don't draw the line if it goes over the edge.
            Position2D node_pos = linked_node->GetPosition();

            node_line.end.x = node_pos.x + _view_position.x;
            node_line.end.y = node_pos.y + _view_position.y;

            _displayed_node_links.push_back(node_line);
        }
    }

    // TODO colored lines per characters for paths done by them.
}

//! \brief Returns whether the node link is within the given links
static bool isNodeWithin(const std::vector<uint32_t>& node_links, uint32_t node_id)
{
    for (auto node_link : node_links) {
        if (node_link == node_id)
            return true;
    }
    return false;
}

bool SkillGraphWindow::_Navigate()
{
    if (!InputManager->ArrowPress())
        return false;

    SkillGraph& skill_graph = vt_global::GlobalManager->GetSkillGraph();
    SkillNode* current_skill_node = skill_graph.GetSkillNode(_selected_node_index);
    const Position2D current_pos = current_skill_node->GetPosition();
    // Get every node links
    auto node_links = current_skill_node->GetChildrenNodeLinks();
    const auto parent_node_links = current_skill_node->GetParentNodeLinks();
    node_links.insert(node_links.end(), parent_node_links.begin(), parent_node_links.end());

    SkillNode* selected_node = nullptr;
    for (SkillNode* target_node : _displayed_skill_nodes) {
        // Don't check against self
        if (target_node == current_skill_node)
            continue;

        // Check whether the node is within current links
        if (!isNodeWithin(node_links, target_node->GetId()))
            continue;

        // Do a simple position check
        Position2D target_pos = target_node->GetPosition();
        if (InputManager->LeftPress()) {
            if (target_pos.x >= current_pos.x)
                continue;
        }
        if (InputManager->RightPress()) {
            if (target_pos.x <= current_pos.x)
                continue;
        }
        if (InputManager->UpPress()) {
            if (target_pos.y >= current_pos.y)
                continue;
        }
        else if (InputManager->DownPress()) {
            if (target_pos.y <= current_pos.y)
                continue;
        }

        /* TODO: check whether tan search helps or is useful
        // Get the tangent of the 2 points
        // and check whether the angle correspond to the direction
        Position2D tan_pos = current_pos;
        tan_pos.x = target_pos.y;

        // Tan X = O / A
        // Actually we have Tan^2 but the result angle is the same.
        // We use the default of undefined (angle of 90°)
        float target_tangent = 90; // Actually, any value above ~25 will do
        if (current_pos.GetDistance2(tan_pos) != 0.0f)
            target_tangent = tan_pos.GetDistance2(target_pos) / current_pos.GetDistance2(tan_pos);
        */

        selected_node = target_node;
        break;
    }

    // Select the new closest node
    if (selected_node)
        _selected_node_index = selected_node->GetId();

    return (selected_node != nullptr);
}

} // namespace private_menu

} // namespace vt_menu
