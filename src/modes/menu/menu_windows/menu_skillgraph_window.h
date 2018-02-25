///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_SKILLGRAPH_WINDOW__
#define __MENU_SKILLGRAPH_WINDOW__

#include "skill_node_bottom_info.h"

#include "common/global/global.h"
#include "common/gui/textbox.h"

#include "common/gui/menu_window.h"
#include "common/gui/option.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

//! \brief The menu sub states
enum SKILLGRAPH_STATE {
    SKILLGRAPH_STATE_NONE = 0,
    SKILLGRAPH_STATE_CHAR = 1,
    SKILLGRAPH_STATE_LIST = 2,
    SKILLGRAPH_STATE_SIZE = 3
};

/**
*** \brief handles showing the skill tree and the selection of new nodes.
*** The player will be able to use skill points and other items to get his/her way
*** through the skill tree and earns either new stats upgrades or new skills.
*** The "tree" is in fact a way point map.
**/
class SkillGraphWindow : public vt_gui::MenuWindow
{
    friend class vt_menu::MenuMode;
    //friend class SkillGraphState;

public:
    SkillGraphWindow();

    virtual ~SkillGraphWindow() override
    {
    }

    //! \brief Performs updates
    void Update() override;

    //! \brief Draws skill graph main window
    void Draw() override;

    //! \brief Draws bottom window
    void DrawBottomWindow();

    //! \brief Result of whether or not this window is active
    //! \return true if this window is active
    bool IsActive() override {
        return _active;
    }

    //! \brief Set the active state of this window, and do any associated work.
    void SetActive(bool is_active_state);

    SKILLGRAPH_STATE GetSkillGraphState() const {
        return _skillgraph_state;
    }

    //! \brief Set the character for this window using the character selection
    //! \returns Whether the character was set
    bool SetCharacter();

private:
    //! \brief The current selected character id
    SKILLGRAPH_STATE _skillgraph_state;

    //! \brief The current selected character id
    vt_global::GlobalCharacter* _selected_character;

    //! \brief the location pointer. this is loaded in the constructor
    vt_video::StillImage _location_pointer;

    //! \brief The character icon. this is loaded in the constructor
    vt_video::StillImage _character_icon;

    //! \brief Offsets for the current skill tree to view in the center of the window
    vt_common::Position2D _current_offset;

    //! \brief The current centered view offsets
    vt_common::Position2D _view_position;

    //! \brief The current index to the location the pointer is on
    uint32_t _selected_node_id;

    //! \brief The current index to the location the character is on
    uint32_t _character_node_id;

    //! \brief Indicates whether this window is active or not
    bool _active;

    //! \brief The currently displayed skill nodes
    std::vector<vt_global::SkillNode*> _displayed_skill_nodes;
    //! \brief The currently displayed link between the nodes
    std::vector<vt_common::Line2D> _displayed_node_links;
    //! \brief The currenty displayed link between nodes obtained by the character
    std::vector<vt_common::Line2D> _colored_displayed_node_links;

    //! \brief The skill node description text, icon, ...
    SkillNodeBottomInfo _bottom_info;

    //! The character select option box
    vt_gui::OptionBox _char_select;

    //! \brief Text to select character
    vt_video::TextImage _select_character_text;

    //! \brief Initializes the character selector
    void _InitCharSelect();

    //! \brief Update function when in character select state
    void _UpdateSkillCharacterSelectState();

    //! \brief Update function when in skill graph list state
    void _UpdateSkillGraphListState();

    //! \brief Draw function when in character select state
    void _DrawCharacterState();

    //! \brief Draw function when in skill graph list state
    void _DrawSkillGraphState();

    //! \brief Reset the view centered on the currently selected node.
    void _ResetSkillGraphView();

    //! \brief Update the skill tree view based on the current offset information
    void _UpdateSkillGraphView(bool scroll = true, bool force = false);

    //! \brief Handles navigation to the neighbor node
    //! in given direction based on key press.
    //! \returns Whether a new node was selected.
    bool _Navigate();

    //! \brief Handles pressing confirm when on the skill graph.
    //! This permits to buy nodes for XP.
    void _HandleNodeTransaction();
};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_SKILLGRAPH_WINDOW__
