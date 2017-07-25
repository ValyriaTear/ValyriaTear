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

//! \brief A simple line showing a link between two nodes
struct NodeLine {
    float x1;
    float y1;
    float x2;
    float y2;
};

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
    void Update();

    //! \brief Draws skill graph main window
    void Draw();

    //! \brief Draws bottom window
    void DrawBottomWindow();

    //! \brief Result of whether or not this window is active
    //! \return true if this window is active
    bool IsActive() {
        return _active;
    }

    //! \brief Set the active state of this window, and do any associated work.
    void SetActive(bool is_active_state);

    SKILLGRAPH_STATE GetSkillGraphState() const {
        return _skillgraph_state;
    }

    //! \brief Set the character for this window
    //! \param character the character to associate with this window
    void SetCharacter(vt_global::GlobalCharacter& character);

private:
    //! \brief the current selected character id
    SKILLGRAPH_STATE _skillgraph_state;

    //! \brief the current selected character id
    uint32_t _selected_character_id;

    //! \brief the location pointer. this is loaded in the constructor
    vt_video::StillImage _location_pointer;

    //! \brief the location pointer. this is loaded in the constructor
    vt_video::StillImage _character_icon;

    //! \brief offsets for the current skill tree to view in the center of the window
    float _current_x_offset;
    float _current_y_offset;

    //! \brief The current centered view offsets
    float _view_x_position;
    float _view_y_position;

    //! \brief the current index to the location the pointer is on
    uint32_t _selected_node_index;

    //! \brief indicates whether this window is active or not
    bool _active;

    //! \brief The currently displayed skill nodes
    std::vector<vt_global::SkillNode*> _displayed_skill_nodes;
    //! \brief The currentlw displayed link between the nodes
    std::vector<NodeLine> _displayed_node_links;

    //! \brief The skill node description text, icon, ...
    SkillNodeBottomInfo _bottom_info;

    //! The character select option box
    vt_gui::OptionBox _char_select;

    //! \brief Text te select character
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
    void _UpdateSkillGraphView(bool scroll = true);
};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_SKILLGRAPH_WINDOW__
