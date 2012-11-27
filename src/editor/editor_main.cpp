////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    editor_main.cpp
*** \author  Philip Vorsilak, gorzuate@allacrost.org
*** \brief   Source file for editor's main() function.
*** ***************************************************************************/

#ifdef __MACH__
#include <unistd.h>
#include <string>
#endif

#include "editor.h"
#include "engine/script/script.h"

#if defined(main) && !defined(_WIN32)
#undef main
#endif

using namespace hoa_script;
using namespace hoa_editor;

int main(int argc, char **argv)
{
#ifndef _WIN32
#ifndef __MACH__
    // Look for data files in DATADIR only if they are not available in the
    // current directory.
    if(std::ifstream("./dat/config/settings.lua") == NULL) {
        if(chdir(PKG_DATADIR) != 0) {
            PRINT_ERROR << "failed to change directory to data location" << std::endl;
        }
    }
#endif
#endif

#ifdef __MACH__
    std::string path;
    path = argv[0];
    // remove the binary name
    path.erase(path.find_last_of('/'));
    // remove the MacOS directory
    path.erase(path.find_last_of('/'));
    // remove the Contents directory
    path.erase(path.find_last_of('/'));
    // remove the Editor.app directory
    path.erase(path.find_last_of('/'));
    // we are now in a common directory containing both the game and the Editor
    path.append("/ValyriaTear.app/Contents/Resources/");
    chdir(path.c_str());
#endif

    QApplication app(argc, argv);
    ScriptManager = ScriptEngine::SingletonCreate();
    ScriptManager->SingletonInitialize();

    Editor *editor = new Editor();
    editor->setWindowTitle("Map Editor");
    //app.setMainWidget(editor); // Now useless according to the Qt 4.8.3 documentation.
    editor->show();

    return app.exec();
}
