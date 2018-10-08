FIND_PACKAGE(Gettext REQUIRED)

# based on the CMake 2.6.3 GETTEXT_CREATE_TRANSLATIONS
# we need our own version because the upstream one contains
# a call to msgmerge (updating po files) which can't be
# disabled
MACRO(VALYRIATEAR_GETTEXT_CREATE_TRANSLATIONS _potFile _firstPoFileArg)
   # make it a real variable, so we can modify it here
   SET(_firstPoFile "${_firstPoFileArg}")

   SET(_moFiles)
   GET_FILENAME_COMPONENT(_potBasename ${_potFile} NAME_WE)
   GET_FILENAME_COMPONENT(_absPotFile ${_potFile} ABSOLUTE)

   SET(_addToAll)
   IF(${_firstPoFile} STREQUAL "ALL")
      SET(_addToAll "ALL")
      SET(_firstPoFile)
   ENDIF(${_firstPoFile} STREQUAL "ALL")

   FOREACH (_currentPoFile ${_firstPoFile} ${ARGN})
      GET_FILENAME_COMPONENT(_absFile ${_currentPoFile} ABSOLUTE)
      GET_FILENAME_COMPONENT(_abs_PATH ${_absFile} PATH)
      GET_FILENAME_COMPONENT(_lang ${_absFile} NAME_WE)

      # Prepare output directories for the translation binaries (mo files)
      SET(_moDir ${CMAKE_CURRENT_BINARY_DIR}/../data/locale/${_lang}/LC_MESSAGES/)
      ADD_CUSTOM_COMMAND(
         OUTPUT ${_moDir}
         COMMAND ${CMAKE_COMMAND} -E remove_directory ${_moDir}
         COMMAND ${CMAKE_COMMAND} -E make_directory ${_moDir}
      )

      # Generate the translation binaries (mo files)
      SET(_moFile ${_moDir}/valyriatear.mo)

      ADD_CUSTOM_COMMAND(
         OUTPUT ${_moFile}
         COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_moFile} ${_absFile}
         DEPENDS ${_absPotFile} ${_absFile} ${_moDir}
      )

      # Install the mo files depending on the OS.
      IF (UNIX)
          # In the linux localedir, e.g.: /usr/share/locale/<lang>/LC_MESSAGES/valyriatear.mo
          INSTALL(FILES ${_moFile} DESTINATION ${LOCALEDIR}/${_lang}/LC_MESSAGES RENAME ${_potBasename}.mo)
      ELSE()
          # In the locale/ data folder
          INSTALL(FILES ${_moFile} DESTINATION ${PKG_DATADIR}/locale/${_lang}/LC_MESSAGES RENAME ${_potBasename}.mo)
      ENDIF()

      SET(_moFiles ${_moFiles} ${_moFile})

   ENDFOREACH (_currentPoFile)

   ADD_CUSTOM_TARGET(translations ${_addToAll} DEPENDS ${_moFiles})
ENDMACRO()

MACRO(VALYRIATEAR_GETTEXT_UPDATE_PO _potFile _languages)
   GET_FILENAME_COMPONENT(_absPotFile ${_potFile} ABSOLUTE)

   ADD_CUSTOM_TARGET(
      update-pot
      # Strings from C++ and Lua
      COMMAND xgettext -C --files-from=translatable-files --directory=. --output=${_potFile} -d valyriatear --keyword=_ --keyword=N_ --keyword=Translate --keyword=UTranslate --keyword=CTranslate --keyword=CUTranslate --keyword=VTranslate:1 --keyword=NVTranslate:1,2 --add-comments=tr: --from-code=UTF-8
      # Map names from Lua
      COMMAND bash create_map_names_translatable_strings.sh
      COMMAND xgettext -C --directory=. -j --output=${_potFile} -d valyriatear --keyword=Translate --add-comments=tr: --from-code=UTF-8  map_names_tr.lua

      # The pot file is ready - We can remove the temp files
      COMMAND rm -f translatable-files
      COMMAND rm -f map_names_files
      COMMAND rm -f map_names_tr.lua
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      )

   ADD_CUSTOM_TARGET(
      update-translatable-files
      COMMAND grep '_\(' -Irl ../src --include=*.cpp | sort > translatable-files
      COMMAND grep 'Translate\(' -Irl ../src --include=*.cpp | sort >> translatable-files
      COMMAND grep 'UTranslate\(' -Irl ../src --include=*.cpp | sort >> translatable-files
      COMMAND grep 'CTranslate\(' -Irl ../src --include=*.cpp | sort >> translatable-files
      COMMAND grep 'CUTranslate\(' -Irl ../src --include=*.cpp | sort >> translatable-files
      COMMAND grep 'Translate\(' -Irl ../data | sort >> translatable-files
      # Add map names and subnames to the translatable strings
      COMMAND grep 'map_name =' -Irl ../data/story --include=*.lua | sort > map_names_files
      COMMAND grep 'map_subname =' -Irl  ../data/story --include=*.lua | sort >> map_names_files
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      )

   FOREACH(_lang ${_languages})
      GET_FILENAME_COMPONENT(_absFile "${_lang}.po" ABSOLUTE)
      SET(_currentPoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.po)
      ADD_CUSTOM_TARGET(
         "update-translation-${_lang}"
         COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --quiet --update --backup=none -s ${_absFile} ${_absPotFile}
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         )
      ADD_DEPENDENCIES("update-translation-${_lang}" update-pot)
      ADD_DEPENDENCIES(update-po "update-translation-${_lang}")
      ADD_DEPENDENCIES(update-pot update-translatable-files)
   ENDFOREACH()
ENDMACRO()

# Iterate locales
SET(languages)
SET(POFILES)
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
foreach(file ${files})
   get_filename_component(_lang ${file} NAME_WE)
   IF(NOT ${_lang} STREQUAL "")
      SET(languages ${languages} ${_lang})
      SET(POFILES ${POFILES} "${CMAKE_CURRENT_SOURCE_DIR}/${_lang}.po")
   ENDIF()
endforeach()

ADD_CUSTOM_TARGET(update-po)

VALYRIATEAR_GETTEXT_CREATE_TRANSLATIONS(valyriatear.pot ALL ${POFILES})
VALYRIATEAR_GETTEXT_UPDATE_PO(valyriatear.pot "${languages}")
