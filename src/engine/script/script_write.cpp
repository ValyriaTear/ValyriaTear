///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    script_write.cpp
*** \author  Daniel Steuernol - steu@allacrost.org,
***          Tyler Olsen - roots@allacrost.org
*** \brief   Source file for the WriteScriptDescriptor class.
*** ***************************************************************************/

#include <fstream>

#include "utils.h"

#include "script.h"
#include "script_write.h"

using namespace std;

using namespace hoa_utils;

namespace hoa_script {

WriteScriptDescriptor::~WriteScriptDescriptor() {
	if (IsFileOpen()) {
		if (SCRIPT_DEBUG)
			cerr << "SCRIPT WARNING: WriteScriptDescriptor destructor was called when file was still open: "
				<< _filename << endl;
		CloseFile();
	}

	_filename = "";
	_access_mode = SCRIPT_CLOSED;
	_error_messages.clear();
	_open_tables.clear();
}

//-----------------------------------------------------------------------------
// File Access Functions
//-----------------------------------------------------------------------------

bool WriteScriptDescriptor::OpenFile(const string& file_name) {
	if (ScriptManager->IsFileOpen(file_name) == true) {
		if (SCRIPT_DEBUG)
			cerr << "SCRIPT WARNING: WriteScriptDescriptor::OpenFile() attempted to open file that is already opened: "
				<< file_name << endl;
		return false;
	}

	// In debug mode, first check if the file exists before overwriting it
	if (SCRIPT_DEBUG) {
		ifstream temp_file;
		temp_file.open(file_name.c_str());
		if (temp_file) {
			cerr << "SCRIPT WARNING: In WriteScriptDescriptor::OpenFile(), the file to be opened "
				<< "already exists and will be overwritten: " << file_name << endl;
		}
		temp_file.close();
	}

	_outfile.open(file_name.c_str());
	if (!_outfile) {
		cerr << "SCRIPT ERROR: WriteScriptDescriptor::OpenFile() failed to open the file "
			<< file_name << " for writing." << endl;
		_access_mode = SCRIPT_CLOSED;
		return false;
	}

	_filename = file_name;
	_access_mode = SCRIPT_WRITE;
	ScriptManager->_AddOpenFile(this);
	return true;
} // bool WriteScriptDescriptor::OpenFile(string file_name)



bool WriteScriptDescriptor::OpenFile() {
	if (_filename == "") {
		if (SCRIPT_DEBUG)
			cerr << "SCRIPT ERROR: WriteScriptDescriptor::OpenFile() could not open file "
				<< "because of an invalid file name (empty string)." << endl;
		return false;
	}

	return OpenFile(_filename);
}



void WriteScriptDescriptor::CloseFile() {
	if (IsFileOpen() == false) {
		if (SCRIPT_DEBUG)
			cerr << "SCRIPT ERROR: in WriteScriptDescriptor::CloseFile(), could not close the "
				<< "file because it was not open." << endl;
		return;
	}

	if (SCRIPT_DEBUG && IsErrorDetected()) {
		cerr << "SCRIPT WARNING: In WriteScriptDescriptor::CloseFile(), the file " << _filename
			<< " had error messages remaining. They are as follows:" << endl;
		cerr << _error_messages.str() << endl;
	}

	_outfile.close();
	_error_messages.clear();
	_open_tables.clear();
	_access_mode = SCRIPT_CLOSED;
	ScriptManager->_RemoveOpenFile(this);
}



bool WriteScriptDescriptor::SaveFile() {
	if (IsFileOpen() == false) {
		PRINT_ERROR << "could not save the file because it was not open" << endl;
		return false;
	}

	if (SCRIPT_DEBUG && IsErrorDetected()) {
		cerr << "SCRIPT WARNING: In WriteScriptDescriptor::CloseFile(), the file " << _filename
			<< " had error messages remaining. They are as follows:" << endl;
		cerr << _error_messages.str() << endl;
	}

	_outfile.flush();
	return _outfile.bad();
}

//-----------------------------------------------------------------------------
// Comment Write Functions
//-----------------------------------------------------------------------------

void WriteScriptDescriptor::InsertNewLine() {
	_outfile << endl;
}



void WriteScriptDescriptor::WriteComment(const string& comment) {
	_outfile << "-- " << comment << endl;
}



void WriteScriptDescriptor::BeginCommentBlock() {
	if (_inside_comment_block == true) {
		_error_messages << "* WriteScriptDescriptor::BeginCommentBlock() was already "
			<< "inside a comment block when it was called" << endl;
		return;
	}

	_inside_comment_block = true;
	_outfile << "--[[" << endl;
}



void WriteScriptDescriptor::EndCommentBlock() {
	if (_inside_comment_block == false) {
		_error_messages << "* WriteScriptDescriptor::EndCommentBlock() was not "
			<< "inside a comment block when it was called" << endl;
		return;
	}

	_inside_comment_block = false;
	_outfile << "--]]" << endl;
}



void WriteScriptDescriptor::WriteLine(const string& comment, bool new_line) {
	_outfile << comment;
	if (new_line)
		_outfile << endl;
}

//-----------------------------------------------------------------------------
// Variable Write Functions
//-----------------------------------------------------------------------------

// WriteBool can not use the _WriteData helper because true/false values must
// be explicitly written to the file
void WriteScriptDescriptor::WriteBool(const string& key, bool value) {
	if (_open_tables.size() == 0) {
		_outfile << key << " = ";
		if (value)
			_outfile << "true" << endl;
		else
			_outfile << "false" << endl;
	}
	else {
		_WriteTablePath();
		_outfile << '.' << key << " = ";
		if (value)
			_outfile << "true" << endl;
		else
			_outfile << "false" << endl;
	}
}



void WriteScriptDescriptor::WriteBool(const int32 key, bool value) {
	if (_open_tables.empty()) {
		_error_messages << "* WriteScriptDescriptor::WriteBool() failed because there were no "
			<< "tables open when attempting to write the key/value: " << key << " = " << value << std::endl;
		return;
	}

	_WriteTablePath();
	_outfile << '[' << key << ']' << " = ";
	if (value)
		_outfile << "true" << endl;
	else
		_outfile << "false" << endl;
}

// WriteString can not use the _WriteData helper because it needs to do additional
// checking and add quotation marks around its value.
// TODO: Check for bad strings (ie, if it contains puncutation charcters like , or ])
void WriteScriptDescriptor::WriteString(const string& key, const string& value) {
	if (_open_tables.size() == 0) {
		_outfile << key << " = \"" << value << "\"" << endl;
	}
	else {
		_WriteTablePath();
		_outfile << '.' << key << " = \"" << value << "\"" << endl;
	}
}



void WriteScriptDescriptor::WriteString(const int32 key, const string& value) {
	if (_open_tables.empty()) {
		_error_messages << "* WriteScriptDescriptor::WriteString() failed because there were no "
			<< "tables open when attempting to write the key/value: [" << key << "] = " << value << std::endl;
		return;
	}

	_WriteTablePath();
	_outfile << '[' << key << ']' << " = \"" << value << "\"" << endl;
}

// WriteUString can not use the _WriteData helper because it needs to do additional
// checking and add quotation marks around its value.
// TODO: Write strings with a call to the gettext library to retrieve translated strings
void WriteScriptDescriptor::WriteUString(const string& key, const string& value) {
	WriteString(key, value);
}



void WriteScriptDescriptor::WriteUString(const int32 key, const string& value) {
	WriteString(key, value);
}

//-----------------------------------------------------------------------------
// Vector Write Functions
//-----------------------------------------------------------------------------

// WriteBoolVector can not use the _WriteDataVector helper because true/false values must
// be explicitly written to the file
void WriteScriptDescriptor::WriteBoolVector(const string& key, std::vector<bool>& vect) {
	if (vect.empty()) {
		_error_messages << "* WriteScriptDescriptor::WriteBoolVector() failed because "
			<< "the vector argument was empty for key name: " << key << endl;
		return;
	}

	if (_open_tables.size() == 0) {
		_outfile << key << " = { ";
	}
	else {
		_WriteTablePath();
		_outfile << '.' << key << " = { ";
	}

	if (vect[0])
		_outfile << "true";
	else
		_outfile << "false";
	for (uint32 i = 1; i < vect.size(); i++) {
		if (vect[i])
			_outfile << ", true";
		else
			_outfile << ", false";
	}
	_outfile << " }" << endl;
}



void WriteScriptDescriptor::WriteBoolVector(const int32 key, std::vector<bool>& vect) {
	if (_open_tables.empty()) {
		_error_messages << "* WriteScriptDescriptor::WriteBoolVector() failed because there were no "
			<< "tables open when attempting for key name: " << key << endl;
		return;
	}

	_WriteTablePath();
	_outfile << '[' << key << "] = { ";

	if (vect[0])
		_outfile << "true";
	else
		_outfile << "false";
	for (uint32 i = 1; i < vect.size(); i++) {
		if (vect[i])
			_outfile << ", true";
		else
			_outfile << ", false";
	}
	_outfile << " }" << endl;
}

// WriteString can not use the _WriteData helper because it needs to do additional
// checking and add quotation marks around its value.
// TODO: Check for bad strings (ie, if it contains puncutation charcters like , or ])
void WriteScriptDescriptor::WriteStringVector(const string &key, std::vector<string>& vect) {
	if (vect.empty()) {
		_error_messages << "* WriteScriptDescriptor::WriteStringVector() failed because there were no "
			<< "tables open when attempting for key name: " << key << endl;
		return;
	}

	if (_open_tables.size() == 0) {
		_outfile << key << " = { ";
	}
	else {
		_WriteTablePath();
		_outfile << '.' << key << " = { ";
	}

	_outfile << "\"" << vect[0] << "\"";
	for (uint32 i = 1; i < vect.size(); i++) {
		_outfile << ", \"" << vect[i] << "\"";
	}
	_outfile << " }" << endl;
}



void WriteScriptDescriptor::WriteStringVector(const int32 key, std::vector<string>& vect) {
	if (vect.empty()) {
		_error_messages << "* WriteScriptDescriptor::WriteStringVector() failed because there were no "
			<< "tables open when attempting for key name: " << key << endl;
		return;
	}
	if (_open_tables.empty()) {
		_error_messages << "* WriteScriptDescriptor::WriteSringVector() failed because there were no "
			<< "tables open when attempting for key name: " << key << endl;
		return;
	}

	_WriteTablePath();
	_outfile << '[' << key << "] = { ";

	_outfile << "\"" << vect[0] << "\"";
	for (uint32 i = 1; i < vect.size(); i++) {
		_outfile << ", \"" << vect[i] << "\"";
	}
	_outfile << " }" << endl;
}



// WriteUString can not use the _WriteData helper because it needs to do additional
// checking and add quotation marks around its value.
// TODO: Write strings with a call to the gettext library to retrieve translated strings
void WriteScriptDescriptor::WriteUStringVector(const string& key, std::vector<string>& vect) {
	WriteStringVector(key, vect);
}



void WriteScriptDescriptor::WriteUStringVector(const int32 key, std::vector<string>& vect) {
	WriteStringVector(key, vect);
}

void WriteScriptDescriptor::WriteNamespace(const string &ns)
{
	_outfile << "local ns = {};" << endl;
	_outfile << "setmetatable(ns, {__index = _G});" << endl;
	_outfile << ns << " = ns;" << endl;
	_outfile << "setfenv(1, ns);" << endl;
}

//-----------------------------------------------------------------------------
// Table Write Functions
//-----------------------------------------------------------------------------

void WriteScriptDescriptor::BeginTable(const string &key) {
	if (_open_tables.size() == 0) {
		_outfile << key << " = {}" << endl;
	}
	else {
		_WriteTablePath();
		_outfile << '.' << key << " = {}" << endl;
	}

	_open_tables.push_back(key);
}



void WriteScriptDescriptor::BeginTable(int32 key) {
	if (_open_tables.size() == 0)
		_outfile << key << " = {}" << endl;
	else {
		_WriteTablePath();
		_outfile << '[' << key << "] = {}" << endl;
	}

	_open_tables.push_back(NumberToString<int32>(key));
}


// This doesn't actually do any file write operations, but the user still needs to call it.
void WriteScriptDescriptor::EndTable() {
	if (_open_tables.empty()) {
		_error_messages << "* WriteScriptDescriptor::EndTable() failed because no tables were open" << endl;
	}
	else {
		_open_tables.pop_back();
	}
}

//-----------------------------------------------------------------------------
// Miscellaneous Functions
//-----------------------------------------------------------------------------

// Writes the path for all the open tables. For example, "table01[table02][table03]"
void WriteScriptDescriptor::_WriteTablePath() {
	if (_open_tables.empty()) {
		_error_messages << "* WriteScriptDescriptor::_WriteTablePath() failed because there were no "
			<< "tables open" << endl;
		return;
	}

	_outfile << _open_tables[0];
	for (uint32 i = 1; i < _open_tables.size(); i++) {
		if (IsStringNumeric(_open_tables[i]))
			_outfile << '[' << _open_tables[i] << ']';
		else
			_outfile << '.' << _open_tables[i];
	}
}

} // namespace hoa_script
