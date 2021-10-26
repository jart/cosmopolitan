# eaccess.m4 serial 2
dnl Copyright (C) 2003, 2009-2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_EACCESS],
[
  AC_CHECK_FUNC([eaccess], ,
    [AC_DEFINE([eaccess], [access],
       [Define as 'access' if you don't have the eaccess() function.])])
])
