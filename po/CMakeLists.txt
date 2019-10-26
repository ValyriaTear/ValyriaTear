FIND_PACKAGE(Gettext REQUIRED)

# Set the pot files suffix name
SET(_fileBasename "valyriatear")
SET(_potFileSuffix "pot")
SET(_poFileSuffix "po")

# Setup filenames
SET(_srcPotFile src-${_fileBasename}.${_potFileSuffix})
SET(_inventoryPotFile inventory-${_fileBasename}.${_potFileSuffix})
SET(_skillsPotFile skills-${_fileBasename}.${_potFileSuffix})
SET(_configPotFile config-${_fileBasename}.${_potFileSuffix})
SET(_entitiesPotFile entities-${_fileBasename}.${_potFileSuffix})
SET(_ep1PotFile ep1-${_fileBasename}.${_potFileSuffix})
SET(_ep2PotFile ep2-${_fileBasename}.${_potFileSuffix})

SET(_potFiles ${_srcPotFile} ${_inventoryPotFile} ${_skillsPotFile} ${_configPotFile} ${_entitiesPotFile} ${_ep1PotFile} ${_ep2PotFile})

SET(_srcPoFile src-${_fileBasename}.${_poFileSuffix})
SET(_inventoryPoFile inventory-${_fileBasename}.${_poFileSuffix})
SET(_skillsPoFile skills-${_fileBasename}.${_poFileSuffix})
SET(_configPoFile config-${_fileBasename}.${_poFileSuffix})
SET(_entitiesPoFile entities-${_fileBasename}.${_poFileSuffix})
SET(_ep1PoFile ep1-${_fileBasename}.${_poFileSuffix})
SET(_ep2PoFile ep2-${_fileBasename}.${_poFileSuffix})

SET(_poFiles ${_srcPoFile} ${_inventoryPoFile} ${_skillsPoFile} ${_configPoFile} ${_entitiesPoFile} ${_ep1PoFile} ${_ep2PoFile})

# Creates a single mo file from all the language po files and install it
MACRO(VALYRIATEAR_GETTEXT_INSTALL_TRANSLATIONS _languages)
    FOREACH (_lang ${languages})
        # Prepare output directories for the translation binaries (mo files)
        SET(_moDir ${CMAKE_CURRENT_BINARY_DIR}/../data/locale/${_lang}/LC_MESSAGES/)
        ADD_CUSTOM_COMMAND(
        OUTPUT ${_moDir}
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${_moDir}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${_moDir}
        )

        # Generate the translation binaries (mo files)
        SET(_moFile ${_moDir}/valyriatear.mo)
        SET(_singlePoFile ${CMAKE_CURRENT_SOURCE_DIR}/${_lang}.po)

        # Generate the po file list dependency
        SET(_langPoFiles)
        FOREACH(_langPoFile ${_poFiles})
            SET(_langPoFiles ${_langPoFiles} "${CMAKE_CURRENT_SOURCE_DIR}/${_lang}/${_langPoFile}")
        ENDFOREACH()

        ADD_CUSTOM_COMMAND(
            OUTPUT ${_moFile}
            # Concatenate the client po files into a single one
            COMMAND msgcat --output-file=${_singlePoFile} ${_langPoFiles}
            # Generate the mo file
            COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_moFile} ${_singlePoFile}
            # Delete the temp po file
            COMMAND ${CMAKE_COMMAND} -E remove -f ${_singlePoFile}
            DEPENDS ${_langPoFiles} ${_moDir}
        )

        # Install the mo files depending on the OS.
        IF (UNIX)
            # In the linux localedir, e.g.: /usr/share/locale/<lang>/LC_MESSAGES/valyriatear.mo
            INSTALL(FILES ${_moFile} DESTINATION ${LOCALEDIR}/${_lang}/LC_MESSAGES)
        ELSE()
            # In the locale/ data folder
            INSTALL(FILES ${_moFile} DESTINATION ${PKG_DATADIR}/locale/${_lang}/LC_MESSAGES)
        ENDIF()

        SET(_moFiles ${_moFiles} ${_moFile})
    ENDFOREACH()

    ADD_CUSTOM_TARGET(translations DEPENDS ${_moFiles})
    ADD_DEPENDENCIES(valyriatear translations)
ENDMACRO()

# Update every po files according to the corresponding pot file for each language
MACRO(VALYRIATEAR_GETTEXT_UPDATE_PO _languages)

    ADD_CUSTOM_TARGET(
        update-pot
        # Strings from C++ and Lua
        COMMAND xgettext -C --files-from=src-translatable-files --directory=. --output=${_srcPotFile} -d valyriatear --keyword=_ --keyword=N_ --keyword=Translate --keyword=UTranslate --keyword=CTranslate --keyword=CUTranslate --keyword=VTranslate:1 --keyword=NVTranslate:1,2 --add-comments=tr: --from-code=UTF-8
        COMMAND xgettext -C --files-from=inventory-translatable-files --directory=. --output=${_inventoryPotFile} -d valyriatear --keyword=_ --keyword=N_ --keyword=Translate --keyword=UTranslate --keyword=CTranslate --keyword=CUTranslate --keyword=VTranslate:1 --keyword=NVTranslate:1,2 --add-comments=tr: --from-code=UTF-8
        COMMAND xgettext -C --files-from=skills-translatable-files --directory=. --output=${_skillsPotFile} -d valyriatear --keyword=_ --keyword=N_ --keyword=Translate --keyword=UTranslate --keyword=CTranslate --keyword=CUTranslate --keyword=VTranslate:1 --keyword=NVTranslate:1,2 --add-comments=tr: --from-code=UTF-8
        COMMAND xgettext -C --files-from=config-translatable-files --directory=. --output=${_configPotFile} -d valyriatear --keyword=_ --keyword=N_ --keyword=Translate --keyword=UTranslate --keyword=CTranslate --keyword=CUTranslate --keyword=VTranslate:1 --keyword=NVTranslate:1,2 --add-comments=tr: --from-code=UTF-8
        COMMAND xgettext -C --files-from=entities-translatable-files --directory=. --output=${_entitiesPotFile} -d valyriatear --keyword=_ --keyword=N_ --keyword=Translate --keyword=UTranslate --keyword=CTranslate --keyword=CUTranslate --keyword=VTranslate:1 --keyword=NVTranslate:1,2 --add-comments=tr: --from-code=UTF-8
        # Story pot files
        COMMAND xgettext -C --files-from=ep1-translatable-files --directory=. --output=${_ep1PotFile} -d valyriatear --keyword=_ --keyword=N_ --keyword=Translate --keyword=UTranslate --keyword=CTranslate --keyword=CUTranslate --keyword=VTranslate:1 --keyword=NVTranslate:1,2 --add-comments=tr: --from-code=UTF-8
        COMMAND xgettext -C --force-po --files-from=ep2-translatable-files --directory=. --output=${_ep2PotFile} -d valyriatear --keyword=_ --keyword=N_ --keyword=Translate --keyword=UTranslate --keyword=CTranslate --keyword=CUTranslate --keyword=VTranslate:1 --keyword=NVTranslate:1,2 --add-comments=tr: --from-code=UTF-8

        ## Map names
        # Episode 1 -> output ep2-map-files-tr.lua
        COMMAND bash create_map_names_translatable_strings.sh ep1-map-files
        COMMAND xgettext -C --directory=. -j --output=${_ep1PotFile} -d valyriatear --keyword=Translate --add-comments=tr: --from-code=UTF-8  ep1-map-files-tr.lua
        # Episode 2 -> output ep2-map-files-tr.lua
        COMMAND bash create_map_names_translatable_strings.sh ep2-map-files
        COMMAND xgettext -C --directory=. -j --output=${_ep2PotFile} -d valyriatear --keyword=Translate --add-comments=tr: --from-code=UTF-8  ep2-map-files-tr.lua

        # The pot files are ready - We can remove the temp files
        COMMAND rm -f src-translatable-files inventory-translatable-files skills-translatable-files
        COMMAND rm -f config-translatable-files entities-translatable-files
        COMMAND rm -f ep1-translatable-files ep2-translatable-files
        COMMAND rm -f ep1-map-files ep2-map-files ep1-map-files-tr.lua ep2-map-files-tr.lua
        COMMAND rm -f map_names_tr.lua
        # Let's also remove the mo files as they are not obsolete, this will trigger their refresh
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/../data/locale
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    ADD_CUSTOM_TARGET(
        update-translatable-files
        # Source code
        COMMAND grep '_\(' -Irl ../src --include=*.cpp | sort > src-translatable-files
        COMMAND grep 'Translate\(' -Irl ../src --include=*.cpp | sort >> src-translatable-files
        COMMAND grep 'UTranslate\(' -Irl ../src --include=*.cpp | sort >> src-translatable-files
        COMMAND grep 'CTranslate\(' -Irl ../src --include=*.cpp | sort >> src-translatable-files
        COMMAND grep 'CUTranslate\(' -Irl ../src --include=*.cpp | sort >> src-translatable-files
        COMMAND grep 'NVTranslate\(' -Irl ../src --include=*.cpp | sort >> src-translatable-files
        # Inventor
        COMMAND grep 'Translate\(' -Irl ../data/inventory | sort > inventory-translatable-files
        # Skills
        COMMAND grep 'Translate\(' -Irl ../data/skills | sort > skills-translatable-files
        # Config
        COMMAND grep 'Translate\(' -Irl ../data/config | sort > config-translatable-files
        # Entities
        COMMAND grep 'Translate\(' -Irl ../data/entities | sort > entities-translatable-files
        ## Story files
        # Episode I
        COMMAND grep 'Translate\(' -Irl ../data/story/ep1 | sort > ep1-translatable-files
        COMMAND grep 'Translate\(' -Irl ../data/boot_menu/ep1 | sort >> ep1-translatable-files
        COMMAND grep 'map_name =' -Irl ../data/story/ep1 --include=*.lua | sort >> ep1-map-files
        COMMAND grep 'map_subname =' -Irl  ../data/story/ep1 --include=*.lua | sort >> ep1-map-files
        # Episode II
        COMMAND grep 'Translate\(' -Irl ../data/story/ep2 | sort > ep2-translatable-files
        # COMMAND grep 'Translate\(' -Irl ../data/boot_menu/ep2 | sort >> ep2-data-translatable-files
        COMMAND grep 'map_name =' -Irl ../data/story/ep2 --include=*.lua | sort >> ep2-map-files
        COMMAND grep 'map_subname =' -Irl  ../data/story/ep2 --include=*.lua | sort >> ep2-map-files

        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    ADD_DEPENDENCIES(update-pot update-translatable-files)

    FOREACH(_lang ${_languages})
        ADD_CUSTOM_TARGET("update-po-${_lang}")
        FOREACH(_potFile ${_potFiles})
            # Get the corresponding po file for the given pot file
            GET_FILENAME_COMPONENT(_currentPo ${_potFile} NAME_WE)
            GET_FILENAME_COMPONENT(_absPoFile "${_lang}/${_currentPo}.po" ABSOLUTE)
            GET_FILENAME_COMPONENT(_absPotFile "${_potFile}" ABSOLUTE)

            ADD_CUSTOM_TARGET(
                "update-po-${_lang}-${_potFile}"
                COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --quiet --update --backup=none -s ${_absPoFile} ${_absPotFile}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
            ADD_DEPENDENCIES(update-po-${_lang} "update-po-${_lang}-${_potFile}")
        ENDFOREACH()
        ADD_DEPENDENCIES(update-po "update-po-${_lang}")
    ENDFOREACH()
ENDMACRO()

# Iterate locales
SET(languages)
FILE(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/LINGUAS" linguas)
FOREACH(_lang ${linguas})
    IF(NOT ${_lang} STREQUAL "")
        SET(languages ${languages} ${_lang})
    ENDIF()
ENDFOREACH()

ADD_CUSTOM_TARGET(update-po)

VALYRIATEAR_GETTEXT_UPDATE_PO("${languages}")
VALYRIATEAR_GETTEXT_INSTALL_TRANSLATIONS("${languages}")
