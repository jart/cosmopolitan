# gnulib-common.m4 serial 46
dnl Copyright (C) 2007-2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_PREREQ([2.62])

# gl_COMMON
# is expanded unconditionally through gnulib-tool magic.
AC_DEFUN([gl_COMMON], [
  dnl Use AC_REQUIRE here, so that the code is expanded once only.
  AC_REQUIRE([gl_00GNULIB])
  AC_REQUIRE([gl_COMMON_BODY])
])
AC_DEFUN([gl_COMMON_BODY], [
  AH_VERBATIM([_Noreturn],
[/* The _Noreturn keyword of C11.  */
#ifndef _Noreturn
# if (defined __cplusplus \
      && ((201103 <= __cplusplus && !(__GNUC__ == 4 && __GNUC_MINOR__ == 7)) \
          || (defined _MSC_VER && 1900 <= _MSC_VER)) \
      && 0)
    /* [[noreturn]] is not practically usable, because with it the syntax
         extern _Noreturn void func (...);
       would not be valid; such a declaration would only be valid with 'extern'
       and '_Noreturn' swapped, or without the 'extern' keyword.  However, some
       AIX system header files and several gnulib header files use precisely
       this syntax with 'extern'.  */
#  define _Noreturn [[noreturn]]
# elif ((!defined __cplusplus || defined __clang__) \
        && (201112 <= (defined __STDC_VERSION__ ? __STDC_VERSION__ : 0)  \
            || 4 < __GNUC__ + (7 <= __GNUC_MINOR__)))
   /* _Noreturn works as-is.  */
# elif 2 < __GNUC__ + (8 <= __GNUC_MINOR__) || 0x5110 <= __SUNPRO_C
#  define _Noreturn __attribute__ ((__noreturn__))
# elif 1200 <= (defined _MSC_VER ? _MSC_VER : 0)
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn
# endif
#endif
])
  AH_VERBATIM([isoc99_inline],
[/* Work around a bug in Apple GCC 4.0.1 build 5465: In C99 mode, it supports
   the ISO C 99 semantics of 'extern inline' (unlike the GNU C semantics of
   earlier versions), but does not display it by setting __GNUC_STDC_INLINE__.
   __APPLE__ && __MACH__ test for Mac OS X.
   __APPLE_CC__ tests for the Apple compiler and its version.
   __STDC_VERSION__ tests for the C99 mode.  */
#if defined __APPLE__ && defined __MACH__ && __APPLE_CC__ >= 5465 && !defined __cplusplus && __STDC_VERSION__ >= 199901L && !defined __GNUC_STDC_INLINE__
# define __GNUC_STDC_INLINE__ 1
#endif])
  AH_VERBATIM([unused_parameter],
[/* Define as a marker that can be attached to declarations that might not
    be used.  This helps to reduce warnings, such as from
    GCC -Wunused-parameter.  */
#if __GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
# define _GL_UNUSED __attribute__ ((__unused__))
#else
# define _GL_UNUSED
#endif
/* The name _UNUSED_PARAMETER_ is an earlier spelling, although the name
   is a misnomer outside of parameter lists.  */
#define _UNUSED_PARAMETER_ _GL_UNUSED

/* gcc supports the "unused" attribute on possibly unused labels, and
   g++ has since version 4.5.  Note to support C++ as well as C,
   _GL_UNUSED_LABEL should be used with a trailing ;  */
#if !defined __cplusplus || __GNUC__ > 4 \
    || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
# define _GL_UNUSED_LABEL _GL_UNUSED
#else
# define _GL_UNUSED_LABEL
#endif

/* The __pure__ attribute was added in gcc 2.96.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
# define _GL_ATTRIBUTE_PURE __attribute__ ((__pure__))
#else
# define _GL_ATTRIBUTE_PURE /* empty */
#endif

/* The __const__ attribute was added in gcc 2.95.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
# define _GL_ATTRIBUTE_CONST __attribute__ ((__const__))
#else
# define _GL_ATTRIBUTE_CONST /* empty */
#endif

/* The __malloc__ attribute was added in gcc 3.  */
#if 3 <= __GNUC__
# define _GL_ATTRIBUTE_MALLOC __attribute__ ((__malloc__))
#else
# define _GL_ATTRIBUTE_MALLOC /* empty */
#endif
])
  AH_VERBATIM([async_safe],
[/* The _GL_ASYNC_SAFE marker should be attached to functions that are
   signal handlers (for signals other than SIGABRT, SIGPIPE) or can be
   invoked from such signal handlers.  Such functions have some restrictions:
     * All functions that it calls should be marked _GL_ASYNC_SAFE as well,
       or should be listed as async-signal-safe in POSIX
       <https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_04>
       section 2.4.3.  Note that malloc(), sprintf(), and fwrite(), in
       particular, are NOT async-signal-safe.
     * All memory locations (variables and struct fields) that these functions
       access must be marked 'volatile'.  This holds for both read and write
       accesses.  Otherwise the compiler might optimize away stores to and
       reads from such locations that occur in the program, depending on its
       data flow analysis.  For example, when the program contains a loop
       that is intended to inspect a variable set from within a signal handler
           while (!signal_occurred)
             ;
       the compiler is allowed to transform this into an endless loop if the
       variable 'signal_occurred' is not declared 'volatile'.
   Additionally, recall that:
     * A signal handler should not modify errno (except if it is a handler
       for a fatal signal and ends by raising the same signal again, thus
       provoking the termination of the process).  If it invokes a function
       that may clobber errno, it needs to save and restore the value of
       errno.  */
#define _GL_ASYNC_SAFE
])
  dnl Hint which direction to take regarding cross-compilation guesses:
  dnl When a user installs a program on a platform they are not intimately
  dnl familiar with, --enable-cross-guesses=conservative is the appropriate
  dnl choice.  It implements the "If we don't know, assume the worst" principle.
  dnl However, when an operating system developer (on a platform which is not
  dnl yet known to gnulib) builds packages for their platform, they want to
  dnl expose, not hide, possible platform bugs; in this case,
  dnl --enable-cross-guesses=risky is the appropriate choice.
  dnl Sets the variables
  dnl gl_cross_guess_normal    (to be used when 'yes' is good and 'no' is bad),
  dnl gl_cross_guess_inverted  (to be used when 'no' is good and 'yes' is bad).
  AC_ARG_ENABLE([cross-guesses],
    [AS_HELP_STRING([--enable-cross-guesses={conservative|risky}],
       [specify policy for cross-compilation guesses])],
    [if test "x$enableval" != xconservative && test "x$enableval" != xrisky; then
       AC_MSG_WARN([invalid argument supplied to --enable-cross-guesses])
       enableval=conservative
     fi
     gl_cross_guesses="$enableval"],
    [gl_cross_guesses=conservative])
  if test $gl_cross_guesses = risky; then
    gl_cross_guess_normal="guessing yes"
    gl_cross_guess_inverted="guessing no"
  else
    gl_cross_guess_normal="guessing no"
    gl_cross_guess_inverted="guessing yes"
  fi
  dnl Preparation for running test programs:
  dnl Tell glibc to write diagnostics from -D_FORTIFY_SOURCE=2 to stderr, not
  dnl to /dev/tty, so they can be redirected to log files.  Such diagnostics
  dnl arise e.g., in the macros gl_PRINTF_DIRECTIVE_N, gl_SNPRINTF_DIRECTIVE_N.
  LIBC_FATAL_STDERR_=1
  export LIBC_FATAL_STDERR_
])

# gl_MODULE_INDICATOR_CONDITION
# expands to a C preprocessor expression that evaluates to 1 or 0, depending
# whether a gnulib module that has been requested shall be considered present
# or not.
m4_define([gl_MODULE_INDICATOR_CONDITION], [1])

# gl_MODULE_INDICATOR_SET_VARIABLE([modulename])
# sets the shell variable that indicates the presence of the given module to
# a C preprocessor expression that will evaluate to 1.
AC_DEFUN([gl_MODULE_INDICATOR_SET_VARIABLE],
[
  gl_MODULE_INDICATOR_SET_VARIABLE_AUX(
    [GNULIB_[]m4_translit([[$1]],
                          [abcdefghijklmnopqrstuvwxyz./-],
                          [ABCDEFGHIJKLMNOPQRSTUVWXYZ___])],
    [gl_MODULE_INDICATOR_CONDITION])
])

# gl_MODULE_INDICATOR_SET_VARIABLE_AUX([variable])
# modifies the shell variable to include the gl_MODULE_INDICATOR_CONDITION.
# The shell variable's value is a C preprocessor expression that evaluates
# to 0 or 1.
AC_DEFUN([gl_MODULE_INDICATOR_SET_VARIABLE_AUX],
[
  m4_if(m4_defn([gl_MODULE_INDICATOR_CONDITION]), [1],
    [
     dnl Simplify the expression VALUE || 1 to 1.
     $1=1
    ],
    [gl_MODULE_INDICATOR_SET_VARIABLE_AUX_OR([$1],
                                             [gl_MODULE_INDICATOR_CONDITION])])
])

# gl_MODULE_INDICATOR_SET_VARIABLE_AUX_OR([variable], [condition])
# modifies the shell variable to include the given condition.  The shell
# variable's value is a C preprocessor expression that evaluates to 0 or 1.
AC_DEFUN([gl_MODULE_INDICATOR_SET_VARIABLE_AUX_OR],
[
  dnl Simplify the expression 1 || CONDITION to 1.
  if test "$[]$1" != 1; then
    dnl Simplify the expression 0 || CONDITION to CONDITION.
    if test "$[]$1" = 0; then
      $1=$2
    else
      $1="($[]$1 || $2)"
    fi
  fi
])

# gl_MODULE_INDICATOR([modulename])
# defines a C macro indicating the presence of the given module
# in a location where it can be used.
#                                             |  Value  |   Value   |
#                                             | in lib/ | in tests/ |
# --------------------------------------------+---------+-----------+
# Module present among main modules:          |    1    |     1     |
# --------------------------------------------+---------+-----------+
# Module present among tests-related modules: |    0    |     1     |
# --------------------------------------------+---------+-----------+
# Module not present at all:                  |    0    |     0     |
# --------------------------------------------+---------+-----------+
AC_DEFUN([gl_MODULE_INDICATOR],
[
  AC_DEFINE_UNQUOTED([GNULIB_]m4_translit([[$1]],
      [abcdefghijklmnopqrstuvwxyz./-],
      [ABCDEFGHIJKLMNOPQRSTUVWXYZ___]),
    [gl_MODULE_INDICATOR_CONDITION],
    [Define to a C preprocessor expression that evaluates to 1 or 0,
     depending whether the gnulib module $1 shall be considered present.])
])

# gl_MODULE_INDICATOR_FOR_TESTS([modulename])
# defines a C macro indicating the presence of the given module
# in lib or tests. This is useful to determine whether the module
# should be tested.
#                                             |  Value  |   Value   |
#                                             | in lib/ | in tests/ |
# --------------------------------------------+---------+-----------+
# Module present among main modules:          |    1    |     1     |
# --------------------------------------------+---------+-----------+
# Module present among tests-related modules: |    1    |     1     |
# --------------------------------------------+---------+-----------+
# Module not present at all:                  |    0    |     0     |
# --------------------------------------------+---------+-----------+
AC_DEFUN([gl_MODULE_INDICATOR_FOR_TESTS],
[
  AC_DEFINE([GNULIB_TEST_]m4_translit([[$1]],
      [abcdefghijklmnopqrstuvwxyz./-],
      [ABCDEFGHIJKLMNOPQRSTUVWXYZ___]), [1],
    [Define to 1 when the gnulib module $1 should be tested.])
])

# gl_ASSERT_NO_GNULIB_POSIXCHECK
# asserts that there will never be a need to #define GNULIB_POSIXCHECK.
# and thereby enables an optimization of configure and config.h.
# Used by Emacs.
AC_DEFUN([gl_ASSERT_NO_GNULIB_POSIXCHECK],
[
  dnl Override gl_WARN_ON_USE_PREPARE.
  dnl But hide this definition from 'aclocal'.
  AC_DEFUN([gl_W][ARN_ON_USE_PREPARE], [])
])

# gl_ASSERT_NO_GNULIB_TESTS
# asserts that there will be no gnulib tests in the scope of the configure.ac
# and thereby enables an optimization of config.h.
# Used by Emacs.
AC_DEFUN([gl_ASSERT_NO_GNULIB_TESTS],
[
  dnl Override gl_MODULE_INDICATOR_FOR_TESTS.
  AC_DEFUN([gl_MODULE_INDICATOR_FOR_TESTS], [])
])

# Test whether <features.h> exists.
# Set HAVE_FEATURES_H.
AC_DEFUN([gl_FEATURES_H],
[
  AC_CHECK_HEADERS_ONCE([features.h])
  if test $ac_cv_header_features_h = yes; then
    HAVE_FEATURES_H=1
  else
    HAVE_FEATURES_H=0
  fi
  AC_SUBST([HAVE_FEATURES_H])
])

# AS_VAR_IF(VAR, VALUE, [IF-MATCH], [IF-NOT-MATCH])
# ----------------------------------------------------
# Backport of autoconf-2.63b's macro.
# Remove this macro when we can assume autoconf >= 2.64.
m4_ifndef([AS_VAR_IF],
[m4_define([AS_VAR_IF],
[AS_IF([test x"AS_VAR_GET([$1])" = x""$2], [$3], [$4])])])

# gl_PROG_CC_C99
# Modifies the value of the shell variable CC in an attempt to make $CC
# understand ISO C99 source code.
# This is like AC_PROG_CC_C99, except that
# - AC_PROG_CC_C99 does not mix well with AC_PROG_CC_STDC
#   <https://lists.gnu.org/r/bug-gnulib/2011-09/msg00367.html>,
#   but many more packages use AC_PROG_CC_STDC than AC_PROG_CC_C99
#   <https://lists.gnu.org/r/bug-gnulib/2011-09/msg00441.html>.
# Remaining problems:
# - When AC_PROG_CC_STDC is invoked twice, it adds the C99 enabling options
#   to CC twice
#   <https://lists.gnu.org/r/bug-gnulib/2011-09/msg00431.html>.
# - AC_PROG_CC_STDC is likely to change now that C11 is an ISO standard.
AC_DEFUN([gl_PROG_CC_C99],
[
  dnl Change that version number to the minimum Autoconf version that supports
  dnl mixing AC_PROG_CC_C99 calls with AC_PROG_CC_STDC calls.
  m4_version_prereq([9.0],
    [AC_REQUIRE([AC_PROG_CC_C99])],
    [AC_REQUIRE([AC_PROG_CC_STDC])])
])

# gl_PROG_AR_RANLIB
# Determines the values for AR, ARFLAGS, RANLIB that fit with the compiler.
# The user can set the variables AR, ARFLAGS, RANLIB if he wants to override
# the values.
AC_DEFUN([gl_PROG_AR_RANLIB],
[
  dnl Minix 3 comes with two toolchains: The Amsterdam Compiler Kit compiler
  dnl as "cc", and GCC as "gcc". They have different object file formats and
  dnl library formats. In particular, the GNU binutils programs ar and ranlib
  dnl produce libraries that work only with gcc, not with cc.
  AC_REQUIRE([AC_PROG_CC])
  dnl The '][' hides this use from 'aclocal'.
  AC_BEFORE([$0], [A][M_PROG_AR])
  AC_CACHE_CHECK([for Minix Amsterdam compiler], [gl_cv_c_amsterdam_compiler],
    [
      AC_EGREP_CPP([Amsterdam],
        [
#ifdef __ACK__
Amsterdam
#endif
        ],
        [gl_cv_c_amsterdam_compiler=yes],
        [gl_cv_c_amsterdam_compiler=no])
    ])

  dnl Don't compete with AM_PROG_AR's decision about AR/ARFLAGS if we are not
  dnl building with __ACK__.
  if test $gl_cv_c_amsterdam_compiler = yes; then
    if test -z "$AR"; then
      AR='cc -c.a'
    fi
    if test -z "$ARFLAGS"; then
      ARFLAGS='-o'
    fi
  else
    dnl AM_PROG_AR was added in automake v1.11.2.  AM_PROG_AR does not AC_SUBST
    dnl ARFLAGS variable (it is filed into Makefile.in directly by automake
    dnl script on-demand, if not specified by ./configure of course).
    dnl Don't AC_REQUIRE the AM_PROG_AR otherwise the code for __ACK__ above
    dnl will be ignored.  Also, pay attention to call AM_PROG_AR in else block
    dnl because AM_PROG_AR is written so it could re-set AR variable even for
    dnl __ACK__.  It may seem like its easier to avoid calling the macro here,
    dnl but we need to AC_SUBST both AR/ARFLAGS (thus those must have some good
    dnl default value and automake should usually know them).
    dnl
    dnl The '][' hides this use from 'aclocal'.
    m4_ifdef([A][M_PROG_AR], [A][M_PROG_AR], [:])
  fi

  dnl In case the code above has not helped with setting AR/ARFLAGS, use
  dnl Automake-documented default values for AR and ARFLAGS, but prefer
  dnl ${host}-ar over ar (useful for cross-compiling).
  AC_CHECK_TOOL([AR], [ar], [ar])
  if test -z "$ARFLAGS"; then
    ARFLAGS='cr'
  fi

  AC_SUBST([AR])
  AC_SUBST([ARFLAGS])
  if test -z "$RANLIB"; then
    if test $gl_cv_c_amsterdam_compiler = yes; then
      RANLIB=':'
    else
      dnl Use the ranlib program if it is available.
      AC_PROG_RANLIB
    fi
  fi
  AC_SUBST([RANLIB])
])

# AC_C_RESTRICT
# This definition is copied from post-2.69 Autoconf and overrides the
# AC_C_RESTRICT macro from autoconf 2.60..2.69.  It can be removed
# once autoconf >= 2.70 can be assumed.  It's painful to check version
# numbers, and in practice this macro is more up-to-date than Autoconf
# is, so override Autoconf unconditionally.
AC_DEFUN([AC_C_RESTRICT],
[AC_CACHE_CHECK([for C/C++ restrict keyword], [ac_cv_c_restrict],
  [ac_cv_c_restrict=no
   # The order here caters to the fact that C++ does not require restrict.
   for ac_kw in __restrict __restrict__ _Restrict restrict; do
     AC_COMPILE_IFELSE(
      [AC_LANG_PROGRAM(
         [[typedef int *int_ptr;
           int foo (int_ptr $ac_kw ip) { return ip[0]; }
           int bar (int [$ac_kw]); /* Catch GCC bug 14050.  */
           int bar (int ip[$ac_kw]) { return ip[0]; }
         ]],
         [[int s[1];
           int *$ac_kw t = s;
           t[0] = 0;
           return foo (t) + bar (t);
         ]])],
      [ac_cv_c_restrict=$ac_kw])
     test "$ac_cv_c_restrict" != no && break
   done
  ])
 AH_VERBATIM([restrict],
[/* Define to the equivalent of the C99 'restrict' keyword, or to
   nothing if this is not supported.  Do not define if restrict is
   supported directly.  */
#undef restrict
/* Work around a bug in older versions of Sun C++, which did not
   #define __restrict__ or support _Restrict or __restrict__
   even though the corresponding Sun C compiler ended up with
   "#define restrict _Restrict" or "#define restrict __restrict__"
   in the previous line.  This workaround can be removed once
   we assume Oracle Developer Studio 12.5 (2016) or later.  */
#if defined __SUNPRO_CC && !defined __RESTRICT && !defined __restrict__
# define _Restrict
# define __restrict__
#endif])
 case $ac_cv_c_restrict in
   restrict) ;;
   no) AC_DEFINE([restrict], []) ;;
   *)  AC_DEFINE_UNQUOTED([restrict], [$ac_cv_c_restrict]) ;;
 esac
])# AC_C_RESTRICT

# gl_BIGENDIAN
# is like AC_C_BIGENDIAN, except that it can be AC_REQUIREd.
# Note that AC_REQUIRE([AC_C_BIGENDIAN]) does not work reliably because some
# macros invoke AC_C_BIGENDIAN with arguments.
AC_DEFUN([gl_BIGENDIAN],
[
  AC_C_BIGENDIAN
])

# gl_CACHE_VAL_SILENT(cache-id, command-to-set-it)
# is like AC_CACHE_VAL(cache-id, command-to-set-it), except that it does not
# output a spurious "(cached)" mark in the midst of other configure output.
# This macro should be used instead of AC_CACHE_VAL when it is not surrounded
# by an AC_MSG_CHECKING/AC_MSG_RESULT pair.
AC_DEFUN([gl_CACHE_VAL_SILENT],
[
  saved_as_echo_n="$as_echo_n"
  as_echo_n=':'
  AC_CACHE_VAL([$1], [$2])
  as_echo_n="$saved_as_echo_n"
])

# AS_VAR_COPY was added in autoconf 2.63b
m4_define_default([AS_VAR_COPY],
[AS_LITERAL_IF([$1[]$2], [$1=$$2], [eval $1=\$$2])])
