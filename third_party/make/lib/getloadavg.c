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

#include <config.h>

/* Specification.  */
#include <stdlib.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>

# include <sys/types.h>

# if HAVE_SYS_PARAM_H
#  include <sys/param.h>
# endif

# include "intprops.h"

# if defined _WIN32 && ! defined __CYGWIN__ && ! defined WINDOWS32
#  define WINDOWS32
# endif

# ifdef NeXT
/* NeXT in the 2.{0,1,2} releases defines BSD in <sys/param.h>, which
   conflicts with the definition understood in this file, that this
   really is BSD. */
#  undef BSD

/* NeXT defines FSCALE in <sys/param.h>.  However, we take FSCALE being
   defined to mean that the nlist method should be used, which is not true.  */
#  undef FSCALE
# endif

/* Same issues as for NeXT apply to the HURD-based GNU system.  */
# ifdef __GNU__
#  undef BSD
#  undef FSCALE
# endif /* __GNU__ */

/* Set values that are different from the defaults, which are
   set a little farther down with #ifndef.  */


/* Some shorthands.  */

# if defined (HPUX) && !defined (hpux)
#  define hpux
# endif

# if defined (__hpux) && !defined (hpux)
#  define hpux
# endif

# if defined (__sun) && !defined (sun)
#  define sun
# endif

# if defined (hp300) && !defined (hpux)
#  define MORE_BSD
# endif

# if defined (__SVR4) && !defined (SVR4)
#  define SVR4
# endif

# if (defined (sun) && defined (SVR4)) || defined (SOLARIS2)
#  define SUNOS_5
# endif

# if defined (__osf__) && (defined (__alpha) || defined (__alpha__))
#  define OSF_ALPHA
#  include <sys/mbuf.h>
#  include <sys/socket.h>
#  include <net/route.h>
#  include <sys/table.h>
/* Tru64 4.0D's table.h redefines sys */
#  undef sys
# endif

# if defined (__osf__) && (defined (mips) || defined (__mips__))
#  define OSF_MIPS
#  include <sys/table.h>
# endif


/* VAX C can't handle multi-line #ifs, or lines longer than 256 chars.  */
# ifndef LOAD_AVE_TYPE

#  ifdef MORE_BSD
#   define LOAD_AVE_TYPE long
#  endif

#  ifdef sun
#   define LOAD_AVE_TYPE long
#  endif

#  ifdef sgi
#   define LOAD_AVE_TYPE long
#  endif

#  ifdef SVR4
#   define LOAD_AVE_TYPE long
#  endif

#  ifdef OSF_ALPHA
#   define LOAD_AVE_TYPE long
#  endif

#  if defined _AIX && ! defined HAVE_LIBPERFSTAT
#   define LOAD_AVE_TYPE long
#  endif

# endif /* No LOAD_AVE_TYPE.  */

# ifdef OSF_ALPHA
/* <sys/param.h> defines an incorrect value for FSCALE on Alpha OSF/1,
   according to ghazi@noc.rutgers.edu.  */
#  undef FSCALE
#  define FSCALE 1024.0
# endif


# ifndef FSCALE

/* SunOS and some others define FSCALE in sys/param.h.  */

#  ifdef MORE_BSD
#   define FSCALE 2048.0
#  endif

#  if defined (MIPS) || defined (SVR4)
#   define FSCALE 256
#  endif

#  if defined (sgi)
/* Sometimes both MIPS and sgi are defined, so FSCALE was just defined
   above under #ifdef MIPS.  But we want the sgi value.  */
#   undef FSCALE
#   define FSCALE 1000.0
#  endif

#  if defined _AIX && !defined HAVE_LIBPERFSTAT
#   define FSCALE 65536.0
#  endif

# endif /* Not FSCALE.  */

# if !defined (LDAV_CVT) && defined (FSCALE)
#  define LDAV_CVT(n) (((double) (n)) / FSCALE)
# endif

# ifndef NLIST_STRUCT
#  if HAVE_NLIST_H
#   define NLIST_STRUCT
#  endif
# endif

# if defined (sgi) || (defined (mips) && !defined (BSD))
#  define FIXUP_KERNEL_SYMBOL_ADDR(nl) ((nl)[0].n_value &= ~(1 << 31))
# endif


# if !defined (KERNEL_FILE) && defined (hpux)
#  define KERNEL_FILE "/hp-ux"
# endif

# if !defined (KERNEL_FILE) && (defined (MIPS) || defined (SVR4) || defined (ISC) || defined (sgi))
#  define KERNEL_FILE "/unix"
# endif


# if !defined (LDAV_SYMBOL) && (defined (hpux) || defined (SVR4) || defined (ISC) || defined (sgi) || (defined (_AIX) && !defined(HAVE_LIBPERFSTAT)))
#  define LDAV_SYMBOL "avenrun"
# endif

# ifdef HAVE_UNISTD_H
#  include <unistd.h>
# endif

/* LOAD_AVE_TYPE should only get defined if we're going to use the
   nlist method.  */
# if !defined (LOAD_AVE_TYPE) && (defined (BSD) || defined (LDAV_CVT) || defined (KERNEL_FILE) || defined (LDAV_SYMBOL))
#  define LOAD_AVE_TYPE double
# endif

# ifdef LOAD_AVE_TYPE

#  ifndef __VMS
#   if !(defined __linux__ || defined __ANDROID__)
#    ifndef NLIST_STRUCT
#     include <a.out.h>
#    else /* NLIST_STRUCT */
#     include <nlist.h>
#    endif /* NLIST_STRUCT */

#    ifdef SUNOS_5
#     include <kvm.h>
#     include <kstat.h>
#    endif

#    if defined (hpux) && defined (HAVE_PSTAT_GETDYNAMIC)
#     include <sys/pstat.h>
#    endif

#    ifndef KERNEL_FILE
#     define KERNEL_FILE "/vmunix"
#    endif /* KERNEL_FILE */

#    ifndef LDAV_SYMBOL
#     define LDAV_SYMBOL "_avenrun"
#    endif /* LDAV_SYMBOL */
#   endif /* __linux__ || __ANDROID__ */

#  else /* __VMS */

#   ifndef eunice
#    include <iodef.h>
#    include <descrip.h>
#   else /* eunice */
#    include <vms/iodef.h>
#   endif /* eunice */
#  endif /* __VMS */

#  ifndef LDAV_CVT
#   define LDAV_CVT(n) ((double) (n))
#  endif /* !LDAV_CVT */

# endif /* LOAD_AVE_TYPE */

# if defined HAVE_LIBPERFSTAT
#  include <sys/protosw.h>
#  include <libperfstat.h>
#  include <sys/proc.h>
#  ifndef SBITS
#   define SBITS 16
#  endif
# endif

# if defined (__GNU__) && !defined (NeXT)
/* Note that NeXT Openstep defines __GNU__ even though it should not.  */
/* GNU system acts much like NeXT, for load average purposes,
   but not exactly.  */
#  define NeXT
#  define host_self mach_host_self
# endif

# ifdef NeXT
#  ifdef HAVE_MACH_MACH_H
#   include <mach/mach.h>
#  else
#   include <mach.h>
#  endif
# endif /* NeXT */

# ifdef sgi
#  include <sys/sysmp.h>
# endif /* sgi */

# ifdef UMAX
#  include <signal.h>
#  include <sys/time.h>
#  include <sys/wait.h>
#  include <sys/syscall.h>

#  ifdef UMAX_43
#   include <machine/cpu.h>
#   include <inq_stats/statistics.h>
#   include <inq_stats/sysstats.h>
#   include <inq_stats/cpustats.h>
#   include <inq_stats/procstats.h>
#  else /* Not UMAX_43.  */
#   include <sys/sysdefs.h>
#   include <sys/statistics.h>
#   include <sys/sysstats.h>
#   include <sys/cpudefs.h>
#   include <sys/cpustats.h>
#   include <sys/procstats.h>
#  endif /* Not UMAX_43.  */
# endif /* UMAX */

# ifdef DGUX
#  include <sys/dg_sys_info.h>
# endif

# if (defined __linux__ || defined __ANDROID__ \
      || defined __CYGWIN__ || defined SUNOS_5 \
      || (defined LOAD_AVE_TYPE && ! defined __VMS))
#  include <fcntl.h>
# endif

/* Avoid static vars inside a function since in HPUX they dump as pure.  */

# ifdef NeXT
static processor_set_t default_set;
static bool getloadavg_initialized;
# endif /* NeXT */

# ifdef UMAX
static unsigned int cpus = 0;
static unsigned int samples;
# endif /* UMAX */

# ifdef DGUX
static struct dg_sys_info_load_info load_info;  /* what-a-mouthful! */
# endif /* DGUX */

# if !defined (HAVE_LIBKSTAT) && defined (LOAD_AVE_TYPE)
/* File descriptor open to /dev/kmem or VMS load ave driver.  */
static int channel;
/* True if channel is valid.  */
static bool getloadavg_initialized;
/* Offset in kmem to seek to read load average, or 0 means invalid.  */
static long offset;

#  if ! defined __VMS && ! defined sgi && ! (defined __linux__ || defined __ANDROID__)
static struct nlist name_list[2];
#  endif

#  ifdef SUNOS_5
static kvm_t *kd;
#  endif /* SUNOS_5 */

# endif /* LOAD_AVE_TYPE && !HAVE_LIBKSTAT */

/* Put the 1 minute, 5 minute and 15 minute load averages
   into the first NELEM elements of LOADAVG.
   Return the number written (never more than 3, but may be less than NELEM),
   or -1 (setting errno) if an error occurred.  */

int
getloadavg (double loadavg[], int nelem)
{
  int elem = 0;                 /* Return value.  */

# ifdef NO_GET_LOAD_AVG
#  define LDAV_DONE
  errno = ENOSYS;
  elem = -1;
# endif

# if !defined (LDAV_DONE) && defined (HAVE_LIBKSTAT)       /* Solaris <= 2.6 */
/* Use libkstat because we don't have to be root.  */
#  define LDAV_DONE
  kstat_ctl_t *kc;
  kstat_t *ksp;
  kstat_named_t *kn;
  int saved_errno;

  kc = kstat_open ();
  if (kc == NULL)
    return -1;
  ksp = kstat_lookup (kc, "unix", 0, "system_misc");
  if (ksp == NULL)
    return -1;
  if (kstat_read (kc, ksp, 0) == -1)
    return -1;


  kn = kstat_data_lookup (ksp, "avenrun_1min");
  if (kn == NULL)
    {
      /* Return -1 if no load average information is available.  */
      nelem = 0;
      elem = -1;
    }

  if (nelem >= 1)
    loadavg[elem++] = (double) kn->value.ul / FSCALE;

  if (nelem >= 2)
    {
      kn = kstat_data_lookup (ksp, "avenrun_5min");
      if (kn != NULL)
        {
          loadavg[elem++] = (double) kn->value.ul / FSCALE;

          if (nelem >= 3)
            {
              kn = kstat_data_lookup (ksp, "avenrun_15min");
              if (kn != NULL)
                loadavg[elem++] = (double) kn->value.ul / FSCALE;
            }
        }
    }

  saved_errno = errno;
  kstat_close (kc);
  errno = saved_errno;
# endif /* HAVE_LIBKSTAT */

# if !defined (LDAV_DONE) && defined (hpux) && defined (HAVE_PSTAT_GETDYNAMIC)
                                                           /* HP-UX */
/* Use pstat_getdynamic() because we don't have to be root.  */
#  define LDAV_DONE
#  undef LOAD_AVE_TYPE

  struct pst_dynamic dyn_info;
  if (pstat_getdynamic (&dyn_info, sizeof (dyn_info), 0, 0) < 0)
    return -1;
  if (nelem > 0)
    loadavg[elem++] = dyn_info.psd_avg_1_min;
  if (nelem > 1)
    loadavg[elem++] = dyn_info.psd_avg_5_min;
  if (nelem > 2)
    loadavg[elem++] = dyn_info.psd_avg_15_min;

# endif /* hpux && HAVE_PSTAT_GETDYNAMIC */

# if ! defined LDAV_DONE && defined HAVE_LIBPERFSTAT       /* AIX */
#  define LDAV_DONE
#  undef LOAD_AVE_TYPE
/* Use perfstat_cpu_total because we don't have to be root. */
  {
    perfstat_cpu_total_t cpu_stats;
    int result = perfstat_cpu_total (NULL, &cpu_stats, sizeof cpu_stats, 1);
    if (result == -1)
      return result;
    loadavg[0] = cpu_stats.loadavg[0] / (double)(1 << SBITS);
    loadavg[1] = cpu_stats.loadavg[1] / (double)(1 << SBITS);
    loadavg[2] = cpu_stats.loadavg[2] / (double)(1 << SBITS);
    elem = 3;
  }
# endif

# if !defined (LDAV_DONE) && (defined __linux__ || defined __ANDROID__ || defined __CYGWIN__)
                                      /* Linux without glibc, Android, Cygwin */
#  define LDAV_DONE
#  undef LOAD_AVE_TYPE

#  ifndef LINUX_LDAV_FILE
#   define LINUX_LDAV_FILE "/proc/loadavg"
#  endif

  char ldavgbuf[3 * (INT_STRLEN_BOUND (int) + sizeof ".00 ")];
  char const *ptr = ldavgbuf;
  int fd, count, saved_errno;

  fd = open (LINUX_LDAV_FILE, O_RDONLY);
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
            {
              errno = ENOTSUP;
              return -1;
            }
          break;
        }

      while ('0' <= *ptr && *ptr <= '9')
        numerator = 10 * numerator + (*ptr++ - '0');

      if (*ptr == '.')
        for (ptr++; '0' <= *ptr && *ptr <= '9'; ptr++)
          numerator = 10 * numerator + (*ptr - '0'), denominator *= 10;

      loadavg[elem++] = numerator / denominator;
    }

  return elem;

# endif /* __linux__ || __ANDROID__ || __CYGWIN__ */

# if !defined (LDAV_DONE) && defined (__NetBSD__)          /* NetBSD < 0.9 */
#  define LDAV_DONE
#  undef LOAD_AVE_TYPE

#  ifndef NETBSD_LDAV_FILE
#   define NETBSD_LDAV_FILE "/kern/loadavg"
#  endif

  unsigned long int load_ave[3], scale;
  int count;
  FILE *fp;

  fp = fopen (NETBSD_LDAV_FILE, "r");
  if (fp == NULL)
    return -1;
  count = fscanf (fp, "%lu %lu %lu %lu\n",
                  &load_ave[0], &load_ave[1], &load_ave[2],
                  &scale);
  (void) fclose (fp);
  if (count != 4)
    {
      errno = ENOTSUP;
      return -1;
    }

  for (elem = 0; elem < nelem; elem++)
    loadavg[elem] = (double) load_ave[elem] / (double) scale;

  return elem;

# endif /* __NetBSD__ */

# if !defined (LDAV_DONE) && defined (NeXT)                /* NeXTStep */
#  define LDAV_DONE
  /* The NeXT code was adapted from iscreen 3.2.  */

  host_t host;
  struct processor_set_basic_info info;
  unsigned int info_count;

  /* We only know how to get the 1-minute average for this system,
     so even if the caller asks for more than 1, we only return 1.  */

  if (!getloadavg_initialized)
    {
      if (processor_set_default (host_self (), &default_set) == KERN_SUCCESS)
        getloadavg_initialized = true;
    }

  if (getloadavg_initialized)
    {
      info_count = PROCESSOR_SET_BASIC_INFO_COUNT;
      if (processor_set_info (default_set, PROCESSOR_SET_BASIC_INFO, &host,
                              (processor_set_info_t) &info, &info_count)
          != KERN_SUCCESS)
        getloadavg_initialized = false;
      else
        {
          if (nelem > 0)
            loadavg[elem++] = (double) info.load_average / LOAD_SCALE;
        }
    }

  if (!getloadavg_initialized)
    {
      errno = ENOTSUP;
      return -1;
    }
# endif /* NeXT */

# if !defined (LDAV_DONE) && defined (UMAX)
#  define LDAV_DONE
/* UMAX 4.2, which runs on the Encore Multimax multiprocessor, does not
   have a /dev/kmem.  Information about the workings of the running kernel
   can be gathered with inq_stats system calls.
   We only know how to get the 1-minute average for this system.  */

  struct proc_summary proc_sum_data;
  struct stat_descr proc_info;
  double load;
  register unsigned int i, j;

  if (cpus == 0)
    {
      register unsigned int c, i;
      struct cpu_config conf;
      struct stat_descr desc;

      desc.sd_next = 0;
      desc.sd_subsys = SUBSYS_CPU;
      desc.sd_type = CPUTYPE_CONFIG;
      desc.sd_addr = (char *) &conf;
      desc.sd_size = sizeof conf;

      if (inq_stats (1, &desc))
        return -1;

      c = 0;
      for (i = 0; i < conf.config_maxclass; ++i)
        {
          struct class_stats stats;
          memset (&stats, 0, sizeof stats);

          desc.sd_type = CPUTYPE_CLASS;
          desc.sd_objid = i;
          desc.sd_addr = (char *) &stats;
          desc.sd_size = sizeof stats;

          if (inq_stats (1, &desc))
            return -1;

          c += stats.class_numcpus;
        }
      cpus = c;
      samples = cpus < 2 ? 3 : (2 * cpus / 3);
    }

  proc_info.sd_next = 0;
  proc_info.sd_subsys = SUBSYS_PROC;
  proc_info.sd_type = PROCTYPE_SUMMARY;
  proc_info.sd_addr = (char *) &proc_sum_data;
  proc_info.sd_size = sizeof (struct proc_summary);
  proc_info.sd_sizeused = 0;

  if (inq_stats (1, &proc_info) != 0)
    return -1;

  load = proc_sum_data.ps_nrunnable;
  j = 0;
  for (i = samples - 1; i > 0; --i)
    {
      load += proc_sum_data.ps_nrun[j];
      if (j++ == PS_NRUNSIZE)
        j = 0;
    }

  if (nelem > 0)
    loadavg[elem++] = load / samples / cpus;
# endif /* UMAX */

# if !defined (LDAV_DONE) && defined (DGUX)
#  define LDAV_DONE
  /* This call can return -1 for an error, but with good args
     it's not supposed to fail.  The first argument is for no
     apparent reason of type 'long int *'.  */
  dg_sys_info ((long int *) &load_info,
               DG_SYS_INFO_LOAD_INFO_TYPE,
               DG_SYS_INFO_LOAD_VERSION_0);

  if (nelem > 0)
    loadavg[elem++] = load_info.one_minute;
  if (nelem > 1)
    loadavg[elem++] = load_info.five_minute;
  if (nelem > 2)
    loadavg[elem++] = load_info.fifteen_minute;
# endif /* DGUX */

# if !defined (LDAV_DONE) && defined (apollo)
#  define LDAV_DONE
/* Apollo code from lisch@mentorg.com (Ray Lischner).

   This system call is not documented.  The load average is obtained as
   three long integers, for the load average over the past minute,
   five minutes, and fifteen minutes.  Each value is a scaled integer,
   with 16 bits of integer part and 16 bits of fraction part.

   I'm not sure which operating system first supported this system call,
   but I know that SR10.2 supports it.  */

  extern void proc1_$get_loadav ();
  unsigned long load_ave[3];

  proc1_$get_loadav (load_ave);

  if (nelem > 0)
    loadavg[elem++] = load_ave[0] / 65536.0;
  if (nelem > 1)
    loadavg[elem++] = load_ave[1] / 65536.0;
  if (nelem > 2)
    loadavg[elem++] = load_ave[2] / 65536.0;
# endif /* apollo */

# if !defined (LDAV_DONE) && defined (OSF_MIPS)
#  define LDAV_DONE

  struct tbl_loadavg load_ave;
  table (TBL_LOADAVG, 0, &load_ave, 1, sizeof (load_ave));
  loadavg[elem++]
    = (load_ave.tl_lscale == 0
       ? load_ave.tl_avenrun.d[0]
       : (load_ave.tl_avenrun.l[0] / (double) load_ave.tl_lscale));
# endif /* OSF_MIPS */

# if !defined (LDAV_DONE) && (defined (__MSDOS__) || defined (WINDOWS32))
                                                           /* DJGPP */
#  define LDAV_DONE

  /* A faithful emulation is going to have to be saved for a rainy day.  */
  for ( ; elem < nelem; elem++)
    {
      loadavg[elem] = 0.0;
    }
# endif  /* __MSDOS__ || WINDOWS32 */

# if !defined (LDAV_DONE) && defined (OSF_ALPHA)           /* OSF/1 */
#  define LDAV_DONE

  struct tbl_loadavg load_ave;
  table (TBL_LOADAVG, 0, &load_ave, 1, sizeof (load_ave));
  for (elem = 0; elem < nelem; elem++)
    loadavg[elem]
      = (load_ave.tl_lscale == 0
         ? load_ave.tl_avenrun.d[elem]
         : (load_ave.tl_avenrun.l[elem] / (double) load_ave.tl_lscale));
# endif /* OSF_ALPHA */

# if ! defined LDAV_DONE && defined __VMS                  /* VMS */
  /* VMS specific code -- read from the Load Ave driver.  */

  LOAD_AVE_TYPE load_ave[3];
  static bool getloadavg_initialized;
#  ifdef eunice
  struct
  {
    int dsc$w_length;
    char *dsc$a_pointer;
  } descriptor;
#  endif

  /* Ensure that there is a channel open to the load ave device.  */
  if (!getloadavg_initialized)
    {
      /* Attempt to open the channel.  */
#  ifdef eunice
      descriptor.dsc$w_length = 18;
      descriptor.dsc$a_pointer = "$$VMS_LOAD_AVERAGE";
#  else
      $DESCRIPTOR (descriptor, "LAV0:");
#  endif
      if (sys$assign (&descriptor, &channel, 0, 0) & 1)
        getloadavg_initialized = true;
    }

  /* Read the load average vector.  */
  if (getloadavg_initialized
      && !(sys$qiow (0, channel, IO$_READVBLK, 0, 0, 0,
                     load_ave, 12, 0, 0, 0, 0) & 1))
    {
      sys$dassgn (channel);
      getloadavg_initialized = false;
    }

  if (!getloadavg_initialized)
    {
      errno = ENOTSUP;
      return -1;
    }
# endif /* ! defined LDAV_DONE && defined __VMS */

# if ! defined LDAV_DONE && defined LOAD_AVE_TYPE && ! defined __VMS
                                                  /* IRIX, other old systems */

  /* UNIX-specific code -- read the average from /dev/kmem.  */

#  define LDAV_PRIVILEGED               /* This code requires special installation.  */

  LOAD_AVE_TYPE load_ave[3];

  /* Get the address of LDAV_SYMBOL.  */
  if (offset == 0)
    {
#  ifndef sgi
#   if ! defined NLIST_STRUCT || ! defined N_NAME_POINTER
      strcpy (name_list[0].n_name, LDAV_SYMBOL);
      strcpy (name_list[1].n_name, "");
#   else /* NLIST_STRUCT */
#    ifdef HAVE_STRUCT_NLIST_N_UN_N_NAME
      name_list[0].n_un.n_name = LDAV_SYMBOL;
      name_list[1].n_un.n_name = 0;
#    else /* not HAVE_STRUCT_NLIST_N_UN_N_NAME */
      name_list[0].n_name = LDAV_SYMBOL;
      name_list[1].n_name = 0;
#    endif /* not HAVE_STRUCT_NLIST_N_UN_N_NAME */
#   endif /* NLIST_STRUCT */

#   ifndef SUNOS_5
      if (
#    if !defined (_AIX)
          nlist (KERNEL_FILE, name_list)
#    else  /* _AIX */
          knlist (name_list, 1, sizeof (name_list[0]))
#    endif
          >= 0)
          /* Omit "&& name_list[0].n_type != 0 " -- it breaks on Sun386i.  */
          {
#    ifdef FIXUP_KERNEL_SYMBOL_ADDR
            FIXUP_KERNEL_SYMBOL_ADDR (name_list);
#    endif
            offset = name_list[0].n_value;
          }
#   endif /* !SUNOS_5 */
#  else  /* sgi */
      ptrdiff_t ldav_off = sysmp (MP_KERNADDR, MPKA_AVENRUN);
      if (ldav_off != -1)
        offset = (long int) ldav_off & 0x7fffffff;
#  endif /* sgi */
    }

  /* Make sure we have /dev/kmem open.  */
  if (!getloadavg_initialized)
    {
#  ifndef SUNOS_5
      /* Set the channel to close on exec, so it does not
         litter any child's descriptor table.  */
#   ifndef O_CLOEXEC
#    define O_CLOEXEC 0
#   endif
      int fd = open ("/dev/kmem", O_RDONLY | O_CLOEXEC);
      if (0 <= fd)
        {
#   if F_DUPFD_CLOEXEC
          if (fd <= STDERR_FILENO)
            {
              int fd1 = fcntl (fd, F_DUPFD_CLOEXEC, STDERR_FILENO + 1);
              close (fd);
              fd = fd1;
            }
#   endif
          if (0 <= fd)
            {
              channel = fd;
              getloadavg_initialized = true;
            }
        }
#  else /* SUNOS_5 */
      /* We pass 0 for the kernel, corefile, and swapfile names
         to use the currently running kernel.  */
      kd = kvm_open (0, 0, 0, O_RDONLY, 0);
      if (kd != NULL)
        {
          /* nlist the currently running kernel.  */
          kvm_nlist (kd, name_list);
          offset = name_list[0].n_value;
          getloadavg_initialized = true;
        }
#  endif /* SUNOS_5 */
    }

  /* If we can, get the load average values.  */
  if (offset && getloadavg_initialized)
    {
      /* Try to read the load.  */
#  ifndef SUNOS_5
      if (lseek (channel, offset, 0) == -1L
          || read (channel, (char *) load_ave, sizeof (load_ave))
          != sizeof (load_ave))
        {
          close (channel);
          getloadavg_initialized = false;
        }
#  else  /* SUNOS_5 */
      if (kvm_read (kd, offset, (char *) load_ave, sizeof (load_ave))
          != sizeof (load_ave))
        {
          kvm_close (kd);
          getloadavg_initialized = false;
        }
#  endif /* SUNOS_5 */
    }

  if (offset == 0 || !getloadavg_initialized)
    {
      errno = ENOTSUP;
      return -1;
    }
# endif /* ! defined LDAV_DONE && defined LOAD_AVE_TYPE && ! defined __VMS */

# if !defined (LDAV_DONE) && defined (LOAD_AVE_TYPE) /* Including VMS.  */
  if (nelem > 0)
    loadavg[elem++] = LDAV_CVT (load_ave[0]);
  if (nelem > 1)
    loadavg[elem++] = LDAV_CVT (load_ave[1]);
  if (nelem > 2)
    loadavg[elem++] = LDAV_CVT (load_ave[2]);

#  define LDAV_DONE
# endif /* !LDAV_DONE && LOAD_AVE_TYPE */

# if !defined LDAV_DONE
  errno = ENOSYS;
  elem = -1;
# endif
  return elem;
}
