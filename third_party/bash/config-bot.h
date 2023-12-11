/* config-bot.h */
/* modify settings or make new ones based on what autoconf tells us. */

/* Copyright (C) 1989-2021 Free Software Foundation, Inc.

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

/*********************************************************/
/* Modify or set defines based on the configure results. */
/*********************************************************/

#if !defined (HAVE_VPRINTF) && defined (HAVE_DOPRNT)
#  define USE_VFPRINTF_EMULATION
#  define HAVE_VPRINTF
#endif

#if defined (HAVE_SYS_RESOURCE_H) && defined (HAVE_GETRLIMIT)
#  define HAVE_RESOURCE
#endif

#if !defined (GETPGRP_VOID)
#  define HAVE_BSD_PGRP
#endif

/* Try this without testing __STDC__ for the time being. */
#if defined (HAVE_STDARG_H)
#  define PREFER_STDARG
#  define USE_VARARGS
#else
#  if defined (HAVE_VARARGS_H)
#    define PREFER_VARARGS
#    define USE_VARARGS
#  endif
#endif

#if defined (HAVE_SYS_SOCKET_H) && defined (HAVE_GETPEERNAME) && defined (HAVE_NETINET_IN_H)
#  define HAVE_NETWORK
#endif

#if defined (HAVE_REGEX_H) && defined (HAVE_REGCOMP) && defined (HAVE_REGEXEC)
#  define HAVE_POSIX_REGEXP
#endif

/* backwards compatibility between different autoconf versions */
#if HAVE_DECL_SYS_SIGLIST && !defined (SYS_SIGLIST_DECLARED)
#  define SYS_SIGLIST_DECLARED
#endif

/***********************************************************************/
/* Unset defines based on what configure reports as missing or broken. */
/***********************************************************************/

/* Ultrix botches type-ahead when switching from canonical to
   non-canonical mode, at least through version 4.3 */
#if !defined (HAVE_TERMIOS_H) || !defined (HAVE_TCGETATTR) || defined (ultrix)
#  define TERMIOS_MISSING
#endif

/* If we have a getcwd(3), but one that does not dynamically allocate memory,
   #undef HAVE_GETCWD so the replacement in getcwd.c will be built.  We do
   not do this on Solaris, because their implementation of loopback mounts
   breaks the traditional file system assumptions that getcwd uses. */
#if defined (HAVE_GETCWD) && defined (GETCWD_BROKEN) && !defined (SOLARIS)
#  undef HAVE_GETCWD
#endif

#if !defined (HAVE_DEV_FD) && defined (NAMED_PIPES_MISSING)
#  undef PROCESS_SUBSTITUTION
#endif

#if defined (JOB_CONTROL_MISSING)
#  undef JOB_CONTROL
#endif

#if defined (STRCOLL_BROKEN)
#  undef HAVE_STRCOLL
#endif

#if !defined (HAVE_POSIX_REGEXP)
#  undef COND_REGEXP
#endif

#if !HAVE_MKSTEMP
#  undef USE_MKSTEMP
#endif

#if !HAVE_MKDTEMP
#  undef USE_MKDTEMP
#endif

/* If the shell is called by this name, it will become restricted. */
#if defined (RESTRICTED_SHELL)
#  define RESTRICTED_SHELL_NAME "rbash"
#endif

/***********************************************************/
/* Make sure feature defines have necessary prerequisites. */
/***********************************************************/

/* BANG_HISTORY requires HISTORY. */
#if defined (BANG_HISTORY) && !defined (HISTORY)
#  define HISTORY
#endif /* BANG_HISTORY && !HISTORY */

#if defined (READLINE) && !defined (HISTORY)
#  define HISTORY
#endif

#if defined (PROGRAMMABLE_COMPLETION) && !defined (READLINE)
#  undef PROGRAMMABLE_COMPLETION
#endif

#if !defined (V9_ECHO)
#  undef DEFAULT_ECHO_TO_XPG
#endif

#if !defined (PROMPT_STRING_DECODE)
#  undef PPROMPT
#  define PPROMPT "$ "
#endif

#if !defined (HAVE_SYSLOG) || !defined (HAVE_SYSLOG_H)
#  undef SYSLOG_HISTORY
#endif

/************************************************/
/* check multibyte capability for I18N code	*/
/************************************************/

/* For platforms which support the ISO C amendment 1 functionality we
   support user defined character classes.  */
/* Solaris 2.5 has a bug: <wchar.h> must be included before <wctype.h>.  */
#if defined (HAVE_WCTYPE_H) && defined (HAVE_WCHAR_H) && defined (HAVE_LOCALE_H)
#  include <wchar.h>
#  include <wctype.h>
#  if defined (HAVE_ISWCTYPE) && \
      defined (HAVE_ISWLOWER) && \
      defined (HAVE_ISWUPPER) && \
      defined (HAVE_MBSRTOWCS) && \
      defined (HAVE_MBRTOWC) && \
      defined (HAVE_MBRLEN) && \
      defined (HAVE_TOWLOWER) && \
      defined (HAVE_TOWUPPER) && \
      defined (HAVE_WCHAR_T) && \
      defined (HAVE_WCTYPE_T) && \
      defined (HAVE_WINT_T) && \
      defined (HAVE_WCWIDTH) && \
      defined (HAVE_WCTYPE)
     /* system is supposed to support XPG5 */
#    define HANDLE_MULTIBYTE      1
#  endif
#endif

/* If we don't want multibyte chars even on a system that supports them, let
   the configuring user turn multibyte support off. */
#if defined (NO_MULTIBYTE_SUPPORT)
#  undef HANDLE_MULTIBYTE
#endif

/* Some systems, like BeOS, have multibyte encodings but lack mbstate_t.  */
#if HANDLE_MULTIBYTE && !defined (HAVE_MBSTATE_T)
#  define wcsrtombs(dest, src, len, ps) (wcsrtombs) (dest, src, len, 0)
#  define mbsrtowcs(dest, src, len, ps) (mbsrtowcs) (dest, src, len, 0)
#  define wcrtomb(s, wc, ps) (wcrtomb) (s, wc, 0)
#  define mbrtowc(pwc, s, n, ps) (mbrtowc) (pwc, s, n, 0)
#  define mbrlen(s, n, ps) (mbrlen) (s, n, 0)
#  define mbstate_t int
#endif

/* Make sure MB_LEN_MAX is at least 16 (some systems define
   MB_LEN_MAX as 1) */
#ifdef HANDLE_MULTIBYTE
#  include <limits.h>
#  if defined(MB_LEN_MAX) && (MB_LEN_MAX < 16)
#    undef MB_LEN_MAX
#  endif
#  if !defined (MB_LEN_MAX)
#    define MB_LEN_MAX 16
#  endif
#endif

/************************************************/
/* end of multibyte capability checks for I18N	*/
/************************************************/

/******************************************************************/
/* Placeholder for builders to #undef any unwanted features from  */
/* config-top.h or created by configure (such as the default mail */
/* file for mail checking).					  */
/******************************************************************/

/* If you don't want bash to provide a default mail file to check. */
/* #undef DEFAULT_MAIL_DIRECTORY */
