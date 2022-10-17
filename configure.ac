
AC_INIT([libvncasync],[0.9.12])
AC_CONFIG_SRCDIR([common/vncauth.c])
AM_INIT_AUTOMAKE( [subdir-objects] )
AM_CONFIG_HEADER( common/config.h )

DATE="`date '+%d %B %Y'`"

SVN_BUILD=m4_esyscmd_s([sh -c "svnversion | cut -f1 -d: "])
AC_REVISION($Revision: $SVN_BUILD $)
SVN_BUILD=m4_esyscmd_s([sh -c "../bld"])

AC_CONFIG_MACRO_DIR([m4])

dnl# Set some basic variables
BUILD_DATE="`date`"
AC_DEFINE_UNQUOTED(BUILD_DATE, "$BUILD_DATE", [ build date ])
AUTHORS="Jose Angel Caso Sanchez (JACS) altomaltes@yahoo.es"
AC_DEFINE_UNQUOTED(AUTHORS, "$AUTHORS", [ library authors ] )


dnl# Supply default CFLAGS, if not specified by "CFLAGS=flags ./configure"
if test ! -z "${CFLAGS}" -o ! -z "${CCOPTS}"; then
  CFLAGS_GIVEN=1
else
  CFLAGS_GIVEN=0
fi

if test -z "${CFLAGS}"; then
    if test -z "${CCOPTS}"; then
        CCOPTS="-O"
    fi
    CFLAGS="$CCOPTS"
fi

AC_ARG_WITH(cc, [  --with-cc=compiler  force to build with a particular compiler (like pgcc, egcs, etc.)],
            CC=$withval,
  if test -z "$CC"; then
    CC=check
  fi
)

if test "$CC" = "check"; then
  dnl# Check for Pentium compiler
  AC_CHECK_PROG(PGCC, pgcc, pgcc)

  if test ! -z "$PGCC"; then
    CC=$PGCC
  else
    unset CC
  fi
fi

AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX

dnl# These must be run after AC_PROG_CC but before any other macros that use
dnl# the C compiler
AC_USE_SYSTEM_EXTENSIONS
AC_SEARCH_LIBS([strerror],[cposix])
LT_INIT

dnl# Where are we?
AC_MSG_CHECKING(for distribution root)
DIST_ROOT=`pwd`
AC_MSG_RESULT($DIST_ROOT)

AC_PROG_GCC_TRADITIONAL

dnl # If using gcc, use -O2.  If -g works with it, use that too
if test "${CFLAGS_GIVEN}" -ne 1; then
    if test "$GCC" = "yes"; then
        if test "$ac_cv_prog_cc_g" = "yes"; then
            CCOPTS='-g -O2'
        else
            CCOPTS='-O2'
        fi
        CFLAGS="$CCOPTS"
    fi
fi

CFLAGS+=" -fPIC -fsigned-char"
CPPFLAGS+="-I../../include"

AC_PROG_INSTALL

dnl# Check the sanity of what we've done so far
AM_SANITY_CHECK

dnl# Most people don't want the developer-only clutter
AM_MAINTAINER_MODE

#dnl bz2lib is required
#AC_CHECK_LIB( bz2, deflate, [LIBS="-lbz2 $LIBS" AC_DEFINE(HAVE_LIBBZ2, 1, [ Define if you have bz2lib. ])]
#            , [AC_MSG_WARN([zlib is required ])]
#            , [-lm])



dnl# If it's there, what the hell?
AM_WITH_DMALLOC

dnl# Look for needed programs
AC_CHECK_PROG( RM, rm, rm, true  )
AC_CHECK_PROG( CP, cp, cp, false )
AC_CHECK_PROG( AR, ar, ar, false )
AC_CHECK_PROG( MV, mv, mv, true  )

AC_C_BIGENDIAN

AC_C_CONST
AC_C_INLINE
#AC_PATH_XTRA

dnl# Checks for header files.
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS( include/procesos.h signal.h errno.h fcntl.h sys/times.h sys/ioctl.h sys/select.h sys/time.h malloc.h unistd.h stdarg.h sys/filio.h sys/wait.h sys/sendfile.h sys/socket.h sys/un.h netinet/in.h netinet/ip6.h netdb.h)
m4_warn([obsolete],
[Update your code to rely only on HAVE_SYS_TIME_H,
then remove this warning and the obsolete code below it.
All current systems provide time.h; it need not be checked for.
Not all systems provide sys/time.h, but those that do, all allow
you to include it and time.h simultaneously.])dnl
AC_CHECK_HEADERS_ONCE([sys/time.h])
# Obsolete code to be removed.
if test $ac_cv_header_sys_time_h = yes; then
  AC_DEFINE([TIME_WITH_SYS_TIME],[1],[Define to 1 if you can safely include both <sys/time.h>
	     and <time.h>.  This macro is obsolete.])
fi
# End of obsolete code.


dnl# Structure members
AC_CHECK_MEMBER( siginfo_t.si_utime, AC_DEFINE_UNQUOTED( SIGINFO_SI_UTIME, 1, [ Defined if siginfo has si_utime field ] ) ,,[#include<signal.h>])



dnl# Missing typedefs and replacements
AC_TYPE_MODE_T
AC_CHECK_TYPE(   off_t, long )
AC_CHECK_TYPE(  temp_t, unsigned long long )
AC_CHECK_TYPE( ticks_t, unsigned long      )
AC_CHECK_TYPE(    byte, unsigned char      )
AC_CHECK_TYPE(    word, unsigned short     )
AC_CHECK_TYPE(   dword, unsigned int       )
AC_CHECK_TYPE(   qword, unsigned long long )
AC_CHECK_TYPE(  handle, int                )
AC_CHECK_TYPE(socklen_t, int )
AC_TYPE_PID_T
AC_TYPE_UID_T

dnl# Checks for library functions.
m4_warn([obsolete],
[your code may safely assume C89 semantics that RETSIGTYPE is void.
Remove this warning and the `AC_CACHE_CHECK' when you adjust the code.])dnl
AC_CACHE_CHECK([return type of signal handlers],[ac_cv_type_signal],[AC_COMPILE_IFELSE(
[AC_LANG_PROGRAM([#include <sys/types.h>
#include <signal.h>
],
		 [return *(signal (0, 0)) (0) == 1;])],
		   [ac_cv_type_signal=int],
		   [ac_cv_type_signal=void])])
AC_DEFINE_UNQUOTED([RETSIGTYPE],[$ac_cv_type_signal],[Define as the return type of signal handlers
		    (`int' or `void').])

AC_CHECK_FUNCS( atexit unsetenv setutent seteuid putenv setresuid setresgid usleep fork vsnprintf )

dnl# Portability checks for various functions
AC_SEARCH_LIBS( getpwuid, sun )
AC_SEARCH_LIBS( connect, socket )
AC_SEARCH_LIBS( gethostbyname, nsl )

dnl#
dnl# Utility stuff
dnl#
dnl# Did they want debugging?
AC_MSG_CHECKING(for debugging level)
AC_ARG_WITH(debugging, [  --with-debugging[=num]  enable debugging output, num is an integer 0-9],
              if test "$withval" = "yes"; then
                  withval=4
              else
                  :
              fi
              if test "$withval" != "no"; then
                  echo "$withval"
                  AC_DEFINE_UNQUOTED(DEBUG, $withval, [ Define debug eval ] )
              else
                  echo "no debugging"
                  AC_DEFINE_UNQUOTED(DEBUG, 0, [ Define debug 0 ] )
              fi, echo "4"
                  AC_DEFINE_UNQUOTED(DEBUG, 4, [ Define debug 4 ] )
)

AC_MSG_CHECKING(for MMX support)
HAVE_MMX=""
AC_ARG_ENABLE(mmx, [  --enable-mmx            enable MMX assembly routines],
               test "x$enableval" = "xyes" && HAVE_MMX="yes"
              ,
               if test x$build_os = xlinux-gnu; then
                 grep mmx /proc/cpuinfo >/dev/null 2>&1 && HAVE_MMX="yes"
               fi
              )
if test "x$HAVE_MMX" = "xyes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE_UNQUOTED(HAVE_MMX, "$HAVE_MMX", [ MMX instruction set ] )
else
  AC_MSG_RESULT(no)
fi
AM_CONDITIONAL(HAVE_MMX, test "x$HAVE_MMX" = "xyes")

dnl Detect windows

AM_CONDITIONAL( WINDOWS,   test "${version_type}" = "windows" )

AC_PATH_PROG([SLACKBUILD], [makepkg], [notfound])

dnl AC_ARG_ENABLE([slack],
dnl     AS_HELP_STRING([--disable-slack], [No general el paquete de instalacion]))
dnl AS_IF([test "x$enable_slack" != "xno"], [ enable_slack=yes ])

AC_ARG_ENABLE([slack],
  [AS_HELP_STRING([--enable-slack=@<:@yes/no@:>@],
    [Configure for building SLACKWARE package @<:@default=yes@:>@ (requires makepkg)])],
  [AS_IF([test "x$enable_slack" = xyes],
    [AS_IF([test "x$SLACKBUILD" = xnotfound],
      [AC_MSG_ERROR([makepkg is required for --enable-slack])],
        [$test "x$SLACKBUILD" = xnotfound],
          [AC_MSG_ERROR([makepkg is required for --enable-slack])])])],
  [enable_slack=yes])

AM_CONDITIONAL(BUILDING_SLACK, test "x$enable_slack" = "xyes" )



AC_SUBST(DIST_ROOT)
AC_SUBST(GDB_CMD_FILE)
AC_SUBST(CC)
AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(LIBS)
AC_SUBST(SLACKBUILD)
AC_SUBST(AUTHORS)
AC_SUBST(DATE)
AC_SUBST(SVN_BUILD)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "
Configuration:
-------------------------------------------------------

  Source code location:       $srcdir
  Host System Type:           $host
  Preprocessor:               $CC $CPPFLAGS
  Compiler:                   $CC $CFLAGS
  Linker:                     $CC $LDFLAGS $LIBS
  Install path:               $prefix
  Slackware packager:         $SLACKBUILD $enable_slack

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  Now type 'make' to build $PACKAGE $VERSION $SVN_BUILD.
"
