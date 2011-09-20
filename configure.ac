
AC_INIT([sshshare], [0.0.0.2], [])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CXX
#AC_LANG([C++])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile sshshare/Makefile])

PKG_CHECK_MODULES([DEPS],[gtk+-2.0 xerces-c])


AC_PROG_LIBTOOL

BOOST_REQUIRE
BOOST_FILESYSTEM
BOOST_REGEX
BOOST_DATE_TIME
BOOST_SYSTEM
STDC_HEADER



##########################################################################
# debug compilation support
##########################################################################

AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [enable debug data generation (def=no)])],
    [debugit="$enableval"],
    [debugit=no])
AC_MSG_RESULT([$debugit])

if test x"$debugit" = x"yes"; then
    AC_DEFINE([DEBUG],[],[Debug Mode])
    AM_CXXFLAGS="$AM_CXXFLAGS -g -Wall -Werror -Wno-uninitialized -O0"
else
    AC_DEFINE([NDEBUG],[],[No-debug Mode])
    #AM_CXXFLAGS="$AM_CXXFLAGS -O3"
fi

##########################################################################



AC_SUBST([AM_CXXFLAGS])

AC_OUTPUT
AC_CONFIG_MACRO_DIR([m4])
