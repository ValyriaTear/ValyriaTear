CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
CMAKE_POLICY(VERSION 2.6)

PROJECT(VALYRIATEAR)

OPTION(DEBUG_FEATURES "Compile the game with the debug features" OFF)
OPTION(DISABLE_TRANSLATIONS "Disable gettext / l10n support" OFF)
OPTION(USE_PCH_COMPILATION "Using precompiled header for compilation for GCC" ON)

IF (NOT VERSION)
    SET(VERSION 0.1.0)
ENDIF()

STRING(REPLACE "." " " _VERSION ${VERSION})
SEPARATE_ARGUMENTS(_VERSION)
LIST(LENGTH _VERSION _LEN)
IF(NOT _LEN EQUAL 4 AND NOT _LEN EQUAL 3)
    MESSAGE(FATAL_ERROR "Version needs to be in the form MAJOR.MINOR.RELEASE[.BUILD]")
ENDIF()

LIST(GET _VERSION 0 VER_MAJOR)
LIST(GET _VERSION 1 VER_MINOR)
LIST(GET _VERSION 2 VER_RELEASE)
IF(_LEN EQUAL 4)
    LIST(GET _VERSION 3 VER_BUILD)
ELSE()
    SET(VER_BUILD 0)
ENDIF()

# Configuration of compilers
## Compiler flags to add
### Flags for GCC and Clang
set(GNU_FLAGS_WARNING_YES "-Wall -Wextra -Wformat=2 -Wpedantic -Wshadow -Wmissing-declarations -Wfloat-equal")
set(GNU_FLAGS_WARNING_NO  "-Wno-unused-command-line-argument -Wno-documentation -Wno-format-nonliteral")
set(GNU_FLAGS_WARNING     "${GNU_FLAGS_WARNING_YES} ${GNU_FLAGS_WARNING_NO}")
set(GNU_FLAGS_ERROR       "-pedantic-errors -Werror") # warning is error
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_C_COMPILER_ID STREQUAL "GCC")
  set(GNU_FLAGS_SUGGEST   "-Wsuggest-attribute=pure")
endif()
set(GNU_FLAGS_DEFAULT     "${GNU_FLAGS_WARNING} ${GNU_FLAGS_ERROR} ${GNU_FLAGS_SUGGEST}")
if (CMAKE_C_COMPILER_ID STREQUAL "GNU"   AND
    CMAKE_C_COMPILER_VERSION VERSION_GREATER 4.9)
  # A color output is easier to read.
  set(GNU_FLAGS_DEFAULT   "${GNU_FLAGS_DEFAULT} -fdiagnostics-color=auto")
endif()
set(GNU_FLAGS_OPTIMIZE    "-O2 -s")
set(GNU_FLAGS_SECURITY_PI "-fPIC -fPIE -pie")
if (( CMAKE_C_COMPILER_ID STREQUAL "GNU"   AND
      CMAKE_C_COMPILER_VERSION VERSION_LESS 4.9 ) OR
    ( CMAKE_C_COMPILER_ID STREQUAL "Clang" AND
      CMAKE_C_COMPILER_VERSION VERSION_LESS 3.5 ))
  set(GNU_FLAGS_SECURITY_STACK "-fstack-protector-all")
  # Protects a bit "too much", but better that nothing
else()
  set(GNU_FLAGS_SECURITY_STACK "-fstack-protector-strong")
endif()
set(GNU_FLAGS_SECURITY_COMPILE "-D_FORTIFY_SOURCE=2")
set(GNU_FLAGS_SECURITY_COMPILE
  "${GNU_FLAGS_SECURITY_COMPILE} ${GNU_FLAGS_SECURITY_PI}")
set(GNU_FLAGS_SECURITY_COMPILE
  "${GNU_FLAGS_SECURITY_COMPILE} ${GNU_FLAGS_SECURITY_STACK}")
set(GNU_FLAGS_SECURITY_LINK    "-Wl,-z,relro -Wl,-z,now")
if (WIN32)
  set(GNU_FLAGS_SECURITY_LINK  "${GNU_FLAGS_SECURITY_LINK} -Wl,dynamicbase") # ASLR
  set(GNU_FLAGS_SECURITY_LINK  "${GNU_FLAGS_SECURITY_LINK} -Wl,nxcompat")    # DEP
endif()
set(GNU_FLAGS_SECURITY  "${GNU_FLAGS_SECURITY_COMPILE} ${GNU_FLAGS_SECURITY_LINK}")
set(GNU_FLAGS_DEBUG     "-O0 -g -D_FORTIFY_SOURCE=0")
set(GNU_C_FLAGS_RELEASE "${GNU_FLAGS_OPTIMIZE} ${GNU_FLAGS_SECURITY}")
set(GNU_C_FLAGS_DEBUG   "${GNU_FLAGS_DEBUG}")
set(GNU_C_FLAGS         "${GNU_FLAGS_DEFAULT}")
if (CMAKE_BUILD_TYPE MATCHES "RELEASE" OR
    CMAKE_BUILD_TYPE MATCHES "release" OR
    CMAKE_BUILD_TYPE MATCHES "Release")
  set(GNU_C_FLAGS       "${GNU_C_FLAGS} ${GNU_C_FLAGS_RELEASE}")
endif()
### Flags for LLVM/Clang
set(LLVM_FLAGS_WARNING_YES
  "-Weverything")
set(LLVM_FLAGS_WARNING_NO
  "-Wno-disabled-macro-expansion -Wno-padded -Wno-bad-function-cast -Wno-unreachable-code -Wno-missing-prototypes")
set(LLVM_FLAGS_WARNING
  "${LLVM_FLAGS_WARNING_YES} ${LLVM_FLAGS_WARNING_NO}")
set(LLVM_FLAGS_SANITIZE
  "-fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=null -fsanitize=object-size -fsanitize=shift")
set(LLVM_FLAGS_DEFAULT "${LLVM_FLAGS_WARNING} ${LLVM_FLAGS_SANITIZE}")
## Add C compiler options
if (CMAKE_C_COMPILER_ID STREQUAL "GNU"   OR
    CMAKE_C_COMPILER_ID STREQUAL "GCC"   OR
    CMAKE_C_COMPILER_ID STREQUAL "Clang" OR
    CMAKE_C_COMPILER_ID STREQUAL "LLVM")
  set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS}       ${GNU_C_FLAGS}")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${GNU_C_FLAGS_DEBUG}")
endif()
if (CMAKE_C_COMPILER_ID STREQUAL "Clang" OR
    CMAKE_C_COMPILER_ID STREQUAL "LLVM")
  set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS}       ${LLVM_FLAGS_DEFAULT}")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${LLVM_FLAGS_DEFAULT}")
endif()

# where to look for cmake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

SET(PKG_NAME "valyriatear")
IF (WIN32)
    SET(PKG_DATADIR ".")
    SET(PKG_BINDIR ".")
    SET(LOCALEDIR ".")
    CONFIGURE_FILE(src/winver.h.in src/winver.h)
ELSE (WIN32)
    IF (NOT OSX AND NOT BEOS)
        OPTION(USE_X11 "Use X11 Clipboard functionality" ON)
    ENDIF ()
    SET(PKG_DATADIR ${CMAKE_INSTALL_PREFIX}/share/valyriatear CACHE PATH "Data dir")
    SET(LOCALEDIR ${CMAKE_INSTALL_PREFIX}/share/locale CACHE PATH "Locale dir")
    SET(PKG_BINDIR ${CMAKE_INSTALL_PREFIX}/bin CACHE PATH "Binary dir")
ENDIF (WIN32)

# The sub-folders to parse
ADD_SUBDIRECTORY(src)

# Add data packages
IF(NOT DISABLE_TRANSLATIONS)
    FIND_PACKAGE(Gettext)
    ADD_SUBDIRECTORY(po)
ENDIF(NOT DISABLE_TRANSLATIONS)

# CPack installation part
If(UNIX)
    # Shortcut desktop file
    INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/valyriatear.desktop"
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
    # data files
    INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data"
            DESTINATION ${PKG_DATADIR} FILES_MATCHING
            PATTERN "*.lua"
            PATTERN "*.png"
            PATTERN "*.ttf"
            PATTERN "*.wav"
            PATTERN "*.ogg")
    # icon file
    INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/data/icons/program_icon_48x48.png"
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/apps RENAME valyriatear.png)
    # KDE/Gnome app center app data
    INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/valyriatear.appdata.xml"
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/appdata)
ENDIF()

SET(CPACK_PACKAGE_NAME "valyriatear")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Valyria Tear")
SET(CPACK_PACKAGE_VENDOR "Bertram")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.GPL-2" )
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "ValyriaTear")
SET(CPACK_PACKAGE_VERSION_MAJOR ${VER_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VER_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VER_RELEASE})
IF(WIN32)
    SET(CPACK_NSIS_DISPLAY_NAME "Valyria Tear")
    SET(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/data/icons/program_icon.ico")
    SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/data/icons/program_icon.png") #TODO: use a bmp
    SET(CPACK_NSIS_URL_INFO_ABOUT "http://valyriatear.blogspot.com")
ENDIF()
INCLUDE(CPack)

# Uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
