FIND_PACKAGE(Gettext REQUIRED)

# based on the CMake 2.6.3 GETTEXT_CREATE_TRANSLATIONS
# we need our own version because the upstream one contains
# a call to msgmerge (updating po files) which can't be
# disabled
MACRO(VALYRIATEAR_GETTEXT_CREATE_TRANSLATIONS _potFile _firstPoFileArg)
   # make it a real variable, so we can modify it here
   SET(_firstPoFile "${_firstPoFileArg}")

   SET(_gmoFiles)
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
      SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)
      SET(_moFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}/LC_MESSAGES/valyriatear.mo)

      ADD_CUSTOM_COMMAND(
         OUTPUT ${_gmoFile}
         COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_absFile}
         DEPENDS ${_absPotFile} ${_absFile}
      )

      # Create the LC_MESSAGES folders locally to easily test the mo files.
      ADD_CUSTOM_COMMAND(
          OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_lang}/LC_MESSAGES/valyriatear.mo
          COMMAND rm -rf ${CMAKE_CURRENT_BINARY_DIR}/${_lang}
          COMMAND mkdir ${CMAKE_CURRENT_BINARY_DIR}/${_lang}
          COMMAND mkdir ${CMAKE_CURRENT_BINARY_DIR}/${_lang}/LC_MESSAGES
          COMMAND cp ${_gmoFile} ${CMAKE_CURRENT_BINARY_DIR}/${_lang}/LC_MESSAGES/valyriatear.mo
          DEPENDS ${_gmoFile}
      )

      INSTALL(FILES ${_gmoFile} DESTINATION share/locale/${_lang}/LC_MESSAGES RENAME ${_potBasename}.mo)
      SET(_gmoFiles ${_gmoFiles} ${_gmoFile})
      SET(_moFiles ${_moFiles} ${_moFile})

   ENDFOREACH (_currentPoFile)

   ADD_CUSTOM_TARGET(translations ${_addToAll} DEPENDS ${_gmoFiles} ${_moFiles})
ENDMACRO()

MACRO(VALYRIATEAR_GETTEXT_UPDATE_PO _potFile _languages)
   GET_FILENAME_COMPONENT(_absPotFile ${_potFile} ABSOLUTE)

   ADD_CUSTOM_TARGET(
      update-pot
      COMMAND xgettext --files-from=translatable-files --directory=. --output=${_potFile} -d valyriatear --keyword=_ --keyword=N_ --keyword=Translate --from-code=UTF-8
      )

   ADD_CUSTOM_TARGET(
      update-translatable-files
      COMMAND grep '_\(' -Irl ../src --include=*.cpp | sort > translatable-files
      COMMAND grep 'Translate\(' -Irl ../dat | sort >> translatable-files
      )

   FOREACH(_lang ${_languages})
      GET_FILENAME_COMPONENT(_absFile "${_lang}.po" ABSOLUTE)
      SET(_currentPoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.po)
      ADD_CUSTOM_TARGET(
         "update-translation-${_lang}"
         COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --quiet --update --backup=none -s ${_absFile} ${_absPotFile}
         )
      ADD_DEPENDENCIES("update-translation-${_lang}" update-pot)
      ADD_DEPENDENCIES(update-po "update-translation-${_lang}")
      ADD_DEPENDENCIES(update-pot update-translatable-files)
   ENDFOREACH()
ENDMACRO()

SET(languages)
SET(POFILES)
FILE(STRINGS LINGUAS _languages)
FOREACH(_lang ${_languages})
   STRING(REGEX REPLACE "#.*" "" _lang "${_lang}")
   IF(NOT ${_lang} STREQUAL "")
      SET(languages ${languages} ${_lang})
      SET(POFILES ${POFILES} "${_lang}.po")
   ENDIF()
ENDFOREACH()

ADD_CUSTOM_TARGET(update-po)

VALYRIATEAR_GETTEXT_CREATE_TRANSLATIONS(valyriatear.pot ALL ${POFILES})
VALYRIATEAR_GETTEXT_UPDATE_PO(valyriatear.pot "${languages}")
