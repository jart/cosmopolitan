/* ulimit.c, created from ulimit.def. */
#line 22 "./ulimit.def"

#line 73 "./ulimit.def"

#if !defined (_MINIX)

#include "config.h"

#include "bashtypes.h"
#if defined (HAVE_SYS_PARAM_H)
#  include <sys/param.h>
#endif

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include <stdio.h>
#include <errno.h>

#include "bashintl.h"

#include "shell.h"
#include "common.h"
#include "bashgetopt.h"
#include "pipesize.h"

#if !defined (errno)
extern int errno;
#endif

/* For some reason, HPUX chose to make these definitions visible only if
   _KERNEL is defined, so we define _KERNEL before including <sys/resource.h>
   and #undef it afterward. */
#if defined (HAVE_RESOURCE)
#  include <sys/time.h>
#  if defined (HPUX) && defined (RLIMIT_NEEDS_KERNEL)
#    define _KERNEL
#  endif
#  include <sys/resource.h>
#  if defined (HPUX) && defined (RLIMIT_NEEDS_KERNEL)
#    undef _KERNEL
#  endif
#elif defined (HAVE_SYS_TIMES_H)
#  include <sys/times.h>
#endif

#if defined (HAVE_LIMITS_H)
#  include <limits.h>
#endif

/* Check for the most basic symbols.  If they aren't present, this
   system's <sys/resource.h> isn't very useful to us. */
#if !defined (RLIMIT_FSIZE) || !defined (HAVE_GETRLIMIT)
#  undef HAVE_RESOURCE
#endif

#if !defined (HAVE_RESOURCE) && defined (HAVE_ULIMIT_H)
#  include <ulimit.h>
#endif

#if !defined (RLIMTYPE)
#  define RLIMTYPE long
#  define string_to_rlimtype(s) strtol(s, (char **)NULL, 10)
#  define print_rlimtype(num, nl) printf ("%ld%s", num, nl ? "\n" : "")
#endif

/* Alternate names */

/* Some systems use RLIMIT_NOFILE, others use RLIMIT_OFILE */
#if defined (HAVE_RESOURCE) && defined (RLIMIT_OFILE) && !defined (RLIMIT_NOFILE)
#  define RLIMIT_NOFILE RLIMIT_OFILE
#endif /* HAVE_RESOURCE && RLIMIT_OFILE && !RLIMIT_NOFILE */

#if defined (HAVE_RESOURCE) && defined (RLIMIT_POSIXLOCKS) && !defined (RLIMIT_LOCKS)
#  define RLIMIT_LOCKS RLIMIT_POSIXLOCKS
#endif /* HAVE_RESOURCE && RLIMIT_POSIXLOCKS && !RLIMIT_LOCKS */

/* Some systems have these, some do not. */
#ifdef RLIMIT_FSIZE
#  define RLIMIT_FILESIZE	RLIMIT_FSIZE
#else
#  define RLIMIT_FILESIZE	256
#endif

#define RLIMIT_PIPESIZE	257

#ifdef RLIMIT_NOFILE
#  define RLIMIT_OPENFILES	RLIMIT_NOFILE
#else
#  define RLIMIT_OPENFILES	258
#endif

#ifdef RLIMIT_VMEM
#  define RLIMIT_VIRTMEM	RLIMIT_VMEM
#  define RLIMIT_VMBLKSZ	1024
#else
#  ifdef RLIMIT_AS
#    define RLIMIT_VIRTMEM	RLIMIT_AS
#    define RLIMIT_VMBLKSZ	1024
#  else
#    define RLIMIT_VIRTMEM	259
#    define RLIMIT_VMBLKSZ	1
#  endif
#endif

#ifdef RLIMIT_NPROC
#  define RLIMIT_MAXUPROC	RLIMIT_NPROC
#else
#  define RLIMIT_MAXUPROC	260
#endif

#if !defined (RLIMIT_PTHREAD) && defined (RLIMIT_NTHR)
#  define RLIMIT_PTHREAD RLIMIT_NTHR
#endif

#if !defined (RLIM_INFINITY)
#  define RLIM_INFINITY 0x7fffffff
#endif

#if !defined (RLIM_SAVED_CUR)
#  define RLIM_SAVED_CUR RLIM_INFINITY
#endif

#if !defined (RLIM_SAVED_MAX)
#  define RLIM_SAVED_MAX RLIM_INFINITY
#endif

#define LIMIT_HARD 0x01
#define LIMIT_SOFT 0x02

/* "Blocks" are defined as 512 bytes when in Posix mode and 1024 bytes
   otherwise. */
#define POSIXBLK	-2

#define BLOCKSIZE(x)	(((x) == POSIXBLK) ? (posixly_correct ? 512 : 1024) : (x))

static int _findlim PARAMS((int));

static int ulimit_internal PARAMS((int, char *, int, int));

static int get_limit PARAMS((int, RLIMTYPE *, RLIMTYPE *));
static int set_limit PARAMS((int, RLIMTYPE, int));

static void printone PARAMS((int, RLIMTYPE, int));
static void print_all_limits PARAMS((int));

static int set_all_limits PARAMS((int, RLIMTYPE));

static int filesize PARAMS((RLIMTYPE *));
static int pipesize PARAMS((RLIMTYPE *));
static int getmaxuprc PARAMS((RLIMTYPE *));
static int getmaxvm PARAMS((RLIMTYPE *, RLIMTYPE *));

typedef struct {
  int  option;			/* The ulimit option for this limit. */
  int  parameter;		/* Parameter to pass to get_limit (). */
  int  block_factor;		/* Blocking factor for specific limit. */
  const char * const description;	/* Descriptive string to output. */
  const char * const units;	/* scale */
} RESOURCE_LIMITS;

static RESOURCE_LIMITS limits[] = {
#ifdef RLIMIT_NPTS
  { 'P',	RLIMIT_NPTS,  1,	"number of pseudoterminals",	(char *)NULL },
#endif
#ifdef RLIMIT_RTTIME
  { 'R',	RLIMIT_RTTIME,  1,	"real-time non-blocking time",	"microseconds" },
#endif
#ifdef RLIMIT_PTHREAD
  { 'T',	RLIMIT_PTHREAD, 1,	"number of threads",	(char *)NULL },
#endif
#ifdef RLIMIT_SBSIZE
  { 'b',	RLIMIT_SBSIZE,  1,	"socket buffer size",	"bytes" },
#endif
#ifdef RLIMIT_CORE
  { 'c',	RLIMIT_CORE,  POSIXBLK,	"core file size",	"blocks" },
#endif
#ifdef RLIMIT_DATA
  { 'd',	RLIMIT_DATA,  1024,	"data seg size",	"kbytes" },
#endif
#ifdef RLIMIT_NICE
  { 'e',	RLIMIT_NICE,  1,	"scheduling priority",	(char *)NULL },
#endif
  { 'f',	RLIMIT_FILESIZE, POSIXBLK,	"file size",		"blocks" },
#ifdef RLIMIT_SIGPENDING
  { 'i',	RLIMIT_SIGPENDING, 1,	"pending signals",	(char *)NULL },
#endif
#ifdef RLIMIT_KQUEUES
  { 'k',	RLIMIT_KQUEUES, 1,	"max kqueues",		(char *)NULL },
#endif
#ifdef RLIMIT_MEMLOCK
  { 'l',	RLIMIT_MEMLOCK, 1024,	"max locked memory",	"kbytes" },
#endif
#ifdef RLIMIT_RSS
  { 'm',	RLIMIT_RSS,   1024,	"max memory size",	"kbytes" },
#endif /* RLIMIT_RSS */
  { 'n',	RLIMIT_OPENFILES, 1,	"open files",		(char *)NULL},
  { 'p',	RLIMIT_PIPESIZE, 512,	"pipe size", 		"512 bytes" },
#ifdef RLIMIT_MSGQUEUE
  { 'q',	RLIMIT_MSGQUEUE, 1,	"POSIX message queues",	"bytes" },
#endif
#ifdef RLIMIT_RTPRIO
  { 'r',	RLIMIT_RTPRIO,  1,	"real-time priority",	(char *)NULL },
#endif
#ifdef RLIMIT_STACK
  { 's',	RLIMIT_STACK, 1024,	"stack size",		"kbytes" },
#endif
#ifdef RLIMIT_CPU
  { 't',	RLIMIT_CPU,      1,	"cpu time",		"seconds" },
#endif /* RLIMIT_CPU */
  { 'u',	RLIMIT_MAXUPROC, 1,	"max user processes",	(char *)NULL },
#if defined (HAVE_RESOURCE)
  { 'v',	RLIMIT_VIRTMEM, RLIMIT_VMBLKSZ, "virtual memory", "kbytes" },
#endif
#ifdef RLIMIT_SWAP
  { 'w',	RLIMIT_SWAP,	1024,	"swap size",		"kbytes" },
#endif
#ifdef RLIMIT_LOCKS
  { 'x',	RLIMIT_LOCKS,	1,	"file locks",		(char *)NULL },
#endif
  { -1, -1, -1, (char *)NULL, (char *)NULL }
};
#define NCMDS	(sizeof(limits) / sizeof(limits[0]))

typedef struct _cmd {
  int cmd;
  char *arg;
} ULCMD;

static ULCMD *cmdlist;
static int ncmd;
static int cmdlistsz;

#if !defined (HAVE_RESOURCE) && !defined (HAVE_ULIMIT)
long
ulimit (cmd, newlim)
     int cmd;
     long newlim;
{
  errno = EINVAL;
  return -1;
}
#endif /* !HAVE_RESOURCE && !HAVE_ULIMIT */

static int
_findlim (opt)
     int opt;
{
  register int i;

  for (i = 0; limits[i].option > 0; i++)
    if (limits[i].option == opt)
      return i;
  return -1;
}

static char optstring[4 + 2 * NCMDS];

/* Report or set limits associated with certain per-process resources.
   See the help documentation in builtins.c for a full description. */
int
ulimit_builtin (list)
     register WORD_LIST *list;
{
  register char *s;
  int c, limind, mode, opt, all_limits;

  mode = 0;

  all_limits = 0;

  /* Idea stolen from pdksh -- build option string the first time called. */
  if (optstring[0] == 0)
    {
      s = optstring;
      *s++ = 'a'; *s++ = 'S'; *s++ = 'H';
      for (c = 0; limits[c].option > 0; c++)
	{
	  *s++ = limits[c].option;
	  *s++ = ';';
	}
      *s = '\0';
    }

  /* Initialize the command list. */
  if (cmdlistsz == 0)
    cmdlist = (ULCMD *)xmalloc ((cmdlistsz = 16) * sizeof (ULCMD));
  ncmd = 0;

  reset_internal_getopt ();
  while ((opt = internal_getopt (list, optstring)) != -1)
    {
      switch (opt)
	{
	case 'a':
	  all_limits++;
	  break;

	/* -S and -H are modifiers, not real options.  */
	case 'S':
	  mode |= LIMIT_SOFT;
	  break;

	case 'H':
	  mode |= LIMIT_HARD;
	  break;

	CASE_HELPOPT;
	case '?':
	  builtin_usage ();
	  return (EX_USAGE);

	default:
	  if (ncmd >= cmdlistsz)
	    cmdlist = (ULCMD *)xrealloc (cmdlist, (cmdlistsz *= 2) * sizeof (ULCMD));
	  cmdlist[ncmd].cmd = opt;
	  cmdlist[ncmd++].arg = list_optarg;
	  break;
	}
    }
  list = loptend;

  if (all_limits)
    {
#ifdef NOTYET
      if (list)		/* setting */
        {
          if (STREQ (list->word->word, "unlimited") == 0)
            {
              builtin_error (_("%s: invalid limit argument"), list->word->word);
              return (EXECUTION_FAILURE);
            }
          return (set_all_limits (mode == 0 ? LIMIT_SOFT|LIMIT_HARD : mode, RLIM_INFINITY));
        }
#endif
      print_all_limits (mode == 0 ? LIMIT_SOFT : mode);
      return (sh_chkwrite (EXECUTION_SUCCESS));
    }

  /* default is `ulimit -f' */
  if (ncmd == 0)
    {
      cmdlist[ncmd].cmd = 'f';
      /* `ulimit something' is same as `ulimit -f something' */
      cmdlist[ncmd++].arg = list ? list->word->word : (char *)NULL;
      if (list)
	list = list->next;
    }

  /* verify each command in the list. */
  for (c = 0; c < ncmd; c++)
    {
      limind = _findlim (cmdlist[c].cmd);
      if (limind == -1)
	{
	  builtin_error (_("`%c': bad command"), cmdlist[c].cmd);
	  return (EX_USAGE);
	}
    }

  /* POSIX compatibility. If the last item in cmdlist does not have an option
     argument, but there is an operand (list != 0), treat the operand as if
     it were an option argument for that last command. */
  if (list && list->word && cmdlist[ncmd - 1].arg == 0)
    {
      cmdlist[ncmd - 1].arg = list->word->word;
      list = list->next;
    }

  for (c = 0; c < ncmd; c++)
    if (ulimit_internal (cmdlist[c].cmd, cmdlist[c].arg, mode, ncmd > 1) == EXECUTION_FAILURE)
      return (EXECUTION_FAILURE);

  return (EXECUTION_SUCCESS);
}

static int
ulimit_internal (cmd, cmdarg, mode, multiple)
     int cmd;
     char *cmdarg;
     int mode, multiple;
{
  int opt, limind, setting;
  int block_factor;
  RLIMTYPE soft_limit, hard_limit, real_limit, limit;

  setting = cmdarg != 0;
  limind = _findlim (cmd);
  if (mode == 0)
    mode = setting ? (LIMIT_HARD|LIMIT_SOFT) : LIMIT_SOFT;
  opt = get_limit (limind, &soft_limit, &hard_limit);
  if (opt < 0)
    {
      builtin_error (_("%s: cannot get limit: %s"), limits[limind].description,
						 strerror (errno));
      return (EXECUTION_FAILURE);
    }

  if (setting == 0)	/* print the value of the specified limit */
    {
      printone (limind, (mode & LIMIT_SOFT) ? soft_limit : hard_limit, multiple);
      return (EXECUTION_SUCCESS);
    }
 
  /* Setting the limit. */
  if (STREQ (cmdarg, "hard"))
    real_limit = hard_limit;
  else if (STREQ (cmdarg, "soft"))
    real_limit = soft_limit;
  else if (STREQ (cmdarg, "unlimited"))
    real_limit = RLIM_INFINITY;
  else if (all_digits (cmdarg))
    {
      limit = string_to_rlimtype (cmdarg);
      block_factor = BLOCKSIZE(limits[limind].block_factor);
      real_limit = limit * block_factor;

      if ((real_limit / block_factor) != limit)
	{
	  sh_erange (cmdarg, _("limit"));
	  return (EXECUTION_FAILURE);
	}
    }
  else
    {
      sh_invalidnum (cmdarg);
      return (EXECUTION_FAILURE);
    }

  if (set_limit (limind, real_limit, mode) < 0)
    {
      builtin_error (_("%s: cannot modify limit: %s"), limits[limind].description,
						    strerror (errno));
      return (EXECUTION_FAILURE);
    }

  return (EXECUTION_SUCCESS);
}

static int
get_limit (ind, softlim, hardlim)
     int ind;
     RLIMTYPE *softlim, *hardlim;
{
  RLIMTYPE value;
#if defined (HAVE_RESOURCE)
  struct rlimit limit;
#endif

  if (limits[ind].parameter >= 256)
    {
      switch (limits[ind].parameter)
	{
	case RLIMIT_FILESIZE:
	  if (filesize (&value) < 0)
	    return -1;
	  break;
	case RLIMIT_PIPESIZE:
	  if (pipesize (&value) < 0)
	    return -1;
	  break;
	case RLIMIT_OPENFILES:
	  value = (RLIMTYPE)getdtablesize ();
	  break;
	case RLIMIT_VIRTMEM:
	  return (getmaxvm (softlim, hardlim));
	case RLIMIT_MAXUPROC:
	  if (getmaxuprc (&value) < 0)
	    return -1;
	  break;
	default:
	  errno = EINVAL;
	  return -1;
	}
      *softlim = *hardlim = value;
      return (0);
    }
  else
    {
#if defined (HAVE_RESOURCE)
      if (getrlimit (limits[ind].parameter, &limit) < 0)
	return -1;
      *softlim = limit.rlim_cur;
      *hardlim = limit.rlim_max;
#  if defined (HPUX9)
      if (limits[ind].parameter == RLIMIT_FILESIZE)
	{
	  *softlim *= 512;
	  *hardlim *= 512;			/* Ugh. */
	}
      else
#  endif /* HPUX9 */
      return 0;
#else
      errno = EINVAL;
      return -1;
#endif
    }
}

static int
set_limit (ind, newlim, mode)
     int ind;
     RLIMTYPE newlim;
     int mode;
{
#if defined (HAVE_RESOURCE)
   struct rlimit limit;
   RLIMTYPE val;
#endif

  if (limits[ind].parameter >= 256)
    switch (limits[ind].parameter)
      {
      case RLIMIT_FILESIZE:
#if !defined (HAVE_RESOURCE)
	return (ulimit (2, newlim / 512L));
#else
	errno = EINVAL;
	return -1;
#endif

      case RLIMIT_OPENFILES:
#if defined (HAVE_SETDTABLESIZE)
#  if defined (__CYGWIN__)
	/* Grrr... Cygwin declares setdtablesize as void. */
	setdtablesize (newlim);
	return 0;
#  else
	return (setdtablesize (newlim));
#  endif
#endif
      case RLIMIT_PIPESIZE:
      case RLIMIT_VIRTMEM:
      case RLIMIT_MAXUPROC:
      default:
	errno = EINVAL;
	return -1;
      }
  else
    {
#if defined (HAVE_RESOURCE)
      if (getrlimit (limits[ind].parameter, &limit) < 0)
	return -1;
#  if defined (HPUX9)
      if (limits[ind].parameter == RLIMIT_FILESIZE)
	newlim /= 512;				/* Ugh. */
#  endif /* HPUX9 */
      val = (current_user.euid != 0 && newlim == RLIM_INFINITY &&
	       (mode & LIMIT_HARD) == 0 &&		/* XXX -- test */
	       (limit.rlim_cur <= limit.rlim_max))
		 ? limit.rlim_max : newlim;
      if (mode & LIMIT_SOFT)
	limit.rlim_cur = val;
      if (mode & LIMIT_HARD)
	limit.rlim_max = val;
	  
      return (setrlimit (limits[ind].parameter, &limit));
#else
      errno = EINVAL;
      return -1;
#endif
    }
}

static int
getmaxvm (softlim, hardlim)
     RLIMTYPE *softlim, *hardlim;
{
#if defined (HAVE_RESOURCE)
  struct rlimit datalim, stacklim;

  if (getrlimit (RLIMIT_DATA, &datalim) < 0)
    return -1;

  if (getrlimit (RLIMIT_STACK, &stacklim) < 0)
    return -1;

  /* Protect against overflow. */
  *softlim = (datalim.rlim_cur / 1024L) + (stacklim.rlim_cur / 1024L);
  *hardlim = (datalim.rlim_max / 1024L) + (stacklim.rlim_max / 1024L);
  return 0;
#else
  errno = EINVAL;
  return -1;
#endif /* HAVE_RESOURCE */
}

static int
filesize(valuep)
     RLIMTYPE *valuep;
{
#if !defined (HAVE_RESOURCE)
  long result;
  if ((result = ulimit (1, 0L)) < 0)
    return -1;
  else
    *valuep = (RLIMTYPE) result * 512;
  return 0;
#else
  errno = EINVAL;
  return -1;
#endif
}

static int
pipesize (valuep)
     RLIMTYPE *valuep;
{
#if defined (PIPE_BUF)
  /* This is defined on Posix systems. */
  *valuep = (RLIMTYPE) PIPE_BUF;
  return 0;
#else
#  if defined (_POSIX_PIPE_BUF)
  *valuep = (RLIMTYPE) _POSIX_PIPE_BUF;
  return 0;
#  else
#    if defined (PIPESIZE)
  /* This is defined by running a program from the Makefile. */
  *valuep = (RLIMTYPE) PIPESIZE;
  return 0;
#    else
  errno = EINVAL;
  return -1;  
#    endif /* PIPESIZE */
#  endif /* _POSIX_PIPE_BUF */
#endif /* PIPE_BUF */
}

static int
getmaxuprc (valuep)
     RLIMTYPE *valuep;
{
  long maxchild;

  maxchild = getmaxchild ();
  if (maxchild < 0)
    {
      errno = EINVAL;
      return -1;
    }
  else
    {
      *valuep = (RLIMTYPE) maxchild;
      return 0;
    }
}

static void
print_all_limits (mode)
     int mode;
{
  register int i;
  RLIMTYPE softlim, hardlim;

  if (mode == 0)
    mode |= LIMIT_SOFT;

  for (i = 0; limits[i].option > 0; i++)
    {
      if (get_limit (i, &softlim, &hardlim) == 0)
	printone (i, (mode & LIMIT_SOFT) ? softlim : hardlim, 1);
      else if (errno != EINVAL)
	builtin_error ("%s: cannot get limit: %s", limits[i].description,
						   strerror (errno));
    }
}

static void
printone (limind, curlim, pdesc)
     int limind;
     RLIMTYPE curlim;
     int pdesc;
{
  char unitstr[64];
  int factor;

  factor = BLOCKSIZE(limits[limind].block_factor);
  if (pdesc)
    {
      if (limits[limind].units)
	sprintf (unitstr, "(%s, -%c) ", limits[limind].units, limits[limind].option);
      else
        sprintf (unitstr, "(-%c) ", limits[limind].option);

      printf ("%-20s %20s", limits[limind].description, unitstr);
    }
  if (curlim == RLIM_INFINITY)
    puts ("unlimited");
  else if (curlim == RLIM_SAVED_MAX)
    puts ("hard");
  else if (curlim == RLIM_SAVED_CUR)
    puts ("soft");
  else
    print_rlimtype ((curlim / factor), 1);
}

/* Set all limits to NEWLIM.  NEWLIM currently must be RLIM_INFINITY, which
   causes all limits to be set as high as possible depending on mode (like
   csh `unlimit').  Returns -1 if NEWLIM is invalid, 0 if all limits
   were set successfully, and 1 if at least one limit could not be set.

   To raise all soft limits to their corresponding hard limits, use
	ulimit -S -a unlimited
   To attempt to raise all hard limits to infinity (superuser-only), use
	ulimit -H -a unlimited
   To attempt to raise all soft and hard limits to infinity, use
	ulimit -a unlimited
*/

static int
set_all_limits (mode, newlim)
     int mode;
     RLIMTYPE newlim;
{
  register int i;
  int retval = 0;

  if (newlim != RLIM_INFINITY)
    {
      errno = EINVAL;
      return -1;
    }
  
  if (mode == 0)
    mode = LIMIT_SOFT|LIMIT_HARD;

  for (retval = i = 0; limits[i].option > 0; i++)
    if (set_limit (i, newlim, mode) < 0)
      {
	builtin_error (_("%s: cannot modify limit: %s"), limits[i].description,
						      strerror (errno));
	retval = 1;
      }
  return retval;
}

#endif /* !_MINIX */
