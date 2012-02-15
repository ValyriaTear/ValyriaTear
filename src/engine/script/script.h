///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    script.h
*** \author  Daniel Steuernol - steu@allacrost.org,
***          Tyler Olsen - roots@allacrost.org
*** \brief   Header file for the scripting engine.
***
*** This code serves as the bridge between the game engine (written in C++) and
*** the script files and code (written in Lua).
***
*** \note You shouldn't need to modify this code if you are wishing to extend
*** the game (either for a new inherited GameMode class, or a new data/scripting
*** file). Contact the author of this code if you feel it lacks functionality
*** that you need.
*** ***************************************************************************/

#ifndef __SCRIPT_HEADER__
#define __SCRIPT_HEADER__

#include <sstream>
#include <fstream>
extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

// This needs a comment: what is check and why is it undefined for darwin?
#ifdef __MACH__
	#undef check
#endif

#include <luabind/luabind.hpp>
#include <luabind/object.hpp>
#include <luabind/adopt_policy.hpp>

#include "utils.h"
#include "defs.h"

//! \brief All calls to the scripting engine are wrapped in this namespace.
namespace hoa_script {

//! \brief The singleton pointer responsible for the interaction between the C++ engine and Lua scripts.
extern ScriptEngine* ScriptManager;

//! \brief Determines whether the code in the hoa_script namespace should print debug statements or not.
extern bool SCRIPT_DEBUG;

/** \name Script File Access Modes
*** \brief Used to indicate with what priveledges a file is to be opened with.
**/
enum SCRIPT_ACCESS_MODE {
	SCRIPT_CLOSED  = 0,
	SCRIPT_READ    = 1,
	SCRIPT_WRITE   = 2,
	SCRIPT_MODIFY  = 3
};

/** \brief A macro for a reference to a Lua object
*** The object may be any type of Lua data, including booleans, integers, floats, strings, tables,
*** functions, etc. This member is typically used outside of this engine as a reference to make
*** Lua function calls.
**/
#define ScriptObject luabind::object

//! \brief A macro for making Lua function calls
#define ScriptCallFunction luabind::call_function

//! An internal namespace to be used only by the scripting engine itself. Don't use this namespace anywhere else!
namespace private_script {

//! \brief Used to reference to the top of a Lua stack.
const int32 STACK_TOP = -1;

//! \brief Used to represent the end of a Lua table that is being iterated
const luabind::iterator TABLE_END;

} // namespace private_script

/** ****************************************************************************
*** \brief An abstract class for representing open script files
***
*** This class is an abstract class for representing a Lua script file. Files
*** with a .lua extension are human-readable, uncompiled Lua text files and
*** files with a .hoa extension are compiled script files.
***
*** \note Compiled Lua files exhibit faster performance than uncompile files.
*** ***************************************************************************/
class ScriptDescriptor {
	friend class ScriptEngine;

public:
	ScriptDescriptor ()
		{ _filename = ""; _access_mode = SCRIPT_CLOSED; _error_messages.clear(); }

	//! \brief If there are any error messages remaining, the destructor will print them when in debug mode
	virtual ~ScriptDescriptor ()
		{}

	//! \name File access functions
	//@{
	/** \param file_name The name of the Lua file to be opened.
	*** \return False on failure or true on success.
	*** \note This is the only function that uses explicit error checking. An error
	*** in this function call will not change the return value of the GetErrors() function.
	**/
	virtual bool OpenFile(const std::string& file_name) = 0;

	/** \brief Opens the file named by the classes' _filename member
	*** \return False on failure, true on success.
	**/
	virtual bool OpenFile() = 0;

	//! \brief Closes the script file and sets the _access_mode member to SCRIPT_CLOSED.
	virtual void CloseFile() = 0;
	//@}

	//! \brief Returns true if the file is open.
	bool IsFileOpen() const
		{ return (_access_mode != SCRIPT_CLOSED); }

	//! \brief Returns true if any errors have been detected but not retrieved
	bool IsErrorDetected() const
		{ return (_error_messages.str() != ""); }

	//! \name Class Member Access Functions
	//@{
	const std::string& GetFilename()
		{ return _filename; }

	//! \note This can also be used to determine what type of derived object this object points to (read, write, modify)
	const SCRIPT_ACCESS_MODE GetAccessMode()
		{ return _access_mode; }

	/** \brief Clears any error messages that have been logged
	*** \note Be careful when calling this function. It won't do any harm, but if used incorrectly it will deny you
	*** useful information when debugging your code or script.
	**/
	void ClearErrors()
		{ _error_messages.clear(); }

	/** \brief Returns a string containing all error messages
	*** \return A string containing a list of error messages, or an empty string if no errors occured
	*** \note Everytime this function is called, the error messages stored by the class are cleared.
	***
	*** You can use this method in place of IsErrorDetected() if you like by simply checking if the
	*** return value is an empty string. It is highly recommended that you check for errors after
	*** making a block of read/write/modify calls. You may choose to check errors after every call
	*** if you like, but this practice is usually not recommended.
	**/
	std::string GetErrorMessages()
		{ std::string errors = _error_messages.str(); ClearErrors(); return errors; }

	/** \note This returns a copy, not a reference, because we don't want it to be modified
	*** outside of the class.
	**/
	std::vector<std::string> GetOpenTables()
		{ return _open_tables; }
	//@}

protected:
	//! \brief The name of the file that the class object represents.
	std::string _filename;

	//! \brief The access mode for the file, including whether or not the file is closed
	SCRIPT_ACCESS_MODE _access_mode;

	//! \brief Contains a list of errors that have occured in recent operations
	std::ostringstream _error_messages;

	//! \brief The names of the Lua tables that are currently open.
	std::vector<std::string> _open_tables;
}; // class ScriptDescriptor


/** ****************************************************************************
*** \brief Singleton class that manages all open script files.
***
*** This class monitors all open script files and their descriptor objects. It
*** maintains a global Lua state that all open Lua files use to communicate
*** with each other and with the C++ engine.
***
*** \note This class is a singleton
*** ***************************************************************************/
class ScriptEngine : public hoa_utils::Singleton<ScriptEngine> {
	friend class hoa_utils::Singleton<ScriptEngine>;
	friend class ScriptDescriptor;
	friend class ReadScriptDescriptor;
	friend class WriteScriptDescriptor;
	friend class ModifyScriptDescriptor;
public:
	~ScriptEngine();

	bool SingletonInitialize ();

	//! \brief Returns a pointer to the global lua state
	lua_State *GetGlobalState()
		{ return _global_state; }

	/** \brief Checks if a file is already in use by a ScriptDescriptor object.
	*** \param filename The name of the file to check.
	*** \return True if the filename is registered to a ScriptDescriptor object who has the file opened.
	**/
	bool IsFileOpen(const std::string& filename);

	/** \brief Handles run-time errors generated in Lua
	*** \param err A reference to the luabind::error instance that was thrown
	***
	*** What this method does is retrieve the Lua error message that has been placed on top of the Lua stack,
	*** prints the error message to stderr, and removes it from the stack.
	***
	*** \note If this method is called after the luabind::error is caught, there is a chance that the lua_State
	*** where the error was generated has become invalid. If this is the case, this method will generate a segmentation
	*** fault.
	**/
	void HandleLuaError(luabind::error& err);

	/** \brief Handles casting errors generated by Lua return values
	*** \param err A reference to the luabind::cast_failed instance that was thrown
	***
	*** This method handles errors specific to casting failures. These errors occur when the return value of a
	*** Lua function that is called can not be converted into a proper C++ type.
	**/
	void HandleCastError(luabind::cast_failed& err);

private:
	ScriptEngine();

	//! \brief Maintains a list of all script files that are currently open
	std::map<std::string, ScriptDescriptor*> _open_files;

	// TODO: not re-opening Lua files introduces serious problems (invalid data being read) and conflicts when
	// two files share the same variable name are opened in succession. I believe this "feature" should be
	// removed, but I'm leaving it here for now until we understand the situation completely.
	/** \brief Maintains a cache of opened lua threads
	*** This is done so that a file that has already been loaded into the lua state will not be loaded again.
	*** Instead the lua_thread will be returned.
	**/
	std::map<std::string, lua_State*> _open_threads;

	//! \brief The lua state shared globally by all files
	lua_State* _global_state;

	//! \brief Adds an open file to the list of open files
	void _AddOpenFile(ScriptDescriptor* sd);

	//! \brief Removes an open file from the list of open files
	void _RemoveOpenFile(ScriptDescriptor* sd);

	// TODO: related to the to-do note above. I don't think this function should be used. For now it
	// always returns NULL.
	/** \brief Checks for the existence of a previously opened lua state from that filename.
	*** This should class because the filename contains the full path
	***
	*** \return A pointer to the lua_State for the file, or NULL if the file has never been opened.
	**/
	lua_State* _CheckForPreviousLuaState(const std::string &filename);
}; // class ScriptEngine : public hoa_utils::Singleton<ScriptEngine>

} // namespace hoa_script

// These #includes are made here rather than the top of the file to avoid a recurisve inclusion problem.
// Each of these files contains a class that dervies from the ScriptDescriptor class that is defined in
// this file. Thus these headers must all include this header file, "script.h". However we would like to
// make the entire script engine API available to the user through one header inclusion (script.h), so
// this file must likewise include these files. When these are placed at the top of this file, the
// ScriptDescriptor definition is not fully parsed by the time control reaches the other script headers,
// hence why they are included here so that the ScriptDescriptor definition is available for the
// other script headers to use.
#include "script_read.h"
#include "script_write.h"
#include "script_modify.h"

#endif // __SCRIPT_HEADER__
