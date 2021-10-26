/* config.h.W32 -- hand-massaged config.h file for Windows builds       -*-C-*-

Copyright (C) 1996-2020 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Suppress some Visual C++ warnings.
   Maybe after the code cleanup for ISO C we can remove some/all of these.  */
#if _MSC_VER > 1000
# pragma warning(disable:4100) /* unreferenced formal parameter */
# pragma warning(disable:4102) /* unreferenced label */
# pragma warning(disable:4127) /* conditional expression is constant */
# pragma warning(disable:4131) /* uses old-style declarator */
# pragma warning(disable:4702) /* unreachable code */
# define _CRT_SECURE_NO_WARNINGS  /* function or variable may be unsafe */
# define _CRT_NONSTDC_NO_WARNINGS /* functions w/o a leading underscore */
#endif

/* Define to 1 if the 'closedir' function returns void instead of 'int'. */
/* #undef CLOSEDIR_VOID */

/* Define to one of '_getb67', 'GETB67', 'getb67' for Cray-2 and Cray-YMP
   systems. This function is required for 'alloca.c' support on those systems.
   */
/* #undef CRAY_STACKSEG_END */

/* Define to 1 if using 'alloca.c'. */
/* #undef C_ALLOCA */

/* Define to 1 if using 'getloadavg.c'. */
#define C_GETLOADAVG 1

/* Define to 1 for DGUX with <sys/dg_sys_info.h>. */
/* #undef DGUX */

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
/* #undef ENABLE_NLS */

/* Use high resolution file timestamps if nonzero. */
#define FILE_TIMESTAMP_HI_RES 0

/* Define to 1 if the 'getloadavg' function needs to be run setuid or setgid.
   */
/* #undef GETLOADAVG_PRIVILEGED */

/* Define to 1 if you have 'alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define to 1 if you have <alloca.h> and it should be used (not on Ultrix).
   */
/* #undef HAVE_ALLOCA_H */

/* Define to 1 if you have the 'atexit' function. */
#define HAVE_ATEXIT 1

/* Use case insensitive file names */
/* #undef HAVE_CASE_INSENSITIVE_FS */

/* Define to 1 if you have the clock_gettime function. */
/* #undef HAVE_CLOCK_GETTIME */

/* Embed GNU Guile support.  Windows build sets this on the
   compilation command line.  */
/* #undef HAVE_GUILE */

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
/* #undef HAVE_DCGETTEXT */

/* Define to 1 if you have the declaration of 'bsd_signal', and to 0 if you
   don't. */
#define HAVE_DECL_BSD_SIGNAL 0

/* Define to 1 if you have the declaration of 'sys_siglist', and to 0 if you
   don't. */
#define HAVE_DECL_SYS_SIGLIST 0

/* Define to 1 if you have the declaration of '_sys_siglist', and to 0 if you
   don't. */
#define HAVE_DECL__SYS_SIGLIST 0

/* Define to 1 if you have the declaration of '__sys_siglist', and to 0 if you
   don't. */
#define HAVE_DECL___SYS_SIGLIST 0

/* Define to 1 if you have the <dirent.h> header file, and it defines 'DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <direct.h> header file, and it defines getcwd()
   and chdir().
   */
#if (defined(_MSC_VER) || defined(__BORLANDC__)) && !defined(__INTERIX)
# define HAVE_DIRECT_H 1
#endif

/* Use platform specific coding */
#define HAVE_DOS_PATHS 1

/* Define to 1 if you have the 'dup2' function. */
#define HAVE_DUP2 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the 'fdopen' function. */
#ifdef __MINGW32__
#define HAVE_FDOPEN 1
#endif

/* Define to 1 if you have the 'fileno' function. */
#define HAVE_FILENO 1

/* Define to 1 if you have the 'getcwd' function.  */
#define HAVE_GETCWD 1

/* Define to 1 if you have the 'getgroups' function. */
/* #undef HAVE_GETGROUPS */

/* Define to 1 if you have the 'gethostbyname' function. */
/* #undef HAVE_GETHOSTBYNAME */

/* Define to 1 if you have the 'gethostname' function. */
/* #undef HAVE_GETHOSTNAME */

/* Define to 1 if you have the 'getloadavg' function. */
/* #undef HAVE_GETLOADAVG */

/* Define to 1 if you have the 'getrlimit' function. */
/* #undef HAVE_GETRLIMIT */

/* Define if the GNU gettext() function is already present or preinstalled. */
/* #undef HAVE_GETTEXT */

/* Define to 1 if you have a standard gettimeofday function */
#ifdef __MINGW32__
#define HAVE_GETTIMEOFDAY 1
#endif

/* Define if you have the iconv() function. */
/* #undef HAVE_ICONV */

/* Define to 1 if you have the <inttypes.h> header file. */
#ifdef __MINGW32__
#define HAVE_INTTYPES_H 1
#endif

/* Define to 1 if you have the 'dgc' library (-ldgc). */
/* #undef HAVE_LIBDGC */

/* Define to 1 if you have the 'kstat' library (-lkstat). */
/* #undef HAVE_LIBKSTAT */

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <locale.h> header file. */
/*#define HAVE_LOCALE_H 1*/

/* Define to 1 if you have the 'lstat' function. */
/* #undef HAVE_LSTAT */

/* Define to 1 if you have the <mach/mach.h> header file. */
/* #undef HAVE_MACH_MACH_H */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the 'mkstemp' function. */
/* #undef HAVE_MKSTEMP */

/* Define to 1 if you have the 'mktemp' function. */
#define HAVE_MKTEMP 1

/* Define to 1 if you have the <ndir.h> header file, and it defines 'DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <nlist.h> header file. */
/* #undef HAVE_NLIST_H */

/* Define to 1 if you have the 'pipe' function. */
/* #undef HAVE_PIPE */

/* Define to 1 if you have the 'pstat_getdynamic' function. */
/* #undef HAVE_PSTAT_GETDYNAMIC */

/* Define to 1 if you have the 'readlink' function. */
/* #undef HAVE_READLINK */

/* Define to 1 if you have the 'realpath' function. */
/* #undef HAVE_REALPATH */

/* Define to 1 if <signal.h> defines the SA_RESTART constant. */
/* #undef HAVE_SA_RESTART */

/* Define to 1 if you have the 'setegid' function. */
/* #undef HAVE_SETEGID */

/* Define to 1 if you have the 'seteuid' function. */
/* #undef HAVE_SETEUID */

/* Define to 1 if you have the 'setlinebuf' function. */
/* #undef HAVE_SETLINEBUF */

/* Define to 1 if you have the 'setlocale' function. */
/*#define HAVE_SETLOCALE 1*/

/* Define to 1 if you have the 'setregid' function. */
/* #undef HAVE_SETREGID */

/* Define to 1 if you have the 'setreuid' function. */
/* #undef HAVE_SETREUID */

/* Define to 1 if you have the 'setrlimit' function. */
/* #undef HAVE_SETRLIMIT */

/* Define to 1 if you have the 'setvbuf' function. */
#define HAVE_SETVBUF 1

/* Define to 1 if you have the 'sigaction' function. */
/* #undef HAVE_SIGACTION */

/* Define to 1 if you have the 'sigsetmask' function. */
/* #undef HAVE_SIGSETMASK */

/* Define to 1 if you have the 'socket' function. */
/* #undef HAVE_SOCKET */

/* Define to 1 if you have the <stdint.h> header file. */
#ifdef __MINGW32__
#define HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the 'strcasecmp' function. */
#ifdef __MINGW32__
#define HAVE_STRCASECMP 1
#endif

/* Define to 1 if you have the 'strcmpi' function. */
#define HAVE_STRCMPI 1

/* Define to 1 if you have the 'strcoll' function and it is properly defined.
   */
#define HAVE_STRCOLL 1

/* Define to 1 if you have the 'strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the 'stricmp' function. */
#define HAVE_STRICMP 1

/* Define to 1 if you have the <strings.h> header file. */
#ifdef __MINGW32__
#define HAVE_STRINGS_H 1
#endif

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the 'strncasecmp' function. */
#ifdef __MINGW32__
#define HAVE_STRNCASECMP 1
#endif

/* Define to 1 if you have the 'strncmpi' function. */
/* #undef HAVE_STRNCMPI */

/* Define to 1 if you have the 'strndup' function. */
/* #undef HAVE_STRNDUP */

/* Define to 1 if you have the 'strnicmp' function. */
#ifdef __MINGW32__
#define HAVE_STRNICMP 1
#endif

/* Define to 1 if you have the 'strsignal' function. */
/* #undef HAVE_STRSIGNAL */

/* Define to 1 if `d_type' is a member of `struct dirent'. */
/* SV 57152: MinGW64 version of dirent doesn't support d_type. */
#ifndef __MINGW64__
# define HAVE_STRUCT_DIRENT_D_TYPE 1
#endif

/* Define to 1 if you have the `isatty' function. */
#define HAVE_ISATTY 1

/* Define to 1 if you have the `ttyname' function. */
#define HAVE_TTYNAME 1
char *ttyname (int);

/* Define to 1 if you have the `umask' function. */
#ifdef __MINGW32__
# define HAVE_UMASK 1
#endif

/* Define to 1 if 'n_un.n_name' is a member of 'struct nlist'. */
/* #undef HAVE_STRUCT_NLIST_N_UN_N_NAME */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines 'DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines 'DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#ifdef __MINGW32__
#define HAVE_SYS_PARAM_H 1
#endif

/* Define to 1 if you have the <sys/resource.h> header file. */
/* #undef HAVE_SYS_RESOURCE_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/timeb.h> header file. */
#define HAVE_SYS_TIMEB_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#ifdef __MINGW32__
#define HAVE_SYS_TIME_H 1
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/wait.h> header file. */
/* #undef HAVE_SYS_WAIT_H */

/* Define to 1 if you have the \'union wait' type in <sys/wait.h>. */
/* #undef HAVE_UNION_WAIT */

/* Define to 1 if you have the <unistd.h> header file. */
#ifdef __MINGW32__
#define HAVE_UNISTD_H 1
#endif

/* Define to 1 if you have the 'wait3' function. */
/* #undef HAVE_WAIT3 */

/* Define to 1 if you have the 'waitpid' function. */
/* #undef HAVE_WAITPID */

/* Build host information. */
#define MAKE_HOST "Windows32"

/* Define to 1 to enable job server support in GNU make. */
#define MAKE_JOBSERVER 1

/* Define to 1 to enable 'load' support in GNU make. */
#define MAKE_LOAD 1

/* Define to 1 to enable symbolic link timestamp checking. */
/* #undef MAKE_SYMLINKS */

/* Define to 1 if your 'struct nlist' has an 'n_un' member. Obsolete, depend
   on 'HAVE_STRUCT_NLIST_N_UN_N_NAME */
/* #undef NLIST_NAME_UNION */

/* Define to 1 if struct nlist.n_name is a pointer rather than an array. */
/* #undef NLIST_STRUCT */

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of this package (needed by automake) */
#define PACKAGE "make"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "bug-make@gnu.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "GNU make"

/* Define to the home page for this package. */
#define PACKAGE_URL "http://www.gnu.org/software/make/"

/* Define to the version of this package. */
#define PACKAGE_VERSION "4.3"

/* Define to the character that separates directories in PATH. */
#define PATH_SEPARATOR_CHAR ';'

/* Define as the return type of signal handlers ('int' or 'void'). */
#define RETSIGTYPE void

/* Define to the name of the SCCS 'get' command. */
#define SCCS_GET "echo no sccs get"

/* Define this if the SCCS 'get' command understands the '-G<file>' option. */
/* #undef SCCS_GET_MINUS_G */

/* Define to 1 if the 'setvbuf' function takes the buffering type as its
   second argument and the buffer pointer as the third, as on System V before
   release 3. */
/* #undef SETVBUF_REVERSED */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run time.
        STACK_DIRECTION > 0 => grows toward higher addresses
        STACK_DIRECTION < 0 => grows toward lower addresses
        STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define to 1 if the 'S_IS*' macros in <sys/stat.h> do not work properly. */
/* #undef STAT_MACROS_BROKEN */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define if struct stat contains a nanoseconds field */
/* #undef ST_MTIM_NSEC */

/* Define to 1 on System V Release 4. */
/* #undef SVR4 */

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#ifdef __MINGW32__
#define TIME_WITH_SYS_TIME 1
#endif

/* Define to 1 for Encore UMAX. */
/* #undef UMAX */

/* Define to 1 for Encore UMAX 4.3 that has <inq_status/cpustats.h> instead of
   <sys/cpustats.h>. */
/* #undef UMAX4_3 */

/* Version number of package */
#define VERSION "4.3"

/* Define if using the dmalloc debugging malloc package */
/* #undef WITH_DMALLOC */

/* Define to 1 if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
/* # undef _ALL_SOURCE */
#endif

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
/* #  undef WORDS_BIGENDIAN */

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for 'stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Define to empty if 'const' does not conform to ANSI C. */
/* #undef const */

#ifdef __MINGW32__
# undef __USE_MINGW_ANSI_STDIO
# define __USE_MINGW_ANSI_STDIO 1
#endif

#include <sys/types.h>

/* Define to 'int' if <sys/types.h> doesn't define. */
#define gid_t int

/* Define to 'int' if <sys/types.h> does not define. */
/* GCC 4.x reportedly defines pid_t.  */
#ifndef _PID_T_
#ifdef _WIN64
#define pid_t __int64
#else
#define pid_t int
#endif
#endif

/* Define to `int' if <sys/types.h> does not define. */
#define ssize_t int

/* Define to 'int' if <sys/types.h> doesn't define. */
#define uid_t int

/* Define uintmax_t if not defined in <stdint.h> or <inttypes.h>. */
#if !HAVE_STDINT_H && !HAVE_INTTYPES_H
#define uintmax_t unsigned long
#endif

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
/* #undef HAVE_SYS_WAIT_H */

/* Define to the installation directory for locales.  */
#define LOCALEDIR ""

/*
 * Refer to README.W32 for info on the following settings
 */


/*
 * If you have a shell that does not grok 'sh -c quoted-command-line'
 * correctly, you need this setting. Please see below for specific
 * shell support.
 */
/*#define BATCH_MODE_ONLY_SHELL 1 */

/*
 * Define if you have the Cygnus "Cygwin" GNU Windows32 tool set.
 * Do NOT define BATCH_MODE_ONLY_SHELL if you define HAVE_CYGWIN_SHELL
 */
/*#define HAVE_CYGWIN_SHELL 1 */

/*
 * Define if you have the MKS tool set or shell. Do NOT define
 * BATCH_MODE_ONLY_SHELL if you define HAVE_MKS_SHELL
 */
/*#define HAVE_MKS_SHELL 1 */

/*
 * Enforce the mutual exclusivity restriction.
 */
#ifdef HAVE_MKS_SHELL
#undef BATCH_MODE_ONLY_SHELL
#endif

#ifdef HAVE_CYGWIN_SHELL
#undef BATCH_MODE_ONLY_SHELL
#endif
