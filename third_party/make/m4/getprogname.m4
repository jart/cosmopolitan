# getprogname.m4 - check for getprogname or replacements for it

# Copyright (C) 2016-2020 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 4

AC_DEFUN([gl_FUNC_GETPROGNAME],
[
  AC_CHECK_FUNCS_ONCE([getprogname getexecname])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  ac_found=0
  AC_CHECK_DECLS([program_invocation_name], [ac_found=1], [],
    [#include <errno.h>])
  AC_CHECK_DECLS([program_invocation_short_name], [ac_found=1], [],
    [#include <errno.h>])
  AC_CHECK_DECLS([__argv], [ac_found=1], [], [#include <stdlib.h>])

  # Incur the cost of this test only if none of the above worked.
  if test $ac_found = 0; then
    # On OpenBSD 5.1, using the global __progname variable appears to be
    # the only way to implement getprogname.
    AC_CACHE_CHECK([whether __progname is defined in default libraries],
      [gl_cv_var___progname],
      [
        gl_cv_var___progname=
        AC_LINK_IFELSE(
          [AC_LANG_PROGRAM(
            [[extern char *__progname;]],
            [[return *__progname;]]
          )],
          [gl_cv_var___progname=yes]
        )
      ]
    )
    if test "$gl_cv_var___progname" = yes; then
      AC_DEFINE([HAVE_VAR___PROGNAME], 1,
        [Define if you have a global __progname variable])
    fi
  fi
])
