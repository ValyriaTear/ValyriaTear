# Set the resource app icon
# -- Windows
RC_FILE = ./icon.rc
# -- Mac
ICON = ./img/logos/mac_logo.icns

# Needed for the QGLWidget
QT += opengl core gui

# normal include path
INCLUDEPATH += ./src
# Dependencies include paths
# Don't forget to copy the unzipped dependencies folder there: 'valyriatear-win32-depends'
INCLUDEPATH += ./valyriatear-win32-depends/include
INCLUDEPATH += ./valyriatear-win32-depends/include/lua5.1
INCLUDEPATH += ./valyriatear-win32-depends/include/AL
INCLUDEPATH += ./valyriatear-win32-depends/include/SDL

# Luabind
INCLUDEPATH += ./src/luabind
INCLUDEPATH += ./src/luabind/src
INCLUDEPATH += ./src/luabind/luabind
INCLUDEPATH += ./src/luabind/luabind/detail

# Linker additional libs
# You'll have to adapt this line below to make find the lib folder as it won't work as is.
LIBS += -L"./valyriatear-win32-depends/lib"

LIBS += -lLua51 -llibiconv2 -lSDL -lSDL_image -lSDL_TTF -llibpng -llibjpeg -llibintl -lOpenAL32 -logg -lvorbis

# Enable rtti needed for typeid() in Luabind
# Enable exceptions handling for script reading support.
# This below doesn't work - Edit qmake.conf in the mkspecs profile for win32-g++ to enable rtti and exceptions
# by default, and remove the -DQT_NO_DYNAMIC_CAST and -DUNICODE flags from the Makefile.Release file
# after qmake invocation and before compiling.
DEFINES -= -DQT_NO_DYNAMIC_CAST -DUNICODE
QMAKE_CXXFLAGS -= -fno-exceptions -fno-rtti
QMAKE_CXXFLAGS += -fexceptions -frtti

HEADERS += \
    ./src/editor/tileset.h \
    ./src/editor/grid.h \
    ./src/editor/editor.h \
    ./src/editor/dialog_boxes.h \
    ./src/engine/script/script_write.h \
    ./src/engine/script/script_read.h \
    ./src/engine/script/script.h \
    ./src/engine/video/color.h \
    ./src/editor/tileset_editor.h \
    ./src/engine/video/image.h \
    ./src/engine/video/image_base.h \
    ./src/engine/video/video.h \
    ./src/engine/video/texture_controller.h \
    ./src/engine/video/texture.h \
    ./src/engine/video/interpolator.h \
    ./src/engine/video/fade.h \
    ./src/engine/system.h \
    ./src/engine/mode_manager.h \
    ./src/engine/video/shake.h \
    ./src/engine/video/text.h \
    ./src/modes/mode_help_window.h \
    ./src/engine/video/particle_system.h \
    ./src/engine/video/particle_manager.h \
    ./src/engine/video/particle_keyframe.h \
    ./src/engine/video/particle_emitter.h \
    ./src/engine/video/particle_effect.h \
    ./src/engine/video/particle.h \
    ./src/engine/script_supervisor.h \
    ./src/engine/audio/audio.h \
    ./src/common/gui/textbox.h \
    ./src/common/gui/option.h \
    ./src/common/gui/menu_window.h \
    ./src/common/gui/gui.h \
    ./src/engine/effect_supervisor.h \
    ./src/engine/input.h \
    ./src/engine/audio/audio_descriptor.h \
    ./src/engine/audio/audio_stream.h \
    ./src/engine/audio/audio_input.h \
    ./src/engine/audio/audio_effects.h

SOURCES += \
    ./src/editor/tileset.cpp \
    ./src/editor/grid.cpp \
    ./src/editor/editor_main.cpp \
    ./src/editor/editor.cpp \
    ./src/editor/dialog_boxes.cpp \
    ./src/engine/script/script_write.cpp \
    ./src/engine/script/script_read.cpp \
    ./src/engine/script/script.cpp \
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
    ./src/utils.cpp \
    ./src/editor/tileset_editor.cpp \
    ./src/engine/video/image.cpp \
    ./src/engine/video/image_base.cpp \
    ./src/engine/video/video.cpp \
    ./src/engine/video/texture_controller.cpp \
    ./src/engine/video/texture.cpp \
    ./src/engine/video/interpolator.cpp \
    ./src/engine/video/fade.cpp \
    ./src/engine/system.cpp \
    ./src/engine/mode_manager.cpp \
    ./src/engine/video/text.cpp \
    ./src/modes/mode_help_window.cpp \
    ./src/engine/video/particle_system.cpp \
    ./src/engine/video/particle_manager.cpp \
    ./src/engine/video/particle_effect.cpp \
    ./src/engine/script_supervisor.cpp \
    ./src/engine/audio/audio.cpp \
    ./src/common/gui/textbox.cpp \
    ./src/common/gui/option.cpp \
    ./src/common/gui/menu_window.cpp \
    ./src/common/gui/gui.cpp \
    ./src/engine/effect_supervisor.cpp \
    ./src/engine/input.cpp \
    ./src/engine/audio/audio_descriptor.cpp \
    ./src/engine/audio/audio_stream.cpp \
    ./src/engine/audio/audio_input.cpp \
    ./src/engine/audio/audio_effects.cpp
