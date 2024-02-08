/* config.h.  Generated from config.h.in by configure.  */
/* config.h -- Configuration file for bash. */

/* Copyright (C) 1987-2009,2011-2012,2013-2019 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef _COSMO_SOURCE
#undef _COSMO_SOURCE
#endif

/* Template settings for autoconf */

/* Configuration feature settings controllable by autoconf. */

/* Define JOB_CONTROL if your operating system supports
   BSD-like job control. */
#define JOB_CONTROL 1

/* Define ALIAS if you want the alias features. */
#define ALIAS 1

/* Define PUSHD_AND_POPD if you want those commands to be compiled in.
   (Also the `dirs' commands.) */
#define PUSHD_AND_POPD 1

/* Define BRACE_EXPANSION if you want curly brace expansion a la Csh:
   foo{a,b} -> fooa foob.  Even if this is compiled in (the default) you
   can turn it off at shell startup with `-nobraceexpansion', or during
   shell execution with `set +o braceexpand'. */
#define BRACE_EXPANSION 1

/* Define READLINE to get the nifty/glitzy editing features.
   This is on by default.  You can turn it off interactively
   with the -nolineediting flag. */
#define READLINE 1

/* Define BANG_HISTORY if you want to have Csh style "!" history expansion.
   This is unrelated to READLINE. */
#define BANG_HISTORY 1

/* Define HISTORY if you want to have access to previously typed commands.

   If both HISTORY and READLINE are defined, you can get at the commands
   with line editing commands, and you can directly manipulate the history
   from the command line.

   If only HISTORY is defined, the `fc' and `history' builtins are
   available. */
#define HISTORY 1

/* Define this if you want completion that puts all alternatives into
   a brace expansion shell expression. */
#if defined (BRACE_EXPANSION) && defined (READLINE)
#  define BRACE_COMPLETION
#endif /* BRACE_EXPANSION */

/* Define DEFAULT_ECHO_TO_XPG if you want the echo builtin to interpret
   the backslash-escape characters by default, like the XPG Single Unix
   Specification V2 for echo.
   This requires that V9_ECHO be defined. */
/* #undef DEFAULT_ECHO_TO_XPG */

/* Define HELP_BUILTIN if you want the `help' shell builtin and the long
   documentation strings compiled into the shell. */
#define HELP_BUILTIN 1

/* Define RESTRICTED_SHELL if you want the generated shell to have the
   ability to be a restricted one.  The shell thus generated can become
   restricted by being run with the name "rbash", or by setting the -r
   flag. */
#define RESTRICTED_SHELL 1

/* Define DISABLED_BUILTINS if you want "builtin foo" to always run the
   shell builtin "foo", even if it has been disabled with "enable -n foo". */
/* #undef DISABLED_BUILTINS */

/* Define PROCESS_SUBSTITUTION if you want the K*rn shell-like process
   substitution features "<(file)". */
/* Right now, you cannot do this on machines without fully operational
   FIFO support.  This currently include NeXT and Alliant. */
#define PROCESS_SUBSTITUTION 1

/* Define PROMPT_STRING_DECODE if you want the backslash-escaped special
   characters in PS1 and PS2 expanded.  Variable expansion will still be
   performed. */
#define PROMPT_STRING_DECODE 1

/* Define SELECT_COMMAND if you want the Korn-shell style `select' command:
	select word in word_list; do command_list; done */
#define SELECT_COMMAND 1

/* Define COMMAND_TIMING of you want the ksh-style `time' reserved word and
   the ability to time pipelines, functions, and builtins. */
#define COMMAND_TIMING 1

/* Define ARRAY_VARS if you want ksh-style one-dimensional array variables. */
#define ARRAY_VARS 1

/* Define DPAREN_ARITHMETIC if you want the ksh-style ((...)) arithmetic
   evaluation command. */
#define DPAREN_ARITHMETIC 1

/* Define EXTENDED_GLOB if you want the ksh-style [*+@?!](patlist) extended
   pattern matching. */
#define EXTENDED_GLOB 1

/* Define EXTGLOB_DEFAULT to the value you'd like the extglob shell option
   to have by default */
#define EXTGLOB_DEFAULT 0

/* Define COND_COMMAND if you want the ksh-style [[...]] conditional
   command. */
#define COND_COMMAND 1

/* Define COND_REGEXP if you want extended regular expression matching and the
   =~ binary operator in the [[...]] conditional command. */
#define COND_REGEXP 1

/* Define COPROCESS_SUPPORT if you want support for ksh-like coprocesses and
   the `coproc' reserved word */
#define COPROCESS_SUPPORT 1

/* Define ARITH_FOR_COMMAND if you want the ksh93-style
	for (( init; test; step )) do list; done
   arithmetic for command. */
#define ARITH_FOR_COMMAND 1

/* Define NETWORK_REDIRECTIONS if you want /dev/(tcp|udp)/host/port to open
   socket connections when used in redirections */
#define NETWORK_REDIRECTIONS 1

/* Define PROGRAMMABLE_COMPLETION for the programmable completion features
   and the complete builtin. */
#define PROGRAMMABLE_COMPLETION 1

/* Define NO_MULTIBYTE_SUPPORT to not compile in support for multibyte
   characters, even if the OS supports them. */
/* #undef NO_MULTIBYTE_SUPPORT */

/* Define DEBUGGER if you want to compile in some features used only by the 
   bash debugger. */
#define DEBUGGER 1

/* Define STRICT_POSIX if you want bash to be strictly posix.2 conformant by
   default (except for echo; that is controlled separately). */
/* #undef STRICT_POSIX */

/* Define MEMSCRAMBLE if you want the bash malloc and free to scramble
   memory contents on malloc() and free(). */
#define MEMSCRAMBLE 1

/* Define for case-modifying variable attributes; variables modified on
   assignment */
#define CASEMOD_ATTRS 1

/* Define for case-modifying word expansions */
#define CASEMOD_EXPANSIONS 1

/* Define to make the `direxpand' shopt option enabled by default. */
/* #undef DIRCOMPLETE_EXPAND_DEFAULT */

/* Define to make the `globasciiranges' shopt option enabled by default. */
#define GLOBASCII_DEFAULT 1

/* Define to allow functions to be imported from the environment. */
#define FUNCTION_IMPORT 1

/* Define AFS if you are using Transarc's AFS. */
/* #undef AFS */

/* #undef ENABLE_NLS */

/* End of configuration settings controllable by autoconf. */
/* Other settable options appear in config-top.h. */

#include "config-top.h"

/* Beginning of autoconf additions. */

/* Characteristics of the C compiler */
/* #undef const */

/* #undef inline */

#define restrict __restrict__

/* #undef volatile */

/* Define if cpp supports the ANSI-C stringizing `#' operator */
#define HAVE_STRINGIZE 1

/* Define if the compiler supports `long double' variables. */
#define HAVE_LONG_DOUBLE 1

#define PROTOTYPES 1
#define __PROTOTYPES 1

/* #undef __CHAR_UNSIGNED__ */

/* Define if the compiler supports `long long int' variables. */
#define HAVE_LONG_LONG_INT 1

#define HAVE_UNSIGNED_LONG_LONG_INT 1

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 8

/* The number of bytes in a pointer to char.  */
#define SIZEOF_CHAR_P 8

/* The number of bytes in a size_t.  */
#define SIZEOF_SIZE_T 8

/* The number of bytes in a double (hopefully 8). */
#define SIZEOF_DOUBLE 8

/* The number of bytes in an `intmax_t'. */
#define SIZEOF_INTMAX_T 8

/* The number of bytes in a `long long', if we have one. */
#define SIZEOF_LONG_LONG 8

/* The number of bytes in a `wchar_t', if supported */
#define SIZEOF_WCHAR_T 4

/* System paths */

#define DEFAULT_MAIL_DIRECTORY "/var/mail"

/* Characteristics of the system's header files and libraries that affect
   the compilation environment. */

/* These are set by AC_USE_SYSTEM_EXTENSIONS */

/* Define if the system does not provide POSIX.1 features except
   with this defined.  */
/* #undef _POSIX_1_SOURCE */

/* Define if you need to in order for stat and other things to work.  */
/* #undef _POSIX_SOURCE */

/* Define to use GNU libc extensions. */
#define _GNU_SOURCE 1

/* Define to enable general system extensions on Solaris. */
#define __EXTENSIONS__ 1

/* General system extensions on AIX */
#define _ALL_SOURCE 1

#define _POSIX_PTHREAD_SEMANTICS 1
#define _TANDEM_SOURCE 1
/* #undef _MINIX */

/* Memory management functions. */

/* Define if using the bash version of malloc in lib/malloc/malloc.c */
/* #undef USING_BASH_MALLOC */

/* #undef DISABLE_MALLOC_WRAPPERS */

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#define HAVE_ALLOCA_H 1

/* Define if major/minor/makedev is defined in <sys/mkdev.h> */
/* #undef MAJOR_IN_MAKEDEV */

/* Define if major/minor/makedev is defined in <sys/sysmacros.h> */
#define MAJOR_IN_SYSMACROS 1

/* SYSTEM TYPES */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef mode_t */

/* Define to `int' if <signal.h> doesn't define. */
/* #undef sigset_t */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define to `short' if <sys/types.h> doesn't define.  */
#define bits16_t short

/* Define to `unsigned short' if <sys/types.h> doesn't define.  */
#define u_bits16_t unsigned short

/* Define to `int' if <sys/types.h> doesn't define.  */
#define bits32_t int

/* Define to `unsigned int' if <sys/types.h> doesn't define.  */
#define u_bits32_t unsigned int

/* Define to `double' if <sys/types.h> doesn't define. */
#define bits64_t char *

/* Define to `unsigned int' if <sys/types.h> doesn't define. */
/* #undef u_int */

/* Define to `unsigned long' if <sys/types.h> doesn't define.  */
/* #undef u_long */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef ptrdiff_t */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef ssize_t */

/* Define to `long' if <stdint.h> doesn't define. */
/* #undef intmax_t */

/* Define to `unsigned long' if <stdint.h> doesn't define. */
/* #undef uintmax_t */

/* Define to integer type wide enough to hold a pointer if <stdint.h> doesn't define. */
/* #undef uintptr_t */
 
/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef uid_t */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef clock_t */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef time_t */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef gid_t */

/* Define to `unsigned int' if <sys/socket.h> doesn't define. */
/* #undef socklen_t */

/* Define to `int' if <signal.h> doesn't define. */
/* #undef sig_atomic_t */

#define HAVE_MBSTATE_T 1

/* Define if you have quad_t in <sys/types.h>. */
#define HAVE_QUAD_T 1

/* Define if you have wchar_t in <wctype.h>. */
#define HAVE_WCHAR_T 1

/* Define if you have wctype_t in <wctype.h>. */
#define HAVE_WCTYPE_T 1

/* Define if you have wint_t in <wctype.h>. */
#define HAVE_WINT_T 1

#define RLIMTYPE rlim_t

/* Define to the type of elements in the array set by `getgroups'.
   Usually this is either `int' or `gid_t'.  */
#define GETGROUPS_T gid_t

/* Characteristics of the machine archictecture. */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if the machine architecture is big-endian. */
/* #undef WORDS_BIGENDIAN */

/* Check for the presence of certain non-function symbols in the system
   libraries. */

/* Define if `sys_siglist' is declared by <signal.h> or <unistd.h>.  */
#define HAVE_DECL_SYS_SIGLIST 0
/* #undef SYS_SIGLIST_DECLARED */

/* Define if `_sys_siglist' is declared by <signal.h> or <unistd.h>.  */
/* #undef UNDER_SYS_SIGLIST_DECLARED */

/* #undef HAVE_SYS_SIGLIST */

/* #undef HAVE_UNDER_SYS_SIGLIST */

#define HAVE_SYS_ERRLIST 1

/* #undef HAVE_TZNAME */
/* #undef HAVE_DECL_TZNAME */

/* Characteristics of some of the system structures. */

#define HAVE_STRUCT_DIRENT_D_INO 1

/* #undef HAVE_STRUCT_DIRENT_D_FILENO */

/* #undef HAVE_STRUCT_DIRENT_D_NAMLEN */

/* #undef TIOCSTAT_IN_SYS_IOCTL */

#define FIONREAD_IN_SYS_IOCTL 1

/* #undef GWINSZ_IN_SYS_IOCTL */

#define STRUCT_WINSIZE_IN_SYS_IOCTL 1

/* #undef TM_IN_SYS_TIME */

/* #undef STRUCT_WINSIZE_IN_TERMIOS */

#define SPEED_T_IN_SYS_TYPES 1

/* #undef TERMIOS_LDISC */

/* #undef TERMIO_LDISC */

#define HAVE_STRUCT_STAT_ST_BLOCKS 1

#define HAVE_STRUCT_TM_TM_ZONE 1
#define HAVE_TM_ZONE 1

#define HAVE_TIMEVAL 1

#define HAVE_STRUCT_TIMEZONE 1

#define WEXITSTATUS_OFFSET 8

#define HAVE_STRUCT_TIMESPEC 1
#define TIME_H_DEFINES_STRUCT_TIMESPEC 1
/* #undef SYS_TIME_H_DEFINES_STRUCT_TIMESPEC */
/* #undef PTHREAD_H_DEFINES_STRUCT_TIMESPEC */

#define HAVE_STRUCT_STAT_ST_ATIM_TV_NSEC 1
#define TYPEOF_STRUCT_STAT_ST_ATIM_IS_STRUCT_TIMESPEC 1
/* #undef HAVE_STRUCT_STAT_ST_ATIMESPEC_TV_NSEC */
/* #undef HAVE_STRUCT_STAT_ST_ATIMENSEC */
/* #undef HAVE_STRUCT_STAT_ST_ATIM_ST__TIM_TV_NSEC */

/* Characteristics of definitions in the system header files. */

#define HAVE_GETPW_DECLS 1

/* #undef HAVE_RESOURCE */

/* #undef HAVE_LIBC_FNM_EXTMATCH */

/* Define if you have <linux/audit.h> and it defines AUDIT_USER_TTY */
#define HAVE_DECL_AUDIT_USER_TTY 0

#define HAVE_DECL_CONFSTR 1

#define HAVE_DECL_PRINTF 1

#define HAVE_DECL_SBRK 0

#define HAVE_DECL_STRCPY 1

#define HAVE_DECL_STRSIGNAL 1

#define HAVE_DECL_STRTOLD 1

/* #undef PRI_MACROS_BROKEN */

/* #undef STRTOLD_BROKEN */

/* Define if WCONTINUED is defined in system headers, but rejected by waitpid */
/* #undef WCONTINUED_BROKEN */

/* These are checked with BASH_CHECK_DECL */

#define HAVE_DECL_STRTOIMAX 1
#define HAVE_DECL_STRTOL 1
#define HAVE_DECL_STRTOLL 1
#define HAVE_DECL_STRTOUL 1
#define HAVE_DECL_STRTOULL 1
#define HAVE_DECL_STRTOUMAX 1

/* Characteristics of system calls and C library functions. */

/* Define if the `getpgrp' function takes no argument.  */
#define GETPGRP_VOID 1

#define NAMED_PIPES_MISSING 1

/* #undef OPENDIR_NOT_ROBUST */

#define PGRP_PIPE 1

/* #undef STAT_MACROS_BROKEN */

/* #undef ULIMIT_MAXFDS */

#define CAN_REDEFINE_GETENV 1

#define HAVE_STD_PUTENV 1

#define HAVE_STD_UNSETENV 1

#define HAVE_PRINTF_A_FORMAT 1

/* Define if you have <langinfo.h> and nl_langinfo(CODESET). */
#define HAVE_LANGINFO_CODESET 1

/* Characteristics of properties exported by the kernel. */

/* Define if the kernel can exec files beginning with #! */
#define HAVE_HASH_BANG_EXEC 1

/* Define if you have the /dev/fd devices to map open files into the file system. */
#define HAVE_DEV_FD 1

/* Defined to /dev/fd or /proc/self/fd (linux). */
#define DEV_FD_PREFIX "/dev/fd/"

/* Define if you have the /dev/stdin device. */
#define HAVE_DEV_STDIN 1

/* The type of iconv's `inbuf' argument */
#define ICONV_CONST 

/* Type and behavior of signal handling functions. */

/* #undef MUST_REINSTALL_SIGHANDLERS */

/* #undef HAVE_BSD_SIGNALS */

#define HAVE_POSIX_SIGNALS 1

/* #undef HAVE_USG_SIGHOLD */

/* #undef UNUSABLE_RT_SIGNALS */

/* Presence of system and C library functions. */

/* Define if you have the arc4random function.  */
/* #undef HAVE_ARC4RANDOM */

/* Define if you have the asprintf function.  */
#define HAVE_ASPRINTF 1

/* Define if you have the bcopy function.  */
#define HAVE_BCOPY 1

/* Define if you have the bzero function.  */
#define HAVE_BZERO 1

/* Define if you have the chown function.  */
#define HAVE_CHOWN 1

/* Define if you have the confstr function.  */
#define HAVE_CONFSTR 1

/* Define if you have the dlclose function.  */
/* #undef HAVE_DLCLOSE */

/* Define if you have the dlopen function.  */
/* #undef HAVE_DLOPEN */

/* Define if you have the dlsym function.  */
/* #undef HAVE_DLSYM */

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if you have the dprintf function.  */
#define HAVE_DPRINTF 1

/* Define if you have the dup2 function.  */
#define HAVE_DUP2 1

/* Define if you have the eaccess function.  */
#define HAVE_EACCESS 1

/* Define if you have the faccessat function.  */
#define HAVE_FACCESSAT 1

/* Define if you have the fcntl function.  */
#define HAVE_FCNTL 1

/* Define if you have the fnmatch function.  */
#define HAVE_FNMATCH 1

/* Can fnmatch be used as a fallback to match [=equiv=] with collation weights? */
#define FNMATCH_EQUIV_FALLBACK 0

/* Define if you have the fpurge/__fpurge function.  */
#define HAVE_FPURGE 1
#define HAVE___FPURGE 1
#define HAVE_DECL_FPURGE 1

/* Define if you have the getaddrinfo function. */
#define HAVE_GETADDRINFO 1

/* Define if you have the getcwd function.  */
#define HAVE_GETCWD 1

/* Define if you have the getentropy function.  */
#define HAVE_GETENTROPY 1

/* Define if you have the getdtablesize function.  */
#define HAVE_GETDTABLESIZE 1

/* Define if you have the getgroups function.  */
#define HAVE_GETGROUPS 1

/* Define if you have the gethostbyname function.  */
#define HAVE_GETHOSTBYNAME 1

/* Define if you have the gethostname function.  */
#define HAVE_GETHOSTNAME 1

/* Define if you have the getpagesize function.  */
#define HAVE_GETPAGESIZE 1

/* Define if you have the getpeername function.  */
#define HAVE_GETPEERNAME 1

/* Define if you have the getpwent function. */
#define HAVE_GETPWENT 1

/* Define if you have the getpwnam function. */
#define HAVE_GETPWNAM 1

/* Define if you have the getpwuid function. */
#define HAVE_GETPWUID 1

/* Define if you have the getrandom function.  */
#define HAVE_GETRANDOM 1

/* Define if you have the getrlimit function.  */
#define HAVE_GETRLIMIT 1

/* Define if you have the getrusage function.  */
#define HAVE_GETRUSAGE 1

/* Define if you have the getservbyname function.  */
#define HAVE_GETSERVBYNAME 1

/* Define if you have the getservent function.  */
#define HAVE_GETSERVENT 1

/* Define if you have the gettimeofday function.  */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the getwd function.  */
/* #undef HAVE_GETWD */

/* Define if you have the iconv function.  */
#define HAVE_ICONV 1

/* Define if you have the imaxdiv function.  */
#define HAVE_IMAXDIV 1

/* Define if you have the inet_aton function.  */
#define HAVE_INET_ATON 1

/* Define if you have the isascii function. */
#define HAVE_ISASCII 1

/* Define if you have the isblank function.  */
#define HAVE_ISBLANK 1

/* Define if you have the isgraph function.  */
#define HAVE_ISGRAPH 1

/* Define if you have the isprint function.  */
#define HAVE_ISPRINT 1

/* Define if you have the isspace function.  */
#define HAVE_ISSPACE 1

/* Define if you have the iswctype function.  */
#define HAVE_ISWCTYPE 1

/* Define if you have the iswlower function.  */
#define HAVE_ISWLOWER 1

/* Define if you have the iswupper function.  */
#define HAVE_ISWUPPER 1

/* Define if you have the isxdigit function.  */
#define HAVE_ISXDIGIT 1

/* Define if you have the kill function.  */
#define HAVE_KILL 1

/* Define if you have the killpg function.  */
#define HAVE_KILLPG 1

/* Define if you have the lstat function. */
#define HAVE_LSTAT 1

/* Define if you have the locale_charset function. */
/* #undef HAVE_LOCALE_CHARSET */

/* Define if you have the mbrlen function. */
#define HAVE_MBRLEN 1

/* Define if you have the mbrtowc function. */
#define HAVE_MBRTOWC 1

/* Define if you have the mbscasecmp function. */
/* #undef HAVE_MBSCASECMP */

/* Define if you have the mbschr function. */
/* #undef HAVE_MBSCHR */

/* Define if you have the mbscmp function. */
/* #undef HAVE_MBSCMP */

/* Define if you have the mbsnrtowcs function. */
#define HAVE_MBSNRTOWCS 1

/* Define if you have the mbsrtowcs function. */
#define HAVE_MBSRTOWCS 1

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE 1

/* Define if you have the memset function.  */
#define HAVE_MEMSET 1

/* Define if you have the mkdtemp function.  */
#define HAVE_MKDTEMP 1

/* Define if you have the mkfifo function.  */
/* #undef HAVE_MKFIFO */

/* Define if you have the mkstemp function.  */
#define HAVE_MKSTEMP 1

/* Define if you have the pathconf function. */
#define HAVE_PATHCONF 1

/* Define if you have the pselect function.  */
#define HAVE_PSELECT 1

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the raise function. */
#define HAVE_RAISE 1

/* Define if you have the random function. */
#define HAVE_RANDOM 1

/* Define if you have the readlink function. */
#define HAVE_READLINK 1

/* Define if you have the regcomp function. */
#define HAVE_REGCOMP 1

/* Define if you have the regexec function. */
#define HAVE_REGEXEC 1

/* Define if you have the rename function. */
#define HAVE_RENAME 1

/* Define if you have the sbrk function. */
/* #undef HAVE_SBRK */

/* Define if you have the select function.  */
#define HAVE_SELECT 1

/* Define if you have the setdtablesize function.  */
/* #undef HAVE_SETDTABLESIZE */

/* Define if you have the setenv function.  */
#define HAVE_SETENV 1

/* Define if you have the setitimer function.  */
#define HAVE_SETITIMER 1

/* Define if you have the setlinebuf function.  */
#define HAVE_SETLINEBUF 1

/* Define if you have the setlocale function.  */
#define HAVE_SETLOCALE 1

/* Define if you have the setostype function.  */
/* #undef HAVE_SETOSTYPE */

/* Define if you have the setregid function.  */
/* #undef HAVE_SETREGID */
#define HAVE_DECL_SETREGID 1

/* Define if you have the setregid function.  */
#define HAVE_SETRESGID 1
/* #undef HAVE_DECL_SETRESGID */

/* Define if you have the setresuid function.  */
#define HAVE_SETRESUID 1
/* #undef HAVE_DECL_SETRESUID */

/* Define if you have the setvbuf function.  */
#define HAVE_SETVBUF 1

/* Define if you have the siginterrupt function.  */
#define HAVE_SIGINTERRUPT 1

/* Define if you have the POSIX.1-style sigsetjmp function.  */
#define HAVE_POSIX_SIGSETJMP 1

/* Define if you have the snprintf function.  */
#define HAVE_SNPRINTF 1

/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strcasestr function.  */
#define HAVE_STRCASESTR 1

/* Define if you have the strchr function.  */
#define HAVE_STRCHR 1

/* Define if you have the strchrnul function.  */
#define HAVE_STRCHRNUL 1

/* Define if you have the strcoll function.  */
#define HAVE_STRCOLL 1

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strftime function. */
#define HAVE_STRFTIME 1

/* Define if you have the strnlen function. */
#define HAVE_STRNLEN 1

/* Define if you have the strpbrk function. */
#define HAVE_STRPBRK 1

/* Define if you have the strstr function. */
#define HAVE_STRSTR 1

/* Define if you have the strtod function. */
#define HAVE_STRTOD 1

/* Define if you have the strtoimax function. */
#define HAVE_STRTOIMAX 1

/* Define if you have the strtol function. */
#define HAVE_STRTOL 1

/* Define if you have the strtoll function. */
#define HAVE_STRTOLL 1

/* Define if you have the strtoul function. */
#define HAVE_STRTOUL 1

/* Define if you have the strtoull function. */
#define HAVE_STRTOULL 1

/* Define if you have the strtoumax function. */
#define HAVE_STRTOUMAX 1

/* Define if you have the strsignal function or macro. */
#define HAVE_STRSIGNAL 1

/* Define if you have the sysconf function. */
#define HAVE_SYSCONF 1

/* Define if you have the syslog function. */
#define HAVE_SYSLOG 1

/* Define if you have the tcgetattr function.  */
#define HAVE_TCGETATTR 1

/* Define if you have the tcgetpgrp function.  */
#define HAVE_TCGETPGRP 1

/* Define if you have the times function.  */
#define HAVE_TIMES 1

/* Define if you have the towlower function.  */
#define HAVE_TOWLOWER 1

/* Define if you have the towupper function.  */
#define HAVE_TOWUPPER 1

/* Define if you have the ttyname function.  */
#define HAVE_TTYNAME 1

/* Define if you have the tzset function. */
#define HAVE_TZSET 1

/* Define if you have the ulimit function. */
/* #undef HAVE_ULIMIT */

/* Define if you have the uname function. */
#define HAVE_UNAME 1

/* Define if you have the unsetenv function.  */
#define HAVE_UNSETENV 1

/* Define if you have the vasprintf function.  */
#define HAVE_VASPRINTF 1

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define if you have the vsnprintf function.  */
#define HAVE_VSNPRINTF 1

/* Define if you have the waitpid function. */
#define HAVE_WAITPID 1

/* Define if you have the wait3 function.  */
#define HAVE_WAIT3 1

/* Define if you have the wcrtomb function.  */
#define HAVE_WCRTOMB 1

/* Define if you have the wcscoll function.  */
#define HAVE_WCSCOLL 1

/* Define if you have the wcsdup function.  */
#define HAVE_WCSDUP 1

/* Define if you have the wctype function.  */
#define HAVE_WCTYPE 1

/* Define if you have the wcswidth function.  */
#define HAVE_WCSWIDTH 1

/* Define if you have the wcwidth function.  */
#define HAVE_WCWIDTH 1

/* and if it works */
/* #undef WCWIDTH_BROKEN */

/* Presence of certain system include files. */

/* Define if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define if you have the <dirent.h> header file.  */
#define HAVE_DIRENT_H 1

/* Define if you have the <dlfcn.h> header file.  */
#define HAVE_DLFCN_H 1

/* Define if you have the <grp.h> header file.  */
#define HAVE_GRP_H 1

/* Define if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have the <langinfo.h> header file.  */
#define HAVE_LANGINFO_H 1

/* Define if you have the <libaudit.h> header file. */
/* #undef HAVE_LIBAUDIT_H */

/* Define if you have the <libintl.h> header file. */
/* #undef HAVE_LIBINTL_H */

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <locale.h> header file.  */
#define HAVE_LOCALE_H 1

/* Define if you have the <mbstr.h> header file.  */
/* #undef HAVE_MBSTR_H */

/* Define if you have the <ndir.h> header file.  */
/* #undef HAVE_NDIR_H */

/* Define if you have the <netdh.h> header file. */
#define HAVE_NETDB_H 1

/* Define if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define if you have the <pwd.h> header file. */
#define HAVE_PWD_H 1

/* Define if you have the <regex.h> header file. */
#define HAVE_REGEX_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <stdarg.h> header file.  */
#define HAVE_STDARG_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <strings.h> header file.  */
#define HAVE_STRINGS_H 1

/* Define if you have the <memory.h> header file.  */
#define HAVE_MEMORY_H 1

/* Define if you have the <stdbool.h> header file. */
#define HAVE_STDBOOL_H 1

/* Define if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define if you have the <sys/dir.h> header file.  */
/* #undef HAVE_SYS_DIR_H */

/* Define if you have the <sys/file.h> header file.  */
#define HAVE_SYS_FILE_H 1

/* Define if you have the <sys/ioctl.h> header file.  */
#define HAVE_SYS_IOCTL_H 1

/* Define if you have the <sys/mman.h> header file.  */
#define HAVE_SYS_MMAN_H 1

/* Define if you have the <sys/ndir.h> header file.  */
/* #undef HAVE_SYS_NDIR_H */

/* Define if you have the <sys/param.h> header file.  */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/pte.h> header file.  */
/* #undef HAVE_SYS_PTE_H */

/* Define if you have the <sys/ptem.h> header file.  */
/* #undef HAVE_SYS_PTEM_H */

/* Define if you have the <sys/random.h> header file.  */
#define HAVE_SYS_RANDOM_H 1

/* Define if you have the <sys/resource.h> header file.  */
#define HAVE_SYS_RESOURCE_H 1

/* Define if you have the <sys/select.h> header file.  */
#define HAVE_SYS_SELECT_H 1

/* Define if you have the <sys/socket.h> header file.  */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/stream.h> header file.  */
/* #undef HAVE_SYS_STREAM_H */

/* Define if you have <sys/time.h> */
#define HAVE_SYS_TIME_H 1

/* Define if you have <sys/times.h> */
#define HAVE_SYS_TIMES_H 1

/* Define if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#define HAVE_SYS_WAIT_H 1

/* Define if you have the <termcap.h> header file.  */
/* #undef HAVE_TERMCAP_H */

/* Define if you have the <termio.h> header file.  */
/* #undef HAVE_TERMIO_H */

/* Define if you have the <termios.h> header file.  */
#define HAVE_TERMIOS_H 1

/* Define if you have the <ulimit.h> header file.  */
/* #undef HAVE_ULIMIT_H */

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the <varargs.h> header file.  */
/* #undef HAVE_VARARGS_H */

/* Define if you have the <wchar.h> header file.  */
#define HAVE_WCHAR_H 1

/* Define if you have the <varargs.h> header file.  */
#define HAVE_WCTYPE_H 1

/* Presence of certain system libraries. */

/* #undef HAVE_LIBDL */

/* #undef HAVE_LIBSUN */

/* #undef HAVE_LIBSOCKET */

/* Are we running the GNU C library, version 2.1 or later? */
/* #undef GLIBC21 */

/* Are we running SVR5 (UnixWare 7)? */
/* #undef SVR5 */

/* Are we running SVR4.2? */
/* #undef SVR4_2 */

/* Are we running some version of SVR4? */
/* #undef SVR4 */

/* Define if job control is unusable or unsupported. */
/* #undef JOB_CONTROL_MISSING */

/* Do we need to define _KERNEL to get the RLIMIT_* defines from
   <sys/resource.h>? */
/* #undef RLIMIT_NEEDS_KERNEL */

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Do strcoll(3) and strcmp(3) give different results in the default locale? */
/* #undef STRCOLL_BROKEN */

/* #undef DUP2_BROKEN */

/* #undef GETCWD_BROKEN */

#define DEV_FD_STAT_BROKEN

/* An array implementation that prioritizes speed (O(1) access) over space,
   in array2.c */
/* #undef ALT_ARRAY_IMPLEMENTATION */

/* Support for $"..." translatable strings. */
#define TRANSLATABLE_STRINGS 1

/* Additional defines for configuring lib/intl, maintained by autoscan/autoheader */

/* Define if you have the <argz.h> header file. */
/* #undef HAVE_ARGZ_H */

/* Define if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define if you have the <stdio_ext.h> header file. */
#define HAVE_STDIO_EXT_H 1

/* Define if you have the `dcgettext' function. */
/* #undef HAVE_DCGETTEXT */

/* Define if you have the `localeconv' function. */
#define HAVE_LOCALECONV 1

/* Define if your system has a working `malloc' function. */
/* #undef HAVE_MALLOC */

/* Define if you have the `mempcpy' function. */
#define HAVE_MEMPCPY 1

/* Define if you have a working `mmap' system call. */
/* #undef HAVE_MMAP */

/* Define if you have the `mremap' function. */
/* #undef HAVE_MREMAP */

/* Define if you have the `munmap' function. */
#define HAVE_MUNMAP 1

/* Define if you have the `nl_langinfo' function. */
/* #undef HAVE_NL_LANGINFO */

/* Define if you have the `stpcpy' function. */
#define HAVE_STPCPY 1

/* Define if you have the `strcspn' function. */
#define HAVE_STRCSPN 1

/* Define if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define if you have the `__argz_count' function. */
/* #undef HAVE___ARGZ_COUNT */

/* Define if you have the `__argz_next' function. */
/* #undef HAVE___ARGZ_NEXT */

/* Define if you have the `__argz_stringify' function. */
/* #undef HAVE___ARGZ_STRINGIFY */

/* End additions for lib/intl */

#include "config-bot.h"

#endif /* _CONFIG_H_ */
