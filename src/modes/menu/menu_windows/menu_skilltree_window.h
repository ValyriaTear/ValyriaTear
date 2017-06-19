///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_SKILLTREE_WINDOW__
#define __MENU_SKILLTREE_WINDOW__

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

/**
*** \brief handles showing the skill tree and the selection of new nodes.
*** The player will be able to use skill points and other items to get his/her way
*** through the skill tree and earns either new stats upgrades or new skills.
*** The "tree" is in fact a way point map.
**/
class SkillTreeWindow : public vt_gui::MenuWindow
{
    friend class vt_menu::MenuMode;
    //friend class SkillTreeState;

public:
    SkillTreeWindow();

    virtual ~SkillTreeWindow() override
    {
    }

    //! \brief Performs updates
    void Update();

    //! \brief Draws window
    //! \return success/failure
    void Draw();

    //! \brief Result of whether or not this window is active
    //! \return true if this window is active
    bool IsActive() {
        return _active;
    }

    //! \brief Set the active state of this window, and do any associated work.
    void SetActive(bool new_state);

    //! \brief Set the character for this window
    //! \param character the character to associate with this window
    void SetCharacter(vt_global::GlobalCharacter& character);

private:
    //! \brief the current selected character id
    uint32_t _selected_character_id;

    //! \brief TEMP: the default location marker. this is loaded in the constructor
    vt_video::AnimatedImage _location_marker;

    //! \brief the location pointer. this is loaded in the constructor
    vt_video::StillImage _location_pointer;

    //! \brief the location pointer. this is loaded in the constructor
    vt_video::StillImage _character_icon;

    //! \brief offsets for the current skill tree to view in the center of the window
    float _current_x_offset;
    float _current_y_offset;

    //! \brief the current index to the location the pointer is on
    uint32_t _selected_node_index;

    //! \brief indicates whether this window is active or not
    bool _active;

    //! \brief The currently displayed skill nodes
    std::vector<vt_global::SkillNode*> _displayed_skill_nodes;
    //! \brief The currentlw displayed link between the nodes
    std::vector<NodeLine> _displayed_node_links;

    //! \brief Reset the view centered on the currently selected node.
    void _ResetSkillTreeView();

    //! \brief Update the skill tree view based on the current offset information
    void _UpdateSkillTreeView();
};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_SKILLTREE_WINDOW__
