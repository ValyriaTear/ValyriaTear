#Valyria Tear /src

FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(SDL_ttf REQUIRED)
FIND_PACKAGE(SDL_image REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(OpenAL REQUIRED)
FIND_PACKAGE(VorbisFile REQUIRED)
FIND_PACKAGE(Lua51 REQUIRED)
FIND_PACKAGE(PNG REQUIRED)
FIND_PACKAGE(jpeg REQUIRED)
FIND_PACKAGE(Gettext REQUIRED)

IF (CMAKE_COMPILER_IS_GNUCXX)
    # Help getting compilation warnings
    SET(CMAKE_CXX_FLAGS "-Wall -Wextra")
    IF (WIN32)
        # This includes enough debug information to get something useful
        # from Dr. Mingw while keeping binary size down. Almost useless
        # with gdb, though.
        SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb0 -gstabs2")
    ENDIF()
ENDIF()

IF (POLICY CMP0015)
    CMAKE_POLICY(SET CMP0015 OLD)
ENDIF()

SET(FLAGS "-DPACKAGE_VERSION=\\\"${VERSION}\\\"")
SET(FLAGS "${FLAGS} -DPKG_DATADIR=\\\"${PKG_DATADIR}/\\\"")
SET(FLAGS "${FLAGS} -DLOCALEDIR=\\\"${LOCALEDIR}/\\\"")
SET(FLAGS "${FLAGS} -DPKG_NAME=\\\"${PKG_NAME}/\\\"")

IF (DEBUG_MENU)
    SET(FLAGS "${FLAGS} -DDEBUG_MENU")
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

        MESSAGE(STATUS "Set Debug build type")
    ENDIF()
ENDIF()

IF (WIN32)
    SET(EXTRA_LIBRARIES ws2_32 winmm)
    FIND_PACKAGE(LibIntl REQUIRED)
ELSEIF (CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR BEOS)
    FIND_PACKAGE(LibIntl REQUIRED)
ELSEIF (CMAKE_SYSTEM_NAME STREQUAL SunOS)
    # explicit linking to libintl is required on Solaris
    SET(EXTRA_LIBRARIES intl)
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
    ${JPEG_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/luabind
    )

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

SET(SRCS_TESTS
test/test_main.cpp
test/test_main.h
)

SET(SRCS_LUABIND_TESTS
luabind/examples/hello_world/hello_world.cpp
luabind/examples/cln/cln_test.cpp
luabind/examples/any_converter/any_converter.cpp
luabind/examples/intrusive_ptr/intrusive_ptr.cpp
luabind/examples/glut/glut_bind.cpp
luabind/examples/filesystem/filesystem.cpp
luabind/examples/filesystem/directory_iterator.hpp
luabind/examples/regexp/regex_wrap.cpp
luabind/test/test_user_defined_converter.cpp
luabind/test/test_virtual_inheritance.cpp
luabind/test/test_super_leak.cpp
luabind/test/main.cpp
luabind/test/test_properties.cpp
luabind/test/test_construction.cpp
luabind/test/test_shared_ptr.cpp
luabind/test/test_scope.cpp
luabind/test/test_exceptions.cpp
luabind/test/test_shadow.cpp
luabind/test/test_const.cpp
luabind/test/test_private_destructors.cpp
luabind/test/test_table.cpp
luabind/test/test_set_instance_value.cpp
luabind/test/test_def_from_base.cpp
luabind/test/test_implicit_cast.cpp
luabind/test/test_simple_class.cpp
luabind/test/benchmark.cpp
luabind/test/test_abstract_base.cpp
luabind/test/test_back_reference.cpp
luabind/test/test_adopt.cpp
luabind/test/test_implicit_raw.cpp
luabind/test/test_typetraits.cpp
luabind/test/test_value_wrapper.cpp
luabind/test/test_smart_ptr_attributes.cpp
luabind/test/test_dynamic_type.cpp
luabind/test/test_policies.cpp
luabind/test/test_tag_function.cpp
luabind/test/test_iterator.cpp
luabind/test/test_null_pointer.cpp
luabind/test/test_create_in_thread.cpp
luabind/test/test_free_functions.cpp
luabind/test/test_exception_handlers.cpp
luabind/test/test_object.cpp
luabind/test/test_automatic_smart_ptr.cpp
luabind/test/test_builtin_converters.cpp
luabind/test/test_yield.cpp
luabind/test/test_vector_of_object.cpp
luabind/test/test_collapse_converter.cpp
luabind/test/test_adopt_wrapper.cpp
luabind/test/test_held_type.cpp
luabind/test/test_attributes.cpp
luabind/test/test_has_get_pointer.cpp
luabind/test/test_lua_classes.cpp
luabind/test/test_separation.cpp
luabind/test/test_operators.cpp
luabind/test/test_class_info.cpp
luabind/test/test.hpp
luabind/test/test_extend_class_in_lua.cpp
luabind/test/test_unsigned_int.cpp
)

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
luabind/luabind/detail/get_signature.hpp
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
engine/effect_supervisor.h
engine/effect_supervisor.cpp
engine/mode_manager.h
engine/mode_manager.cpp
engine/script_supervisor.h
engine/script_supervisor.cpp
engine/system.cpp
engine/system.h
engine/video/video.h
engine/video/video.cpp
engine/video/texture_controller.h
engine/video/texture_controller.cpp
engine/video/texture.cpp
engine/video/texture.h
engine/video/image.cpp
engine/video/image.h
engine/video/image_base.cpp
engine/video/image_base.h
engine/video/interpolator.cpp
engine/video/interpolator.h
engine/video/fade.h
engine/video/fade.cpp
engine/video/text.cpp
engine/video/text.h
engine/video/shake.cpp
engine/video/shake.h
engine/video/particle_manager.h
engine/video/particle_manager.cpp
engine/video/particle_effect.h
engine/video/particle_effect.cpp
engine/video/particle_system.h
engine/video/particle_system.cpp
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
engine/script/script.h
engine/script/script.cpp
engine/script/script_read.h
engine/script/script_read.cpp
engine/script/script_write.h
engine/script/script_write.cpp
utils.h
utils.cpp
defs.h
)

SET(SRCS
main.cpp
common/common_bindings.cpp
common/common.cpp
common/dialogue.cpp
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
common/gui/menu_window.cpp
common/gui/menu_window.h
common/gui/gui.h
common/gui/textbox.cpp
common/gui/textbox.h
common/gui/option.cpp
common/gui/gui.cpp
common/dialogue.h
common/common.h
common/common_bindings.cpp
main_options.h
modes/pause.cpp
modes/shop/shop_root.h
modes/shop/shop_buy.cpp
modes/shop/shop_leave.h
modes/shop/shop_sell.cpp
modes/shop/shop_trade.cpp
modes/shop/shop_utils.h
modes/shop/shop_root.cpp
modes/shop/shop_trade.h
modes/shop/shop_confirm.h
modes/shop/shop.cpp
modes/shop/shop_leave.cpp
modes/shop/shop_confirm.cpp
modes/shop/shop_buy.h
modes/shop/shop_utils.cpp
modes/shop/shop.h
modes/shop/shop_sell.h
modes/scene.h
modes/mode_bindings.cpp
modes/battle/battle_effects.cpp
modes/battle/battle_sequence.h
modes/battle/battle_actors.cpp
modes/battle/battle_indicators.h
modes/battle/battle_actors.h
modes/battle/battle_command.h
modes/battle/battle_dialogue.cpp
modes/battle/battle_actions.h
modes/battle/battle.h
modes/battle/battle_utils.h
modes/battle/battle_indicators.cpp
modes/battle/battle_finish.h
modes/battle/battle_actions.cpp
modes/battle/battle_utils.cpp
modes/battle/battle_command.cpp
modes/battle/battle_dialogue.h
modes/battle/battle_effects.h
modes/battle/battle.cpp
modes/battle/battle_finish.cpp
modes/battle/battle_sequence.cpp
modes/scene.cpp
modes/boot/boot.h
modes/boot/boot.cpp
modes/boot/boot_welcome.cpp
modes/boot/boot_menu.h
modes/boot/boot_welcome.h
modes/boot/boot_menu.cpp
modes/save/save_mode.h
modes/save/save_mode.cpp
modes/map/map_dialogue.h
modes/map/map_zones.h
modes/map/map_treasure.h
modes/map/map_events.h
modes/map/map_tiles.h
modes/map/map_utils.h
modes/map/map.cpp
modes/map/map_dialogue.cpp
modes/map/map.h
modes/map/map_utils.cpp
modes/map/map_objects.cpp
modes/map/map_events.cpp
modes/map/map_tiles.cpp
modes/map/map_sprites.cpp
modes/map/map_treasure.cpp
modes/map/map_sprites.h
modes/map/map_zones.cpp
modes/map/map_objects.h
modes/menu/menu.h
modes/menu/menu.cpp
modes/menu/menu_views.cpp
modes/menu/menu_views.h
modes/pause.h
engine/script/script_modify.h
engine/script/script_modify.cpp
engine/input.h
engine/input.cpp
engine/engine_bindings.cpp
engine/video/screen_rect.h
engine/video/color.h
engine/video/particle_emitter.h
engine/video/particle_keyframe.h
engine/video/context.h
engine/video/particle.h
engine/video/coord_sys.h
utils.h
main_options.cpp
    )

IF (WIN32)
    SET(SRCS
        ${SRCS}
       icon.rc
    )
ENDIF ()

SET (PROGRAMS valyriatear)

ADD_EXECUTABLE(valyriatear WIN32 ${SRCS} ${SRCS_COMMON} ${SRCS_LUABIND})

TARGET_LINK_LIBRARIES(valyriatear
    ${INTERNAL_LIBRARIES}
    ${SDL_LIBRARY}
    ${SDLTTF_LIBRARY}
    ${SDLIMAGE_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${OPENAL_LIBRARY}
    ${VORBISFILE_LIBRARIES}
    ${PNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${LUA_LIBRARIES}
    ${X11_LIBRARIES}
    ${LIBINTL_LIBRARIES}
    ${EXTRA_LIBRARIES})

INSTALL(TARGETS valyriatear RUNTIME DESTINATION ${PKG_BINDIR})

IF (CMAKE_SYSTEM_NAME STREQUAL SunOS)
    # we expect the SMCgtxt package to be present on Solaris;
    # the Solaris gettext is not API-compatible to GNU gettext
    SET_TARGET_PROPERTIES(valyriatear PROPERTIES LINK_FLAGS "-L/usr/local/lib")
ENDIF()

SET_TARGET_PROPERTIES(valyriatear PROPERTIES COMPILE_FLAGS "${FLAGS}")


# Editor part
IF (EDITOR_SUPPORT)

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

    ADD_EXECUTABLE(vt-editor
        ${SRCS_EDITOR}
        ${SRCS_LUABIND}
        ${EDITOR_QT_HEADERS_MOC}
        ${SRCS_COMMON}
    )

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
        ${JPEG_LIBRARIES}
        ${LUA_LIBRARIES}
        ${X11_LIBRARIES}
        ${LIBINTL_LIBRARIES}
        ${EXTRA_LIBRARIES}
    )

    INSTALL(TARGETS vt-editor RUNTIME DESTINATION ${PKG_BINDIR})
    SET_TARGET_PROPERTIES(vt-editor PROPERTIES COMPILE_FLAGS "${FLAGS} -DQT3_SUPPORT")
ENDIF(EDITOR_SUPPORT)
