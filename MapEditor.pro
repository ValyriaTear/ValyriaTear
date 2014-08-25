# Set the resource app icon
# -- Windows
RC_FILE = ./icon.rc
# -- Mac
ICON = ./Xcode/mac_logo.icns

# Needed for the QGLWidget
QT += opengl core gui

# normal include path
INCLUDEPATH += ./src
# Dependencies include paths
# Don't forget to copy the unzipped dependencies folder there: 'valyriatear-win32-depends'
INCLUDEPATH += ./valyriatear-win32-depends/include
INCLUDEPATH += ./valyriatear-win32-depends/include/lua5.1

# Luabind
INCLUDEPATH += ./src/luabind
INCLUDEPATH += ./src/luabind/src
INCLUDEPATH += ./src/luabind/luabind
INCLUDEPATH += ./src/luabind/luabind/detail

# Linker additional libs
# You'll have to adapt this line below to make find the lib folder as it won't work as is.
LIBS += -L"../ValyriaTear/valyriatear-win32-depends/lib"

LIBS += -lLua51

# Enable rtti needed for typeid() in Luabind
# Enable exceptions handling for script reading support.
# This below doesn't work - Edit qmake.conf in the mkspecs profile for win32-g++ to enable rtti and exceptions
# by default, and remove the -DQT_NO_DYNAMIC_CAST and -DUNICODE flags from the Makefile.Release file
# after qmake invocation and before compiling.
# Please also note that depending on the Qt version you're using, you'll have to use a specific mingw version.
# E.g.: Mingw 4.4 for Qt 4.8.x
DEFINES -= QT_NO_DYNAMIC_CAST
DEFINES -= UNICODE
DEFINES += EDITOR_BUILD QT_DYNAMIC_CAST NO_UNICODE

QMAKE_CXXFLAGS -= -fno-exceptions -fno-rtti
QMAKE_CXXFLAGS += -fexceptions -frtti

PRECOMPILED_HEADER = ./src/utils/utils_pch.h

HEADERS += \
    ./src/editor/dialog_boxes.h \
    ./src/editor/editor.h \
    ./src/editor/grid.h \
    ./src/editor/tileset.h \
    ./src/editor/tileset_editor.h \
    ./src/engine/script/script.h \
    ./src/engine/script/script_read.h \
    ./src/engine/script/script_write.h \
    ./src/utils/utils_files.h \
    ./src/utils/utils_numeric.h \
    ./src/utils/utils_random.h \
    ./src/utils/utils_strings.h \

SOURCES += \
    ./src/editor/dialog_boxes.cpp \
    ./src/editor/editor.cpp \
    ./src/editor/editor_main.cpp \
    ./src/editor/grid.cpp \
    ./src/editor/tileset.cpp \
    ./src/editor/tileset_editor.cpp \
    ./src/engine/script/script.cpp \
    ./src/engine/script/script_read.cpp \
    ./src/engine/script/script_write.cpp \
    ./src/utils/utils_files.cpp \
    ./src/utils/utils_numeric.cpp \
    ./src/utils/utils_random.cpp \
    ./src/utils/utils_strings.cpp \
    ./src/luabind/src/wrapper_base.cpp \
    ./src/luabind/src/weak_ref.cpp \
    ./src/luabind/src/stack_content_by_name.cpp \
    ./src/luabind/src/scope.cpp \
    ./src/luabind/src/pcall.cpp \
    ./src/luabind/src/open.cpp \
    ./src/luabind/src/object_rep.cpp \
    ./src/luabind/src/link_compatibility.cpp \
    ./src/luabind/src/inheritance.cpp \
    ./src/luabind/src/function.cpp \
    ./src/luabind/src/exception_handler.cpp \
    ./src/luabind/src/error.cpp \
    ./src/luabind/src/create_class.cpp \
    ./src/luabind/src/class_rep.cpp \
    ./src/luabind/src/class_registry.cpp \
    ./src/luabind/src/class_info.cpp \
    ./src/luabind/src/class.cpp \
