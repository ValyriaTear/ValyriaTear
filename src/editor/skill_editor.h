///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software 
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/*!****************************************************************************
* \file    skill_editor.h
* \author  Daniel Steuernol, steu@allacrost.org
* \brief   Header file for editor's skill editor dialog
*****************************************************************************/

#ifndef __SKILL_EDITOR_HEADER__
#define __SKILL_EDITOR_HEADER__

#include <string>
#include <vector>

#include <QDialog>
#include <QTabWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QButtonGroup>
#include <QRadioButton>

#include "global_skills.h"
#include "script.h"

namespace hoa_editor
{

class SkillEditor : public QWidget
{
	//! Macro needed to use Qt's slots and signals
	Q_OBJECT

public:
	SkillEditor(QWidget *parent, const QString &name);
	~SkillEditor();	

private slots:
	//! \brief slot to handle tab changes
	void _ChangeCurrentTab(int index);
	//! \brief button clicked slots
	//@{
	void _LeftButtonClicked();
	void _RightButtonClicked();
	//@}

private:
	//! \brief Loads the skills from the game
	void _LoadSkills();
	//! \brief Loads the skills from an individual file
	//! the ReadScriptDescriptor must already be opened.
	//! this function closes the script when finished.
	void _LoadSkills(hoa_script::ReadScriptDescriptor &script, std::vector<hoa_global::GlobalSkill *> &skills, hoa_global::GLOBAL_SKILL type);
	//! \brief Create the UI for the given tab page.
	void _CreateTab(hoa_global::GLOBAL_SKILL type, std::vector<hoa_global::GlobalSkill *> skills, QString name);
	//! \brief Create the buttons at the bottom of the tab
	void _CreateTabBottomButtons(hoa_global::GLOBAL_SKILL type);
	//! \brief clean up functions, to delete all the controls on the individual tab pages
	void _CleanupTab(hoa_global::GLOBAL_SKILL type);
	//! \brief returns the skill list associated with the current tab
	std::vector<hoa_global::GlobalSkill *> &_GetCurrentSkillList();
	//! \brief Reloads the current tab with the currently selected skill
	void _ReloadTab();

	//! \brief a tab control to hold the tabs for attack skills, support skills, and defense skills
	QTabWidget *_tab_skill_groups;
	//! \brief this is the layout control that the tab control lives in.
	QHBoxLayout *_hbox;
	//! \brief vectors to hold the global skills
	//@{
	std::vector<hoa_global::GlobalSkill *> _attack_skills;
	std::vector<hoa_global::GlobalSkill *> _defense_skills;
	std::vector<hoa_global::GlobalSkill *> _support_skills;
	//@}

	//! \brief the currently selected tab
	hoa_global::GLOBAL_SKILL _current_tab;

	//! \brief tab page controls
	//@{
	int32 _current_skill_index[3];
	QVBoxLayout * _tab_vboxes[3];
	QHBoxLayout * _tab_bottom_hboxes[3];
	QGridLayout * _gl_layouts[3];
	QWidget * _tab_pages[3];
	QPushButton * _left_buttons[3];
	QPushButton * _right_buttons[3];
	QPushButton * _save_buttons[3];
	QPushButton * _new_buttons[3];
	QSpacerItem * _button_spacers[3];
	QSpacerItem * _tab_spacers[3];
	QLabel * _lbl_skill_names[3];
	QLineEdit * _le_skill_names[3];
	QLabel * _lbl_description[3];
	QLineEdit * _le_description[3];
	QLabel * _lbl_sp_required[3];
	QLineEdit * _le_sp_required[3];
	QLabel * _lbl_warmup_time[3];
	QLineEdit * _le_warmup_time[3];
	QLabel * _lbl_cooldown_time[3];
	QLineEdit * _le_cooldown_time[3];
	QLabel * _lbl_target_type[3];
	QComboBox * _cb_target_type[3];
	QLabel * _lbl_target_ally[3];
	QButtonGroup * _bg_target_ally[3];
	QRadioButton * _rb_target_ally_true[3];
	QRadioButton * _rb_target_ally_false[3];
	QHBoxLayout * _hbox_target_ally[3];
	QSpacerItem * _target_ally_spacers[3];
	//@}
};

} // namespace hoa_editor

#endif
// __SKILL_EDITOR_HEADER__
