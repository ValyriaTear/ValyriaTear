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
*** \file    script_write.h
*** \author  Daniel Steuernol - steu@allacrost.org,
*** \author  Tyler Olsen - roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for the WriteScriptDescriptor class.
*** ***************************************************************************/

#ifndef __SCRIPT_WRITE_HEADER__
#define __SCRIPT_WRITE_HEADER__

#include "script.h"

#include "utils/utils_strings.h"

namespace vt_script
{

/** ****************************************************************************
*** \brief Provides a simplistic interface for writing data to a new Lua file
***
*** This class does not actually involve Lua in any way. Rather, it provides an
*** interface with which to write a Lua file from scratch. One area of the code
*** base which uses this class graciously is the editor when it saves
*** new map files. Be cautious when using this class, as it can overwrite
*** existing Lua files. This class is primarily for writing data, not
*** code, although it is indeed possible to write script code using this class.
***
*** \note With the exception of the open/close/save file functions, all methods of
*** this class assume that the file is open and do not check otherwise. If you
*** try to invoke these functions with an unopened file, you will generate a
*** segmentation fault.
***
*** \note Except where noted in the function's comments, <b>all</b> methods
*** automatically insert a new line into the file after they write their
*** requested contents.
***
*** \todo Implement the WriteUString methods that will automatically insert the
*** gettext call to retrieve the translated value of the string.
***
*** \todo Allow the user to automatically compile the newly written file and
*** possibly also delete the plain text version when this is done.
*** ***************************************************************************/
class WriteScriptDescriptor : public ScriptDescriptor
{
    friend class ScriptEngine;
public:
    WriteScriptDescriptor() :
        _inside_comment_block(false) {}

    ~WriteScriptDescriptor();

    /** \name File Access Functions
    *** \note Most of these are derived from ScriptDescriptor, refer to the comments for these
    *** methods in the header file for that class.
    **/
    //@{
    bool OpenFile(const std::string &file_name);
    bool OpenFile();
    void CloseFile();

    /** \brief Saves the file with the newly written contents
    *** \return True if the save operation was successful
    ***
    *** All this method actually does is flush the output file stream. This method does not need
    *** to be called normally since simply calling CloseFile() will achieve the same effect. The purpose
    *** of this method is to provide the option to save the file contents without also closing the file.
    **/
    bool SaveFile();
    //@}

    /** \name Comment Write Functions
    *** \brief Writes comments into a Lua file
    *** \param comment The comment to write to the file.
    **/
    //@{
    //! \brief Inserts a blank line into the text.
    void InsertNewLine();

    /** \brief Writes a string of text and prepends it with a comment.
    *** This is equivalent to the `// comment` in C++.
    **/
    void WriteComment(const std::string &comment);

    //! \brief After this function is invoked, all other data written will be a comment
    void BeginCommentBlock();

    //! \brief Ends a comment block.
    void EndCommentBlock();

    /** \brief Writes an unaltered string to the file
    *** \param comment The string of text to write to the file
    *** \param new_line If true, automatically appends a new line (enabled by default)
    *** \note Typically, you should <b>really</b> try to avoid using this function unless you know
    *** what you are doing. It is, however, safe to use it between the beginning and end of a
    *** comment block (just don't write the syntax that ends the block, which is `--]]`).
    **/
    void WriteLine(const std::string &comment, bool new_line = true);
    //@}

    /** \name Variable Write Functions
    *** \brief These functions will write a single variable and its value to a Lua file.
    *** \param *key The name of the Lua variable to write.
    *** \param value The value of the new global variable to write.
    *** \note If no write tables are open when these calls are made, the variables become global
    *** in the Lua file. Otherwise, they become keys of the most recently opened table.
    **/
    //@{
    void WriteBool(const std::string &key, bool value);
    void WriteBool(const int32 key, bool value);

    void WriteInt(const std::string &key, int32 value) {
        _WriteData(key, value);
    }

    void WriteInt(const int32 key, int32 value) {
        _WriteData(key, value);
    }

    void WriteUInt(const std::string &key, uint32 value) {
        _WriteData(key, value);
    }

    void WriteUInt(const int32 key, uint32 value) {
        _WriteData(key, value);
    }

    void WriteFloat(const std::string &key, float value) {
        _WriteData(key, value);
    }

    void WriteFloat(const int32 key, float value) {
        _WriteData(key, value);
    }

    void WriteString(const std::string &key, const std::string &value);
    void WriteString(const int32 key, const std::string &value);

    /** The WriteUString functions actually take standard strings, not ustrings, for their value.
    *** What these methods do is write the string to the Lua file with the gettext translation
    *** code attached so that when the value is retrieved, the appropriate string translation
    *** is returned from gettext instead of the actual base string contained in the file.
    **/
    void WriteUString(const std::string &key, const std::string &value);
    void WriteUString(const int32 key, const std::string &value);
    //@}

    /** \name Vector Write Functions
    *** \brief These functions write a vector of data to a Lua file.
    *** \param key The name of the table to use in the Lua file to represent the data.
    *** \param &vect A reference to the vector of elements to write.
    **/
    //@{
    void WriteBoolVector(const std::string &key, std::vector<bool> &vect);
    void WriteBoolVector(const int32 key, std::vector<bool> &vect);

    void WriteIntVector(const std::string &key, std::vector<int32> &vect) {
        _WriteDataVector(key, vect);
    }

    void WriteIntVector(const int32 key, std::vector<int32> &vect) {
        _WriteDataVector(key, vect);
    }

    void WriteUIntVector(const std::string &key, std::vector<uint32> &vect) {
        _WriteDataVector(key, vect);
    }

    void WriteUIntVector(const int32 key, std::vector<uint32> &vect) {
        _WriteDataVector(key, vect);
    }

    void WriteFloatVector(const std::string &key, std::vector<float> &vect) {
        _WriteDataVector(key, vect);
    }

    void WriteFloatVector(const int32 key, std::vector<float> &vect) {
        _WriteDataVector(key, vect);
    }

    void WriteStringVector(const std::string &key, std::vector<std::string> &vect);
    void WriteStringVector(const int32 key, std::vector<std::string> &vect);
    void WriteUStringVector(const std::string &key, std::vector<std::string> &vect);
    void WriteUStringVector(const int32 key, std::vector<std::string> &vect);
    //@}

    /** \brief Write out the namespace header
    *** \param the name of the namespace for this lua file.
    **/
    void WriteNamespace(const std::string &ns);

    /** \name Table Write Functions
    *** \brief These functions write the beginning and ends of Lua tables
    *** \param key The name of the table to write.
    *** \note If you begin a new table and then begin another when you haven't ended the first one, the
    *** new table will become a key to the first. A table will only become global when there are no other
    *** write tables open.
    **/
    //@{
    void BeginTable(const std::string &key);
    void BeginTable(int32 key);
    void EndTable();
    //@}

private:
    //! \brief The output file stream to write to for when the file is opened in write mode.
    std::ofstream _outfile;

    //! \brief Set to true after a comment block begins and false after the comment block ends
    bool _inside_comment_block;

    /** \name Variable Write Templates
    *** \brief These template functions are called by the public WriteTYPE functions of this class.
    *** \param key The name or numeric identifier of the Lua variable to write.
    *** \param value The value to write for the new variable
    *** \note The integer keys are only valid for variables stored in a table, not for global variables.
    **/
    //@{
    template <class T> void _WriteData(const std::string &key, T value);
    template <class T> void _WriteData(const int32 key, T value);
    //@}

    /** \name Vector Read Templates
    *** \brief These template functions are called by the public ReadTYPEVector functions of this class.
    *** \param key The name or numeric identifier of the Lua variable to access.
    *** \param vect A reference to the vector where the read variables should be store
    *** \note Integer keys are only valid for variables stored in a table, not for global variables.
    **/
    //@{
    template <class T> void _WriteDataVector(const std::string &key, std::vector<T> &vect);
    template <class T> void _WriteDataVector(const int32 key, std::vector<T> &vect);
    //@}

    //! \brief Writes the pathname of all open tables (i.e., table1[table2][table3])
    void _WriteTablePath();
}; // class WriteScriptDescriptor : public ScriptDescriptor

//-----------------------------------------------------------------------------
// Template Function Definitions
//-----------------------------------------------------------------------------

template <class T> void WriteScriptDescriptor::_WriteData(const std::string &key, T value)
{
    if(_open_tables.size() == 0) {
        _outfile << key << " = " << value << std::endl;
    } else {
        _WriteTablePath();
        _outfile << '.' << key << " = " << value << std::endl;
    }
}


template <class T> void WriteScriptDescriptor::_WriteData(const int32 key, T value)
{
    if(_open_tables.empty()) {
        _error_messages << "* WriteScriptDescriptor::_WriteData() failed because there were no "
                        << "tables open when attempting to write the key/value: [" << key << "] = " << value << std::endl;
        return;
    }

    _WriteTablePath();
    _outfile << '[' << key << ']' << " = " << value << std::endl;
}



template <class T> void WriteScriptDescriptor::_WriteDataVector(const std::string &key, std::vector<T> &vect)
{
    if(vect.empty()) {
        _error_messages << "* WriteScriptDescriptor::_WriteDataVector() failed because "
                        << "the vector argument was empty for key name: " << key << std::endl;
        return;
    }

    if(_open_tables.size() == 0) {
        _outfile << key << " = { ";
    } else {
        _WriteTablePath();
        if(vt_utils::IsStringNumeric(key))
            _outfile << "[" << key << "] = { ";
        else
            _outfile << '.' << key << " = { ";
    }

    _outfile << vect[0];
    for(uint32 i = 1; i < vect.size(); i++) {
        _outfile << ", " << vect[i];
    }
    _outfile << " }" << std::endl;
}



template <class T> void WriteScriptDescriptor::_WriteDataVector(const int32 key, std::vector<T>& vect)
{
    if(_open_tables.empty()) {
        _error_messages << "* WriteScriptDescriptor::_WriteDataVector() failed because there were no "
                        << "tables open when attempting for key name: " << key << std::endl;
        return;
    }

    _WriteTablePath();
    _outfile << '[' << key << "] = { ";

    _outfile << vect[0];
    for(uint32 i = 1; i < vect.size(); i++) {
        _outfile << ", " << vect[i];
    }
    _outfile << " }" << std::endl;
}

} // namespace vt_script

#endif // __SCRIPT_WRITE_HEADER_
