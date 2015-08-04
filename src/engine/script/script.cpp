///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    script.cpp
*** \author  Daniel Steuernol - steu@allacrost.org,
*** \author  Tyler Olsen - roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
***
*** \brief   Source file for the scripting engine.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "script.h"

#include "script_read.h"

using namespace luabind;

using namespace vt_utils;
using namespace vt_script::private_script;

namespace vt_script
{

ScriptEngine *ScriptManager = nullptr;
bool SCRIPT_DEBUG = false;

//-----------------------------------------------------------------------------
// ScriptEngine Class Functions
//-----------------------------------------------------------------------------

ScriptEngine::ScriptEngine()
{
    IF_PRINT_DEBUG(SCRIPT_DEBUG) << "ScriptEngine constructor invoked." << std::endl;

    // Initialize Lua and LuaBind
    _global_state = luaL_newstate();
    luaL_openlibs(_global_state);
    luabind::open(_global_state);
}

ScriptEngine::~ScriptEngine()
{
    IF_PRINT_DEBUG(SCRIPT_DEBUG) << "ScriptEngine Destruction..." << std::endl;

    for (std::pair<std::string, vt_script::ScriptDescriptor*> openedFile : _open_files)
        PRINT_WARNING << "Script file still open when quitting application: " << openedFile.first << std::endl;

    if (!_open_threads.empty())
        PRINT_WARNING << "Lua threads still open when quitting application: " << _open_threads.size() << std::endl;

    lua_close(_global_state);
    _global_state = nullptr;
}

bool ScriptEngine::SingletonInitialize()
{
    return true;
}

bool ScriptEngine::IsFileOpen(const std::string &filename)
{
    if(_open_files.find(filename) != _open_files.end())
        return true;

    return false;
}

void ScriptEngine::HandleLuaError(const luabind::error &err)
{
    lua_State *state = err.state();
    PRINT_ERROR << "a runtime Lua error has occured with the following error message:\n  " << std::endl;
    std::string k = lua_tostring(state, lua_gettop(state)) ;
    PRINT_ERROR << k << std::endl;
    lua_pop(state, 1);
}

void ScriptEngine::HandleCastError(const luabind::cast_failed &err)
{
    PRINT_ERROR << "the return value of a Lua function call could not be successfully converted "
                << "to the specified C++ type: " << err.what() << std::endl;
}

void ScriptEngine::_AddOpenFile(ScriptDescriptor *sd)
{
    // NOTE: This function assumes that the file is not already open
    _open_files.insert(std::make_pair(sd->_filename, sd));
    // Add the lua_State to the list of opened lua states if it is not already present
    if(sd->GetAccessMode() == SCRIPT_READ) {
        ReadScriptDescriptor *rsd = dynamic_cast<ReadScriptDescriptor *>(sd);
        if(_open_threads.find(rsd->GetFilename()) == _open_threads.end())
            _open_threads[rsd->GetFilename()] = rsd->_lstack;
    }
}

void ScriptEngine::_RemoveOpenFile(ScriptDescriptor *sd)
{
    // NOTE: Function assumes that the ScriptDescriptor file is already open
    _open_files.erase(sd->_filename);

    // Remove the thread reference from memory, permitting lua to later drop it.
    _open_threads.erase(sd->_filename);
}

lua_State* ScriptEngine::_CheckForPreviousLuaState(const std::string &filename)
{
    if(_open_threads.find(filename) != _open_threads.end())
        return _open_threads[filename];
    else
        return nullptr;
}

void ScriptEngine::DEBUG_PrintLuaStack(lua_State* luaState)
{
    if (luaState == nullptr) {
        PRINT_ERROR << "Empty lua State!" << std::endl;
        return;
    }

    std::cout << "SCRIPT DEBUG: Printing script's lua stack:" << std::endl;
    // Print each element starting from the top of the stack
    for(int32 i = lua_gettop(luaState); i > 0; --i) {
        int32 type = lua_type(luaState, i);
        switch(type) {
        case LUA_TNIL:
            std::cout << "* " << i << "= NIL" << std::endl;
            break;
        case LUA_TBOOLEAN:
            std::cout << "* " << i << "= BOOLEAN: " << lua_toboolean(luaState, i) << std::endl;
            break;
        case LUA_TNUMBER:
            std::cout << "* " << i << "= NUMBER:  " << lua_tonumber(luaState, i) << std::endl;
            break;
        case LUA_TSTRING:
            std::cout << "* " << i << "= STRING:  " << lua_tostring(luaState, i) << std::endl;
            break;
        case LUA_TTABLE:
            std::cout << "* " << i << "= TABLE" << std::endl;
            break;
        case LUA_TFUNCTION:
            std::cout << "* " << i << "= FUNCTION" << std::endl;
            break;
        case LUA_TUSERDATA:
            std::cout << "* " << i << "= USERDATA " << std::endl;
            break;
        case LUA_TLIGHTUSERDATA:
            std::cout << "* " << i << "= LIGHTUSERDATA " << std::endl;
            break;
        case LUA_TTHREAD:
            std::cout << "* " << i << "= THREAD " << std::endl;
            break;
        default:
            std::cout << "* " << i << "= OTHER: " << lua_typename(luaState, type) << std::endl;
            break;
        }
    }
    std::cout << std::endl;
}

void ScriptEngine::DEBUG_PrintGlobals(lua_State* luaState)
{
    if (luaState == nullptr) {
        PRINT_ERROR << "Empty lua State!" << std::endl;
        return;
    }

    std::cout << "SCRIPT DEBUG: Printing script's global variables:" << std::endl;

    // Push the global table on top of the stack
    lua_pushglobaltable(luaState);

    // -1 is the value on top of the stack, here the global table index
    luabind::object o(luabind::from_stack(luaState, -1));
    for(luabind::iterator it(o), end; it != end; ++it) {
        std::cout << it.key() << " = " << (*it) << " ::: data type = " << luabind::type(*it) << std::endl;
        if(luabind::type(*it) == LUA_TTABLE) {
            if(luabind::object_cast<std::string>(it.key()) != "_G")
                DEBUG_PrintTable(luabind::object(*it), 1);
        }
    }
    std::cout << std::endl;

    // Remove the table afterwards
    lua_pop(luaState, 1);
}

void ScriptEngine::DEBUG_PrintTable(luabind::object table, int tab)
{
    for(luabind::iterator it(table), end; it != end; ++it) {
        for(int i = 0; i < tab; ++i)
            std::cout << '\t';
        std::cout << it.key() << " = " << (*it) << " (Type: " << luabind::type(*it) << ")" << std::endl;
        if(type(*it) == LUA_TTABLE)
            DEBUG_PrintTable(luabind::object(*it), tab + 1);
    }
}

void ScriptEngine::DEBUG_DumpScriptsState()
{
    std::cout << "Lua script states/threads open: " << _open_threads.size() << std::endl;
    std::cout << "Script files open: " << _open_files.size() << std::endl;

    std::cout << "-----------------" << std::endl
                << "Different lua state contents:" << std::endl;
    for (std::pair<std::string, vt_script::ScriptDescriptor*> openedFile : _open_files) {
        std::cout << "Script file: " << openedFile.first << " ";
        if (openedFile.second->GetAccessMode() != SCRIPT_ACCESS_MODE::SCRIPT_READ) {
            std::cout << "(Write mode)" << std::endl;
            continue;
        }
        std::cout << "(Read mode)" << std::endl;
        ReadScriptDescriptor* readScript = static_cast<ReadScriptDescriptor*>(openedFile.second);
        ScriptEngine::DEBUG_PrintLuaStack(readScript->GetLuaState());
    }
    std::cout << "-----------------" << std::endl;
    std::cout << "Global stack content:" << std::endl;
    ScriptEngine::DEBUG_PrintLuaStack(_global_state);
}

} // namespace vt_script
