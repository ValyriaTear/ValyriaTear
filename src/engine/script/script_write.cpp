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
*** \file    script_write.cpp
*** \author  Daniel Steuernol - steu@allacrost.org,
*** \author  Tyler Olsen - roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for the WriteScriptDescriptor class.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "script_write.h"

#include "script.h"

using namespace vt_utils;

namespace vt_script
{

WriteScriptDescriptor::~WriteScriptDescriptor()
{
    if(IsFileOpen()) {
        IF_PRINT_WARNING(SCRIPT_DEBUG)
                << "SCRIPT WARNING: WriteScriptDescriptor destructor was called when file was still open: "
                << _filename << std::endl;
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

bool WriteScriptDescriptor::OpenFile(const std::string &file_name)
{
    if(ScriptManager->IsFileOpen(file_name) == true) {
        IF_PRINT_WARNING(SCRIPT_DEBUG)
                << "SCRIPT WARNING: WriteScriptDescriptor::OpenFile() attempted to open file that is already opened: "
                << file_name << std::endl;
        return false;
    }

    // In debug mode, first check if the file exists before overwriting it
    if(SCRIPT_DEBUG) {
        std::ifstream temp_file;
        temp_file.open(file_name.c_str());
        if(temp_file) {
            PRINT_WARNING << "SCRIPT WARNING: In WriteScriptDescriptor::OpenFile(), the file to be opened "
                          << "already exists and will be overwritten: " << file_name << std::endl;
        }
        temp_file.close();
    }

    _outfile.open(file_name.c_str());
    if(!_outfile.is_open()) {
        PRINT_ERROR << "SCRIPT ERROR: WriteScriptDescriptor::OpenFile() failed to open the file "
                    << file_name << " for writing." << std::endl;
        _access_mode = SCRIPT_CLOSED;
        return false;
    }

    _filename = file_name;
    _access_mode = SCRIPT_WRITE;
    ScriptManager->_AddOpenFile(this);
    return true;
} // bool WriteScriptDescriptor::OpenFile(string file_name)



bool WriteScriptDescriptor::OpenFile()
{
    if(_filename == "") {
        IF_PRINT_WARNING(SCRIPT_DEBUG)
                << "SCRIPT ERROR: WriteScriptDescriptor::OpenFile() could not open file "
                << "because of an invalid file name (empty string)." << std::endl;
        return false;
    }

    return OpenFile(_filename);
}



void WriteScriptDescriptor::CloseFile()
{
    if(IsFileOpen() == false) {
        IF_PRINT_WARNING(SCRIPT_DEBUG)
                << "SCRIPT ERROR: in WriteScriptDescriptor::CloseFile(), could not close the "
                << "file because it was not open." << std::endl;
        return;
    }

    if(SCRIPT_DEBUG && IsErrorDetected()) {
        PRINT_WARNING << "SCRIPT WARNING: In WriteScriptDescriptor::CloseFile(), the file " << _filename
                      << " had error messages remaining. They are as follows:" << std::endl
                      << _error_messages.str() << std::endl;
    }

    _outfile.close();
    _error_messages.clear();
    _open_tables.clear();
    _access_mode = SCRIPT_CLOSED;
    ScriptManager->_RemoveOpenFile(this);
}



bool WriteScriptDescriptor::SaveFile()
{
    if(IsFileOpen() == false) {
        PRINT_ERROR << "could not save the file because it was not open" << std::endl;
        return false;
    }

    if(SCRIPT_DEBUG && IsErrorDetected()) {
        PRINT_WARNING << "SCRIPT WARNING: In WriteScriptDescriptor::CloseFile(), the file " << _filename
                      << " had error messages remaining. They are as follows:" << std::endl
                      << _error_messages.str() << std::endl;
    }

    _outfile.flush();
    return _outfile.bad();
}

//-----------------------------------------------------------------------------
// Comment Write Functions
//-----------------------------------------------------------------------------

void WriteScriptDescriptor::InsertNewLine()
{
    _outfile << std::endl;
}



void WriteScriptDescriptor::WriteComment(const std::string &comment)
{
    _outfile << "-- " << comment << std::endl;
}



void WriteScriptDescriptor::BeginCommentBlock()
{
    if(_inside_comment_block == true) {
        _error_messages << "* WriteScriptDescriptor::BeginCommentBlock() was already "
                        << "inside a comment block when it was called" << std::endl;
        return;
    }

    _inside_comment_block = true;
    _outfile << "--[[" << std::endl;
}



void WriteScriptDescriptor::EndCommentBlock()
{
    if(_inside_comment_block == false) {
        _error_messages << "* WriteScriptDescriptor::EndCommentBlock() was not "
                        << "inside a comment block when it was called" << std::endl;
        return;
    }

    _inside_comment_block = false;
    _outfile << "--]]" << std::endl;
}



void WriteScriptDescriptor::WriteLine(const std::string &comment, bool new_line)
{
    _outfile << comment;
    if(new_line)
        _outfile << std::endl;
}

//-----------------------------------------------------------------------------
// Variable Write Functions
//-----------------------------------------------------------------------------

// WriteBool can not use the _WriteData helper because true/false values must
// be explicitly written to the file
void WriteScriptDescriptor::WriteBool(const std::string &key, bool value)
{
    if(_open_tables.size() == 0) {
        _outfile << key << " = ";
        if(value)
            _outfile << "true" << std::endl;
        else
            _outfile << "false" << std::endl;
    } else {
        _WriteTablePath();
        _outfile << '.' << key << " = ";
        if(value)
            _outfile << "true" << std::endl;
        else
            _outfile << "false" << std::endl;
    }
}



void WriteScriptDescriptor::WriteBool(const int32 key, bool value)
{
    if(_open_tables.empty()) {
        _error_messages << "* WriteScriptDescriptor::WriteBool() failed because there were no "
                        << "tables open when attempting to write the key/value: " << key << " = " << value << std::endl;
        return;
    }

    _WriteTablePath();
    _outfile << '[' << key << ']' << " = ";
    if(value)
        _outfile << "true" << std::endl;
    else
        _outfile << "false" << std::endl;
}

// WriteString can not use the _WriteData helper because it needs to do additional
// checking and add quotation marks around its value.
// TODO: Check for bad strings (ie, if it contains puncutation charcters like , or ])
void WriteScriptDescriptor::WriteString(const std::string &key, const std::string &value)
{
    if(_open_tables.size() == 0) {
        _outfile << key << " = \"" << value << "\"" << std::endl;
    } else {
        _WriteTablePath();
        _outfile << '.' << key << " = \"" << value << "\"" << std::endl;
    }
}



void WriteScriptDescriptor::WriteString(const int32 key, const std::string &value)
{
    if(_open_tables.empty()) {
        _error_messages << "* WriteScriptDescriptor::WriteString() failed because there were no "
                        << "tables open when attempting to write the key/value: [" << key << "] = " << value << std::endl;
        return;
    }

    _WriteTablePath();
    _outfile << '[' << key << ']' << " = \"" << value << "\"" << std::endl;
}

// WriteUString can not use the _WriteData helper because it needs to do additional
// checking and add quotation marks around its value.
void WriteScriptDescriptor::WriteUString(const std::string &key, const std::string &value)
{
    WriteString(key, value);
}



void WriteScriptDescriptor::WriteUString(const int32 key, const std::string &value)
{
    WriteString(key, value);
}

//-----------------------------------------------------------------------------
// Vector Write Functions
//-----------------------------------------------------------------------------

// WriteBoolVector can not use the _WriteDataVector helper because true/false values must
// be explicitly written to the file
void WriteScriptDescriptor::WriteBoolVector(const std::string &key, std::vector<bool>& vect)
{
    if(vect.empty()) {
        _error_messages << "* WriteScriptDescriptor::WriteBoolVector() failed because "
                        << "the vector argument was empty for key name: " << key << std::endl;
        return;
    }

    if(_open_tables.size() == 0) {
        _outfile << key << " = { ";
    } else {
        _WriteTablePath();
        _outfile << '.' << key << " = { ";
    }

    if(vect[0])
        _outfile << "true";
    else
        _outfile << "false";
    for(uint32 i = 1; i < vect.size(); i++) {
        if(vect[i])
            _outfile << ", true";
        else
            _outfile << ", false";
    }
    _outfile << " }" << std::endl;
}



void WriteScriptDescriptor::WriteBoolVector(const int32 key, std::vector<bool>& vect)
{
    if(_open_tables.empty()) {
        _error_messages << "* WriteScriptDescriptor::WriteBoolVector() failed because there were no "
                        << "tables open when attempting for key name: " << key << std::endl;
        return;
    }

    _WriteTablePath();
    _outfile << '[' << key << "] = { ";

    if(vect[0])
        _outfile << "true";
    else
        _outfile << "false";
    for(uint32 i = 1; i < vect.size(); i++) {
        if(vect[i])
            _outfile << ", true";
        else
            _outfile << ", false";
    }
    _outfile << " }" << std::endl;
}

// WriteString can not use the _WriteData helper because it needs to do additional
// checking and add quotation marks around its value.
// TODO: Check for bad strings (ie, if it contains puncutation charcters like , or ])
void WriteScriptDescriptor::WriteStringVector(const std::string &key, std::vector<std::string>& vect)
{
    if(vect.empty()) {
        _error_messages << "* WriteScriptDescriptor::WriteStringVector() failed because there were no "
                        << "tables open when attempting for key name: " << key << std::endl;
        return;
    }

    if(_open_tables.size() == 0) {
        _outfile << key << " = { ";
    } else {
        _WriteTablePath();
        _outfile << '.' << key << " = { ";
    }

    _outfile << "\"" << vect[0] << "\"";
    for(uint32 i = 1; i < vect.size(); i++) {
        _outfile << ", \"" << vect[i] << "\"";
    }
    _outfile << " }" << std::endl;
}



void WriteScriptDescriptor::WriteStringVector(const int32 key, std::vector<std::string>& vect)
{
    if(vect.empty()) {
        _error_messages << "* WriteScriptDescriptor::WriteStringVector() failed because there were no "
                        << "tables open when attempting for key name: " << key << std::endl;
        return;
    }
    if(_open_tables.empty()) {
        _error_messages << "* WriteScriptDescriptor::WriteSringVector() failed because there were no "
                        << "tables open when attempting for key name: " << key << std::endl;
        return;
    }

    _WriteTablePath();
    _outfile << '[' << key << "] = { ";

    _outfile << "\"" << vect[0] << "\"";
    for(uint32 i = 1; i < vect.size(); i++) {
        _outfile << ", \"" << vect[i] << "\"";
    }
    _outfile << " }" << std::endl;
}



// WriteUString can not use the _WriteData helper because it needs to do additional
// checking and add quotation marks around its value.
void WriteScriptDescriptor::WriteUStringVector(const std::string &key, std::vector<std::string>& vect)
{
    WriteStringVector(key, vect);
}



void WriteScriptDescriptor::WriteUStringVector(const int32 key, std::vector<std::string>& vect)
{
    WriteStringVector(key, vect);
}

void WriteScriptDescriptor::WriteNamespace(const std::string &ns)
{
    _outfile << "local ns = {};" << std::endl;
    _outfile << "setmetatable(ns, {__index = _G});" << std::endl;
    _outfile << ns << " = ns;" << std::endl;
    _outfile << "setfenv(1, ns);" << std::endl;
}

//-----------------------------------------------------------------------------
// Table Write Functions
//-----------------------------------------------------------------------------

void WriteScriptDescriptor::BeginTable(const std::string &key)
{
    if(_open_tables.size() == 0) {
        _outfile << key << " = {}" << std::endl;
    } else {
        _WriteTablePath();
        _outfile << '.' << key << " = {}" << std::endl;
    }

    _open_tables.push_back(key);
}



void WriteScriptDescriptor::BeginTable(int32 key)
{
    if(_open_tables.size() == 0)
        _outfile << key << " = {}" << std::endl;
    else {
        _WriteTablePath();
        _outfile << '[' << key << "] = {}" << std::endl;
    }

    _open_tables.push_back(NumberToString<int32>(key));
}


// This doesn't actually do any file write operations, but the user still needs to call it.
void WriteScriptDescriptor::EndTable()
{
    if(_open_tables.empty()) {
        _error_messages << "* WriteScriptDescriptor::EndTable() failed because no tables were open" << std::endl;
    } else {
        _open_tables.pop_back();
    }
}

//-----------------------------------------------------------------------------
// Miscellaneous Functions
//-----------------------------------------------------------------------------

// Writes the path for all the open tables. For example, "table01[table02][table03]"
void WriteScriptDescriptor::_WriteTablePath()
{
    if(_open_tables.empty()) {
        _error_messages << "* WriteScriptDescriptor::_WriteTablePath() failed because there were no "
                        << "tables open" << std::endl;
        return;
    }

    _outfile << _open_tables[0];
    for(uint32 i = 1; i < _open_tables.size(); i++) {
        if(IsStringNumeric(_open_tables[i]))
            _outfile << '[' << _open_tables[i] << ']';
        else
            _outfile << '.' << _open_tables[i];
    }
}

} // namespace vt_script
