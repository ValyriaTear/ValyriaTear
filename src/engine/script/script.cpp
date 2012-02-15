///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    script.cpp
*** \author  Daniel Steuernol - steu@allacrost.org,
***          Tyler Olsen - roots@allacrost.org
***
*** \brief   Source file for the scripting engine.
*** ***************************************************************************/

#include <iostream>
#include <stdarg.h>

#include "script.h"

using namespace std;
using namespace luabind;

using namespace hoa_utils;
using namespace hoa_script::private_script;

template<> hoa_script::ScriptEngine* Singleton<hoa_script::ScriptEngine>::_singleton_reference = NULL;

namespace hoa_script {

ScriptEngine* ScriptManager = NULL;
bool SCRIPT_DEBUG = false;

//-----------------------------------------------------------------------------
// ScriptEngine Class Functions
//-----------------------------------------------------------------------------

ScriptEngine::ScriptEngine() {
	IF_PRINT_DEBUG(SCRIPT_DEBUG) << "ScriptEngine constructor invoked." << endl;

	// Initialize Lua and LuaBind
	_global_state = lua_open();
	luaL_openlibs(_global_state);
	luabind::open(_global_state);
}



ScriptEngine::~ScriptEngine() {
	IF_PRINT_DEBUG(SCRIPT_DEBUG) << "ScriptEngine destructor invoked." << endl;

	_open_files.clear();
	lua_close(_global_state);
	_global_state = NULL;
}



bool ScriptEngine::SingletonInitialize() {
	// TODO: Open the user setting's file and apply those settings
	return true;
}



bool ScriptEngine::IsFileOpen(const std::string& filename) {
	return false; // TEMP: working on resolving the issue with files being opened multiple times

	if (_open_files.find(filename) != _open_files.end()) {
		return true;
	}
	return false;
}



void ScriptEngine::HandleLuaError(luabind::error& err) {
	lua_State *state = err.state();
	PRINT_ERROR << "a runtime Lua error has occured with the following error message:\n  " << endl;
	std::string k = lua_tostring(state, lua_gettop(state)) ;
	cerr << k << endl;
	lua_pop(state, 1);
}



void ScriptEngine::HandleCastError(luabind::cast_failed& err) {
	PRINT_ERROR << "the return value of a Lua function call could not be successfully converted "
		<< "to the specified C++ type: " << err.what() << endl;
}



void ScriptEngine::_AddOpenFile(ScriptDescriptor* sd) {
	// NOTE: This function assumes that the file is not already open

	_open_files.insert(make_pair(sd->_filename, sd));
	// Add the lua_State to the list of opened lua states if it is not already present
	if (sd->GetAccessMode() == SCRIPT_READ || sd->GetAccessMode() == SCRIPT_MODIFY) {
		ReadScriptDescriptor* rsd = dynamic_cast<ReadScriptDescriptor*>(sd);
		if (_open_threads.find(rsd->GetFilename()) == _open_threads.end())
			_open_threads[rsd->GetFilename()] = rsd->_lstack;
	}
}



void ScriptEngine::_RemoveOpenFile(ScriptDescriptor* sd) {
	// NOTE: Function assumes that the ScriptDescriptor file is already open
	_open_files.erase(sd->_filename);
}



lua_State *ScriptEngine::_CheckForPreviousLuaState(const std::string &filename) {
	return NULL; // TEMP, see todo notes in script.h

	if (_open_threads.find(filename) != _open_threads.end())
		return _open_threads[filename];
	else
		return NULL;
}


} // namespace hoa_script
