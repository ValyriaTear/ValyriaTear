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

#include "skill_editor.h"

using namespace std;

using namespace hoa_utils;

using namespace hoa_script;
using namespace hoa_global;
using namespace hoa_editor;


SkillEditor::SkillEditor(QWidget *parent, const QString &name)
: QWidget(parent, static_cast<const char *>(name))
{
	setCaption("Skill Editor");

	_current_skill_index[GLOBAL_SKILL_ATTACK] = -1;
	_current_skill_index[GLOBAL_SKILL_DEFEND] = -1;
	_current_skill_index[GLOBAL_SKILL_SUPPORT] = -1;


	_LoadSkills();

	// initialize tabs
	_tab_skill_groups = new QTabWidget();
	_tab_skill_groups->setTabPosition(QTabWidget::North);
	connect(_tab_skill_groups, SIGNAL(currentChanged(int)), this, SLOT(_ChangeCurrentTab(int)));

	// set up the layout boxes
	_hbox = new QHBoxLayout();
	_hbox->addWidget(_tab_skill_groups);

	// add layout to form
	setLayout(_hbox);

	_CreateTab(GLOBAL_SKILL_ATTACK, _attack_skills, "attack");
	_CreateTab(GLOBAL_SKILL_DEFEND, _defense_skills, "defense");
	_CreateTab(GLOBAL_SKILL_SUPPORT, _support_skills, "support");

	_current_tab = GLOBAL_SKILL_ATTACK;
} // SkillEditor

SkillEditor::~SkillEditor()
{
	for (uint32 i = 0; i < _attack_skills.size(); ++i)
		delete _attack_skills[i];
	for (uint32 i = 0; i < _defense_skills.size(); ++i)
		delete _defense_skills[i];
	for (uint32 i = 0; i < _support_skills.size(); ++i)
		delete _support_skills[i];

	_CleanupTab(GLOBAL_SKILL_ATTACK);
	_CleanupTab(GLOBAL_SKILL_DEFEND);
	_CleanupTab(GLOBAL_SKILL_SUPPORT);

	delete _hbox;
	delete _tab_skill_groups;
} // ~SkillEditor

void SkillEditor::_ChangeCurrentTab(int index)
{
	if (index == 0)
		_current_tab = GLOBAL_SKILL_ATTACK;
	else if (index == 1)
		_current_tab = GLOBAL_SKILL_DEFEND;
	else if (index == 2)
		_current_tab = GLOBAL_SKILL_SUPPORT;
} // _ChangeCurrentTab()

void SkillEditor::_LeftButtonClicked()
{
	_current_skill_index[_current_tab]--;
	if (_current_skill_index[_current_tab] < 0)
		_current_skill_index[_current_tab] = _GetCurrentSkillList().size() - 1;

	_ReloadTab();
} // _LeftButtonClicked()

void SkillEditor::_RightButtonClicked()
{
	_current_skill_index[_current_tab]++;
	if (_current_skill_index[_current_tab] >= static_cast<int32>(_GetCurrentSkillList().size()))
		_current_skill_index[_current_tab] = 0;

	_ReloadTab();
} // _RightButtonClicked()

void SkillEditor::_LoadSkills()
{
	string path = string("dat/skills/");
	ReadScriptDescriptor script;
	script.OpenFile(path + "defense.lua", true);
	script.CloseFile();
	script.OpenFile(path + "support.lua", true);
	script.CloseFile();
	vector<GlobalSkill *> skills;
	if (script.OpenFile(path + "attack.lua", true) != false)
		_LoadSkills(script, skills, GLOBAL_SKILL_ATTACK);
	// Now clean up the skills script (due to the way we're storing the scripts in lua, they're all actually in one big table in lua
	// regardless of the file split on disk
	vector<GlobalSkill *>::iterator i = skills.begin();
	for (; i != skills.end(); ++i)
	{
		if ((*i)->GetType() == GLOBAL_SKILL_ATTACK)
			_attack_skills.push_back(*i);
		else if ((*i)->GetType() == GLOBAL_SKILL_DEFEND)
			_defense_skills.push_back(*i);
		else if ((*i)->GetType() == GLOBAL_SKILL_SUPPORT)
			_support_skills.push_back(*i);
	}
	if (_attack_skills.size() > 0)
		_current_skill_index[GLOBAL_SKILL_ATTACK] = 0;
	if (_defense_skills.size() > 0)
		_current_skill_index[GLOBAL_SKILL_DEFEND] = 0;
	if (_support_skills.size() > 0)
		_current_skill_index[GLOBAL_SKILL_SUPPORT] = 0;

} // _LoadSkills()

void SkillEditor::_LoadSkills(ReadScriptDescriptor &script, vector<GlobalSkill *> &skills, GLOBAL_SKILL type)
{
	script.OpenTable("skills");
	vector<uint32> keys;
	script.ReadTableKeys(keys);
	for (uint32 i = 0; i < keys.size(); ++i)
		skills.push_back(new GlobalSkill(keys[i]));
	script.CloseAllTables();
	script.CloseFile();

} // _LoadSkills(ReadScriptDescriptor, vector<GlobalSkill *>)

void SkillEditor::_CreateTab(GLOBAL_SKILL type, vector<GlobalSkill *>skills, QString tab_name)
{
	_gl_layouts[type] = new QGridLayout();
	// add the labels for the different parts of the skill
	_lbl_skill_names[type] = new QLabel();
	_lbl_skill_names[type]->setText("Skill Name:");
	_gl_layouts[type]->addWidget(_lbl_skill_names[type], 0, 0);

	_lbl_description[type] = new QLabel();
	_lbl_description[type]->setText("Description:");
	_gl_layouts[type]->addWidget(_lbl_description[type], 1, 0);

	_lbl_sp_required[type] = new QLabel();
	_lbl_sp_required[type]->setText("SP Required:");
	_gl_layouts[type]->addWidget(_lbl_sp_required[type], 2, 0);

	_lbl_warmup_time[type] = new QLabel();
	_lbl_warmup_time[type]->setText("Warmup Time:");
	_gl_layouts[type]->addWidget(_lbl_warmup_time[type], 2, 2);

	_lbl_cooldown_time[type] = new QLabel();
	_lbl_cooldown_time[type]->setText("Cooldown Time:");
	_gl_layouts[type]->addWidget(_lbl_cooldown_time[type], 3, 0);

	_lbl_target_type[type] = new QLabel();
	_lbl_target_type[type]->setText("Target Type:");
	_gl_layouts[type]->addWidget(_lbl_target_type[type], 4, 0);

	// add the line edits for the skill
	_le_skill_names[type] = new QLineEdit();
	_le_description[type] = new QLineEdit();
	_le_sp_required[type] = new QLineEdit();
	_le_warmup_time[type] = new QLineEdit();
	_le_cooldown_time[type] = new QLineEdit();
	_cb_target_type[type] = new QComboBox();

	if (_current_skill_index[type] != -1)
	{
		GlobalSkill *skill = skills[_current_skill_index[type]];
		// this skill is enabled so set the text
		string text = MakeStandardString(skill->GetName());
		// get the char array from the standard string, because otherwise the QString ends up with
		// some gibberish text at the beginning of it
		_le_skill_names[type]->setText(QString(text.c_str()));

		text = MakeStandardString(skill->GetDescription());
		_le_description[type]->setText(QString(text.c_str()));

		text = NumberToString<uint32>(skill->GetSPRequired());
		_le_sp_required[type]->setText(QString(text.c_str()));

		text = NumberToString<uint32>(skill->GetWarmupTime());
		_le_warmup_time[type]->setText(QString(text.c_str()));

		text = NumberToString<uint32>(skill->GetCooldownTime());
		_le_cooldown_time[type]->setText(QString(text.c_str()));

		_cb_target_type[type]->insertItem("Attack Point");
		_cb_target_type[type]->insertItem("Actor");
		_cb_target_type[type]->insertItem("Party");
		_cb_target_type[type]->setCurrentIndex(static_cast<int32>(skill->GetTargetType()));
	}
	else
	{
		// disable the line edits (no skills for this group)
		_le_skill_names[type]->setDisabled(true);
		_le_description[type]->setDisabled(true);
		_le_sp_required[type]->setDisabled(true);
		_le_warmup_time[type]->setDisabled(true);
		_le_cooldown_time[type]->setDisabled(true);
		_cb_target_type[type]->setDisabled(true);
		_rb_target_ally_false[type]->setDisabled(true);
		_rb_target_ally_true[type]->setDisabled(true);
	}

	// add above widgets to grid layout
	_gl_layouts[type]->addWidget(_le_skill_names[type], 0, 1, 1, 3);
	_gl_layouts[type]->addWidget(_le_description[type], 1, 1, 1, 3);
	_gl_layouts[type]->addWidget(_le_sp_required[type], 2, 1);
	_gl_layouts[type]->addWidget(_le_warmup_time[type], 2, 3);
	_gl_layouts[type]->addWidget(_le_cooldown_time[type], 3, 1);
	_gl_layouts[type]->addWidget(_cb_target_type[type], 4, 1);
	_hbox_target_ally[type] = new QHBoxLayout();
	_hbox_target_ally[type]->addWidget(_rb_target_ally_true[type]);
	_hbox_target_ally[type]->addWidget(_rb_target_ally_false[type]);
	_target_ally_spacers[type] = new QSpacerItem(10, 5, QSizePolicy::Expanding);
	_hbox_target_ally[type]->addItem(_target_ally_spacers[type]);
	_gl_layouts[type]->addLayout(_hbox_target_ally[type], 3, 3);

	// create the vertical layout for the tab
	_tab_vboxes[type] = new QVBoxLayout();
	_tab_vboxes[type]->addLayout(_gl_layouts[type]);

	// add a spacer to the vbox to push up the grid layout
	_tab_spacers[type] = new QSpacerItem(10,5, QSizePolicy::Minimum, QSizePolicy::Expanding);
	_tab_vboxes[type]->addItem(_tab_spacers[type]);

	// create the horizontal layout underneath the grid in the tab
	_tab_bottom_hboxes[type] = new QHBoxLayout();

	_CreateTabBottomButtons(type);

	_tab_vboxes[type]->addLayout(_tab_bottom_hboxes[type]);

	_tab_pages[type] = new QWidget();
	_tab_pages[type]->setLayout(_tab_vboxes[type]);
	_tab_skill_groups->addTab(_tab_pages[type], tab_name);
} // _CreateTab(GLOBAL_SKILL)

void SkillEditor::_CreateTabBottomButtons(GLOBAL_SKILL type)
{
	// add all the buttons
	_new_buttons[type] = new QPushButton();
	_new_buttons[type]->setText("Create New Skill");
	_new_buttons[type]->setDisabled(true);
	_new_buttons[type]->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	_tab_bottom_hboxes[type]->addWidget(_new_buttons[type]);

	_save_buttons[type] = new QPushButton();
	_save_buttons[type]->setText("Save Changes");
	_save_buttons[type]->setDisabled(true);
	_save_buttons[type]->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	_tab_bottom_hboxes[type]->addWidget(_save_buttons[type]);

	// this spacer pushes the nav buttons to the right, and the other 2 buttons to the left
	_button_spacers[type] = new QSpacerItem(30, 10, QSizePolicy::Expanding);
	_tab_bottom_hboxes[type]->addItem(_button_spacers[type]);

	_left_buttons[type] = new QPushButton();
	_left_buttons[type]->setIcon(QIcon("img/misc/editor-tools/arrow-left.png"));
	_left_buttons[type]->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	connect(_left_buttons[type], SIGNAL(clicked()), this, SLOT(_LeftButtonClicked()));
	_tab_bottom_hboxes[type]->addWidget(_left_buttons[type]);

	_right_buttons[type] = new QPushButton();
	_right_buttons[type]->setIcon(QIcon("img/misc/editor-tools/arrow-right.png"));
	_right_buttons[type]->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	connect(_right_buttons[type], SIGNAL(clicked()), this, SLOT(_RightButtonClicked()));
	_tab_bottom_hboxes[type]->addWidget(_right_buttons[type]);
}

void SkillEditor::_CleanupTab(GLOBAL_SKILL type)
{
	delete _tab_vboxes[type];
	delete _tab_bottom_hboxes[type];
	delete _gl_layouts[type];
	delete _tab_pages[type];
	delete _left_buttons[type];
	delete _right_buttons[type];
	delete _save_buttons[type];
	delete _button_spacers[type];
	delete _tab_spacers[type];
	delete _lbl_skill_names[type];
	delete _lbl_description[type];
	delete _lbl_sp_required[type];
	delete _lbl_warmup_time[type];
	delete _lbl_cooldown_time[type];
	delete _lbl_target_type[type];
	delete _le_skill_names[type];
	delete _le_description[type];
	delete _le_sp_required[type];
	delete _le_warmup_time[type];
	delete _le_cooldown_time[type];
	delete _cb_target_type[type];
	delete _bg_target_ally[type];
	delete _rb_target_ally_true[type];
	delete _rb_target_ally_false[type];
	delete _hbox_target_ally[type];
	delete _target_ally_spacers[type];
} // _CleanupTab(GLOBAL_SKILL)

vector<GlobalSkill *> &SkillEditor::_GetCurrentSkillList()
{
	if (_current_tab == GLOBAL_SKILL_ATTACK)
		return _attack_skills;
	else if (_current_tab == GLOBAL_SKILL_DEFEND)
		return _defense_skills;
	return _support_skills;
} // _GetCurrentSkillList

void SkillEditor::_ReloadTab()
{

	if (_GetCurrentSkillList().empty()) {
		return;
	}

	GlobalSkill *skill = _GetCurrentSkillList()[_current_skill_index[_current_tab]];

	string text = MakeStandardString(skill->GetName());
	_le_skill_names[_current_tab]->setText(QString(text.c_str()));

	text = MakeStandardString(skill->GetDescription());
	_le_description[_current_tab]->setText(QString(text.c_str()));

	text = NumberToString<uint32>(skill->GetSPRequired());
	_le_sp_required[_current_tab]->setText(QString(text.c_str()));

	text = NumberToString<uint32>(skill->GetWarmupTime());
	_le_warmup_time[_current_tab]->setText(QString(text.c_str()));

	text = NumberToString<uint32>(skill->GetCooldownTime());
	_le_cooldown_time[_current_tab]->setText(QString(text.c_str()));

	_cb_target_type[_current_tab]->setCurrentIndex(static_cast<int32>(skill->GetTargetType()));

	// TODO: as more controls are added to the tab, reload them here.
}

