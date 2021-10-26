# Check for getloadavg.

# Copyright (C) 1992-1996, 1999-2000, 2002-2003, 2006, 2008-2020 Free Software
# Foundation, Inc.

# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

#serial 8

# Autoconf defines AC_FUNC_GETLOADAVG, but that is obsolescent.
# New applications should use gl_GETLOADAVG instead.

# gl_GETLOADAVG
# -------------
AC_DEFUN([gl_GETLOADAVG],
[AC_REQUIRE([gl_STDLIB_H_DEFAULTS])

# Persuade glibc <stdlib.h> to declare getloadavg().
AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

gl_save_LIBS=$LIBS

# getloadavg is present in libc on glibc >= 2.2, Mac OS X, FreeBSD >= 2.0,
# NetBSD >= 0.9, OpenBSD >= 2.0, Solaris >= 7.
HAVE_GETLOADAVG=1
AC_CHECK_FUNC([getloadavg], [],
  [gl_func_getloadavg_done=no

   # Some systems with -lutil have (and need) -lkvm as well, some do not.
   # On Solaris, -lkvm requires nlist from -lelf, so check that first
   # to get the right answer into the cache.
   # For kstat on solaris, we need to test for libelf and libkvm to force the
   # definition of SVR4 below.
   if test $gl_func_getloadavg_done = no; then
     AC_CHECK_LIB([elf], [elf_begin], [LIBS="-lelf $LIBS"])
     AC_CHECK_LIB([kvm], [kvm_open], [LIBS="-lkvm $LIBS"])
     # Check for the 4.4BSD definition of getloadavg.
     AC_CHECK_LIB([util], [getloadavg],
       [LIBS="-lutil $LIBS" gl_func_getloadavg_done=yes])
   fi

   if test $gl_func_getloadavg_done = no; then
     # There is a commonly available library for RS/6000 AIX.
     # Since it is not a standard part of AIX, it might be installed locally.
     gl_getloadavg_LIBS=$LIBS
     LIBS="-L/usr/local/lib $LIBS"
     AC_CHECK_LIB([getloadavg], [getloadavg],
                  [LIBS="-lgetloadavg $LIBS" gl_func_getloadavg_done=yes],
                  [LIBS=$gl_getloadavg_LIBS])
   fi

   # Set up the replacement function if necessary.
   if test $gl_func_getloadavg_done = no; then
     HAVE_GETLOADAVG=0

     # Solaris has libkstat which does not require root.
     AC_CHECK_LIB([kstat], [kstat_open])
     test $ac_cv_lib_kstat_kstat_open = yes && gl_func_getloadavg_done=yes

     # AIX has libperfstat which does not require root
     if test $gl_func_getloadavg_done = no; then
       AC_CHECK_LIB([perfstat], [perfstat_cpu_total])
       test $ac_cv_lib_perfstat_perfstat_cpu_total = yes && gl_func_getloadavg_done=yes
     fi

     if test $gl_func_getloadavg_done = no; then
       AC_CHECK_HEADER([sys/dg_sys_info.h],
         [gl_func_getloadavg_done=yes
          AC_DEFINE([DGUX], [1], [Define to 1 for DGUX with <sys/dg_sys_info.h>.])
          AC_CHECK_LIB([dgc], [dg_sys_info])])
     fi
   fi])

if test "x$gl_save_LIBS" = x; then
  GETLOADAVG_LIBS=$LIBS
else
  GETLOADAVG_LIBS=`echo "$LIBS" | sed "s!$gl_save_LIBS!!"`
fi
LIBS=$gl_save_LIBS

AC_SUBST([GETLOADAVG_LIBS])dnl

# Test whether the system declares getloadavg. Solaris has the function
# but declares it in <sys/loadavg.h>, not <stdlib.h>.
AC_CHECK_HEADERS([sys/loadavg.h])
if test $ac_cv_header_sys_loadavg_h = yes; then
  HAVE_SYS_LOADAVG_H=1
else
  HAVE_SYS_LOADAVG_H=0
fi
AC_CHECK_DECL([getloadavg], [], [HAVE_DECL_GETLOADAVG=0],
  [[#if HAVE_SYS_LOADAVG_H
    /* OpenIndiana has a bug: <sys/time.h> must be included before
       <sys/loadavg.h>.  */
    # include <sys/time.h>
    # include <sys/loadavg.h>
    #endif
    #include <stdlib.h>]])
])# gl_GETLOADAVG


# gl_PREREQ_GETLOADAVG
# --------------------
# Set up the AC_LIBOBJ replacement of 'getloadavg'.
AC_DEFUN([gl_PREREQ_GETLOADAVG],
[
# Figure out what our getloadavg.c needs.

AC_CHECK_HEADERS_ONCE([sys/param.h unistd.h])

# On HPUX9, an unprivileged user can get load averages this way.
if test $gl_func_getloadavg_done = no; then
  AC_CHECK_FUNCS([pstat_getdynamic], [gl_func_getloadavg_done=yes])
fi

# We cannot check for <dwarf.h>, because Solaris 2 does not use dwarf (it
# uses stabs), but it is still SVR4.  We cannot check for <elf.h> because
# Irix 4.0.5F has the header but not the library.
if test $gl_func_getloadavg_done = no && test "$ac_cv_lib_elf_elf_begin" = yes \
    && test "$ac_cv_lib_kvm_kvm_open" = yes; then
  gl_func_getloadavg_done=yes
  AC_DEFINE([SVR4], [1], [Define to 1 on System V Release 4.])
fi

if test $gl_func_getloadavg_done = no; then
  AC_CHECK_HEADER([inq_stats/cpustats.h],
  [gl_func_getloadavg_done=yes
   AC_DEFINE([UMAX], [1], [Define to 1 for Encore UMAX.])
   AC_DEFINE([UMAX4_3], [1],
             [Define to 1 for Encore UMAX 4.3 that has <inq_status/cpustats.h>
              instead of <sys/cpustats.h>.])])
fi

if test $gl_func_getloadavg_done = no; then
  AC_CHECK_HEADER([sys/cpustats.h],
  [gl_func_getloadavg_done=yes; AC_DEFINE([UMAX])])
fi

if test $gl_func_getloadavg_done = no; then
  AC_CHECK_HEADERS([mach/mach.h])
fi

AC_CHECK_HEADERS([nlist.h],
[AC_CHECK_MEMBERS([struct nlist.n_un.n_name],
                  [], [],
                  [@%:@include <nlist.h>])
 AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <nlist.h>]],
                   [[struct nlist x;
                    #ifdef HAVE_STRUCT_NLIST_N_UN_N_NAME
                    x.n_un.n_name = "";
                    #else
                    x.n_name = "";
                    #endif]])],
                [AC_DEFINE([N_NAME_POINTER], [1],
                           [Define to 1 if the nlist n_name member is a pointer])])
])dnl
])# gl_PREREQ_GETLOADAVG
