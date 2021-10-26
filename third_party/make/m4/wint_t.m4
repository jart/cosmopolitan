# wint_t.m4 serial 7
dnl Copyright (C) 2003, 2007-2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Bruno Haible.
dnl Test whether <wchar.h> has the 'wint_t' type and whether gnulib's
dnl <wchar.h> or <wctype.h> would, if present, override 'wint_t'.
dnl Prerequisite: AC_PROG_CC

AC_DEFUN([gt_TYPE_WINT_T],
[
  AC_CACHE_CHECK([for wint_t], [gt_cv_c_wint_t],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[
/* Tru64 with Desktop Toolkit C has a bug: <stdio.h> must be included before
   <wchar.h>.
   BSD/OS 4.0.1 has a bug: <stddef.h>, <stdio.h> and <time.h> must be included
   before <wchar.h>.  */
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
            wint_t foo = (wchar_t)'\0';]],
          [[]])],
       [gt_cv_c_wint_t=yes],
       [gt_cv_c_wint_t=no])])
  if test $gt_cv_c_wint_t = yes; then
    AC_DEFINE([HAVE_WINT_T], [1], [Define if you have the 'wint_t' type.])

    dnl Determine whether gnulib's <wchar.h> or <wctype.h> would, if present,
    dnl override 'wint_t'.
    AC_CACHE_CHECK([whether wint_t is too small],
      [gl_cv_type_wint_t_too_small],
      [AC_COMPILE_IFELSE(
           [AC_LANG_PROGRAM([[
/* Tru64 with Desktop Toolkit C has a bug: <stdio.h> must be included before
   <wchar.h>.
   BSD/OS 4.0.1 has a bug: <stddef.h>, <stdio.h> and <time.h> must be
   included before <wchar.h>.  */
#if !(defined __GLIBC__ && !defined __UCLIBC__)
# include <stddef.h>
# include <stdio.h>
# include <time.h>
#endif
#include <wchar.h>
              int verify[sizeof (wint_t) < sizeof (int) ? -1 : 1];
              ]])],
           [gl_cv_type_wint_t_too_small=no],
           [gl_cv_type_wint_t_too_small=yes])])
    if test $gl_cv_type_wint_t_too_small = yes; then
      GNULIB_OVERRIDES_WINT_T=1
    else
      GNULIB_OVERRIDES_WINT_T=0
    fi
  else
    GNULIB_OVERRIDES_WINT_T=0
  fi
  AC_SUBST([GNULIB_OVERRIDES_WINT_T])
])

dnl Prerequisites of the 'wint_t' override.
AC_DEFUN([gl_TYPE_WINT_T_PREREQ],
[
  AC_CHECK_HEADERS_ONCE([crtdefs.h])
  if test $ac_cv_header_crtdefs_h = yes; then
    HAVE_CRTDEFS_H=1
  else
    HAVE_CRTDEFS_H=0
  fi
  AC_SUBST([HAVE_CRTDEFS_H])
])
