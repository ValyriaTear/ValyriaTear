# - Try to find ogg
# Once done this will define
#
# OGG_FOUND - system has ogg
# OGG_INCLUDE_DIR
# OGG_LIBRARY
#
# $OGGDIR is an environment variable used
# for finding ogg.
#
# Several changes and additions by Fabian 'x3n' Landau
# Most of all rewritten by Adrian Friedli
# Debug versions and simplifications by Reto Grieder
# > www.orxonox.net <

INCLUDE(FindPackageHandleStandardArgs)
INCLUDE(HandleLibraryTypes)

FIND_PATH(OGG_INCLUDE_DIR ogg/ogg.h
PATHS $ENV{OGGDIR}
PATH_SUFFIXES include
)
IF (WIN32)
FIND_LIBRARY(OGG_LIBRARY_OPTIMIZED
NAMES libogg libogg-static-mt
PATHS $ENV{OGGDIR}
PATH_SUFFIXES Release
)
ELSE()
FIND_LIBRARY(OGG_LIBRARY_OPTIMIZED
NAMES ogg
PATH_SUFFIXES lib
)
ENDIF(WIN32)


IF(WIN32)
FIND_LIBRARY(OGG_LIBRARY_DEBUG
NAMES liboggd libogg_d liboggD libogg_D libogg-static-mt-debug
PATHS $ENV{OGGDIR}
PATH_SUFFIXES Debug
)
ELSE()
FIND_LIBRARY(OGG_LIBRARY_DEBUG
NAMES ogg
PATH_SUFFIXES lib
)
ENDIF(WIN32)


# Handle the REQUIRED argument and set OGG_FOUND
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Ogg DEFAULT_MSG
OGG_LIBRARY_OPTIMIZED
OGG_INCLUDE_DIR
)

# Collect optimized and debug libraries
HANDLE_LIBRARY_TYPES(OGG)

MARK_AS_ADVANCED(
OGG_INCLUDE_DIR
OGG_LIBRARY_OPTIMIZED
OGG_LIBRARY_DEBUG
) 
