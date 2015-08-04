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
*** \file    script_read.cpp
*** \author  Daniel Steuernol - steu@allacrost.org,
*** \author  Tyler Olsen - roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for the ReadScriptDescriptor class.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "script_read.h"

#include "script.h"

#include "utils/utils_files.h"
#include "utils/utils_strings.h"

using namespace luabind;

using namespace vt_utils;
using namespace vt_script::private_script;

namespace vt_script
{

ReadScriptDescriptor::~ReadScriptDescriptor()
{
    if(IsFileOpen()) {
        IF_PRINT_WARNING(SCRIPT_DEBUG) << "destructor was called when file was still open: " << _filename << std::endl;
        CloseFile();
    }
}

//-----------------------------------------------------------------------------
// File Access Functions
//-----------------------------------------------------------------------------

bool ReadScriptDescriptor::OpenFile(const std::string &filename)
{
    // check for file existence
    if(!DoesFileExist(filename)) {
        PRINT_ERROR << "Attempted to open unavailable file: "
                    << filename << std::endl;
        return false;
    }

    if(ScriptManager->IsFileOpen(filename)) {
        PRINT_ERROR << "Attempted to open file that is already opened: "
                    << filename << std::endl;
        return false;
    }

    // Check that the thread stack is in sync with open files.
    assert(ScriptManager->_CheckForPreviousLuaState(filename) == nullptr);

    // Increases the global stack size by 1 element. That is needed because the new thread will be pushed in the
    // stack and we have to be sure there is enough space there.
    lua_checkstack(ScriptManager->GetGlobalState(), 1);
    _lstack = lua_newthread(ScriptManager->GetGlobalState());

    // Attempt to load and execute the Lua file
    if(luaL_loadfile(_lstack, filename.c_str()) != 0 || lua_pcall(_lstack, 0, 0, 0)) {
        PRINT_ERROR << "could not open script file: " << filename << ", error message:" << std::endl
                    << lua_tostring(_lstack, private_script::STACK_TOP) << std::endl;
        _access_mode = SCRIPT_CLOSED;
        return false;
    }

    _filename = filename;
    _access_mode = SCRIPT_READ;
    // If the file is already open, we don't add it twice.
    ScriptManager->_AddOpenFile(this);

    return true;
} // bool ReadScriptDescriptor::OpenFile(string file_name, bool force_reload)



bool ReadScriptDescriptor::OpenFile()
{
    if(_filename.empty()) {
        PRINT_ERROR << "could not open file because of an invalid file name (empty string)" << std::endl;
        return false;
    }

    return OpenFile(_filename);
}



void ReadScriptDescriptor::CloseFile()
{
    if(!IsFileOpen()) {
        PRINT_WARNING << "Could not close the file: " << _filename
                      << " because it was not open." << std::endl;
        return;
    }

    // Probably not needed. Script errors should be printed immediately.
    if(IsErrorDetected()) {
        PRINT_WARNING
                << "the file " << _filename << " had the following error messages remaining:"
                << std::endl << _error_messages.str() << std::endl;
    }

    _lstack = nullptr;
    _error_messages.clear();
    _open_tables.clear();
    _access_mode = SCRIPT_CLOSED;
    ScriptManager->_RemoveOpenFile(this);
}

//-----------------------------------------------------------------------------
// Existence Checking Functions
//-----------------------------------------------------------------------------

bool ReadScriptDescriptor::_DoesDataExist(const std::string &key, int32 type)
{
    // Check whether the user is trying to read a global variable or one stored in a table
    if(_open_tables.size() == 0) {  // Variable is a global
        lua_getglobal(_lstack, key.c_str());
        luabind::object o(luabind::from_stack(_lstack, private_script::STACK_TOP));
        return _CheckDataType(type, o);
    }

    else { // Variable is a member of a table
        luabind::object o(luabind::from_stack(_lstack, private_script::STACK_TOP));
        if(luabind::type(o) != LUA_TTABLE) {
            _error_messages << "* _DoesDataExist() failed because the top of the stack was not "
                            << "a table when trying to check for the table member: " << key << std::endl;
            return false;
        }

        luabind::object obj(o[key]);
        return _CheckDataType(type, obj);
    }
}



bool ReadScriptDescriptor::_DoesDataExist(int32 key, int32 type)
{
    if(_open_tables.size() == 0) {
        IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because no tables were open when trying to "
                                       << "examine the table member: " << key << std::endl;
        return false;
    }

    luabind::object o(luabind::from_stack(_lstack, private_script::STACK_TOP));
    if(luabind::type(o) != LUA_TTABLE) {
        IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because the top of the stack was not "
                                       << "a table when trying to check for the table member: " << key << std::endl;
        return false;
    }

    luabind::object obj(o[key]);
    return _CheckDataType(type, obj);
}



bool ReadScriptDescriptor::_CheckDataType(int32 type, luabind::object &obj_check)
{
    int32 object_type = luabind::type(obj_check);

    if(obj_check.is_valid() == false)
        return false;

    // When this type is passed to the function, we don't care what type the object is as long
    // as it was seen to be something
    if(type == LUA_TNIL)
        return true;

    // Simple type comparison is all that is needed for all non-numeric types
    if(type == object_type)
        return true;

    // Because Lua only has a "number" type, we have to do perform a special cast
    // to examine integer versus floating point types
    if(object_type == LUA_TNUMBER) {
        if(type == INTEGER_TYPE) {
            try {
                luabind::object_cast<int32>(obj_check);
                return true;
            } catch(...) {
                return false;
            }
        } else if(type == UINTEGER_TYPE) {
            try {
                luabind::object_cast<uint32>(obj_check);
                return true;
            } catch(...) {
                return false;
            }
        } else if(type == FLOAT_TYPE) {
            try {
                luabind::object_cast<float>(obj_check);
                return true;
            } catch(...) {
                return false;
            }
        } else {
            return false;
        }
    }

    else {
        return false;
    }
} // bool ReadScriptDescriptor::_CheckDataType(int32 type, luabind::object& obj_check)

//-----------------------------------------------------------------------------
// Function Pointer Read Functions
//-----------------------------------------------------------------------------

object ReadScriptDescriptor::ReadFunctionPointer(const std::string &key)
{
    if(_open_tables.size() == 0) {  // The function should be in the global space
        lua_getglobal(_lstack, key.c_str());

        luabind::object o(from_stack(_lstack, STACK_TOP));

        if(!o.is_valid()) {
            IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because it was unable to access the function "
                                           << "for the global key: " << key << std::endl;
            return luabind::object();
        }

        if(type(o) != LUA_TFUNCTION) {
            IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because the data retrieved was not a function "
                                           << "for the global key: " << key << std::endl;
            return luabind::object();
        }

        return o;
    }

    else { // The function should be an element of the most recently opened table
        luabind::object o(from_stack(_lstack, STACK_TOP));
        if(type(o) != LUA_TTABLE) {
            IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because the top of the stack was not a table "
                                           << "for the table element key: " << key << std::endl;
            return luabind::object();
        }

        if(type(o[key]) != LUA_TFUNCTION) {
            IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because the data retrieved was not a function "
                                           << "for the table element key: " << key << std::endl;
            return luabind::object();
        }

        return o[key];
    }
} // object ReadScriptDescriptor::ReadFunctionPointer(string key)



object ReadScriptDescriptor::ReadFunctionPointer(int32 key)
{
    // Fucntion is always a table element for integer keys
    luabind::object o(from_stack(_lstack, STACK_TOP));
    if(type(o) != LUA_TTABLE) {
        IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because the top of the stack was not a table "
                                       << "for the table element key: " << key << std::endl;
        return o;
    }

    if(type(o[key]) != LUA_TFUNCTION) {
        IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because the data retrieved was not a function "
                                       << "for the table element key: " << key << std::endl;
        return o;
    }

    return o[key];
} // object ReadScriptDescriptor::ReadFunctionPointer(int32 key)

//-----------------------------------------------------------------------------
// Table Operation Functions
//-----------------------------------------------------------------------------

bool ReadScriptDescriptor::OpenTable(const std::string &table_name, bool use_global)
{
    if(_open_tables.size() == 0 || use_global) {  // Fetch the table from the global space
        lua_getglobal(_lstack, table_name.c_str());
        if(!lua_istable(_lstack, STACK_TOP)) {
            IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because the data retrieved was not a table "
                                           << "or did not exist for the global key " << table_name << std::endl;
            return false;
        }
        _open_tables.push_back(table_name);
    }

    else { // The table to fetch is an element of another table
        if (!DoesTableExist(table_name))
            return false;

        lua_pushstring(_lstack, table_name.c_str());
        lua_gettable(_lstack, STACK_TOP - 1);
        if(!lua_istable(_lstack, STACK_TOP)) {
            IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because the data retrieved was not a table "
                                           << "or did not exist for the table element key " << table_name << std::endl;
            return false;
        }
        _open_tables.push_back(table_name);
    }
    return true;
} // bool ReadScriptDescriptor::OpenTable(string key)



bool ReadScriptDescriptor::OpenTable(int32 table_name)
{
    // At least one table must be open to use a numerical key
    if(_open_tables.size() == 0) {
        IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because there were no tables open when trying "
                                       << "to open the with the element key " << table_name << std::endl;
        return false;
    }

    if (!DoesTableExist(table_name))
        return false;

    lua_pushnumber(_lstack, table_name);

    if(!lua_istable(_lstack, STACK_TOP - 1)) {
        IF_PRINT_WARNING(SCRIPT_DEBUG) << "about to fail because STACK_TOP - 1 is not a "
                                       << "table, or the table does not exist for the table element key: " << table_name << std::endl;
        return false;
    }

    // Note: This call is unsafe and might make the game crash.
    lua_gettable(_lstack, STACK_TOP - 1);

    if(!lua_istable(_lstack, STACK_TOP)) {
        IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because the data retrieved was not a table "
                                       << "or did not exist for the table element key " << table_name << std::endl;
        return false;
    }

    _open_tables.push_back(NumberToString(table_name));
    return true;
} // bool ReadScriptDescriptor::OpenTable(int32 key)


std::string ReadScriptDescriptor::OpenTablespace()
{
    if(!IsFileOpen()) {
        PRINT_ERROR << "Can't open a table space without opening a script file." << std::endl;
        return std::string();
    }

    std::string tablespace = GetTableSpace();

    if(tablespace.empty()) {
        PRINT_ERROR << "The script filename is not valid to be used as tablespace name: " << _filename << std::endl;
        return std::string();
    }

    if (OpenTable(tablespace, true)) // Open the tablespace from the global stack.
        return tablespace;
    else
        return std::string();
}


void ReadScriptDescriptor::CloseTable()
{
    if(_open_tables.size() == 0) {
        IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because there were no open tables to close" << std::endl;
        return;
    }

    _open_tables.pop_back();
    lua_pop(_lstack, 1);
}



void ReadScriptDescriptor::CloseAllTables()
{
    while(_open_tables.size() != 0) {
        CloseTable();
    }
}



uint32 ReadScriptDescriptor::GetTableSize(const std::string &table_name)
{
    uint32 size = 0;

    if (OpenTable(table_name)) {
        size = GetTableSize();
        CloseTable();
    }
    return size;
}

void ReadScriptDescriptor::ClearStack(uint32 levels_to_clear)
{
    _open_tables.clear();
    for(uint32 i = 0; i < levels_to_clear; ++i)
        lua_remove(_lstack, 0);
}


uint32 ReadScriptDescriptor::GetTableSize(int32 table_name)
{
    uint32 size = 0;

    if (OpenTable(table_name)) {
        size = GetTableSize();
        CloseTable();
    }

    return size;
}


// Attempts to get the size of the most recently opened table
uint32 ReadScriptDescriptor::GetTableSize()
{
    if(_open_tables.size() == 0) {
        IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because there were no open tables to get the size of" << std::endl;
        return 0;
    }

    // lua returns 0 on table sizes in a couple situations
    // 1. the indexes don't start from what lua expects
    // 2. a hash table instead of an array table.
    // So we'll just count the table size ourselves
    object o(from_stack(_lstack, STACK_TOP));

    if(type(o) != LUA_TTABLE) {
        IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because the top of the stack is not a table." << std::endl;
        return 0;
    }

    uint32 table_size = 0;
    for(luabind::iterator i(o); i != private_script::TABLE_END; ++i)
        table_size++;

    return table_size;
}


bool ReadScriptDescriptor::RunScriptFunction(const std::string &filename,
        const std::string &function_name,
        bool global)
{
    // The error message handling is done into OpenFile()
    if(!OpenFile(filename))
        return false;

    if(!DoesFunctionExist(function_name)) {
        PRINT_ERROR << "No '" << function_name << "' function!" << std::endl;
        CloseFile();
        return false;
    }

    bool ran = false;
    if(global || !OpenTablespace().empty())
        ran = RunScriptFunction(function_name);

    CloseFile();

    return ran;
}


bool ReadScriptDescriptor::RunScriptFunction(const std::string &function_name)
{
    if(!IsFileOpen()) {
        PRINT_ERROR << "Can't call function " << function_name << "without opening a script file." << std::endl;
        return false;
    }

    try {
        ScriptCallFunction<void>(GetLuaState(), function_name.c_str());
    } catch(const luabind::error &e) {
        PRINT_ERROR << "Error while loading :" << function_name << std::endl;
        ScriptManager->HandleLuaError(e);
        return false;
    } catch(const luabind::cast_failed &e) {
        PRINT_ERROR << "Error while loading :" << function_name << std::endl;
        ScriptManager->HandleCastError(e);
    }

    return true;
}


bool ReadScriptDescriptor::RunScriptObject(const luabind::object &object)
{

    // Don't log in that case because we might want to run invalid (empty) objects
    // to simplify the caller code.
    if(!object.is_valid())
        return true;

    try {
        ScriptCallFunction<void>(object);
    } catch(const luabind::error &e) {
        PRINT_ERROR << "Error while loading script object." << std::endl;
        ScriptManager->HandleLuaError(e);
        return false;
    } catch(const luabind::cast_failed &e) {
        PRINT_ERROR << "Error while loading script object." << std::endl;
        ScriptManager->HandleCastError(e);
    }
    return true;
}

} // namespace vt_script
