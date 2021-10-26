# fcntl.m4 serial 9
dnl Copyright (C) 2009-2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# For now, this module ensures that fcntl()
# - supports F_DUPFD correctly
# - supports or emulates F_DUPFD_CLOEXEC
# - supports F_GETFD
# Still to be ported to mingw:
# - F_SETFD
# - F_GETFL, F_SETFL
# - F_GETOWN, F_SETOWN
# - F_GETLK, F_SETLK, F_SETLKW
AC_DEFUN([gl_FUNC_FCNTL],
[
  dnl Persuade glibc to expose F_DUPFD_CLOEXEC.
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([gl_FCNTL_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_CHECK_FUNCS_ONCE([fcntl])
  if test $ac_cv_func_fcntl = no; then
    gl_REPLACE_FCNTL
  else
    dnl cygwin 1.5.x F_DUPFD has wrong errno, and allows negative target
    dnl haiku alpha 2 F_DUPFD has wrong errno
    AC_CACHE_CHECK([whether fcntl handles F_DUPFD correctly],
      [gl_cv_func_fcntl_f_dupfd_works],
      [AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <errno.h>
              #include <fcntl.h>
              #include <limits.h>
              #include <sys/resource.h>
              #include <unistd.h>
              #ifndef RLIM_SAVED_CUR
              # define RLIM_SAVED_CUR RLIM_INFINITY
              #endif
              #ifndef RLIM_SAVED_MAX
              # define RLIM_SAVED_MAX RLIM_INFINITY
              #endif
            ]],
            [[int result = 0;
              int bad_fd = INT_MAX;
              struct rlimit rlim;
              if (getrlimit (RLIMIT_NOFILE, &rlim) == 0
                  && 0 <= rlim.rlim_cur && rlim.rlim_cur <= INT_MAX
                  && rlim.rlim_cur != RLIM_INFINITY
                  && rlim.rlim_cur != RLIM_SAVED_MAX
                  && rlim.rlim_cur != RLIM_SAVED_CUR)
                bad_fd = rlim.rlim_cur;
              if (fcntl (0, F_DUPFD, -1) != -1) result |= 1;
              if (errno != EINVAL) result |= 2;
              if (fcntl (0, F_DUPFD, bad_fd) != -1) result |= 4;
              if (errno != EINVAL) result |= 8;
              /* On OS/2 kLIBC, F_DUPFD does not work on a directory fd */
              {
                int fd;
                fd = open (".", O_RDONLY);
                if (fd == -1)
                  result |= 16;
                else if (fcntl (fd, F_DUPFD, STDERR_FILENO + 1) == -1)
                  result |= 32;

                close (fd);
              }
              return result;]])],
         [gl_cv_func_fcntl_f_dupfd_works=yes],
         [gl_cv_func_fcntl_f_dupfd_works=no],
         [case $host_os in
            aix* | cygwin* | haiku*)
               gl_cv_func_fcntl_f_dupfd_works="guessing no" ;;
            *) gl_cv_func_fcntl_f_dupfd_works="guessing yes" ;;
          esac])])
    case $gl_cv_func_fcntl_f_dupfd_works in
      *yes) ;;
      *) gl_REPLACE_FCNTL
        AC_DEFINE([FCNTL_DUPFD_BUGGY], [1], [Define this to 1 if F_DUPFD
          behavior does not match POSIX]) ;;
    esac

    dnl Many systems lack F_DUPFD_CLOEXEC
    AC_CACHE_CHECK([whether fcntl understands F_DUPFD_CLOEXEC],
      [gl_cv_func_fcntl_f_dupfd_cloexec],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <fcntl.h>
#ifndef F_DUPFD_CLOEXEC
choke me
#endif
         ]])],
         [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#ifdef __linux__
/* The Linux kernel only added F_DUPFD_CLOEXEC in 2.6.24, so we always replace
   it to support the semantics on older kernels that failed with EINVAL.  */
choke me
#endif
           ]])],
           [gl_cv_func_fcntl_f_dupfd_cloexec=yes],
           [gl_cv_func_fcntl_f_dupfd_cloexec="needs runtime check"])],
         [gl_cv_func_fcntl_f_dupfd_cloexec=no])])
    if test "$gl_cv_func_fcntl_f_dupfd_cloexec" != yes; then
      gl_REPLACE_FCNTL
      dnl No witness macro needed for this bug.
    fi
  fi
  dnl Replace fcntl() for supporting the gnulib-defined fchdir() function,
  dnl to keep fchdir's bookkeeping up-to-date.
  m4_ifdef([gl_FUNC_FCHDIR], [
    gl_TEST_FCHDIR
    if test $HAVE_FCHDIR = 0; then
      gl_REPLACE_FCNTL
    fi
  ])
])

AC_DEFUN([gl_REPLACE_FCNTL],
[
  AC_REQUIRE([gl_FCNTL_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([fcntl])
  if test $ac_cv_func_fcntl = no; then
    HAVE_FCNTL=0
  else
    REPLACE_FCNTL=1
  fi
])
