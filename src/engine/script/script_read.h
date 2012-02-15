///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    script_read.h
*** \author  Daniel Steuernol - steu@allacrost.org,
***          Tyler Olsen - roots@allacrost.org
*** \brief   Header file for the ReadScriptDescriptor class.
*** ***************************************************************************/

#ifndef __SCRIPT_READ_HEADER__
#define __SCRIPT_READ_HEADER__

#include "utils.h"
#include "defs.h"

#include "script.h"

namespace hoa_script {

namespace private_script {

/** \brief Constants used in the ReadScriptDescriptor data existence checking functions
*** These members are necessary because the Lua type constants defined do not distinguish
*** between integers and floats (there is only a single LUA_TNUMBER). We, however, need
*** to sometimes distinguish between the two types when reading in data. These constants
*** were assigned arbitrary and unlikely values to be represented by other Lua types
*** (which as of Lua 5.0, go from -1 to 8).
**/
//@{
const int32 INTEGER_TYPE    = 0x12345678;
const int32 UINTEGER_TYPE   = 0x87654321;
const int32 FLOAT_TYPE      = 0x12344321;
//@}

} // namespace private_script

/** ****************************************************************************
*** \brief Represents a Lua file opened with read and execute permissions
***
*** This is the most commonly used of the script descriptor classes. This class
*** allows you to read simple data types, open and close tables contained within
*** the file, check for the existence of specific variable names and identifiers,
*** retrieve pointers to Lua functions which can then be executed, and more.
***
*** The most important concept for understanding how to use this class is that of
*** <b>table scope</b>. A scope is the portion of the Lua file that many of the
*** data read operations in this class operate on. For example, the global scope
*** contains all global variables, functions, and tables. There is a unique scope
*** inherit inside each table in the file (including the tables of tables). When
*** you invoke a read operation, ReadInt() for example, it attempts to read an
*** integer in the current scope. If you have no tables open, it will look in the
*** global space. Otherwise, it will look in the most recently opened table. Thus,
*** it is <b>extremely</b> important that you know which scope is "active", and
*** to know when to open and close tables to be in the correct scope.
***
*** \note With the exception of the open/close file functions, all methods of
*** this class assume that the file is open and do not check otherwise. If you
*** try to invoke these functions with an unopened file, you will generate a
*** segmentation fault.
*** ***************************************************************************/
class ReadScriptDescriptor : public ScriptDescriptor {
	friend class ScriptEngine;
public:
	ReadScriptDescriptor() :
		_lstack(NULL) {}

	virtual ~ReadScriptDescriptor();

	/** \name File Access Functions
	*** \note These are derived from ScriptDescriptor, refer to the comments for these
	*** methods in the header file for that class.
	**/
	//@{
	virtual bool OpenFile(const std::string& file_name);
	virtual bool OpenFile(const std::string& file_name, bool force_reload);
	virtual bool OpenFile();
	virtual void CloseFile();
	//@}

	/** \name Existence Checking Functions
	*** \brief Methods which check if there exist certain data names and types in a script file
	*** \param key The variable, table, or function name to check for
	*** \return True if the appropriate data type represented by that variable name was found.
	***
	*** Under normal circumstances, invoking any of these functions should not result in error
	*** messages being printed, even if the variable could not be found. There are some
	*** circumstances, however, where error messages may be added, such as when it is detected
	*** that a table is missing or the active scope is not appropriate for the function call.
	***
	*** \note The DoesVariableExist checks if is there is <b>any</b> data type, structure, or
	*** function referenced by the key name.
	***
	*** \note The DoesNumberExists checks if there is <b>any</b> numeric data type integer or
	*** floating point value referenced by the key name.
	**/
	//@{
	bool DoesVariableExist(const std::string& key)
		{ return _DoesDataExist(key, LUA_TNIL); }

	bool DoesVariableExist(int32 key)
		{ return _DoesDataExist(key, LUA_TNIL); }

	bool DoesNumberExist(const std::string& key)
		{ return _DoesDataExist(key, LUA_TNUMBER); }

	bool DoesNumberExist(int32 key)
		{ return _DoesDataExist(key, LUA_TNUMBER); }

	bool DoesBoolExist(const std::string& key)
		{ return _DoesDataExist(key, LUA_TBOOLEAN); }

	bool DoesBoolExist(int32 key)
		{ return _DoesDataExist(key, LUA_TBOOLEAN); }

	bool DoesIntExist(const std::string& key)
		{ return _DoesDataExist(key, private_script::INTEGER_TYPE); }

	bool DoesIntExist(int32 key)
		{ return _DoesDataExist(key, private_script::INTEGER_TYPE); }

	bool DoesUIntExist(const std::string& key)
		{ return _DoesDataExist(key, private_script::UINTEGER_TYPE); }

	bool DoesUIntExist(int32 key)
		{ return _DoesDataExist(key, private_script::UINTEGER_TYPE); }

	bool DoesFloatExist(const std::string& key)
		{ return _DoesDataExist(key, private_script::FLOAT_TYPE); }

	bool DoesFloatExist(int32 key)
		{ return _DoesDataExist(key, private_script::FLOAT_TYPE); }

	bool DoesStringExist(const std::string& key)
		{ return _DoesDataExist(key, LUA_TSTRING); }

	bool DoesStringExist(int32 key)
		{ return _DoesDataExist(key, LUA_TSTRING); }

	bool DoesFunctionExist(const std::string& key)
		{ return _DoesDataExist(key, LUA_TFUNCTION); }

	bool DoesFunctionExist(int32 key)
		{ return _DoesDataExist(key, LUA_TFUNCTION); }

	bool DoesTableExist(const std::string& key)
		{ return _DoesDataExist(key, LUA_TTABLE); }

	bool DoesTableExist(int32 key)
		{ return _DoesDataExist(key, LUA_TTABLE); }
	//@}

	/** \name Variable Read Functions
	*** \brief These functions grab a basic data type from the Lua file and return its value.
	*** \param key The name or numeric id of the Lua variable to access.
	*** \return The value of the variable requested.
	*** \note The integer keys are only valid for variables stored in a table, not for global variables.
	***
	*** These functions call the template _ReadData() functions with a default return value.
	**/
	//@{
	bool ReadBool(const std::string& key)
		{ return _ReadData<bool>(key, false); }

	bool ReadBool(int32 key)
		{ return _ReadData<bool>(key, false); }

	int32 ReadInt(const std::string& key)
		{ return _ReadData<int32>(key, 0); }

	int32 ReadInt(int32 key)
		{ return _ReadData<uint32>(key, 0); }

	uint32 ReadUInt(const std::string& key)
		{ return _ReadData<int32>(key, 0); }

	uint32 ReadUInt(int32 key)
		{ return _ReadData<uint32>(key, 0); }

	float ReadFloat(const std::string& key)
		{ return _ReadData<float>(key, 0.0f); }

	float ReadFloat(int32 key)
		{ return _ReadData<float>(key, 0.0f); }

	std::string ReadString(const std::string& key)
		{ return _ReadData<std::string>(key, ""); }

	std::string ReadString(int32 key)
		{ return _ReadData<std::string>(key, ""); }

	hoa_utils::ustring ReadUString(const std::string& key)
		{ return _ReadData<hoa_utils::ustring>(key, hoa_utils::MakeUnicodeString("")); }

	hoa_utils::ustring ReadUString(int32 key)
		{ return _ReadData<hoa_utils::ustring>(key, hoa_utils::MakeUnicodeString("")); }
	//@}

	/** \name Vector Read Functions
	*** \brief These functions fill a vector with members of a table read from the Lua file.
	*** \param key The name of the table to use to fill the vector.
	*** \param vect A reference to the vector of elements to fill.
	***
	*** The table that these functions attempt to access is assumed to be <b>closed</b>. If the
	*** table is open prior to calling these functions, they will not operate properly. All of
	*** these functions call the template _ReadDataVector() functions to perform their operations.
	***
	*** \note The integer keys are only valid for tables that are elements of a parent table.
	*** They can not be used to access tables in the global space.
	**/
	//@{
	void ReadBoolVector(const std::string& key, std::vector<bool>& vect)
		{ _ReadDataVector<bool>(key, vect); }

	void ReadBoolVector(int32 key, std::vector<bool>& vect)
		{ _ReadDataVector<bool>(key, vect); }

	void ReadIntVector(const std::string& key, std::vector<int32>& vect)
		{ _ReadDataVector<int32>(key, vect); }

	void ReadIntVector(int32 key, std::vector<int32>& vect)
		{ _ReadDataVector<int32>(key, vect); }

	void ReadUIntVector(const std::string& key, std::vector<uint32>& vect)
		{ _ReadDataVector<uint32>(key, vect); }

	void ReadUIntVector(int32 key, std::vector<uint32>& vect)
		{ _ReadDataVector<uint32>(key, vect); }

	void ReadFloatVector(const std::string& key, std::vector<float>& vect)
		{ _ReadDataVector<float>(key, vect); }

	void ReadFloatVector(int32 key, std::vector<float>& vect)
		{ _ReadDataVector<float>(key, vect); }

	void ReadStringVector(const std::string& key, std::vector<std::string>& vect)
		{ _ReadDataVector<std::string>(key, vect); }

	void ReadStringVector(int32 key, std::vector<std::string>& vect)
		{ _ReadDataVector<std::string>(key, vect); }

	void ReadUStringVector(const std::string& key, std::vector<hoa_utils::ustring>& vect)
		{ _ReadDataVector<hoa_utils::ustring>(key, vect); }

	void ReadUStringVector(int32 key, std::vector<hoa_utils::ustring>& vect)
		{ _ReadDataVector<hoa_utils::ustring>(key, vect); }
	//@}

	/** \name Function Pointer Read Functions
	*** \param key The name of the function if it is contained in the global space, or the key
	*** if the function is embedded in a table.
	*** \return A luabind::object class object, which can be used to call the function. It effectively
	*** serves as a function pointer.
	**/
	//@{
	luabind::object ReadFunctionPointer(const std::string& key);

	//! \note The calling function may <b>not</b> be contained within the global space for an integer key.
	luabind::object ReadFunctionPointer(int32 key);
	//@}

	/** \name Table Operation Functions
	*** After a table is opened, it becomes the active "space" that all of the data read
	*** operations operate on. You must <b>always</b> remember to close a table once you are
	*** finished reading data from it.
	**/
	//@{
	//! \param table_name The name of the table to open
	//! \param use_global This overrides the open_tables vector, the reason for this is
	//! when a function is called from lua, any open tables are no longer on the stack passed to the function
	//! so to start a new chain of open tables we have to ignore the open tables vector
	void OpenTable(const std::string& table_name, bool use_global = false);

	/** \param table_name The integer key of the table to open
	*** \note This function will only work when there is at least one other table already open
	**/
	void OpenTable(int32 table_name);

	//! \brief Closes the most recently opened table
	void CloseTable();

	//! \brief Closes any open tables and returns the user to the global scope
	void CloseAllTables();

	/** \brief Returns the number of elements stored in an un-opened table
	*** \param table_name The name of the (un-open) table to get the size of
	**/
	uint32 GetTableSize(const std::string& table_name);

	/** \brief Returns the number of elements stored in an un-opened table
	*** \param table_name The integer key of the (un-open) table to get the size of
	*** \note This function will only work when there is at least one other table already open
	**/
	uint32 GetTableSize(int32 table_name);

	//! \brief Returns the number of elements stored in the most recently opened table
	uint32 GetTableSize();

	//! \brief Emptys all values off the lua stack
	void ClearStack(uint32 levels_to_clear);

	/** \brief Fills a vector with all of the keys of a table
	*** \param table_name The name of the table to open and retrieve the keys from
	*** \param keys A reference to the vector where to store the table keys
	***
	*** The functions without a table_name argument will retrieve the keys for the
	*** most recently opened table. The keys vector will cleared before the function
	*** starts populating it with table keys, so make sure nothing important is contained
	*** in the keys vector before calling this function. If the keys vector is empty
	*** after the function was called, either an error occured or the table was empty.
	***
	*** \note These functions will only work successfully for tables that have the same
	*** data type for all their keys (ie, all string keys or all integer keys). A table
	*** with mixed key types (integers and strings for example) will not be processed
	*** successfully by these functions.
	**/
	//@{
	void ReadTableKeys(std::vector<std::string>& keys)
		{ _ReadTableKeys(keys); }

	void ReadTableKeys(std::vector<int32>& keys)
		{ _ReadTableKeys(keys); }

	void ReadTableKeys(std::vector<uint32>& keys)
		{ _ReadTableKeys(keys); }

	void ReadTableKeys(const std::string& table_name, std::vector<std::string>& keys)
		{ OpenTable(table_name); _ReadTableKeys(keys); CloseTable(); }

	void ReadTableKeys(const std::string& table_name, std::vector<int32>& keys)
		{ OpenTable(table_name); _ReadTableKeys(keys); CloseTable(); }

	void ReadTableKeys(const std::string& table_name, std::vector<uint32>& keys)
		{ OpenTable(table_name); _ReadTableKeys(keys); CloseTable(); }

	void ReadTableKeys(int32 table_name, std::vector<std::string>& keys)
		{ OpenTable(table_name); _ReadTableKeys(keys); CloseTable(); }

	void ReadTableKeys(int32 table_name, std::vector<int32>& keys)
		{ OpenTable(table_name); _ReadTableKeys(keys); CloseTable(); }

	void ReadTableKeys(int32 table_name, std::vector<uint32>& keys)
		{ OpenTable(table_name); _ReadTableKeys(keys); CloseTable(); }
	//@}
	//@}

	//! \brief Returns a pointer to the local lua state (use with caution)
	lua_State* GetLuaState()
		{ return _lstack; }

	/** \brief Prints out the contents of the Lua stack mechanism to standard output
	*** The elements are printed from stack top to stack bottom.
	**/
	void DEBUG_PrintLuaStack();

	//! \brief Prints out all global variable names to standard output
	void DEBUG_PrintGlobals();

	//! \brief Prints out a specific table
	void DEBUG_PrintTable(luabind::object table, int tab = 0);

protected:
	//! \brief The Lua stack, which handles all data sharing between C++ and Lua.
	lua_State *_lstack;

	/** \name Data Existence Check Functions
	*** \brief These functions are called by the public DoesTYPEExist functions of this class.
	*** \param key The name or numeric id of the Lua data to check.
	*** \param type The correct type that the data should check out to be (uses the Lua type constants)
	*** \return True if data of the corresponding key and type is found.
	*** \note The integer keys are only valid for variables stored in a table, not for global variables.
	**/
	//@{
	bool _DoesDataExist(const std::string& key, int32 type);
	bool _DoesDataExist(int32 key, int32 type);

	/** \brief A helper function for the _DoesDataExist functions that performs the data type check
	*** \param type An integer type to compare with the type of the object
	*** \param obj_check The Luabind object whose type to compare to the integer type
	*** \return True if the two types are equivalent
	**/
	bool _CheckDataType(int32 type, luabind::object& obj_check);
	//@}

	/** \name Variable Read Templates
	*** \brief These template functions are called by the public ReadTYPE functions of this class.
	*** \param key The name or numeric identifier of the Lua variable to access.
	*** \param default_value The value for the function to return if an error occurs.
	*** \return The value of the variable requested.
	*** \note Integer keys are only valid for variables stored in a table, not for global variables.
	**/
	//@{
	template <class T> T _ReadData(const std::string& key, T default_value);
	template <class T> T _ReadData(int32 key, T default_value);
	//@}

	/** \name Vector Read Templates
	*** \brief These template functions are called by the public ReadTYPEVector functions of this class.
	*** \param key The name or numeric identifier of the Lua variable to access.
	*** \param vect A reference to the vector where the read variables should be stored
	*** \note Integer keys are only valid for variables stored in a table, not for global variables.
	**/
	//@{
	template <class T> void _ReadDataVector(const std::string& key, std::vector<T>& vect);
	template <class T> void _ReadDataVector(int32 key, std::vector<T>& vect);
	//! \brief This template method is a helper function for the other two
	template <class T> void _ReadDataVectorHelper(std::vector<T>& vect);
	//@}

	/** \name Table Key Template
	*** \brief This template function fills a vector with all of the keys contained by the table
	*** \param vect A reference to the vector where the keys should be stored
	*** \note This function will fail for any table that has multiple key types (ie, contains both
	*** integer and string keys).
	**/
	template <class T> void _ReadTableKeys(std::vector<T>& keys);
}; // class ReadScriptDescriptor

//-----------------------------------------------------------------------------
// Template Function Definitions
//-----------------------------------------------------------------------------

template <class T> T ReadScriptDescriptor::_ReadData(const std::string &key, T default_value) {
	// Check whether the user is trying to read a global variable or one stored in a table
	if (_open_tables.size() == 0) { // Variable is a global
		lua_getglobal(_lstack, key.c_str());
		luabind::object o(luabind::from_stack(_lstack, private_script::STACK_TOP));

		if (!o) {
			IF_PRINT_WARNING(SCRIPT_DEBUG) << "unable to access the global variable: " << key
			<< "   Type: " << luabind::type(o) << std::endl;
			return default_value;
		}

		try {
			T ret_val = luabind::object_cast<T>(o);
			lua_pop(_lstack, 1);
			return ret_val;
		}
		catch (...) {
			IF_PRINT_WARNING(SCRIPT_DEBUG) << "unable to cast value to correct type for global variable: " << key << std::endl;
			return default_value;
		}
	}

	else { // Variable is a member of a table
		luabind::object o(luabind::from_stack(_lstack, private_script::STACK_TOP));
		if (luabind::type(o) != LUA_TTABLE) {
			IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because the top of the stack was not a table when trying to read variable: " << key
			<< "   Type: " << luabind::type(o) << std::endl;
			return default_value;
		}

		try {
			return luabind::object_cast<T>(o[key]);
		}
		catch (...) {
			IF_PRINT_WARNING(SCRIPT_DEBUG) << "unable to access the table variable: " << key << std::endl;
			return default_value;
		}
	}
	return default_value;
} // template <class T> T ReadScriptDescriptor::_ReadData(const char *key, T default_value)



template <class T> T ReadScriptDescriptor::_ReadData(int32 key, T default_value) {
	if (_open_tables.size() == 0) {
		IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because no tables were open when trying to access the table variable: "
			<< key << std::endl;
		return default_value;
	}

	luabind::object o(luabind::from_stack(_lstack, private_script::STACK_TOP));
	if (luabind::type(o) != LUA_TTABLE) {
		IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because the top of the stack was not a table when trying to read variable: "
			<< key << std::endl;
		return default_value;
	}

	try {
		return luabind::object_cast<T>(o[key]);
	}
	catch (...) {
		IF_PRINT_WARNING(SCRIPT_DEBUG) << "unable to access the table variable: " << key << std::endl;
		return default_value;
	}

	return default_value;
} // template <class T> T ReadScriptDescriptor::_ReadData(int32 key, T default_value)



template <class T> void ReadScriptDescriptor::_ReadDataVector(const std::string& key, std::vector<T>& vect) {
	// Open the table and grab if off the stack
	OpenTable(key);
	_ReadDataVectorHelper(vect);
	CloseTable();
} // template <class T> void ReadScriptDescriptor::_ReadDataVector(std::string key, std::vector<T> &vect)



template <class T> void ReadScriptDescriptor::_ReadDataVector(int32 key, std::vector<T>& vect) {
	if (_open_tables.size() == 0) {
		IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because no tables were open when trying to access the table variable: "
			<< key << std::endl;;
		return;
	}

	// Open the table and grab if off the stack
	OpenTable(key);
	_ReadDataVectorHelper(vect);
	CloseTable();
} // template <class T> void ReadScriptDescriptor::_ReadDataVector(int32 key, std::vector<T> &vect)



template <class T> void ReadScriptDescriptor::_ReadDataVectorHelper(std::vector<T>& vect) {
	luabind::object o(luabind::from_stack(_lstack, private_script::STACK_TOP));

	if (luabind::type(o) != LUA_TTABLE) {
		IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because the top of the stack was not a table" << std::endl;
		return;
	}

	// Iterate through all the items of the table and place it in the vector
	for (luabind::iterator it(o); it != private_script::TABLE_END; it++) {
		try {
			vect.push_back(luabind::object_cast<T>((*it)));
		}
		catch (...) {
			IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed due to a type cast failure when reading the table" << std::endl;
		}
	}
} // template <class T> void ReadScriptDescriptor::_ReadDataVectorHelper(std::vector<T>& vect)



template <class T> void ReadScriptDescriptor::_ReadTableKeys(std::vector<T>& keys) {
	keys.clear();

	if (_open_tables.size() == 0) {
		IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because there were no open tables to get the keys of" << std::endl;
		return;
	}

	luabind::object table(luabind::from_stack(_lstack, private_script::STACK_TOP));

	if (luabind::type(table) != LUA_TTABLE) {
		IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed because the top of the stack was not a table" << std::endl;
		return;
	}

	for (luabind::iterator i(table); i != private_script::TABLE_END; i++) {
		try {
			keys.push_back(luabind::object_cast<T>(i.key()));
		}
		catch (...) {
			IF_PRINT_WARNING(SCRIPT_DEBUG) << "failed due to a type cast failure when retrieving a table key" << std::endl;
			keys.clear();
			return;
		}
	}
} // template <class T> void ReadScriptDescriptor::ReadTableKeys(std::vector<T>& keys) {

} // namespace hoa_script

#endif // __SCRIPT_READ_HEADER__
