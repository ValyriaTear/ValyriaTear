dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(Hero of Allacrost, 0.1.0, team@allacrost.org, allacrost)
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl Check for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_GNU_SOURCE
CXXFLAGS="-Wall"

dnl Check for compiler optimization option.
AC_MSG_CHECKING(whether to enable -O3 compiler optimization)
optimize_default="yes"
AC_ARG_ENABLE(optimize, [  --enable-optimize=[no/yes]  will build with -O3 compiler optimization [default=yes]], , enable_optimize=$optimize_default)
if test "x$enable_optimize" = "xyes"; then
    AC_MSG_RESULT(yes)
    CXXFLAGS="$CXXFLAGS -O3"
else
    AC_MSG_RESULT(no)
    CXXFLAGS="$CXXFLAGS -O0"
fi

dnl Check for debugging option.
AC_MSG_CHECKING(whether to enable debugging)
debug_default="no"
AC_ARG_ENABLE(debug, [  --enable-debug=[no/yes]     will build with debugging capability [default=no]], , enable_debug=$debug_default)
if test "x$enable_debug" = "xyes"; then
    AC_MSG_RESULT(yes)
    CXXFLAGS="$CXXFLAGS -g -pg"
else
    AC_MSG_RESULT(no)
fi

dnl Check for libraries.
AC_PATH_XTRA
X_LIBS="$X_LIBS -L/usr/X11R6/lib"
X_CFLAGS="$X_CFLAGS -I/usr/X11R6/include"
LIBS="$LIBS $X_LIBS"
CPPFLAGS="$CPPFLAGS $X_CFLAGS"

dnl Check for map editor and Qt.
AC_MSG_CHECKING(whether to enable usage of map editor)
editor_default="yes"
AC_ARG_ENABLE(editor, [  --enable-editor=[no/yes]    will build the map editor [default=yes]], , enable_editor=$editor_default)
if test "x$enable_editor" = "xyes"; then
    AC_MSG_RESULT(yes)
    HOA_FIND_QT4(Qt4,,)
    HOA_FIND_QT4(QtOpenGL,,)
else
    AC_MSG_RESULT(no)
fi
AM_CONDITIONAL([COND_EDITOR], [test "$enable_editor" = yes])

AC_CHECK_LIB([X11], [XCreateWindow], [], [AC_MSG_ERROR([X11 GLX not found])])
AC_CHECK_LIB([GL], [glGetString], [], [AC_MSG_ERROR([OpenGL not found])])
AC_CHECK_LIB([GLU], [gluGetString], [], [AC_MSG_ERROR([GLU not found])])

AC_CHECK_LIB([png], [png_read_info], [], \
	[echo "Could not find the png library. Check that it is properly installed on your system"
	 exit -1])
AC_CHECK_LIB([jpeg], [jpeg_read_raw_data], [], \
	[echo "Could not find the jpeg library. Check that it is properly installed on your system"
	 exit -1])
AC_CHECK_LIB([SDL], [SDL_InitSubSystem], [], \
	[echo "Could not find the SDL library. Check that it is properly installed on your system"
	 exit -1])
AC_CHECK_LIB([openal], [alGetError], [], \
	[echo "You need to install the OpenAL library."
	 exit -1])
AC_CHECK_LIB([vorbisfile], [ov_open], [], \
	[echo "Could not find the vorbis library. Check that it is properly installed on your system"
	 exit -1])
AC_CHECK_LIB([SDL_ttf], [TTF_Init], [], \
	[echo "Could not find the SDL_ttf library. Check that it is properly installed on your system"
	 exit -1])
AC_CHECK_LIB([lua5.1], [lua_newstate], [], \
	[AC_CHECK_LIB([lua51], [lua_newstate], [], \
	[AC_CHECK_LIB([lua], [lua_newstate], [], \
 		[echo "Could not find the lua 5.1 library. Check that it is properly installed on your system"
		 exit -1])])])
AC_CHECK_LIB([m], [log], [], \
	[echo "Could not find the math library. Check that it is properly installed on your system"
	 exit -1])

AM_GNU_GETTEXT([external])
AC_SUBST([PACKAGE])
LOCALEDIR=txt
AC_SUBST([LOCALEDIR])

dnl Check for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

dnl Check for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_C_VOLATILE

dnl Check for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([atexit chdir mkdir rmdir sqrt strcasecmp])

AC_CONFIG_FILES([Makefile txt/Makefile.in])

AC_OUTPUT

echo ""
echo "Now type 'make'."
echo ""
