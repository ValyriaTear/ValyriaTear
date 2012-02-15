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
#include "script.h"
#include "global.h"

#if defined(main) && !defined(_WIN32)
	#undef main
#endif

using namespace std;

using namespace hoa_global;
using namespace hoa_script;

using namespace hoa_editor;



int main(int argc, char **argv) {
#ifndef _WIN32
#ifndef __MACH__
	// Look for data files in DATADIR only if they are not available in the
	// current directory.
	if (ifstream("./dat/config/settings.lua") == NULL) {
		if (chdir(DATADIR) != 0) {
			PRINT_ERROR << "failed to change directory to data location" << endl;
		}
	}
#endif
#endif

#ifdef __MACH__
	string path;
	path = argv[0];
	// remove the binary name
	path.erase(path.find_last_of('/'));
	// remove the MacOS directory
	path.erase(path.find_last_of('/'));
	// remove the Contents directory
	path.erase(path.find_last_of('/'));
	// remove the Editor.app directory
	path.erase(path.find_last_of('/'));
	// we are now in a common directory containing both Allacrost and the Editor
	path.append("/Allacrost.app/Contents/Resources/");
	chdir(path.c_str());
#endif

	QApplication app(argc, argv);
	ScriptManager = ScriptEngine::SingletonCreate();
	ScriptManager->SingletonInitialize();
	GlobalManager = GameGlobal::SingletonCreate();
	GlobalManager->SingletonInitialize();

	hoa_defs::BindCommonCode();

	Editor* editor = new Editor();
	editor->setCaption("Hero of Allacrost Level Editor");
	app.setMainWidget(editor);
	editor->show();

	return app.exec();
}
