#Valyria Tear /src

FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(SDL2_ttf REQUIRED)
FIND_PACKAGE(SDL2_image REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(OpenAL REQUIRED)
FIND_PACKAGE(VorbisFile REQUIRED)
FIND_PACKAGE(Lua 5.1 REQUIRED)
FIND_PACKAGE(PNG REQUIRED)
FIND_PACKAGE(Gettext REQUIRED)
FIND_PACKAGE(Boost 1.46.1 REQUIRED)

# Check for Linux
IF (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Check for Debian
    find_file (DEBIAN_VERSION NAMES debian_version PATHS /etc NO_DEFAULT_PATH)
    IF (${DEBIAN_VERSION} STREQUAL "/etc/debian_version")
        MESSAGE(STATUS "Detected Debian System")
        # Install dev target to ease development setup
        ADD_CUSTOM_TARGET(install_debian_packages
        sudo apt install libsdl2-ttf-dev libsdl2-image-dev libgl1-mesa-dev libglew-dev libopenal-dev libvorbis-dev liblua5.3-dev libpng-dev gettext libboost-dev
        COMMENT "Installing Debian development dependencies ..."
        VERBATIM
        )
    ENDIF()
ENDIF()

# Set the release mode if not told otherwise
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Handles Clang flags
IF(CMAKE_CXX_COMPILER MATCHES ".*clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")
ENDIF()

# Handles gcc flags
IF(CMAKE_COMPILER_IS_GNUCXX)
    # Help getting compilation warnings and c++11 support
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")
    IF(WIN32)
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
    IF(CMAKE_BUILD_TYPE_TOLOWER MATCHES debug)
        SET(FLAGS "${FLAGS} -DDEBUG")

        IF (CMAKE_COMPILER_IS_GNUCXX)
            # Help getting debug info
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pg")
        ENDIF()
    ELSEIF(CMAKE_BUILD_TYPE_TOLOWER MATCHES relwithdebinfo)
        IF (CMAKE_COMPILER_IS_GNUCXX)
            # Help getting debug info
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
        ENDIF()
    ENDIF()
ENDIF()

IF (NOT DISABLE_TRANSLATIONS)
    IF (WIN32)
        SET(EXTRA_LIBRARIES ws2_32 winmm)
        FIND_PACKAGE(LibIntl REQUIRED)
    ELSEIF (CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR BEOS OR CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
        FIND_PACKAGE(LibIntl REQUIRED)
        FIND_PACKAGE(LibIconv REQUIRED)
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
    ${SDL2_INCLUDE_DIR}
    ${SDL2_TTF_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_PATH}
    ${OPENAL_INCLUDE_DIR}
    ${VORBISFILE_INCLUDE_DIR}
    ${PNG_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
    )

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/luabind")
MESSAGE(STATUS "Using included luabind files")

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/utils/src/")
MESSAGE(STATUS "Using included utils files")

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/luabind/build_information.hpp.cmake_in" "${CMAKE_CURRENT_SOURCE_DIR}/luabind/luabind/build_information.hpp")
MESSAGE(STATUS "Configured luabind information file...")

# Enable C++11 in luabind
SET(FLAGS "${FLAGS} -DLUABIND_USE_CXX11")

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
luabind/src/lua51compat.cpp
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
luabind/luabind/detail/property.hpp
luabind/luabind/detail/other.hpp
luabind/luabind/detail/link_compatibility.hpp
luabind/luabind/detail/typetraits.hpp
luabind/luabind/detail/has_get_pointer.hpp
luabind/luabind/detail/debug.hpp
luabind/luabind/detail/garbage_collector.hpp
luabind/luabind/detail/instance_holder.hpp
luabind/luabind/detail/class_rep.hpp
luabind/luabind/detail/signature_match.hpp
luabind/luabind/detail/call_member.hpp
luabind/luabind/detail/pointee_sizeof.hpp
luabind/luabind/detail/policy.hpp
luabind/luabind/detail/deduce_signature.hpp
luabind/luabind/detail/class_registry.hpp
luabind/luabind/detail/yes_no.hpp
luabind/luabind/detail/operator_id.hpp
luabind/luabind/detail/make_instance.hpp
luabind/luabind/detail/pcall.hpp
luabind/luabind/detail/call.hpp
luabind/luabind/detail/call_operator_iterate.hpp
luabind/luabind/yield_policy.hpp
luabind/luabind/error.hpp
luabind/luabind/wrapper_base.hpp
)

# Files used by both the game and the editor
SET(SRCS_COMMON
utils/src/script/script.cpp
utils/src/script/script_read.cpp
utils/src/script/script_write.cpp
utils/src/utils/utils_random.cpp
utils/src/utils/utils_files.cpp
utils/src/utils/utils_numeric.cpp
utils/src/utils/utils_strings.cpp
utils/src/utils/ustring.cpp
utils/src/utils/exception.cpp
)

SET(SRCS
common/global/battle_media.cpp
common/global/quest_log.cpp
common/global/global.cpp
common/global/global_event_group.cpp
common/global/actors/global_actor.cpp
common/global/actors/global_attack_point.cpp
common/global/actors/global_character.cpp
common/global/actors/global_enemy.cpp
common/global/actors/global_party.cpp
common/global/global_effects.cpp
common/global/objects/global_object.cpp
common/global/objects/global_item.cpp
common/global/objects/global_weapon.cpp
common/global/objects/global_armor.cpp
common/global/objects/global_spirit.cpp
common/global/global_skills.cpp
common/global/global_utils.cpp
common/global/worldmap_location.cpp
common/global/skill_graph/skill_node.cpp
common/global/skill_graph/skill_graph.cpp
common/gui/option.cpp
common/gui/menu_window.cpp
common/gui/textbox.cpp
common/gui/gui.cpp
common/message_window.cpp
common/dialogue.cpp
common/common.cpp
common/options_handler.cpp
common/app_settings.cpp
common/common_bindings.cpp
engine/audio/audio.cpp
engine/audio/audio_descriptor.cpp
engine/audio/audio_input.cpp
engine/audio/audio_stream.cpp
engine/audio/audio_effects.cpp
engine/effect_supervisor.cpp
engine/mode_manager.cpp
engine/script_supervisor.cpp
engine/indicator_supervisor.cpp
engine/system.cpp
engine/input.cpp
engine/engine_bindings.cpp
engine/video/fade.cpp
engine/video/gl/gl_particle_system.cpp
engine/video/gl/gl_render_target.cpp
engine/video/gl/gl_shader.cpp
engine/video/gl/gl_shader_program.cpp
engine/video/gl/gl_shader_programs.h
engine/video/gl/gl_sprite.cpp
engine/video/gl/gl_transform.cpp
engine/video/gl/gl_vector.cpp
engine/video/image.cpp
engine/video/image_base.cpp
engine/video/interpolator.cpp
engine/video/particle_effect.cpp
engine/video/particle_manager.cpp
engine/video/particle_system.cpp
engine/video/text.cpp
engine/video/texture.cpp
engine/video/texture_controller.cpp
engine/video/video.cpp
modes/shop/shop_buy.cpp
modes/shop/shop_sell.cpp
modes/shop/shop_trade.cpp
modes/shop/shop_root.cpp
modes/shop/shop.cpp
modes/shop/shop_utils.cpp
modes/battle/battle_objects/battle_actor.cpp
modes/battle/battle_objects/battle_animation.cpp
modes/battle/battle_objects/battle_character.cpp
modes/battle/battle_objects/battle_enemy.cpp
modes/battle/battle_objects/battle_particle_effect.cpp
modes/battle/battle.cpp
modes/battle/battle_actions/battle_action.cpp
modes/battle/battle_actions/item_action.cpp
modes/battle/battle_actions/skill_action.cpp
modes/battle/battle_command.cpp
modes/battle/battle_effects.cpp
modes/battle/battle_item.cpp
modes/battle/battle_menu.cpp
modes/battle/battle_target.cpp
modes/battle/battle_utils.cpp
modes/battle/transition_to_battle.cpp
modes/battle/finish/battle_defeat.cpp
modes/battle/finish/battle_victory.cpp
modes/battle/battle_sequence.cpp
modes/boot/boot.cpp
modes/save/save_mode.cpp
modes/map/map_mode.cpp
modes/map/map_dialogue_supervisor.cpp
modes/map/map_dialogues/map_dialogue_options.cpp
modes/map/map_dialogues/map_sprite_dialogue.cpp
modes/map/map_utils.cpp
modes/map/map_object_supervisor.cpp
modes/map/map_objects/map_object.cpp
modes/map/map_objects/map_physical_object.cpp
modes/map/map_objects/map_particle.cpp
modes/map/map_objects/map_save_point.cpp
modes/map/map_objects/map_escape_point.cpp
modes/map/map_objects/map_halo.cpp
modes/map/map_objects/map_light.cpp
modes/map/map_objects/map_sound.cpp
modes/map/map_objects/map_treasure.cpp
modes/map/map_objects/map_trigger.cpp
modes/map/map_escape.cpp
modes/map/map_events.cpp
modes/map/map_event_supervisor.cpp
modes/map/map_tiles.cpp
modes/map/map_sprites/map_sprite.cpp
modes/map/map_sprites/map_virtual_sprite.cpp
modes/map/map_sprites/map_enemy_sprite.cpp
modes/map/map_treasure_supervisor.cpp
modes/map/map_treasure_content.cpp
modes/map/map_zones.cpp
modes/map/map_minimap.cpp
modes/map/map_status_effects.cpp
modes/menu/menu_mode.cpp
modes/menu/menu_states/menu_abstract_state.cpp
modes/menu/menu_states/menu_main_state.cpp
modes/menu/menu_states/menu_inventory_state.cpp
modes/menu/menu_states/menu_equip_state.cpp
modes/menu/menu_states/menu_skill_state.cpp
modes/menu/menu_states/menu_party_state.cpp
modes/menu/menu_states/menu_quest_state.cpp
modes/menu/menu_states/menu_worldmap_state.cpp
modes/menu/menu_windows/menu_battle_formation_window.cpp
modes/menu/menu_windows/menu_character_window.cpp
modes/menu/menu_windows/menu_equip_window.cpp
modes/menu/menu_windows/menu_inventory_window.cpp
modes/menu/menu_windows/menu_party_window.cpp
modes/menu/menu_windows/menu_quest_window.cpp
modes/menu/menu_windows/menu_quest_list_window.cpp
modes/menu/menu_windows/menu_skills_window.cpp
modes/menu/menu_windows/menu_skillgraph_window.cpp
modes/menu/menu_windows/menu_worldmap_window.cpp
modes/menu/menu_windows/skill_node_bottom_info.cpp
modes/pause.cpp
modes/mode_bindings.cpp
modes/mode_help_window.cpp
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

ADD_EXECUTABLE(valyriatear WIN32 ${SRCS} ${SRCS_COMMON} ${SRCS_LUABIND})

# Vorbis, vorbisfile and ogg are explcitely needed on OpenBSD
IF (CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
    TARGET_LINK_LIBRARIES(valyriatear
        ${INTERNAL_LIBRARIES}
        ${SDL2_LIBRARY}
        ${SDL2_TTF_LIBRARY}
        ${SDL2_IMAGE_LIBRARY}
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARY}
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
        ${SDL2_LIBRARY}
        ${SDL2_TTF_LIBRARY}
        ${SDL2_IMAGE_LIBRARY}
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARY}
        ${OPENAL_LIBRARY}
        ${VORBISFILE_LIBRARIES}
        ${PNG_LIBRARIES}
        ${LUA_LIBRARIES}
        ${X11_LIBRARIES}
        ${LIBINTL_LIBRARIES}
        ${ICONV_LIBRARIES}
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

# Copy frameworks, resources etc.
if (APPLE)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../dat DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../Resources/ FILES_MATCHING PATTERN "*.lua" PATTERN "*.png")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../img DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../Resources/ FILES_MATCHING PATTERN "*.lua" PATTERN "*.png" PATTERN "*.ttf")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../mus DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../Resources/ FILES_MATCHING PATTERN "*.ogg")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../snd DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../Resources/ FILES_MATCHING PATTERN "*.wav" PATTERN "*.ogg")
    file(COPY ${SDL2_LIBRARY} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${SDL2_TTF_LIBRARY} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${SDL2_IMAGE_LIBRARY} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
ENDIF()

IF (CMAKE_SYSTEM_NAME STREQUAL SunOS)
    # we expect the SMCgtxt package to be present on Solaris;
    # the Solaris gettext is not API-compatible to GNU gettext
    SET_TARGET_PROPERTIES(valyriatear PROPERTIES LINK_FLAGS "-L/usr/local/lib")
ENDIF()

SET_TARGET_PROPERTIES(valyriatear PROPERTIES COMPILE_FLAGS "${FLAGS}")
