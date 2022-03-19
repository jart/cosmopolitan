/* clang-format off */
/* Get the system load averages.

   Copyright (C) 1985-1989, 1991-1995, 1997, 1999-2000, 2003-2020 Free Software
   Foundation, Inc.

   NOTE: The canonical source of this file is maintained with gnulib.
   Bugs can be reported to bug-gnulib@gnu.org.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Compile-time symbols that this file uses:

   HAVE_PSTAT_GETDYNAMIC        Define this if your system has the
                                pstat_getdynamic function.  I think it
                                is unique to HPUX9.  The best way to get the
                                definition is through the AC_FUNC_GETLOADAVG
                                macro that comes with autoconf 2.13 or newer.
                                If that isn't an option, then just put
                                AC_CHECK_FUNCS(pstat_getdynamic) in your
                                configure.ac file.
   HAVE_LIBPERFSTAT Define this if your system has the
                                perfstat_cpu_total function in libperfstat (AIX).
   FIXUP_KERNEL_SYMBOL_ADDR()   Adjust address in returned struct nlist.
   KERNEL_FILE                  Name of the kernel file to nlist.
   LDAV_CVT()                   Scale the load average from the kernel.
                                Returns a double.
   LDAV_SYMBOL                  Name of kernel symbol giving load average.
   LOAD_AVE_TYPE                Type of the load average array in the kernel.
                                Must be defined unless one of
                                apollo, DGUX, NeXT, or UMAX is defined;
                                or we have libkstat;
                                otherwise, no load average is available.
   HAVE_NLIST_H                 nlist.h is available.  NLIST_STRUCT defaults
                                to this.
   NLIST_STRUCT                 Include nlist.h, not a.out.h.
   N_NAME_POINTER               The nlist n_name element is a pointer,
                                not an array.
   HAVE_STRUCT_NLIST_N_UN_N_NAME 'n_un.n_name' is member of 'struct nlist'.
   LINUX_LDAV_FILE              [__linux__, __ANDROID__, __CYGWIN__]: File
                                containing load averages.

   Specific system predefines this file uses, aside from setting
   default values if not emacs:

   apollo
   BSD                          Real BSD, not just BSD-like.
   DGUX
   eunice                       UNIX emulator under VMS.
   hpux
   __MSDOS__                    No-op for MSDOS.
   NeXT
   sgi
   UMAX
   UMAX4_3
   VMS
   _WIN32                       Native Windows (possibly also defined on Cygwin)
   __linux__, __ANDROID__       Linux: assumes /proc file system mounted.
                                Support from Michael K. Johnson.
   __CYGWIN__                   Cygwin emulates linux /proc/loadavg.
   __NetBSD__                   NetBSD: assumes /kern file system mounted.

   In addition, to avoid nesting many #ifdefs, we internally set
   LDAV_DONE to indicate that the load average has been computed.

   We also #define LDAV_PRIVILEGED if a program will require
   special installation to be able to call getloadavg.  */

#include "libc/sysv/consts/o.h"
#include "libc/errno.h"
#include "libc/dce.h"
#include "libc/dce.h"
#include "libc/dce.h"
#include "libc/sysv/errfuns.h"
#include "libc/stdio/stdio.h"
#include "third_party/make/src/config.h"
#include "third_party/make/lib/intprops.h"

/* Put the 1 minute, 5 minute and 15 minute load averages
   into the first NELEM elements of LOADAVG.
   Return the number written (never more than 3, but may be less than NELEM),
   or -1 (setting errno) if an error occurred.  */

int
getloadavg (double loadavg[], int nelem)
{
  int elem = 0;                 /* Return value.  */
  if (IsLinux()) {
    char ldavgbuf[3 * (INT_STRLEN_BOUND (int) + sizeof ".00 ")];
    char const *ptr = ldavgbuf;
    int fd, count, saved_errno;
    fd = open ("/proc/loadavg", O_RDONLY);
    if (fd == -1)
      return -1;
    count = read (fd, ldavgbuf, sizeof ldavgbuf - 1);
    saved_errno = errno;
    (void) close (fd);
    errno = saved_errno;
    if (count <= 0)
      return -1;
    ldavgbuf[count] = '\0';
    for (elem = 0; elem < nelem; elem++)
    {
      double numerator = 0;
      double denominator = 1;
      while (*ptr == ' ')
        ptr++;
      /* Finish if this number is missing, and report an error if all
         were missing.  */
      if (! ('0' <= *ptr && *ptr <= '9'))
      {
        if (elem == 0)
          return enotsup();
        break;
      }
      while ('0' <= *ptr && *ptr <= '9')
        numerator = 10 * numerator + (*ptr++ - '0');
      if (*ptr == '.')
        for (ptr++; '0' <= *ptr && *ptr <= '9'; ptr++)
          numerator = 10 * numerator + (*ptr - '0'), denominator *= 10;
      loadavg[elem++] = numerator / denominator;
    }
  } else if (IsNetbsd()) {
    unsigned long int load_ave[3], scale;
    int count;
    FILE *fp;
    fp = fopen ("/kern/loadavg", "r");
    if (fp == NULL)
      return -1;
    count = fscanf (fp, "%lu %lu %lu %lu\n",
                    &load_ave[0], &load_ave[1], &load_ave[2],
                    &scale);
    fclose (fp);
    if (count != 4)
      return enotsup();
    for (elem = 0; elem < nelem; elem++)
      loadavg[elem] = (double) load_ave[elem] / (double) scale;
  } else if (IsWindows()) {
    for ( ; elem < nelem; elem++)
      loadavg[elem] = 0.0;
  } else {
    errno = ENOSYS;
    elem = -1;
  }
  return elem;
}
