/* config-top.h - various user-settable options not under the control of autoconf. */

/* Copyright (C) 2002-2021 Free Software Foundation, Inc.

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

/* Define CONTINUE_AFTER_KILL_ERROR if you want the kill command to
   continue processing arguments after one of them fails.  This is
   what POSIX.2 specifies. */
#define CONTINUE_AFTER_KILL_ERROR

/* Define BREAK_COMPLAINS if you want the non-standard, but useful
   error messages about `break' and `continue' out of context. */
#define BREAK_COMPLAINS

/* Define CD_COMPLAINS if you want the non-standard, but sometimes-desired
   error messages about multiple directory arguments to `cd'. */
#define CD_COMPLAINS

/* Define BUFFERED_INPUT if you want the shell to do its own input
   buffering, rather than using stdio.  Do not undefine this; it's
   required to preserve semantics required by POSIX. */
#define BUFFERED_INPUT

/* Define ONESHOT if you want sh -c 'command' to avoid forking to execute
   `command' whenever possible.  This is a big efficiency improvement. */
#define ONESHOT

/* Define V9_ECHO if you want to give the echo builtin backslash-escape
   interpretation using the -e option, in the style of the Bell Labs 9th
   Edition version of echo.  You cannot emulate the System V echo behavior
   without this option. */
#define V9_ECHO

/* Define DONT_REPORT_SIGPIPE if you don't want to see `Broken pipe' messages
   when a job like `cat jobs.c | exit 1' terminates due to a SIGPIPE. */
#define DONT_REPORT_SIGPIPE

/* Define DONT_REPORT_SIGTERM if you don't want to see `Terminates' message
   when a job exits due to SIGTERM, since that's the default signal sent
   by the kill builtin. */
#define DONT_REPORT_SIGTERM

/* Define DONT_REPORT_BROKEN_PIPE_WRITE_ERRORS if you don't want builtins
   like `echo' and `printf' to report errors when output does not succeed
   due to EPIPE. */
/* #define DONT_REPORT_BROKEN_PIPE_WRITE_ERRORS */

/* The default value of the PATH variable. */
#ifndef DEFAULT_PATH_VALUE
#define DEFAULT_PATH_VALUE \
  "/usr/local/bin:/usr/local/sbin:/usr/bin:/usr/sbin:/bin:/sbin:."
#endif

/* If you want to unconditionally set a value for PATH in every restricted
   shell, set this. */
/* #define RBASH_STATIC_PATH_VALUE "/rbin:/usr/rbin" */

/* The value for PATH when invoking `command -p'.  This is only used when
   the Posix.2 confstr () function, or CS_PATH define are not present. */
#ifndef STANDARD_UTILS_PATH
#define STANDARD_UTILS_PATH \
  "/bin:/usr/bin:/sbin:/usr/sbin:/etc:/usr/etc"
#endif

/* The default path for enable -f */
#ifndef DEFAULT_LOADABLE_BUILTINS_PATH
#define DEFAULT_LOADABLE_BUILTINS_PATH \
  "/usr/local/lib/bash:/usr/lib/bash:/opt/local/lib/bash:/usr/pkg/lib/bash:/opt/pkg/lib/bash:."
#endif

/* Default primary and secondary prompt strings. */
#define PPROMPT "\\s-\\v\\$ "
#define SPROMPT "> "

/* Undefine this if you don't want the ksh-compatible behavior of reprinting
   the select menu after a valid choice is made only if REPLY is set to NULL
   in the body of the select command.  The menu is always reprinted if the
   reply to the select query is an empty line. */
#define KSH_COMPATIBLE_SELECT

/* Default interactive shell startup file. */
#define DEFAULT_BASHRC "~/.bashrc"

/* System-wide .bashrc file for interactive shells. */
/* #define SYS_BASHRC "/etc/bash.bashrc" */

/* System-wide .bash_logout for login shells. */
/* #define SYS_BASH_LOGOUT "/etc/bash.bash_logout" */

/* Define this to make non-interactive shells begun with argv[0][0] == '-'
   run the startup files when not in posix mode. */
/* #define NON_INTERACTIVE_LOGIN_SHELLS */

/* Define this if you want bash to try to check whether it's being run by
   sshd and source the .bashrc if so (like the rshd behavior).  This checks
   for the presence of SSH_CLIENT or SSH2_CLIENT in the initial environment,
   which can be fooled under certain not-uncommon circumstances. */
/* #define SSH_SOURCE_BASHRC */

/* Define if you want the case-toggling operators (~[~]) and the
   `capcase' variable attribute (declare -c). */
/* TAG: bash-5.2 disable? */
#define CASEMOD_TOGGLECASE
#define CASEMOD_CAPCASE

/* This is used as the name of a shell function to call when a command
   name is not found.  If you want to name it something other than the
   default ("command_not_found_handle"), change it here. */
/* #define NOTFOUND_HOOK "command_not_found_handle" */

/* Define if you want each line saved to the history list in bashhist.c:
   bash_add_history() to be sent to syslog(). */
/* #define SYSLOG_HISTORY */
#if defined (SYSLOG_HISTORY)
#  define SYSLOG_FACILITY LOG_USER
#  define SYSLOG_LEVEL LOG_INFO
#  define OPENLOG_OPTS LOG_PID
#endif

/* Define if you want syslogging history to be controllable at runtime via a
   shell option; if defined, the value is the default for the syslog_history
   shopt option */
#if defined (SYSLOG_HISTORY)
/* #define SYSLOG_SHOPT 1 */
#endif

/* Define if you want to include code in shell.c to support wordexp(3) */
/* #define WORDEXP_OPTION */

/* Define as 1 if you want to enable code that implements multiple coprocs
   executing simultaneously */
#ifndef MULTIPLE_COPROCS
#  define MULTIPLE_COPROCS 0
#endif

/* Define to 0 if you want the checkwinsize option off by default, 1 if you
   want it on. */
#define CHECKWINSIZE_DEFAULT	1

/* Define to 1 if you want to optimize for sequential array assignment when
   using indexed arrays, 0 if you want bash-4.2 behavior, which favors
   random access but is O(N) for each array assignment. */
#define OPTIMIZE_SEQUENTIAL_ARRAY_ASSIGNMENT	1

/* Define to 1 if you want to be able to export indexed arrays to processes
   using the foo=([0]=one [1]=two) and so on */
/* #define ARRAY_EXPORT 1 */

/* Define to 1 if you want the shell to exit if it is running setuid and its
   attempt to drop privilege using setuid(getuid()) fails with errno == EAGAIN */
/* #define EXIT_ON_SETUID_FAILURE 1 */

/* Define to 1 if you want the shell to re-check $PATH if a hashed filename
   no longer exists.  This behavior is the default in Posix mode. */
#define CHECKHASH_DEFAULT 0

/* Define to the maximum level of recursion you want for the eval builtin
   and trap handlers (since traps are run as if run by eval).
   0 means the limit is not active. */
#define EVALNEST_MAX 0

/* Define to the maximum level of recursion you want for the source/. builtin.
   0 means the limit is not active. */
#define SOURCENEST_MAX 0

/* Define to use libc mktemp/mkstemp instead of replacements in lib/sh/tmpfile.c */
#define USE_MKTEMP
#define USE_MKSTEMP
#define USE_MKDTEMP

/* Define to force the value of OLDPWD inherited from the environment to be a
   directory */
#define OLDPWD_CHECK_DIRECTORY 1

/* Define to set the initial size of the history list ($HISTSIZE). This must
   be a string. */
/*#define HISTSIZE_DEFAULT "500"*/

/* Define to 0 if you want history expansion to be disabled by default in
   interactive shells; define to 1 for the historical behavior of enabling
   when the shell is interactive. */
#define HISTEXPAND_DEFAULT	1

/* Undefine or define to 0 if you don't want to allow associative array
   assignment using a compound list of key-value pairs. */
#define ASSOC_KVPAIR_ASSIGNMENT 1
