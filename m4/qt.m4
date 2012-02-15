# HOA_FIND_QT(VARIABLE-PREFIX, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# Check whether the Qt libraries are available.  Adapted from Poppler qt.m4 which was adapted from
# OpenOffice.org configure.in
#
# --------------------------------------------------------------
AC_DEFUN([HOA_FIND_QT],
[
dnl Search paths for Qt 
qt_incdirs="$QTINC /usr/local/qt/include /usr/include/qt /usr/include /usr/X11R6/include/X11/qt /usr/X11R6/include/qt /usr/lib/qt3/include /usr/lib/qt/include /usr/share/qt3/include"
qt_libdirs="$QTLIB /usr/local/qt/lib /usr/lib/qt /usr/lib /usr/X11R6/lib/X11/qt /usr/X11R6/lib/qt /usr/lib/qt3/lib /usr/lib/qt/lib /usr/share/qt3/lib"
if test -n "$QTDIR" ; then
    qt_incdirs="$QTDIR/include $qt_incdirs"
    qt_libdirs="$QTDIR/lib $qt_libdirs"
fi

dnl What to test
qt_test_include="qstyle.h"
qt_test_la_library="libqt-mt.la"
qt_test_library="libqt-mt.so"

dnl Check for Qt headers
AC_MSG_CHECKING([for Qt headers])
qt_incdir="no"
for it in $qt_incdirs ; do
    if test -r "$it/$qt_test_include" ; then
        qt_incdir="$it"
        break
    fi
done
AC_MSG_RESULT([$qt_incdir])

dnl Check for Qt libraries
AC_MSG_CHECKING([for Qt libraries])
qt_libdir="no"
for qt_check in $qt_libdirs ; do
    if test -r "$qt_check/$qt_test_la_library" ; then
        qt_libdir="$qt_check"
        break
    fi

    if test -r "$qt_check/$qt_test_library" ; then
        qt_libdir="$qt_check"
        break
    fi
done
AC_MSG_RESULT([$qt_libdir])

if test "x$qt_libdir" != "xno" ; then
    if test "x$qt_incdir" != "xno" ; then
        have_qt=yes
    fi
fi

if test "x$have_qt" == "xyes"; then
    AC_LANG_PUSH([C++])
    pthread_needed=no

    save_LDFLAGS=$LDFLAGS
    save_CXXFLAGS=$CXXFLAGS
    save_LIBS=$LIBS
    CXXFLAGS="$CXXFLAGS -I$qt_incdir"
    LIBS="$LIBS $qt_libdir/$qt_test_library"
    AC_MSG_CHECKING([if Qt needs -pthread])
    AC_TRY_LINK([#include <qt.h>], [QString s;], [pthread_needed=no], [pthread_needed=yes])
    if test "x$pthread_needed" = "xyes"; then
        LDFLAGS="$LDFLAGS -pthread"
        AC_TRY_LINK([#include <qt.h>], [QString s;], [pthread_needed=yes], [pthread_needed=no])
    fi
    AC_MSG_RESULT([$pthread_needed])
    LDFLAGS=$save_LDFLAGS
    CXXFLAGS=$save_CXXFLAGS
    LIBS=$save_LIBS

    AC_LANG_POP

    qtpthread=''
    if test "x$pthread_needed" = "xyes"; then
        qtpthread="-pthread"
    fi

    $1[]_CXXFLAGS="-I$qt_incdir"
    $1[]_LIBS="$qtpthread $qt_libdir/$qt_test_library"
    ifelse([$2], , :, [$2])
else
    ifelse([$3], , [AC_MSG_FAILURE(dnl
[Qt development libraries not found])],
  	   [$3])
fi
])


# HOA_FIND_QT4(VARIABLE-PREFIX, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# Check whether the Qt4 libraries are available. Adapted from Poppler qt.m4.
#
# --------------------------------------------------------------
AC_DEFUN([HOA_FIND_QT4],
[
have_qt4=no
dnl Search paths for Qt4 - not much real experience with this yet.
qt4_incdirs="/usr/local/qt/include /usr/include/qt4 /usr/include/qt /usr/include /usr/X11R6/include/X11/qt /usr/X11R6/include/qt /usr/lib/qt/include /usr/lib/qt4/include"
qt4_libdirs="/usr/local/qt/lib /usr/lib/qt /usr/lib /usr/X11R6/lib/X11/qt /usr/X11R6/lib/qt /usr/lib/qt/lib /usr/lib/qt4/lib /usr/lib/qt4"

if test -n "$QTDIR" ; then
    qt4_incdirs="$QTDIR/include $qt4_incdirs"
    qt4_libdirs="$QTDIR/lib $qt4_libdirs"
fi

dnl What to test
if test "x$1" == "xQtOpenGL" ; then
    qt4_test_include="QtOpenGL/QGLWidget"
    qt4_test_la_library="libQtOpenGL.la"
    qt4_test_library="libQtOpenGL.so"
    qt4_windows_test_library="QtOpenGL4.dll"
else
    qt4_test_include="QtCore/QCoreApplication"
    qt4_test_la_library="libQtCore.la"
    qt4_test_library="libQtCore.so"
    qt4_windows_test_library="QtCore4.dll"
fi

dnl Check for Qt4 headers
AC_MSG_CHECKING([for $1 headers])
qt4_incdir="no"
for it in $qt4_incdirs ; do
    if test -r "$it/$qt4_test_include" ; then
        qt4_incdir="$it"
        break
    fi
done
AC_MSG_RESULT([$qt4_incdir])

dnl Check for Qt4 libraries
AC_MSG_CHECKING([for $1 libraries])
qt4_libdir="no"
for qt4_check in $qt4_libdirs ; do
    if test -r "$qt4_check/$qt4_test_la_library" ; then
        qt4_libdir="$qt4_check"
        break
    fi

    if test -r "$qt4_check/$qt4_test_library" ; then
        qt4_libdir="$qt4_check"
        break
    fi

    if test -r "$qt4_check/$qt4_windows_test_library" ; then
        qt4_libdir="$qt4_check"
        windows_qt="yes"
        break
    fi
done
AC_MSG_RESULT([$qt4_libdir])

if test "x$qt4_libdir" != "xno" ; then
    if test "x$qt4_incdir" != "xno" ; then
        have_qt4=yes;
    fi
fi

if test "x$have_qt4" == "xyes"; then
    QT_CXXFLAGS="-I$qt4_incdir -I$qt4_incdir/QtGui -I$qt4_incdir/QtCore -I$qt4_incdir/QtOpenGL -I$qt4_incdir/Qt3Support"
    if test x$windows_qt = xyes; then
	QT_LDADD="-L$qt4_libdir -lQtCore4 -lQtGui4 -lQtOpenGL4 -lQt3Support4"
    else
	QT_LDADD="-L$qt4_libdir -lQtCore -lQtGui -lQtOpenGL -lQt3Support"
    fi
	AC_SUBST(QT_CXXFLAGS)
	AC_SUBST(QT_LDADD)
    ifelse([$2], , :, [$2])
else
    ifelse([$3], , [AC_MSG_FAILURE(dnl
[$1 development libraries not found])],
  	   [$3])
fi

dnl Check that moc is in path
AC_CHECK_PROG(MOC, moc-qt4, moc-qt4)
if test x$MOC = x ; then
	AC_CHECK_PROG(MOC, moc, moc)
	if test x$MOC = x ; then
		AC_MSG_ERROR([*** moc must be in path])
	fi
fi
])


# HOA_FIND_QT4TEST(VARIABLE-PREFIX,
#                  [ACTION-IF-FOUND],
#                  [ACTION-IF-NOT-FOUND])
#
# Check whether the Qt4 libraries are available. Adapted from Poppler qt.m4.
#
# --------------------------------------------------------------
AC_DEFUN([HOA_FIND_QT4TEST],
[
have_qt4testlib=no

qt4_incdirs="/usr/local/qt/include /usr/include/qt4 /usr/include/qt /usr/include /usr/X11R6/include/X11/qt /usr/X11R6/include/qt /usr/lib/qt/include"
qt4_libdirs="/usr/local/qt/lib /usr/lib/qt /usr/lib /usr/X11R6/lib/X11/qt /usr/X11R6/lib/qt /usr/lib/qt/lib"

if test -n "$QTDIR" ; then
    qt4_incdirs="$QTDIR/include $qt4_incdirs"
    qt4_libdirs="$QTDIR/lib $qt4_libdirs"
fi

dnl What to test
qt4test_test_include="QtTest/QtTest"
qt4test_test_la_library="libQtTest.la"
qt4test_test_library="libQtTest.so"

dnl Check for QtTestLib headers
AC_MSG_CHECKING([for QtTestLib headers])
qt4test_incdir="no"
for it in $qt4_incdirs ; do
    if test -r "$it/$qt4test_test_include" ; then
        qt4test_incdir="$it"
        break
    fi
done
AC_MSG_RESULT([$qt4test_incdir])

dnl Check for Qt4 libraries
AC_MSG_CHECKING([for QtTestLib libraries])
qt4test_libdir="no"
for qt4test_check in $qt4_libdirs ; do
    if test -r "$qt4test_check/$qt4test_test_la_library" ; then
        qt4test_libdir="$qt4test_check"
        break
    fi

    if test -r "$qt4test_check/$qt4test_test_library" ; then
        qt4test_libdir="$qt4test_check"
        break
    fi
done
AC_MSG_RESULT([$qt4test_libdir])

if test "x$qt4test_libdir" != "xno" ; then
    if test "x$qt4test_incdir" != "xno" ; then
        have_qt4testlib=yes;
    fi
fi

if test "x$have_qt4testlib" == "xyes"; then
    $1[]_CXXFLAGS="-I$qt4test_incdir"
    $1[]_LIBS="-L$qt4test_libdir -lQtTest"
    ifelse([$2], , :, [$2])
else
    ifelse([$3], , [AC_MSG_FAILURE(dnl
[Qt4 test libraries not found])],
  	   [$3])
fi
])
