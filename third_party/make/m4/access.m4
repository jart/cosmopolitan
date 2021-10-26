# access.m4 serial 1
dnl Copyright (C) 2019-2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ACCESS],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  dnl On native Windows, access (= _access) does not support the X_OK mode.
  dnl It works by chance on some versions of mingw.
  case "$host_os" in
    mingw*) REPLACE_ACCESS=1 ;;
  esac
])
