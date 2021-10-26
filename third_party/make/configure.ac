# Process this file with autoconf to produce a configure script.
#
# Copyright (C) 1993-2020 Free Software Foundation, Inc.
# This file is part of GNU Make.
#
# GNU Make is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3 of the License, or (at your option) any later
# version.
#
# GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.

AC_INIT([GNU make],[4.3],[bug-make@gnu.org])

AC_PREREQ([2.69])

# Autoconf setup
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/vpath.c])
AC_CONFIG_HEADERS([src/config.h])

AC_CONFIG_LIBOBJ_DIR([lib])

# Automake setup
# We have to enable "foreign" because ChangeLog is auto-generated
# Automake 1.15 and gnulib don't get along: gnulib has some strange error
# in the way it handles getloadavg.c which causes make distcheck to fail.
# http://lists.gnu.org/archive/html/bug-gnulib/2018-06/msg00024.html
AM_INIT_AUTOMAKE([1.16.1 foreign -Werror -Wall])

# Checks for programs.
AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC

# Configure gnulib
gl_EARLY
gl_INIT

AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PROG_CPP
AC_CHECK_PROG([AR], [ar], [ar], [ar])
# Perl is needed for the test suite (only)
AC_CHECK_PROG([PERL], [perl], [perl], [perl])

# Specialized system macros
AC_CANONICAL_HOST
AC_AIX
AC_ISC_POSIX
AC_MINIX
AC_C_BIGENDIAN

# Enable gettext, in "external" mode.
AM_GNU_GETTEXT_VERSION([0.19.4])
AM_GNU_GETTEXT([external])

# This test must come as early as possible after the compiler configuration
# tests, because the choice of the file model can (in principle) affect
# whether functions and headers are available, whether they work, etc.
AC_SYS_LARGEFILE

# Checks for libraries.
AC_SEARCH_LIBS([getpwnam], [sun])

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_DIRENT
AC_HEADER_STAT
AC_HEADER_TIME
AC_CHECK_HEADERS([stdlib.h locale.h unistd.h limits.h fcntl.h string.h \
                  memory.h sys/param.h sys/resource.h sys/time.h sys/timeb.h \
                  sys/select.h sys/file.h spawn.h])

AM_PROG_CC_C_O
AC_C_CONST
AC_TYPE_SIGNAL
AC_TYPE_UID_T
AC_TYPE_PID_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINTMAX_T

# Find out whether our struct stat returns nanosecond resolution timestamps.

AC_STRUCT_ST_MTIM_NSEC
AC_CACHE_CHECK([whether to use high resolution file timestamps],
               [make_cv_file_timestamp_hi_res],
[ make_cv_file_timestamp_hi_res=no
  AS_IF([test "$ac_cv_struct_st_mtim_nsec" != no],
        [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#if HAVE_INTTYPES_H
# include <inttypes.h>
#endif]],
                      [[char a[0x7fffffff < (uintmax_t)-1 >> 30 ? 1 : -1];]])],
        [make_cv_file_timestamp_hi_res=yes])
  ])])
AS_IF([test "$make_cv_file_timestamp_hi_res" = yes], [val=1], [val=0])
AC_DEFINE_UNQUOTED([FILE_TIMESTAMP_HI_RES], [$val],
                   [Use high resolution file timestamps if nonzero.])

AS_IF([test "$make_cv_file_timestamp_hi_res" = yes],
[ # Solaris 2.5.1 needs -lposix4 to get the clock_gettime function.
  # Solaris 7 prefers the library name -lrt to the obsolescent name -lposix4.
  AC_SEARCH_LIBS([clock_gettime], [rt posix4])
  AS_IF([test "$ac_cv_search_clock_gettime" != no],
  [ AC_DEFINE([HAVE_CLOCK_GETTIME], [1],
              [Define to 1 if you have the clock_gettime function.])
  ])
])

# Check for DOS-style pathnames.
pds_AC_DOS_PATHS

# See if we have a standard version of gettimeofday().  Since actual
# implementations can differ, just make sure we have the most common
# one.
AC_CACHE_CHECK([for standard gettimeofday], [ac_cv_func_gettimeofday],
  [ac_cv_func_gettimeofday=no
   AC_RUN_IFELSE([AC_LANG_SOURCE([[#include <sys/time.h>
                  int main ()
                  {
                    struct timeval t; t.tv_sec = -1; t.tv_usec = -1;
                    return gettimeofday (&t, 0) != 0
                          || t.tv_sec < 0 || t.tv_usec < 0;
                  }]])],
                  [ac_cv_func_gettimeofday=yes],
                  [ac_cv_func_gettimeofday=no],
                  [ac_cv_func_gettimeofday="no (cross-compiling)"])])
AS_IF([test "$ac_cv_func_gettimeofday" = yes],
[ AC_DEFINE([HAVE_GETTIMEOFDAY], [1],
            [Define to 1 if you have a standard gettimeofday function])
])

AC_CHECK_FUNCS([strdup strndup memrchr umask mkstemp mktemp fdopen \
                dup dup2 getcwd realpath sigsetmask sigaction \
                getgroups seteuid setegid setlinebuf setreuid setregid \
                getrlimit setrlimit setvbuf pipe strsignal \
                lstat readlink atexit isatty ttyname pselect posix_spawn \
                posix_spawnattr_setsigmask])

# We need to check declarations, not just existence, because on Tru64 this
# function is not declared without special flags, which themselves cause
# other problems.  We'll just use our own.
AC_CHECK_DECLS([bsd_signal], [], [], [[#define _GNU_SOURCE 1
#include <signal.h>]])

AC_FUNC_FORK

AC_FUNC_SETVBUF_REVERSED

# Rumor has it that strcasecmp lives in -lresolv on some odd systems.
# It doesn't hurt much to use our own if we can't find it so I don't
# make the effort here.
AC_CHECK_FUNCS([strcasecmp strncasecmp strcmpi strncmpi stricmp strnicmp])

# strcoll() is used by the GNU glob library
AC_FUNC_STRCOLL
AC_FUNC_CLOSEDIR_VOID

# dir.c and our glob.c use dirent.d_type if available
AC_STRUCT_DIRENT_D_TYPE

# See if the user wants to add (or not) GNU Guile support
AC_ARG_WITH([guile], [AS_HELP_STRING([--with-guile],
            [Support GNU Guile for embedded scripting])])

# Annoyingly, each version of Guile comes with it's own PC file so we have to
# specify them as individual packages.  Ugh.
PKG_PROG_PKG_CONFIG

AS_IF([test "x$with_guile" != xno],
[ guile_versions="3.0 2.2 2.0 1.8"
  guile_version=no
  have_guile=no
  AC_MSG_CHECKING([for GNU Guile])
  for v in $guile_versions; do
    PKG_CHECK_EXISTS([guile-$v], [guile_version=$v; have_guile=yes; break], [])
  done
  AC_MSG_RESULT([$guile_version])
  AS_IF([test "$have_guile" = yes],
  [ PKG_CHECK_MODULES(GUILE, [guile-$guile_version])
    # Unfortunately Guile requires a C99 compiler but GNU make doesn't, so
    # verify we can actually compile the header.
    keep_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $pkg_cv_GUILE_CFLAGS"
    AC_CHECK_HEADER([libguile.h],
                    [AC_DEFINE([HAVE_GUILE], [1], [Embed GNU Guile support])],
                    [have_guile=no],
                    [/* Avoid configuration error warnings. */])
    CPPFLAGS="$keep_CPPFLAGS"
  ])
])

AM_CONDITIONAL([HAVE_GUILE], [test "$have_guile" = "yes"])

AC_CHECK_DECLS([sys_siglist, _sys_siglist, __sys_siglist], , ,
  [AC_INCLUDES_DEFAULT
#include <signal.h>
/* NetBSD declares sys_siglist in unistd.h.  */
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
])


# Check out the wait reality.
AC_CHECK_HEADERS([sys/wait.h],[],[],[[#include <sys/types.h>]])
AC_CHECK_FUNCS([waitpid wait3])
AC_CACHE_CHECK([for union wait], [make_cv_union_wait],
[ AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <sys/types.h>
#include <sys/wait.h>]],
     [[union wait status; int pid; pid = wait (&status);
#ifdef WEXITSTATUS
/* Some POSIXoid systems have both the new-style macros and the old
   union wait type, and they do not work together.  If union wait
   conflicts with WEXITSTATUS et al, we don't want to use it at all.  */
        if (WEXITSTATUS (status) != 0) pid = -1;
#ifdef WTERMSIG
        /* If we have WEXITSTATUS and WTERMSIG, just use them on ints.  */
        -- blow chunks here --
#endif
#endif
#ifdef HAVE_WAITPID
        /* Make sure union wait works with waitpid.  */
        pid = waitpid (-1, &status, 0);
#endif
      ]])],
    [make_cv_union_wait=yes],
    [make_cv_union_wait=no])
])
AS_IF([test "$make_cv_union_wait" = yes],
[ AC_DEFINE([HAVE_UNION_WAIT], [1],
            [Define to 1 if you have the 'union wait' type in <sys/wait.h>.])
])


# If we're building on Windows/DOS/OS/2, add some support for DOS drive specs.
AS_IF([test "$PATH_SEPARATOR" = ';'],
[ AC_DEFINE([HAVE_DOS_PATHS], [1],
            [Define to 1 if your system requires backslashes or drive specs in pathnames.])
])

# See if the user wants to use pmake's "customs" distributed build capability
AC_SUBST([REMOTE]) REMOTE=stub
use_customs=false
AC_ARG_WITH([customs],
[AC_HELP_STRING([--with-customs=DIR],
                [enable remote jobs via Customs--see README.customs])],
[ AS_CASE([$withval], [n|no], [:],
    [make_cppflags="$CPPFLAGS"
     AS_CASE([$withval],
             [y|ye|yes], [:],
             [CPPFLAGS="$CPPFLAGS -I$with_customs/include/customs"
              make_ldflags="$LDFLAGS -L$with_customs/lib"])
     CF_NETLIBS
     AC_CHECK_HEADER([customs.h],
                     [use_customs=true
                      REMOTE=cstms
                      LIBS="$LIBS -lcustoms" LDFLAGS="$make_ldflags"],
                     [with_customs=no
                      CPPFLAGS="$make_cppflags" make_badcust=yes])
    ])
])

# Tell automake about this, so it can include the right .c files.
AM_CONDITIONAL([USE_CUSTOMS], [test "$use_customs" = true])

# See if the user asked to handle case insensitive file systems.
AH_TEMPLATE([HAVE_CASE_INSENSITIVE_FS], [Use case insensitive file names])
AC_ARG_ENABLE([case-insensitive-file-system],
  AC_HELP_STRING([--enable-case-insensitive-file-system],
                 [assume file systems are case insensitive]),
  [AS_IF([test "$enableval" = yes], [AC_DEFINE([HAVE_CASE_INSENSITIVE_FS])])])

# See if we can handle the job server feature, and if the user wants it.
AC_ARG_ENABLE([job-server],
  AC_HELP_STRING([--disable-job-server],
                 [disallow recursive make communication during -jN]),
  [make_cv_job_server="$enableval" user_job_server="$enableval"],
  [make_cv_job_server="yes"])

AS_IF([test "$ac_cv_func_waitpid" = no && test "$ac_cv_func_wait3" = no],
      [has_wait_nohang=no],
      [has_wait_nohang=yes])

AC_CACHE_CHECK([for SA_RESTART], [make_cv_sa_restart], [
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <signal.h>]],
      [[return SA_RESTART;]])],
    [make_cv_sa_restart=yes],
    [make_cv_sa_restart=no])])

AS_IF([test "$make_cv_sa_restart" != no],
[ AC_DEFINE([HAVE_SA_RESTART], [1],
     [Define to 1 if <signal.h> defines the SA_RESTART constant.])
])

# Only allow jobserver on systems that support it
AS_CASE([/$ac_cv_func_pipe/$ac_cv_func_sigaction/$make_cv_sa_restart/$has_wait_nohang/],
  [*/no/*], [make_cv_job_server=no])

# Also supported on OS2 and MinGW
AS_CASE([$host_os], [os2*|mingw*], [make_cv_job_server=yes])

# If we support it and the user didn't disable it, build with jobserver
AS_CASE([/$make_cv_job_server/$user_job_server/],
  [*/no/*], [: no jobserver],
  [AC_DEFINE(MAKE_JOBSERVER, 1,
             [Define to 1 to enable job server support in GNU make.])
  ])

# If dl*() functions are supported we can enable the load operation
AC_CHECK_DECLS([dlopen, dlsym, dlerror], [], [],
  [[#include <dlfcn.h>]])

AC_ARG_ENABLE([load],
  AC_HELP_STRING([--disable-load],
                 [disable support for the 'load' operation]),
  [make_cv_load="$enableval" user_load="$enableval"],
  [make_cv_load="yes"])

AS_CASE([/$ac_cv_have_decl_dlopen/$ac_cv_have_decl_dlsym/$ac_cv_have_decl_dlerror/],
  [*/no/*], [make_cv_load=no])

# We might need -ldl
AS_IF([test "$make_cv_load" = yes], [
  AC_SEARCH_LIBS([dlopen], [dl], [], [make_cv_load=])
  ])

AS_CASE([/$make_cv_load/$user_load/],
  [*/no/*], [make_cv_load=no],
  [AC_DEFINE(MAKE_LOAD, 1,
             [Define to 1 to enable 'load' support in GNU make.])
  ])

# If we want load support, we might need to link with export-dynamic.
# See if we can figure it out.  Unfortunately this is very difficult.
# For example passing -rdynamic to the SunPRO linker gives a warning
# but succeeds and creates a shared object, not an executable!
AS_IF([test "$make_cv_load" = yes], [
  AC_MSG_CHECKING([If the linker accepts -Wl,--export-dynamic])
  old_LDFLAGS="$LDFLAGS"
  LDFLAGS="$LDFLAGS -Wl,--export-dynamic"
  AC_LINK_IFELSE([AC_LANG_SOURCE([int main(){}])],
    [AC_MSG_RESULT([yes])
     AC_SUBST([AM_LDFLAGS], [-Wl,--export-dynamic])],
    [AC_MSG_RESULT([no])
     AC_MSG_CHECKING([If the linker accepts -rdynamic])
     LDFLAGS="$old_LDFLAGS -rdynamic"
     AC_LINK_IFELSE([AC_LANG_SOURCE([int main(){}])],
       [AC_MSG_RESULT([yes])
        AC_SUBST([AM_LDFLAGS], [-rdynamic])],
       [AC_MSG_RESULT([no])])
   ])
  LDFLAGS="$old_LDFLAGS"
])

# if we have both lstat() and readlink() then we can support symlink
# timechecks.
AS_IF([test "$ac_cv_func_lstat" = yes && test "$ac_cv_func_readlink" = yes],
  [ AC_DEFINE([MAKE_SYMLINKS], [1],
              [Define to 1 to enable symbolic link timestamp checking.])
])

# Use posix_spawn if we have support and the user didn't disable it

AC_ARG_ENABLE([posix-spawn],
  AC_HELP_STRING([--disable-posix-spawn],
                 [disable support for posix_spawn()]),
  [make_cv_posix_spawn="$enableval" user_posix_spawn="$enableval"],
  [make_cv_posix_spawn="yes"])

AS_CASE([/$ac_cv_header_spawn/$ac_cv_func_posix_spawn/],
  [*/no/*], [make_cv_posix_spawn=no])

AS_IF([test "$make_cv_posix_spawn" = yes],
  AC_CACHE_CHECK([for posix_spawn that fails synchronously],
    [make_cv_synchronous_posix_spawn],
    [make_cv_synchronous_posix_spawn=no
     AC_RUN_IFELSE([AC_LANG_SOURCE([[
       #include <spawn.h>
       #include <string.h>

       extern char **environ;

       int main() {
         char* path = strdup("./non-existent");
         char *argv[[2]];
         argv[[0]] = path;
         argv[[1]] =  0;
         return posix_spawn(0, path, 0, 0, argv, environ);
       }]])],
       [make_cv_synchronous_posix_spawn=no],
       [make_cv_synchronous_posix_spawn=yes],
       [make_cv_synchronous_posix_spawn="no (cross-compiling)"])]))

AS_CASE([/$user_posix_spawn/$make_cv_posix_spawn/$make_cv_synchronous_posix_spawn/],
  [*/no/*], [make_cv_posix_spawn=no],
  [AC_DEFINE(USE_POSIX_SPAWN, 1, [Define to 1 to use posix_spawn().])
  ])

# Find the SCCS commands, so we can include them in our default rules.

AC_CACHE_CHECK([for location of SCCS get command], [make_cv_path_sccs_get], [
  AS_IF([test -f /usr/sccs/get],
        [make_cv_path_sccs_get=/usr/sccs/get],
        [make_cv_path_sccs_get=get])
])
AC_DEFINE_UNQUOTED([SCCS_GET], ["$make_cv_path_sccs_get"],
                   [Define to the name of the SCCS 'get' command.])

ac_clean_files="$ac_clean_files s.conftest conftoast" # Remove these later.
AS_IF([(/usr/sccs/admin -n s.conftest || admin -n s.conftest) >/dev/null 2>&1 &&
   test -f s.conftest],
[ # We successfully created an SCCS file.
  AC_CACHE_CHECK([if SCCS get command understands -G], [make_cv_sys_get_minus_G],
    [AS_IF([$make_cv_path_sccs_get -Gconftoast s.conftest >/dev/null 2>&1 &&
            test -f conftoast],
           [make_cv_sys_get_minus_G=yes],
           [make_cv_sys_get_minus_G=no])
    ])
  AS_IF([test "$make_cv_sys_get_minus_G" = yes],
    [AC_DEFINE([SCCS_GET_MINUS_G], [1],
     [Define to 1 if the SCCS 'get' command understands the '-G<file>' option.])
    ])
])
rm -f s.conftest conftoast

# Let the makefile know what our build host is

AC_DEFINE_UNQUOTED([MAKE_HOST],["$host"],[Build host information.])
MAKE_HOST="$host"
AC_SUBST([MAKE_HOST])

w32_target_env=no
AM_CONDITIONAL([WINDOWSENV], [false])

AS_CASE([$host],
  [*-*-mingw32],
   [AM_CONDITIONAL([WINDOWSENV], [true])
    w32_target_env=yes
    AC_DEFINE([WINDOWS32], [1], [Use platform specific coding])
    AC_DEFINE([HAVE_DOS_PATHS], [1], [Use platform specific coding])
  ])

AC_DEFINE_UNQUOTED([PATH_SEPARATOR_CHAR],['$PATH_SEPARATOR'],
        [Define to the character that separates directories in PATH.])

# Include the Maintainer's Makefile section, if it's here.

MAINT_MAKEFILE=/dev/null
AS_IF([test -r "$srcdir/maintMakefile"],
[ MAINT_MAKEFILE="$srcdir/maintMakefile"
])
AC_SUBST_FILE([MAINT_MAKEFILE])

# Allow building with dmalloc
AM_WITH_DMALLOC

# Forcibly disable SET_MAKE.  If it's set it breaks things like the test
# scripts, etc.
SET_MAKE=

# Sanity check and inform the user of what we found

AS_IF([test "x$make_badcust" = xyes], [
echo
echo "WARNING: --with-customs specified but no customs.h could be found;"
echo "         disabling Customs support."
echo
])

AS_CASE([$with_customs],
[""|n|no|y|ye|yes], [:],
[AS_IF([test -f "$with_customs/lib/libcustoms.a"], [:],
[ echo
  echo "WARNING: '$with_customs/lib' does not appear to contain the"
  echo "         Customs library.  You must build and install Customs"
  echo "         before compiling GNU make."
  echo
])])

AS_IF([test "x$has_wait_nohang" = xno],
[ echo
  echo "WARNING: Your system has neither waitpid() nor wait3()."
  echo "         Without one of these, signal handling is unreliable."
  echo "         You should be aware that running GNU make with -j"
  echo "         could result in erratic behavior."
  echo
])

AS_IF([test "x$make_cv_job_server" = xno && test "x$user_job_server" = xyes],
[ echo
  echo "WARNING: Make job server requires a POSIX-ish system that"
  echo "         supports the pipe(), sigaction(), and either"
  echo "         waitpid() or wait3() functions.  Your system doesn't"
  echo "         appear to provide one or more of those."
  echo "         Disabling job server support."
  echo
])

AS_IF([test "x$make_cv_load" = xno && test "x$user_load" = xyes],
[ echo
  echo "WARNING: 'load' support requires a POSIX-ish system that"
  echo "         supports the dlopen(), dlsym(), and dlerror() functions."
  echo "         Your system doesn't appear to provide one or more of these."
  echo "         Disabling 'load' support."
  echo
])

AS_IF([test "x$make_cv_posix_spawn" = xno && test "x$user_posix_spawn" = xyes],
[ echo
  echo "WARNING: posix_spawn() is not supported on this system."
  echo
])

# Specify what files are to be created.
AC_CONFIG_FILES([Makefile build.cfg lib/Makefile po/Makefile.in doc/Makefile \
                 tests/config-flags.pm])
# We don't need this: the standard automake output suffices for POSIX systems.
#mk/Posix.mk

# OK, do it!

AC_OUTPUT

dnl Local Variables:
dnl comment-start: "dnl "
dnl comment-end: ""
dnl comment-start-skip: "\\bdnl\\b\\s *"
dnl compile-command: "make configure config.h.in"
dnl End:
