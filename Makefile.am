EXTRA_DIST = ABOUT-NLS AUTHORS COPYING config.rpath configure dat doc img INSTALL MANUAL mus README snd

SUBDIRS = txt

AM_CPPFLAGS = -DDATADIR=\"$(datadir)/allacrost\" -DLOCALEDIR=\"$(datadir)/allacrost/$(LOCALEDIR)\" -DPACKAGE=\"$(PACKAGE)\"
ACLOCAL_AMFLAGS = -I m4

INCLUDES = \
	-I/usr/include/SDL \
	-I/usr/include/AL \
	-I/usr/include/lua5.1 \
	-I$(top_srcdir)/src/luabind \
	-I$(top_srcdir)/src/luabind/luabind \
	-I$(top_srcdir)/src/luabind/luabind/detail \
	-I$(top_srcdir)/src/luabind/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/engine \
	-I$(top_srcdir)/src/engine/audio \
	-I$(top_srcdir)/src/engine/video \
	-I$(top_srcdir)/src/engine/script \
	-I$(top_srcdir)/src/global \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/common/global \
	-I$(top_srcdir)/src/common/gui \
	-I$(top_srcdir)/src/modes \
	-I$(top_srcdir)/src/modes/battle \
	-I$(top_srcdir)/src/modes/boot \
	-I$(top_srcdir)/src/modes/map \
	-I$(top_srcdir)/src/modes/menu \
	-I$(top_srcdir)/src/modes/save \
	-I$(top_srcdir)/src/modes/shop

VIDEO_DIR = src/engine/video
video_SOURCES = \
	$(VIDEO_DIR)/color.h \
	$(VIDEO_DIR)/context.h \
	$(VIDEO_DIR)/coord_sys.h \
	$(VIDEO_DIR)/effects.cpp \
	$(VIDEO_DIR)/fade.cpp \
	$(VIDEO_DIR)/fade.h \
	$(VIDEO_DIR)/image_base.cpp \
	$(VIDEO_DIR)/image_base.h \
	$(VIDEO_DIR)/image.cpp \
	$(VIDEO_DIR)/image.h \
	$(VIDEO_DIR)/interpolator.cpp \
	$(VIDEO_DIR)/interpolator.h \
	$(VIDEO_DIR)/particle.h \
	$(VIDEO_DIR)/particle_effect.cpp \
	$(VIDEO_DIR)/particle_effect.h \
	$(VIDEO_DIR)/particle_emitter.h \
	$(VIDEO_DIR)/particle_keyframe.h \
	$(VIDEO_DIR)/particle_manager.cpp \
	$(VIDEO_DIR)/particle_manager.h \
	$(VIDEO_DIR)/particle_system.cpp \
	$(VIDEO_DIR)/particle_system.h \
	$(VIDEO_DIR)/screen_rect.h \
	$(VIDEO_DIR)/shake.cpp \
	$(VIDEO_DIR)/shake.h \
	$(VIDEO_DIR)/text.cpp \
	$(VIDEO_DIR)/text.h \
	$(VIDEO_DIR)/texture.cpp \
	$(VIDEO_DIR)/texture.h \
	$(VIDEO_DIR)/texture_controller.cpp \
	$(VIDEO_DIR)/texture_controller.h \
	$(VIDEO_DIR)/video.cpp \
	$(VIDEO_DIR)/video.h

AUDIO_DIR = src/engine/audio
audio_SOURCES = \
	$(AUDIO_DIR)/audio.cpp \
	$(AUDIO_DIR)/audio.h \
	$(AUDIO_DIR)/audio_descriptor.cpp \
	$(AUDIO_DIR)/audio_descriptor.h \
	$(AUDIO_DIR)/audio_effects.cpp \
	$(AUDIO_DIR)/audio_effects.h \
	$(AUDIO_DIR)/audio_input.cpp \
	$(AUDIO_DIR)/audio_input.h \
	$(AUDIO_DIR)/audio_stream.cpp \
	$(AUDIO_DIR)/audio_stream.h

SCRIPT_DIR = src/engine/script
script_SOURCES = \
	$(SCRIPT_DIR)/script.cpp \
	$(SCRIPT_DIR)/script.h \
	$(SCRIPT_DIR)/script_read.cpp \
	$(SCRIPT_DIR)/script_read.h \
	$(SCRIPT_DIR)/script_write.cpp \
	$(SCRIPT_DIR)/script_write.h \
	$(SCRIPT_DIR)/script_modify.cpp \
	$(SCRIPT_DIR)/script_modify.h

LUABIND_DIR = src/luabind
luabind_SOURCES = \
	$(LUABIND_DIR)/luabind/adopt_policy.hpp \
	$(LUABIND_DIR)/luabind/back_reference_fwd.hpp \
	$(LUABIND_DIR)/luabind/back_reference.hpp \
	$(LUABIND_DIR)/luabind/class.hpp \
	$(LUABIND_DIR)/luabind/class_info.hpp \
	$(LUABIND_DIR)/luabind/config.hpp \
	$(LUABIND_DIR)/luabind/container_policy.hpp \
	$(LUABIND_DIR)/luabind/copy_policy.hpp \
	$(LUABIND_DIR)/luabind/dependency_policy.hpp \
	$(LUABIND_DIR)/luabind/detail \
	$(LUABIND_DIR)/luabind/discard_result_policy.hpp \
	$(LUABIND_DIR)/luabind/error.hpp \
	$(LUABIND_DIR)/luabind/exception_handler.hpp \
	$(LUABIND_DIR)/luabind/from_stack.hpp \
	$(LUABIND_DIR)/luabind/function.hpp \
	$(LUABIND_DIR)/luabind/get_main_thread.hpp \
	$(LUABIND_DIR)/luabind/get_pointer.hpp \
	$(LUABIND_DIR)/luabind/handle.hpp \
	$(LUABIND_DIR)/luabind/iterator_policy.hpp \
	$(LUABIND_DIR)/luabind/luabind.hpp \
	$(LUABIND_DIR)/luabind/lua_include.hpp \
	$(LUABIND_DIR)/luabind/make_function.hpp \
	$(LUABIND_DIR)/luabind/nil.hpp \
	$(LUABIND_DIR)/luabind/object.hpp \
	$(LUABIND_DIR)/luabind/open.hpp \
	$(LUABIND_DIR)/luabind/operator.hpp \
	$(LUABIND_DIR)/luabind/out_value_policy.hpp \
	$(LUABIND_DIR)/luabind/prefix.hpp \
	$(LUABIND_DIR)/luabind/raw_policy.hpp \
	$(LUABIND_DIR)/luabind/return_reference_to_policy.hpp \
	$(LUABIND_DIR)/luabind/scope.hpp \
	$(LUABIND_DIR)/luabind/shared_ptr_converter.hpp \
	$(LUABIND_DIR)/luabind/tag_function.hpp \
	$(LUABIND_DIR)/luabind/typeid.hpp \
	$(LUABIND_DIR)/luabind/value_wrapper.hpp \
	$(LUABIND_DIR)/luabind/version.hpp \
	$(LUABIND_DIR)/luabind/weak_ref.hpp \
	$(LUABIND_DIR)/luabind/wrapper_base.hpp \
	$(LUABIND_DIR)/luabind/yield_policy.hpp \
	$(LUABIND_DIR)/luabind/detail/calc_arity.hpp \
	$(LUABIND_DIR)/luabind/detail/call_function.hpp \
	$(LUABIND_DIR)/luabind/detail/call.hpp \
	$(LUABIND_DIR)/luabind/detail/call_member.hpp \
	$(LUABIND_DIR)/luabind/detail/call_operator_iterate.hpp \
	$(LUABIND_DIR)/luabind/detail/class_cache.hpp \
	$(LUABIND_DIR)/luabind/detail/class_registry.hpp \
	$(LUABIND_DIR)/luabind/detail/class_rep.hpp \
	$(LUABIND_DIR)/luabind/detail/compute_score.hpp \
	$(LUABIND_DIR)/luabind/detail/constructor.hpp \
	$(LUABIND_DIR)/luabind/detail/conversion_storage.hpp \
	$(LUABIND_DIR)/luabind/detail/convert_to_lua.hpp \
	$(LUABIND_DIR)/luabind/detail/debug.hpp \
	$(LUABIND_DIR)/luabind/detail/decorate_type.hpp \
	$(LUABIND_DIR)/luabind/detail/deduce_signature.hpp \
	$(LUABIND_DIR)/luabind/detail/enum_maker.hpp \
	$(LUABIND_DIR)/luabind/detail/format_signature.hpp \
	$(LUABIND_DIR)/luabind/detail/garbage_collector.hpp \
	$(LUABIND_DIR)/luabind/detail/get_signature.hpp \
	$(LUABIND_DIR)/luabind/detail/has_get_pointer.hpp \
	$(LUABIND_DIR)/luabind/detail/inheritance.hpp \
	$(LUABIND_DIR)/luabind/detail/instance_holder.hpp \
	$(LUABIND_DIR)/luabind/detail/is_indirect_const.hpp \
	$(LUABIND_DIR)/luabind/detail/link_compatibility.hpp \
	$(LUABIND_DIR)/luabind/detail/make_instance.hpp \
	$(LUABIND_DIR)/luabind/detail/most_derived.hpp \
	$(LUABIND_DIR)/luabind/detail/object_call.hpp \
	$(LUABIND_DIR)/luabind/detail/object_funs.hpp \
	$(LUABIND_DIR)/luabind/detail/object_rep.hpp \
	$(LUABIND_DIR)/luabind/detail/open.hpp \
	$(LUABIND_DIR)/luabind/detail/operator_id.hpp \
	$(LUABIND_DIR)/luabind/detail/other.hpp \
	$(LUABIND_DIR)/luabind/detail/pcall.hpp \
	$(LUABIND_DIR)/luabind/detail/pointee_sizeof.hpp \
	$(LUABIND_DIR)/luabind/detail/pointee_typeid.hpp \
	$(LUABIND_DIR)/luabind/detail/policy.hpp \
	$(LUABIND_DIR)/luabind/detail/primitives.hpp \
	$(LUABIND_DIR)/luabind/detail/property.hpp \
	$(LUABIND_DIR)/luabind/detail/ref.hpp \
	$(LUABIND_DIR)/luabind/detail/signature_match.hpp \
	$(LUABIND_DIR)/luabind/detail/stack_utils.hpp \
	$(LUABIND_DIR)/luabind/detail/typetraits.hpp \
	$(LUABIND_DIR)/luabind/detail/yes_no.hpp \
	$(LUABIND_DIR)/src/class.cpp \
	$(LUABIND_DIR)/src/class_info.cpp \
	$(LUABIND_DIR)/src/class_registry.cpp \
	$(LUABIND_DIR)/src/class_rep.cpp \
	$(LUABIND_DIR)/src/create_class.cpp \
	$(LUABIND_DIR)/src/error.cpp \
	$(LUABIND_DIR)/src/exception_handler.cpp \
	$(LUABIND_DIR)/src/function.cpp \
	$(LUABIND_DIR)/src/inheritance.cpp \
	$(LUABIND_DIR)/src/link_compatibility.cpp \
	$(LUABIND_DIR)/src/object_rep.cpp \
	$(LUABIND_DIR)/src/open.cpp \
	$(LUABIND_DIR)/src/pcall.cpp \
	$(LUABIND_DIR)/src/scope.cpp \
	$(LUABIND_DIR)/src/stack_content_by_name.cpp \
	$(LUABIND_DIR)/src/weak_ref.cpp \
	$(LUABIND_DIR)/src/wrapper_base.cpp

ENGINE_DIR = src/engine
engine_SOURCES = \
	$(ENGINE_DIR)/system.cpp \
	$(ENGINE_DIR)/system.h \
	$(ENGINE_DIR)/input.cpp \
	$(ENGINE_DIR)/input.h \
	$(ENGINE_DIR)/mode_manager.cpp \
	$(ENGINE_DIR)/mode_manager.h \
	$(ENGINE_DIR)/engine_bindings.cpp \
	$(audio_SOURCES) \
	$(video_SOURCES) \
	$(script_SOURCES)

GLOBAL_DIR = src/common/global
global_SOURCES = \
	$(GLOBAL_DIR)/global.cpp \
	$(GLOBAL_DIR)/global.h \
	$(GLOBAL_DIR)/global_actors.cpp \
	$(GLOBAL_DIR)/global_actors.h \
	$(GLOBAL_DIR)/global_effects.cpp \
	$(GLOBAL_DIR)/global_effects.h \
	$(GLOBAL_DIR)/global_objects.cpp \
	$(GLOBAL_DIR)/global_objects.h \
	$(GLOBAL_DIR)/global_skills.cpp \
	$(GLOBAL_DIR)/global_skills.h \
	$(GLOBAL_DIR)/global_utils.cpp \
	$(GLOBAL_DIR)/global_utils.h

GUI_DIR = src/common/gui
gui_SOURCES = \
	$(GUI_DIR)/gui.cpp \
	$(GUI_DIR)/gui.h \
	$(GUI_DIR)/menu_window.cpp \
	$(GUI_DIR)/menu_window.h \
	$(GUI_DIR)/option.cpp \
	$(GUI_DIR)/option.h \
	$(GUI_DIR)/textbox.cpp \
	$(GUI_DIR)/textbox.h

COMMON_DIR = src/common
common_SOURCES = \
	$(COMMON_DIR)/common.cpp \
	$(COMMON_DIR)/common.h \
	$(COMMON_DIR)/common_bindings.cpp \
	$(COMMON_DIR)/dialogue.cpp \
	$(COMMON_DIR)/dialogue.h \
	$(global_SOURCES) \
	$(gui_SOURCES)

MODES_DIR = src/modes
modes_SOURCES = \
	$(MODES_DIR)/mode_bindings.cpp \
	$(MODES_DIR)/battle/battle.cpp \
	$(MODES_DIR)/battle/battle.h \
	$(MODES_DIR)/battle/battle_actions.cpp \
	$(MODES_DIR)/battle/battle_actions.h \
	$(MODES_DIR)/battle/battle_actors.cpp \
	$(MODES_DIR)/battle/battle_actors.h \
	$(MODES_DIR)/battle/battle_command.cpp \
	$(MODES_DIR)/battle/battle_command.h \
	$(MODES_DIR)/battle/battle_dialogue.cpp \
	$(MODES_DIR)/battle/battle_dialogue.h \
	$(MODES_DIR)/battle/battle_effects.cpp \
	$(MODES_DIR)/battle/battle_effects.h \
	$(MODES_DIR)/battle/battle_finish.cpp \
	$(MODES_DIR)/battle/battle_finish.h \
	$(MODES_DIR)/battle/battle_indicators.cpp \
	$(MODES_DIR)/battle/battle_indicators.h \
	$(MODES_DIR)/battle/battle_sequence.cpp \
	$(MODES_DIR)/battle/battle_sequence.h \
	$(MODES_DIR)/battle/battle_utils.cpp \
	$(MODES_DIR)/battle/battle_utils.h \
	$(MODES_DIR)/boot/boot.cpp \
	$(MODES_DIR)/boot/boot.h \
	$(MODES_DIR)/boot/boot_menu.cpp \
	$(MODES_DIR)/boot/boot_menu.h \
	$(MODES_DIR)/boot/boot_credits.cpp \
	$(MODES_DIR)/boot/boot_credits.h \
	$(MODES_DIR)/boot/boot_welcome.cpp \
	$(MODES_DIR)/boot/boot_welcome.h \
	$(MODES_DIR)/map/map.cpp \
	$(MODES_DIR)/map/map.h \
	$(MODES_DIR)/map/map_dialogue.cpp \
	$(MODES_DIR)/map/map_dialogue.h \
	$(MODES_DIR)/map/map_events.cpp \
	$(MODES_DIR)/map/map_events.h \
	$(MODES_DIR)/map/map_objects.cpp \
	$(MODES_DIR)/map/map_objects.h \
	$(MODES_DIR)/map/map_sprites.cpp \
	$(MODES_DIR)/map/map_sprites.h \
	$(MODES_DIR)/map/map_tiles.cpp \
	$(MODES_DIR)/map/map_tiles.h \
	$(MODES_DIR)/map/map_treasure.cpp \
	$(MODES_DIR)/map/map_treasure.h \
	$(MODES_DIR)/map/map_utils.cpp \
	$(MODES_DIR)/map/map_utils.h \
	$(MODES_DIR)/map/map_zones.cpp \
	$(MODES_DIR)/map/map_zones.h \
	$(MODES_DIR)/menu/menu.cpp \
	$(MODES_DIR)/menu/menu.h \
	$(MODES_DIR)/menu/menu_views.cpp \
	$(MODES_DIR)/menu/menu_views.h \
	$(MODES_DIR)/pause.cpp \
	$(MODES_DIR)/pause.h \
	$(MODES_DIR)/save/save_mode.h \
	$(MODES_DIR)/save/save_mode.cpp \
	$(MODES_DIR)/scene.cpp \
	$(MODES_DIR)/scene.h \
	$(MODES_DIR)/shop/shop.h \
	$(MODES_DIR)/shop/shop.cpp \
	$(MODES_DIR)/shop/shop_buy.h \
	$(MODES_DIR)/shop/shop_buy.cpp \
	$(MODES_DIR)/shop/shop_confirm.h \
	$(MODES_DIR)/shop/shop_confirm.cpp \
	$(MODES_DIR)/shop/shop_leave.h \
	$(MODES_DIR)/shop/shop_leave.cpp \
	$(MODES_DIR)/shop/shop_root.h \
	$(MODES_DIR)/shop/shop_root.cpp \
	$(MODES_DIR)/shop/shop_sell.h \
	$(MODES_DIR)/shop/shop_sell.cpp \
	$(MODES_DIR)/shop/shop_trade.h \
	$(MODES_DIR)/shop/shop_trade.cpp \
	$(MODES_DIR)/shop/shop_utils.h \
	$(MODES_DIR)/shop/shop_utils.cpp

if COND_EDITOR
EDITOR_DIR = src/editor
EDITOR_BIN = allacrost-editor
AM_CXXFLAGS := $(CXXFLAGS) $(QT_CXXFLAGS) -DQT_CLEAN_NAMESPACE -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT3_SUPPORT -DQT_SHARED
BUILT_SOURCES = $(allacrost_editor_MOC)
CLEANFILES = $(BUILT_SOURCES) $(EDITOR_BIN)
$(EDITOR_DIR)%.moc.cpp: $(EDITOR_DIR)%.h
	$(MOC) -o $@ $<
allacrost_editor_MOC = $(EDITOR_DIR)/dialog_boxes.moc.cpp $(EDITOR_DIR)/editor.moc.cpp $(EDITOR_DIR)/grid.moc.cpp $(EDITOR_DIR)/skill_editor.moc.cpp $(EDITOR_DIR)/tileset_editor.moc.cpp
allacrost_editor_SOURCES = \
	$(EDITOR_DIR)/editor_main.cpp \
	$(EDITOR_DIR)/editor.cpp \
	$(EDITOR_DIR)/editor.h \
	$(EDITOR_DIR)/dialog_boxes.h \
	$(EDITOR_DIR)/dialog_boxes.cpp \
	$(EDITOR_DIR)/grid.cpp \
	$(EDITOR_DIR)/grid.h \
	$(EDITOR_DIR)/skill_editor.cpp \
	$(EDITOR_DIR)/skill_editor.h \
	$(EDITOR_DIR)/sprites.cpp \
	$(EDITOR_DIR)/sprites.h \
	$(EDITOR_DIR)/tileset.cpp \
	$(EDITOR_DIR)/tileset.h \
	$(EDITOR_DIR)/tileset_editor.cpp \
	$(EDITOR_DIR)/tileset_editor.h \
	$(video_SOURCES) \
	$(script_SOURCES) \
	src/defs.h \
	src/utils.cpp \
	src/utils.h \
	$(luabind_SOURCES) \
	$(common_SOURCES) \
	$(ENGINE_DIR)/mode_manager.cpp \
	$(ENGINE_DIR)/mode_manager.h \
	$(ENGINE_DIR)/system.cpp \
	$(ENGINE_DIR)/system.h
nodist_allacrost_editor_SOURCES = $(allacrost_editor_MOC)
allacrost_editor_LDADD = $(QT_GUILINK) $(QT_LDADD)
else
EDITOR_BIN =
endif

bin_PROGRAMS = allacrost $(EDITOR_BIN)

allacrost_SOURCES = \
	src/main.cpp \
	src/main_options.cpp \
	src/main_options.h \
	src/defs.h \
	src/utils.cpp \
	src/utils.h \
	$(luabind_SOURCES) \
	$(engine_SOURCES) \
	$(common_SOURCES) \
	$(modes_SOURCES)

dist-hook:
	rm -rf `find $(distdir) -name .svn`

bindir = ${prefix}/games
datarootdir = ${prefix}/share/games
datadirs = dat doc img mus snd txt
find_filter = -name .svn -prune -o -name ".\#*" -o -type d
find_datadirs = ( cd $(top_srcdir) && find $(datadirs) $(find_filter) -print )
find_data = ( cd $(top_srcdir) && find $(datadirs) $(find_filter) -o -print )

install-data-local:
	@$(NORMAL_INSTALL)
	test -z "$(datadir)" || $(mkdir_p) "$(DESTDIR)$(datadir)"
	( $(find_datadirs) ) | while read dir; do \
		$(mkdir_p) "$(DESTDIR)$(datadir)/allacrost/$$dir"; \
	done; \
	( $(find_data) ) | while read file; do \
		echo $(INSTALL_DATA) "$$file" "$(DESTDIR)$(datadir)/allacrost/$$file"; \
		$(INSTALL_DATA) "$$file" "$(DESTDIR)$(datadir)/allacrost/$$file"; \
	done

uninstall-local:
	@$(NORMAL_UNINSTALL)
	rm -r "$(DESTDIR)$(datadir)/allacrost"
