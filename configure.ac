# -*- mode: autoconf -*-

AC_PREREQ(2.69)

AC_INIT(pmbw, 0.6.2)
AC_CONFIG_SRCDIR(pmbw.cc)
AC_CONFIG_AUX_DIR(acscripts)

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(foreign)
AM_MAINTAINER_MODE

# Check for Windows system and set compilation flag.

AC_MSG_CHECKING(building for Windows)
case "$target_os" in
    *cygwin* | *mingw32*)
        ON_WINDOWS=true
        AC_MSG_RESULT(yes)
        ;;
    *)
        ON_WINDOWS=false
        AC_MSG_RESULT(no)
        ;;
esac
AC_DEFINE_UNQUOTED(ON_WINDOWS, $ON_WINDOWS)

# set debug info flag if no optimization flags are set.

if test "$CXXFLAGS" == ""; then
    CXXFLAGS="-g -O2"
fi

# check for programs.

AC_PROG_CXX
AC_LANG([C++])

# test support for -march=x86-64 (older gcc's don't have it)

save_cxxflags="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -march=x86-64"
AC_MSG_CHECKING([whether $CXX supports -march=x86-64])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
  [AC_MSG_RESULT([yes])],
  [AC_MSG_RESULT([no]); CXXFLAGS="$save_cxxflags"]
)

# check for libraries.

AC_CHECK_LIB(pthread, pthread_mutex_init,
  [LIBS="$LIBS -lpthread"],
  [AC_MSG_ERROR(pthread library needed!)]
)

if [ ! "$ON_WINDOWS" ]; then

   AC_CHECK_LIB(rt, clock_gettime,
     [LIBS="$LIBS -lrt"],
     [AC_MSG_ERROR(rt library needed!)]
   )

fi

HAVE_POSIX_MEMALIGN=0
AC_CHECK_LIB(c, posix_memalign, [HAVE_POSIX_MEMALIGN=1])
AC_DEFINE_UNQUOTED(HAVE_POSIX_MEMALIGN, $HAVE_POSIX_MEMALIGN)

# transform Makefiles

AC_OUTPUT([Makefile])
