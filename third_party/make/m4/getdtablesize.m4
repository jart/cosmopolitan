# getdtablesize.m4 serial 7
dnl Copyright (C) 2008-2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_GETDTABLESIZE],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_CHECK_FUNCS_ONCE([getdtablesize])
  AC_CHECK_DECLS_ONCE([getdtablesize])
  if test $ac_cv_func_getdtablesize = yes &&
     test $ac_cv_have_decl_getdtablesize = yes; then
    AC_CACHE_CHECK([whether getdtablesize works],
      [gl_cv_func_getdtablesize_works],
      [dnl There are two concepts: the "maximum possible file descriptor value + 1"
       dnl and the "maximum number of open file descriptors in a process".
       dnl Per SUSv2 and POSIX, getdtablesize() should return the first one.
       dnl On most platforms, the first and the second concept are the same.
       dnl On OpenVMS, however, they are different and getdtablesize() returns
       dnl the second one; thus the test below fails. But we don't care
       dnl because there's no good way to write a replacement getdtablesize().
       case "$host_os" in
         vms*) gl_cv_func_getdtablesize_works="no (limitation)" ;;
         *)
           dnl Cygwin 1.7.25 automatically increases the RLIMIT_NOFILE soft
           dnl limit up to an unchangeable hard limit; all other platforms
           dnl correctly require setrlimit before getdtablesize() can report
           dnl a larger value.
           AC_RUN_IFELSE([
             AC_LANG_PROGRAM([[#include <unistd.h>]],
               [int size = getdtablesize();
                if (dup2 (0, getdtablesize()) != -1)
                  return 1;
                if (size != getdtablesize())
                  return 2;
               ])],
             [gl_cv_func_getdtablesize_works=yes],
             [gl_cv_func_getdtablesize_works=no],
             [case "$host_os" in
                cygwin*) # on cygwin 1.5.25, getdtablesize() automatically grows
                  gl_cv_func_getdtablesize_works="guessing no" ;;
                *) gl_cv_func_getdtablesize_works="guessing yes" ;;
              esac
             ])
           ;;
       esac
      ])
    case "$gl_cv_func_getdtablesize_works" in
      *yes | "no (limitation)") ;;
      *) REPLACE_GETDTABLESIZE=1 ;;
    esac
  else
    HAVE_GETDTABLESIZE=0
  fi
])

# Prerequisites of lib/getdtablesize.c.
AC_DEFUN([gl_PREREQ_GETDTABLESIZE], [:])
