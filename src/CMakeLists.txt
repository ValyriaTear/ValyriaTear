#Valyria Tear /src

FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(SDL_ttf REQUIRED)
FIND_PACKAGE(SDL_image REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(OpenAL REQUIRED)
FIND_PACKAGE(VorbisFile REQUIRED)
FIND_PACKAGE(Lua51 REQUIRED)
FIND_PACKAGE(PNG REQUIRED)
FIND_PACKAGE(Gettext REQUIRED)
FIND_PACKAGE(Boost 1.46.1 REQUIRED)

IF (UNIX AND USE_SYSTEM_LUABIND)
    # Find the luabind package
    FIND_PACKAGE(Luabind REQUIRED)
    MESSAGE(STATUS "Using system luabind")
ENDIF()

# Set the release mode if not told otherwise
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

IF (CMAKE_COMPILER_IS_GNUCXX)
    # Help getting compilation warnings
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    IF (WIN32)
        # This includes enough debug information to get something useful
        # from Dr. Mingw while keeping binary size down. Almost useless
        # with gdb, though.
        SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb0 -gstabs2")
    ENDIF()
ENDIF()

# in case a user overwrites these paths and specifies a relative one
FOREACH(dir PKG_DATADIR LOCALEDIR)
    IF(WIN32)
        # no change for win32
        SET(FULL_${dir} ${${dir}})
    ELSE(WIN32)
        IF(NOT IS_ABSOLUTE ${${dir}})
            SET(FULL_${dir} "${CMAKE_INSTALL_PREFIX}/${${dir}}")
        ELSE()
            SET(FULL_${dir} ${${dir}})
        ENDIF()
    ENDIF(WIN32)
ENDFOREACH()

SET(FLAGS "-DPACKAGE_VERSION=\\\"${VERSION}\\\"")
SET(FLAGS "${FLAGS} -DPKG_DATADIR=\\\"${FULL_PKG_DATADIR}/\\\"")
SET(FLAGS "${FLAGS} -DLOCALEDIR=\\\"${FULL_LOCALEDIR}/\\\"")
SET(FLAGS "${FLAGS} -DPKG_NAME=\\\"${PKG_NAME}/\\\"")

IF (DEBUG_FEATURES)
    SET(FLAGS "${FLAGS} -DDEBUG_FEATURES")
    MESSAGE(STATUS "Developer features enabled")
ENDIF()

IF (DISABLE_TRANSLATIONS)
    SET(FLAGS "${FLAGS} -DDISABLE_TRANSLATIONS")
    MESSAGE(STATUS "l10n support disabled")
ELSE()
    MESSAGE(STATUS "l10n support enabled")
ENDIF()

IF (CMAKE_BUILD_TYPE)
    STRING(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_TOLOWER)
    IF(CMAKE_BUILD_TYPE_TOLOWER MATCHES debug OR
       CMAKE_BUILD_TYPE_TOLOWER MATCHES relwithdebinfo)
        SET(FLAGS "${FLAGS} -DDEBUG")

        IF (CMAKE_COMPILER_IS_GNUCXX)
            # Help getting debug info
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pg")
        ENDIF()
    ENDIF()
ENDIF()

IF (NOT DISABLE_TRANSLATIONS)
    IF (WIN32)
        SET(EXTRA_LIBRARIES ws2_32 winmm)
        FIND_PACKAGE(LibIntl REQUIRED)
    ELSEIF (CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR BEOS OR CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
        FIND_PACKAGE(LibIntl REQUIRED)
    ELSEIF (CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
        FIND_PACKAGE(LibIntl REQUIRED)
    ELSEIF (CMAKE_SYSTEM_NAME STREQUAL SunOS)
        # explicit linking to libintl is required on Solaris
        SET(EXTRA_LIBRARIES intl)
    ENDIF()
ENDIF()

IF (CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
    FIND_PACKAGE(Ogg REQUIRED)
    FIND_PACKAGE(Vorbis REQUIRED)
ENDIF()

IF (USE_X11)
    FIND_PACKAGE(X11 REQUIRED)
    INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
    SET(FLAGS "${FLAGS} -DUSE_X11")
ENDIF (USE_X11)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SDL_INCLUDE_DIR}
    ${SDLTTF_INCLUDE_DIR}
    ${SDLIMAGE_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${OPENAL_INCLUDE_DIR}
    ${VORBISFILE_INCLUDE_DIR}
    ${PNG_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
    )

IF (NOT UNIX OR NOT USE_SYSTEM_LUABIND)
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/luabind")
    MESSAGE(STATUS "Using included luabind files")
ENDIF()

# enable rc-handling with mingw
# most likely this part can be kicked out with some later cmake version
IF (MINGW)
    FIND_PATH(MINGW_INCLUDE_DIR windows.h $ENV{INCLUDE})
    IF (MINGW_INCLUDE_DIR)
        MESSAGE(STATUS "Found mingw headers: ${MINGW_INCLUDE_DIR}")
        INCLUDE_DIRECTORIES(${MINGW_INCLUDE_DIR})
    ELSE()
        MESSAGE(FATAL_ERROR "Unable to find mingw headers. Required for windres")
    ENDIF()
    SET(CMAKE_RC_COMPILER_INIT windres)
    ENABLE_LANGUAGE(RC)
    SET(CMAKE_RC_COMPILE_OBJECT
      "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -O coff -o <OBJECT> <SOURCE>")
ENDIF()

# Fix some stuff that gets not hidden by mainline modules
MARK_AS_ADVANCED(SDLMAIN_LIBRARY)
MARK_AS_ADVANCED(SDL_INCLUDE_DIR)
MARK_AS_ADVANCED(SDL_LIBRARY)
MARK_AS_ADVANCED(SDLIMAGE_INCLUDE_DIR)
MARK_AS_ADVANCED(SDLIMAGE_LIBRARY)

SET(SRCS_LUABIND
luabind/src/class_registry.cpp
luabind/src/class_rep.cpp
luabind/src/object_rep.cpp
luabind/src/pcall.cpp
luabind/src/scope.cpp
luabind/src/stack_content_by_name.cpp
luabind/src/inheritance.cpp
luabind/src/class.cpp
luabind/src/error.cpp
luabind/src/weak_ref.cpp
luabind/src/class_info.cpp
luabind/src/create_class.cpp
luabind/src/function.cpp
luabind/src/link_compatibility.cpp
luabind/src/exception_handler.cpp
luabind/src/wrapper_base.cpp
luabind/src/open.cpp
luabind/luabind/prefix.hpp
luabind/luabind/operator.hpp
luabind/luabind/back_reference_fwd.hpp
luabind/luabind/config.hpp
luabind/luabind/out_value_policy.hpp
luabind/luabind/return_reference_to_policy.hpp
luabind/luabind/back_reference.hpp
luabind/luabind/function.hpp
luabind/luabind/object.hpp
luabind/luabind/scope.hpp
luabind/luabind/copy_policy.hpp
luabind/luabind/version.hpp
luabind/luabind/container_policy.hpp
luabind/luabind/class.hpp
luabind/luabind/lua_include.hpp
luabind/luabind/open.hpp
luabind/luabind/get_pointer.hpp
luabind/luabind/raw_policy.hpp
luabind/luabind/class_info.hpp
luabind/luabind/get_main_thread.hpp
luabind/luabind/luabind.hpp
luabind/luabind/from_stack.hpp
luabind/luabind/dependency_policy.hpp
luabind/luabind/typeid.hpp
luabind/luabind/adopt_policy.hpp
luabind/luabind/weak_ref.hpp
luabind/luabind/iterator_policy.hpp
luabind/luabind/shared_ptr_converter.hpp
luabind/luabind/discard_result_policy.hpp
luabind/luabind/exception_handler.hpp
luabind/luabind/value_wrapper.hpp
luabind/luabind/make_function.hpp
luabind/luabind/tag_function.hpp
luabind/luabind/nil.hpp
luabind/luabind/handle.hpp
luabind/luabind/detail/object_rep.hpp
luabind/luabind/detail/enum_maker.hpp
luabind/luabind/detail/call_function.hpp
luabind/luabind/detail/format_signature.hpp
luabind/luabind/detail/constructor.hpp
luabind/luabind/detail/object_call.hpp
luabind/luabind/detail/inheritance.hpp
luabind/luabind/detail/decorate_type.hpp
luabind/luabind/detail/primitives.hpp
luabind/luabind/detail/convert_to_lua.hpp
luabind/luabind/detail/most_derived.hpp
luabind/luabind/detail/stack_utils.hpp
luabind/luabind/detail/open.hpp
luabind/luabind/detail/property.hpp
luabind/luabind/detail/other.hpp
luabind/luabind/detail/object_funs.hpp
luabind/luabind/detail/is_indirect_const.hpp
luabind/luabind/detail/link_compatibility.hpp
luabind/luabind/detail/typetraits.hpp
luabind/luabind/detail/has_get_pointer.hpp
luabind/luabind/detail/conversion_storage.hpp
luabind/luabind/detail/ref.hpp
luabind/luabind/detail/debug.hpp
luabind/luabind/detail/garbage_collector.hpp
luabind/luabind/detail/instance_holder.hpp
luabind/luabind/detail/class_rep.hpp
luabind/luabind/detail/signature_match.hpp
luabind/luabind/detail/class_cache.hpp
luabind/luabind/detail/call_member.hpp
luabind/luabind/detail/pointee_sizeof.hpp
luabind/luabind/detail/policy.hpp
luabind/luabind/detail/deduce_signature.hpp
luabind/luabind/detail/class_registry.hpp
luabind/luabind/detail/pointee_typeid.hpp
luabind/luabind/detail/yes_no.hpp
luabind/luabind/detail/operator_id.hpp
luabind/luabind/detail/calc_arity.hpp
luabind/luabind/detail/make_instance.hpp
luabind/luabind/detail/pcall.hpp
luabind/luabind/detail/compute_score.hpp
luabind/luabind/detail/call.hpp
luabind/luabind/detail/call_operator_iterate.hpp
luabind/luabind/yield_policy.hpp
luabind/luabind/error.hpp
luabind/luabind/wrapper_base.hpp
)

# Files used by both the game and the editor
SET(SRCS_COMMON
engine/script/script.h
engine/script/script.cpp
engine/script/script_read.h
engine/script/script_read.cpp
engine/script/script_write.h
engine/script/script_write.cpp
utils/utils_pch.h
utils/utils_pch.cpp
utils/singleton.h
utils/utils_random.h
utils/utils_random.cpp
utils/utils_files.h
utils/utils_files.cpp
utils/utils_numeric.h
utils/utils_numeric.cpp
utils/utils_strings.h
utils/utils_strings.cpp
)

SET(SRCS
common/global/global.cpp
common/global/global.h
common/global/global_actors.cpp
common/global/global_actors.h
common/global/global_effects.cpp
common/global/global_effects.h
common/global/global_objects.cpp
common/global/global_objects.h
common/global/global_skills.cpp
common/global/global_skills.h
common/global/global_utils.cpp
common/global/global_utils.h
common/gui/option.h
common/gui/option.cpp
common/gui/menu_window.cpp
common/gui/menu_window.h
common/gui/textbox.cpp
common/gui/textbox.h
common/gui/gui.cpp
common/gui/gui.h
common/message_window.cpp
common/message_window.h
common/dialogue.cpp
common/dialogue.h
common/common.cpp
common/common.h
common/options_handler.cpp
common/options_handler.h
common/common_bindings.cpp
engine/audio/audio.h
engine/audio/audio.cpp
engine/audio/audio_descriptor.h
engine/audio/audio_descriptor.cpp
engine/audio/audio_input.cpp
engine/audio/audio_stream.h
engine/audio/audio_stream.cpp
engine/audio/audio_input.h
engine/audio/audio_effects.h
engine/audio/audio_effects.cpp
engine/effect_supervisor.h
engine/effect_supervisor.cpp
engine/mode_manager.h
engine/mode_manager.cpp
engine/script_supervisor.h
engine/script_supervisor.cpp
engine/indicator_supervisor.h
engine/indicator_supervisor.cpp
engine/system.cpp
engine/system.h
engine/input.h
engine/input.cpp
engine/engine_bindings.cpp
engine/video/color.h
engine/video/context.h
engine/video/coord_sys.h
engine/video/fade.cpp
engine/video/fade.h
engine/video/image.cpp
engine/video/image.h
engine/video/image_base.cpp
engine/video/image_base.h
engine/video/interpolator.cpp
engine/video/interpolator.h
engine/video/particle.h
engine/video/particle_effect.cpp
engine/video/particle_effect.h
engine/video/particle_emitter.h
engine/video/particle_keyframe.h
engine/video/particle_manager.cpp
engine/video/particle_manager.h
engine/video/particle_system.cpp
engine/video/particle_system.h
engine/video/screen_rect.h
engine/video/shake.h
engine/video/text.cpp
engine/video/text.h
engine/video/texture.cpp
engine/video/texture.h
engine/video/texture_controller.cpp
engine/video/texture_controller.h
engine/video/transform2d.cpp
engine/video/transform2d.h
engine/video/video.cpp
engine/video/video.h
modes/shop/shop_root.h
modes/shop/shop_buy.cpp
modes/shop/shop_sell.cpp
modes/shop/shop_trade.cpp
modes/shop/shop_utils.h
modes/shop/shop_root.cpp
modes/shop/shop_trade.h
modes/shop/shop.cpp
modes/shop/shop_buy.h
modes/shop/shop_utils.cpp
modes/shop/shop.h
modes/shop/shop_sell.h
modes/battle/battle_effects.cpp
modes/battle/battle_effects.h
modes/battle/battle_actors.cpp
modes/battle/battle_actors.h
modes/battle/battle_actions.cpp
modes/battle/battle_actions.h
modes/battle/battle_utils.cpp
modes/battle/battle_utils.h
modes/battle/battle_command.cpp
modes/battle/battle_command.h
modes/battle/battle.cpp
modes/battle/battle.h
modes/battle/battle_finish.cpp
modes/battle/battle_finish.h
modes/battle/battle_sequence.cpp
modes/battle/battle_sequence.h
modes/boot/boot.h
modes/boot/boot.cpp
modes/save/save_mode.h
modes/save/save_mode.cpp
modes/map/map_dialogue.h
modes/map/map_zones.h
modes/map/map_treasure.h
modes/map/map_events.h
modes/map/map_tiles.h
modes/map/map_utils.h
modes/map/map_mode.cpp
modes/map/map_dialogue.cpp
modes/map/map_mode.h
modes/map/map_utils.cpp
modes/map/map_objects.cpp
modes/map/map_events.cpp
modes/map/map_tiles.cpp
modes/map/map_sprites.cpp
modes/map/map_treasure.cpp
modes/map/map_sprites.h
modes/map/map_zones.cpp
modes/map/map_objects.h
modes/map/map_minimap.cpp
modes/map/map_minimap.h
modes/map/map_status_effects.cpp
modes/map/map_status_effects.h
modes/menu/menu.h
modes/menu/menu.cpp
modes/menu/menu_views.cpp
modes/menu/menu_views.h
modes/pause.cpp
modes/pause.h
modes/mode_bindings.cpp
modes/mode_help_window.h
modes/mode_help_window.cpp
utils/ustring.h
utils/ustring.cpp
utils/exception.h
utils/exception.cpp
main_options.h
main_options.cpp
main.cpp
    )

IF (WIN32)
    SET(SRCS
        ${SRCS}
       icon.rc
    )
ENDIF ()

SET (PROGRAMS valyriatear)

# Adds pch compilation support for gcc using cmake
IF (CMAKE_COMPILER_IS_GNUCXX AND USE_PCH_COMPILATION)
    MACRO (ADD_PCH_RULE  _header_filename _src_list)
        SET(_gch_filename "${_header_filename}.gch")
        LIST(APPEND ${_src_list} ${_gch_filename})
        SET (_args ${CMAKE_CXX_FLAGS} ${FLAGS})

        IF (CMAKE_BUILD_TYPE_TOLOWER MATCHES release)
            # Adds the release cxx compile flags
            LIST(APPEND _args ${CMAKE_CXX_FLAGS_RELEASE})
        ENDIF()

        LIST(APPEND _args -c ${_header_filename} -o ${_gch_filename})
        GET_DIRECTORY_PROPERTY(DIRINC INCLUDE_DIRECTORIES)
        foreach (_inc ${DIRINC})
            LIST(APPEND _args " -I" ${_inc})
        endforeach(_inc ${DIRINC})
        SEPARATE_ARGUMENTS(_args)
        add_custom_command(OUTPUT ${_gch_filename}
            COMMAND rm -f ${_gch_filename}
            COMMAND ${CMAKE_CXX_COMPILER} ${_args}
                    DEPENDS ${_header_filename})
    ENDMACRO(ADD_PCH_RULE _header_filename _src_list)

    ADD_PCH_RULE(${CMAKE_CURRENT_SOURCE_DIR}/utils/utils_pch.h SRCS)
ENDIF()

# Don't add the luabind static lib files if requested.
IF (UNIX AND USE_SYSTEM_LUABIND)
    ADD_EXECUTABLE(valyriatear WIN32 ${SRCS} ${SRCS_COMMON}
        # The system include files.
        ${LUABIND_INCLUDE_DIRS}
    )

    # In that case the luabind system headers will be needed
    TARGET_LINK_LIBRARIES(valyriatear ${LUABIND_LIBRARIES})
ELSE()
    ADD_EXECUTABLE(valyriatear WIN32 ${SRCS} ${SRCS_COMMON} ${SRCS_LUABIND})
ENDIF()

# Vorbis, vorbisfile and ogg are explcitely needed on OpenBSD
IF (CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
    TARGET_LINK_LIBRARIES(valyriatear
        ${INTERNAL_LIBRARIES}
        ${SDL_LIBRARY}
        ${SDLTTF_LIBRARY}
        ${SDLIMAGE_LIBRARY}
        ${OPENGL_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${VORBISFILE_LIBRARIES}
        ${OGG_LIBRARY}
        ${VORBIS_LIBRARIES}
        ${PNG_LIBRARIES}
        ${LUA_LIBRARIES}
        ${X11_LIBRARIES}
        ${LIBINTL_LIBRARIES}
        ${EXTRA_LIBRARIES})
ELSE()
    TARGET_LINK_LIBRARIES(valyriatear
        ${INTERNAL_LIBRARIES}
        ${SDL_LIBRARY}
        ${SDLTTF_LIBRARY}
        ${SDLIMAGE_LIBRARY}
        ${OPENGL_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${VORBISFILE_LIBRARIES}
        ${PNG_LIBRARIES}
        ${LUA_LIBRARIES}
        ${X11_LIBRARIES}
        ${LIBINTL_LIBRARIES}
        ${EXTRA_LIBRARIES})
ENDIF()

INSTALL(TARGETS valyriatear RUNTIME DESTINATION ${PKG_BINDIR})

IF (UNIX)
      # uninstall target
      ADD_CUSTOM_TARGET(
          uninstall-valyriatear
          COMMAND rm -rf ${PKG_DATADIR}
          COMMAND rm -f "${PKG_BINDIR}/valyriatear"
          VERBATIM
      )
ENDIF()


IF (CMAKE_SYSTEM_NAME STREQUAL SunOS)
    # we expect the SMCgtxt package to be present on Solaris;
    # the Solaris gettext is not API-compatible to GNU gettext
    SET_TARGET_PROPERTIES(valyriatear PROPERTIES LINK_FLAGS "-L/usr/local/lib")
ENDIF()

# Adds invalid pch compilation warning for the game.
IF (CMAKE_COMPILER_IS_GNUCXX AND USE_PCH_COMPILATION)
    SET_TARGET_PROPERTIES(valyriatear PROPERTIES COMPILE_FLAGS "${FLAGS}  -Winvalid-pch")
ELSE()
    SET_TARGET_PROPERTIES(valyriatear PROPERTIES COMPILE_FLAGS "${FLAGS}")
ENDIF()

# Editor part
IF (EDITOR_SUPPORT)
    MESSAGE(STATUS "Editor build on")
    FIND_PACKAGE(Qt4 REQUIRED)
    INCLUDE_DIRECTORIES(
    ${QT_QTOPENGL_INCLUDE_DIR}
    )
    INCLUDE(${QT_USE_FILE})
    ADD_DEFINITIONS(${QT_DEFINITIONS})

    SET(EDITOR_QT_HEADERS
    editor/dialog_boxes.h
    editor/editor.h
    editor/grid.h
    editor/tileset_editor.h
    )

    SET(SRCS_EDITOR
    editor/dialog_boxes.cpp
    editor/editor.cpp
    editor/editor_main.cpp
    editor/grid.cpp
    editor/tileset.cpp
    editor/tileset.h
    editor/tileset_editor.cpp
    )

    QT4_WRAP_CPP(EDITOR_QT_HEADERS_MOC ${EDITOR_QT_HEADERS})

    SET (PROGRAMS vt-editor)

    # Don't add the static luabind source if requested.
    IF (UNIX AND USE_SYSTEM_LUABIND)
            ADD_EXECUTABLE(vt-editor
                ${SRCS_EDITOR}
                ${EDITOR_QT_HEADERS_MOC}
                ${SRCS_COMMON}
                # The system include files.
                ${LUABIND_INCLUDE_DIRS}
            )

        # In that case the luabind system headers will be needed
        TARGET_LINK_LIBRARIES(vt-editor ${LUABIND_LIBRARIES})
    ELSE()
            ADD_EXECUTABLE(vt-editor
                ${SRCS_EDITOR}
                ${SRCS_LUABIND}
                ${EDITOR_QT_HEADERS_MOC}
                ${SRCS_COMMON}
            )
    ENDIF()

    TARGET_LINK_LIBRARIES(vt-editor
        ${INTERNAL_LIBRARIES}
        ${QT_LIBRARIES}
        ${QT_QTOPENGL_LIBRARY}
        ${SDL_LIBRARY}
        ${SDLIMAGE_LIBRARY}
        ${SDLTTF_LIBRARY}
        ${OPENGL_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${VORBISFILE_LIBRARIES}
        ${PNG_LIBRARIES}
        ${LUA_LIBRARIES}
        ${X11_LIBRARIES}
        ${LIBINTL_LIBRARIES}
        ${EXTRA_LIBRARIES}
    )

    INSTALL(TARGETS vt-editor RUNTIME DESTINATION ${PKG_BINDIR})
    SET_TARGET_PROPERTIES(vt-editor PROPERTIES COMPILE_FLAGS "${FLAGS} -DEDITOR_BUILD")

    IF (UNIX)
        # uninstall target
        ADD_CUSTOM_TARGET(
            uninstall-vt-editor
            COMMAND rm -f "${PKG_BINDIR}/vt-editor"
            VERBATIM
        )
    ENDIF()
ENDIF(EDITOR_SUPPORT)
