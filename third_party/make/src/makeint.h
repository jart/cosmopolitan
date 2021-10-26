/* Miscellaneous global declarations and portability cruft for GNU Make.
Copyright (C) 1988-2020 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* We use <config.h> instead of "config.h" so that a compilation
   using -I. -I$srcdir will use ./config.h rather than $srcdir/config.h
   (which it would do because makeint.h was found in $srcdir).  */
#include <config.h>
#undef  HAVE_CONFIG_H
#define HAVE_CONFIG_H 1

/* Specify we want GNU source code.  This must be defined before any
   system headers are included.  */

#define _GNU_SOURCE 1

/* AIX requires this to be the first thing in the file.  */
#if HAVE_ALLOCA_H
# include <alloca.h>
#else
# ifdef _AIX
 #pragma alloca
# else
#  if !defined(__GNUC__) && !defined(WINDOWS32)
#   ifndef alloca /* predefined by HP cc +Olibcalls */
char *alloca ();
#   endif
#  endif
# endif
#endif

/* Disable assert() unless we're a maintainer.
   Some asserts are compute-intensive.  */
#ifndef MAKE_MAINTAINER_MODE
# define NDEBUG 1
#endif

/* Include the externally-visible content.
   Be sure to use the local one, and not one installed on the system.
   Define GMK_BUILDING_MAKE for proper selection of dllexport/dllimport
   declarations for MS-Windows.  */
#ifdef WINDOWS32
# define GMK_BUILDING_MAKE
#endif
#include "gnumake.h"

#ifdef  CRAY
/* This must happen before #include <signal.h> so
   that the declaration therein is changed.  */
# define signal bsdsignal
#endif

/* If we're compiling for the dmalloc debugger, turn off string inlining.  */
#if defined(HAVE_DMALLOC_H) && defined(__GNUC__)
# define __NO_STRING_INLINES
#endif

#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdio.h>
#include <ctype.h>

#ifdef HAVE_SYS_TIMEB_H
/* SCO 3.2 "devsys 4.2" has a prototype for 'ftime' in <time.h> that bombs
   unless <sys/timeb.h> has been included first.  */
# include <sys/timeb.h>
#endif
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <errno.h>

#ifndef errno
extern int errno;
#endif

#ifdef __VMS
/* In strict ANSI mode, VMS compilers should not be defining the
   VMS macro.  Define it here instead of a bulk edit for the correct code.
 */
# ifndef VMS
#  define VMS
# endif
#endif

#ifdef  HAVE_UNISTD_H
# include <unistd.h>
/* Ultrix's unistd.h always defines _POSIX_VERSION, but you only get
   POSIX.1 behavior with 'cc -YPOSIX', which predefines POSIX itself!  */
# if defined (_POSIX_VERSION) && !defined (ultrix) && !defined (VMS)
#  define POSIX 1
# endif
#endif

/* Some systems define _POSIX_VERSION but are not really POSIX.1.  */
#if (defined (butterfly) || defined (__arm) || (defined (__mips) && defined (_SYSTYPE_SVR3)) || (defined (sequent) && defined (i386)))
# undef POSIX
#endif

#if !defined (POSIX) && defined (_AIX) && defined (_POSIX_SOURCE)
# define POSIX 1
#endif

#ifndef RETSIGTYPE
# define RETSIGTYPE     void
#endif

#ifndef sigmask
# define sigmask(sig)   (1 << ((sig) - 1))
#endif

#ifndef HAVE_SA_RESTART
# define SA_RESTART 0
#endif

#ifdef HAVE_VFORK_H
# include <vfork.h>
#endif

#ifdef  HAVE_LIMITS_H
# include <limits.h>
#endif
#ifdef  HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif

#ifndef PATH_MAX
# ifndef POSIX
#  define PATH_MAX      MAXPATHLEN
# endif
#endif
#ifndef MAXPATHLEN
# define MAXPATHLEN 1024
#endif

#ifdef  PATH_MAX
# define GET_PATH_MAX   PATH_MAX
# define PATH_VAR(var)  char var[PATH_MAX+1]
#else
# define NEED_GET_PATH_MAX 1
# define GET_PATH_MAX   (get_path_max ())
# define PATH_VAR(var)  char *var = alloca (GET_PATH_MAX+1)
unsigned int get_path_max (void);
#endif

#ifndef CHAR_BIT
# define CHAR_BIT 8
#endif

#ifndef USHRT_MAX
# define USHRT_MAX 65535
#endif

/* Nonzero if the integer type T is signed.
   Use <= to avoid GCC warnings about always-false expressions.  */
#define INTEGER_TYPE_SIGNED(t) ((t) -1 <= 0)

/* The minimum and maximum values for the integer type T.
   Use ~ (t) 0, not -1, for portability to 1's complement hosts.  */
#define INTEGER_TYPE_MINIMUM(t) \
  (! INTEGER_TYPE_SIGNED (t) ? (t) 0 : ~ (t) 0 << (sizeof (t) * CHAR_BIT - 1))
#define INTEGER_TYPE_MAXIMUM(t) (~ (t) 0 - INTEGER_TYPE_MINIMUM (t))

#ifndef CHAR_MAX
# define CHAR_MAX INTEGER_TYPE_MAXIMUM (char)
#endif

#ifdef STAT_MACROS_BROKEN
# ifdef S_ISREG
#  undef S_ISREG
# endif
# ifdef S_ISDIR
#  undef S_ISDIR
# endif
#endif  /* STAT_MACROS_BROKEN.  */

#ifndef S_ISREG
# define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
# define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

#ifdef VMS
# include <fcntl.h>
# include <types.h>
# include <unixlib.h>
# include <unixio.h>
# include <perror.h>
/* Needed to use alloca on VMS.  */
# include <builtins.h>

extern int vms_use_mcr_command;
extern int vms_always_use_cmd_file;
extern int vms_gnv_shell;
extern int vms_comma_separator;
extern int vms_legacy_behavior;
extern int vms_unix_simulation;
#endif

#if !defined(__attribute__) && (__GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 5) || __STRICT_ANSI__)
/* Don't use __attribute__ if it's not supported.  */
# define ATTRIBUTE(x)
#else
# define ATTRIBUTE(x) __attribute__ (x)
#endif

/* The __-protected variants of 'format' and 'printf' attributes
   are accepted by gcc versions 2.6.4 (effectively 2.7) and later.  */
#if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 7)
# define __format__ format
# define __printf__ printf
#endif

#define UNUSED   ATTRIBUTE ((unused))
#define NORETURN ATTRIBUTE ((noreturn))

#if defined (STDC_HEADERS) || defined (__GNU_LIBRARY__)
# include <stdlib.h>
# include <string.h>
# define ANSI_STRING 1
#else   /* No standard headers.  */
# ifdef HAVE_STRING_H
#  include <string.h>
#  define ANSI_STRING 1
# else
#  include <strings.h>
# endif
# ifdef HAVE_MEMORY_H
#  include <memory.h>
# endif
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# else
void *malloc (int);
void *realloc (void *, int);
void free (void *);

void abort (void) NORETURN;
void exit (int) NORETURN;
# endif /* HAVE_STDLIB_H.  */

#endif /* Standard headers.  */

/* These should be in stdlib.h.  Make sure we have them.  */
#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
# define EXIT_FAILURE 1
#endif

#ifndef  ANSI_STRING

/* SCO Xenix has a buggy macro definition in <string.h>.  */
#undef  strerror
#if !defined(__DECC)
char *strerror (int errnum);
#endif

#endif  /* !ANSI_STRING.  */
#undef  ANSI_STRING

#if HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#if HAVE_STDINT_H
# include <stdint.h>
#endif
#define FILE_TIMESTAMP uintmax_t

#if !defined(HAVE_STRSIGNAL)
char *strsignal (int signum);
#endif

#if !defined(HAVE_UMASK)
typedef int mode_t;
extern mode_t umask (mode_t);
#endif

/* ISDIGIT offers the following features:
   - Its arg may be any int or unsigned int; it need not be an unsigned char.
   - It's guaranteed to evaluate its argument exactly once.
      NOTE!  Make relies on this behavior, don't change it!
   - It's typically faster.
   POSIX 1003.2-1992 section 2.5.2.1 page 50 lines 1556-1558 says that
   only '0' through '9' are digits.  Prefer ISDIGIT to isdigit() unless
   it's important to use the locale's definition of 'digit' even when the
   host does not conform to POSIX.  */
#define ISDIGIT(c) ((unsigned) (c) - '0' <= 9)

/* Test if two strings are equal. Is this worthwhile?  Should be profiled.  */
#define streq(a, b) \
   ((a) == (b) || \
    (*(a) == *(b) && (*(a) == '\0' || !strcmp ((a) + 1, (b) + 1))))

/* Test if two strings are equal, but match case-insensitively on systems
   which have case-insensitive filesystems.  Should only be used for
   filenames!  */
#ifdef HAVE_CASE_INSENSITIVE_FS
# define patheq(a, b) \
    ((a) == (b) \
     || (tolower((unsigned char)*(a)) == tolower((unsigned char)*(b)) \
         && (*(a) == '\0' || !strcasecmp ((a) + 1, (b) + 1))))
#else
# define patheq(a, b) streq(a, b)
#endif

#define strneq(a, b, l) (strncmp ((a), (b), (l)) == 0)

#if defined(ENUM_BITFIELDS) || (defined(__GNUC__) && !defined(__STRICT_ANSI__))
# define ENUM_BITFIELD(bits)    :bits
#else
# define ENUM_BITFIELD(bits)
#endif

/* Handle gettext and locales.  */

#if HAVE_LOCALE_H
# include <locale.h>
#else
# define setlocale(category, locale)
#endif

#include <gettext.h>

#define _(msgid)            gettext (msgid)
#define N_(msgid)           gettext_noop (msgid)
#define S_(msg1,msg2,num)   ngettext (msg1,msg2,num)

/* This is needed for getcwd() and chdir(), on some W32 systems.  */
#if defined(HAVE_DIRECT_H)
# include <direct.h>
#endif

#ifdef WINDOWS32
# include <fcntl.h>
# include <malloc.h>
# define pipe(_p)        _pipe((_p), 512, O_BINARY)
# define kill(_pid,_sig) w32_kill((_pid),(_sig))
/* MSVC and Watcom C don't have ftruncate.  */
# if defined(_MSC_VER) || defined(__WATCOMC__)
#  define ftruncate(_fd,_len) _chsize(_fd,_len)
# endif
/* MinGW64 doesn't have _S_ISDIR.  */
# ifndef _S_ISDIR
#  define _S_ISDIR(m)  S_ISDIR(m)
# endif

void sync_Path_environment (void);
int w32_kill (pid_t pid, int sig);
int find_and_set_default_shell (const char *token);

/* indicates whether or not we have Bourne shell */
extern int no_default_sh_exe;

/* is default_shell unixy? */
extern int unixy_shell;

/* We don't have a preferred fixed value for LOCALEDIR.  */
# ifndef LOCALEDIR
#  define LOCALEDIR NULL
# endif

/* Include only the minimal stuff from windows.h.   */
# define WIN32_LEAN_AND_MEAN
#endif  /* WINDOWS32 */

#define ANY_SET(_v,_m)  (((_v)&(_m)) != 0)
#define NONE_SET(_v,_m) (! ANY_SET ((_v),(_m)))

#define MAP_NUL         0x0001
#define MAP_BLANK       0x0002
#define MAP_NEWLINE     0x0004
#define MAP_COMMENT     0x0008
#define MAP_SEMI        0x0010
#define MAP_EQUALS      0x0020
#define MAP_COLON       0x0040
#define MAP_VARSEP      0x0080
#define MAP_PIPE        0x0100
#define MAP_DOT         0x0200
#define MAP_COMMA       0x0400

/* These are the valid characters for a user-defined function.  */
#define MAP_USERFUNC    0x2000
/* This means not only a '$', but skip the variable reference.  */
#define MAP_VARIABLE    0x4000
/* The set of characters which are directory separators is OS-specific.  */
#define MAP_DIRSEP      0x8000

#ifdef VMS
# define MAP_VMSCOMMA   MAP_COMMA
#else
# define MAP_VMSCOMMA   0x0000
#endif

#define MAP_SPACE       (MAP_BLANK|MAP_NEWLINE)

/* Handle other OSs.
   To overcome an issue parsing paths in a DOS/Windows environment when
   built in a unix based environment, override the PATH_SEPARATOR_CHAR
   definition unless being built for Cygwin. */
#if defined(HAVE_DOS_PATHS) && !defined(__CYGWIN__)
# undef PATH_SEPARATOR_CHAR
# define PATH_SEPARATOR_CHAR ';'
# define MAP_PATHSEP    MAP_SEMI
#elif !defined(PATH_SEPARATOR_CHAR)
# if defined (VMS)
#  define PATH_SEPARATOR_CHAR (vms_comma_separator ? ',' : ':')
#  define MAP_PATHSEP    (vms_comma_separator ? MAP_COMMA : MAP_SEMI)
# else
#  define PATH_SEPARATOR_CHAR ':'
#  define MAP_PATHSEP    MAP_COLON
# endif
#elif PATH_SEPARATOR_CHAR == ':'
# define MAP_PATHSEP     MAP_COLON
#elif PATH_SEPARATOR_CHAR == ';'
# define MAP_PATHSEP     MAP_SEMI
#elif PATH_SEPARATOR_CHAR == ','
# define MAP_PATHSEP     MAP_COMMA
#else
# error "Unknown PATH_SEPARATOR_CHAR"
#endif

#define STOP_SET(_v,_m) ANY_SET(stopchar_map[(unsigned char)(_v)],(_m))

#define ISBLANK(c)      STOP_SET((c),MAP_BLANK)
#define ISSPACE(c)      STOP_SET((c),MAP_SPACE)
#define NEXT_TOKEN(s)   while (ISSPACE (*(s))) ++(s)
#define END_OF_TOKEN(s) while (! STOP_SET (*(s), MAP_SPACE|MAP_NUL)) ++(s)

/* We can't run setrlimit when using posix_spawn.  */
#if defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT) && defined(HAVE_SETRLIMIT) && !defined(USE_POSIX_SPAWN)
# define SET_STACK_SIZE
#endif
#ifdef SET_STACK_SIZE
# include <sys/resource.h>
extern struct rlimit stack_limit;
#endif

#include <glob.h>

#define NILF ((floc *)0)

#define CSTRLEN(_s)           (sizeof (_s)-1)
#define STRING_SIZE_TUPLE(_s) (_s), CSTRLEN(_s)

/* The number of bytes needed to represent the largest integer as a string.  */
#define INTSTR_LENGTH         CSTRLEN ("18446744073709551616")

#define DEFAULT_TTYNAME "true"
#ifdef HAVE_TTYNAME
# define TTYNAME(_f) ttyname (_f)
#else
# define TTYNAME(_f) DEFAULT_TTYNAME
#endif



/* Specify the location of elements read from makefiles.  */
typedef struct
  {
    const char *filenm;
    unsigned long lineno;
    unsigned long offset;
  } floc;

const char *concat (unsigned int, ...);
void message (int prefix, size_t length, const char *fmt, ...)
              ATTRIBUTE ((__format__ (__printf__, 3, 4)));
void error (const floc *flocp, size_t length, const char *fmt, ...)
            ATTRIBUTE ((__format__ (__printf__, 3, 4)));
void fatal (const floc *flocp, size_t length, const char *fmt, ...)
            ATTRIBUTE ((noreturn, __format__ (__printf__, 3, 4)));
void out_of_memory () NORETURN;

/* When adding macros to this list be sure to update the value of
   XGETTEXT_OPTIONS in the po/Makevars file.  */
#define O(_t,_a,_f)           _t((_a), 0, (_f))
#define OS(_t,_a,_f,_s)       _t((_a), strlen (_s), (_f), (_s))
#define OSS(_t,_a,_f,_s1,_s2) _t((_a), strlen (_s1) + strlen (_s2), \
                                 (_f), (_s1), (_s2))
#define OSSS(_t,_a,_f,_s1,_s2,_s3) _t((_a), strlen (_s1) + strlen (_s2) + strlen (_s3), \
                                      (_f), (_s1), (_s2), (_s3))
#define ON(_t,_a,_f,_n)       _t((_a), INTSTR_LENGTH, (_f), (_n))
#define ONN(_t,_a,_f,_n1,_n2) _t((_a), INTSTR_LENGTH*2, (_f), (_n1), (_n2))

#define OSN(_t,_a,_f,_s,_n)   _t((_a), strlen (_s) + INTSTR_LENGTH, \
                                 (_f), (_s), (_n))
#define ONS(_t,_a,_f,_n,_s)   _t((_a), INTSTR_LENGTH + strlen (_s), \
                                 (_f), (_n), (_s))

void die (int) NORETURN;
void pfatal_with_name (const char *) NORETURN;
void perror_with_name (const char *, const char *);
#define xstrlen(_s) ((_s)==NULL ? 0 : strlen (_s))
void *xmalloc (size_t);
void *xcalloc (size_t);
void *xrealloc (void *, size_t);
char *xstrdup (const char *);
char *xstrndup (const char *, size_t);
char *find_next_token (const char **, size_t *);
char *next_token (const char *);
char *end_of_token (const char *);
void collapse_continuations (char *);
char *lindex (const char *, const char *, int);
int alpha_compare (const void *, const void *);
void print_spaces (unsigned int);
char *find_percent (char *);
const char *find_percent_cached (const char **);
FILE *get_tmpfile (char **, const char *);
ssize_t writebuf (int, const void *, size_t);
ssize_t readbuf (int, void *, size_t);

#ifndef HAVE_MEMRCHR
void *memrchr(const void *, int, size_t);
#endif

#ifndef NO_ARCHIVES
int ar_name (const char *);
void ar_parse_name (const char *, char **, char **);
int ar_touch (const char *);
time_t ar_member_date (const char *);

typedef long int (*ar_member_func_t) (int desc, const char *mem, int truncated,
                                      long int hdrpos, long int datapos,
                                      long int size, long int date, int uid,
                                      int gid, unsigned int mode,
                                      const void *arg);

long int ar_scan (const char *archive, ar_member_func_t function, const void *arg);
int ar_name_equal (const char *name, const char *mem, int truncated);
#ifndef VMS
int ar_member_touch (const char *arname, const char *memname);
#endif
#endif

int dir_file_exists_p (const char *, const char *);
int file_exists_p (const char *);
int file_impossible_p (const char *);
void file_impossible (const char *);
const char *dir_name (const char *);
void print_dir_data_base (void);
void dir_setup_glob (glob_t *);
void hash_init_directories (void);

void define_default_variables (void);
void undefine_default_variables (void);
void set_default_suffixes (void);
void install_default_suffix_rules (void);
void install_default_implicit_rules (void);

void build_vpath_lists (void);
void construct_vpath_list (char *pattern, char *dirpath);
const char *vpath_search (const char *file, FILE_TIMESTAMP *mtime_ptr,
                          unsigned int* vpath_index, unsigned int* path_index);
int gpath_search (const char *file, size_t len);

void construct_include_path (const char **arg_dirs);

void user_access (void);
void make_access (void);
void child_access (void);

char *strip_whitespace (const char **begpp, const char **endpp);

void show_goal_error (void);

/* String caching  */
void strcache_init (void);
void strcache_print_stats (const char *prefix);
int strcache_iscached (const char *str);
const char *strcache_add (const char *str);
const char *strcache_add_len (const char *str, size_t len);

/* Guile support  */
int guile_gmake_setup (const floc *flocp);

/* Loadable object support.  Sets to the strcached name of the loaded file.  */
typedef int (*load_func_t)(const floc *flocp);
int load_file (const floc *flocp, const char **filename, int noerror);
void unload_file (const char *name);

/* Maintainer mode support */
#ifdef MAKE_MAINTAINER_MODE
# define SPIN(_s) spin (_s)
void spin (const char* suffix);
#else
# define SPIN(_s)
#endif

/* We omit these declarations on non-POSIX systems which define _POSIX_VERSION,
   because such systems often declare them in header files anyway.  */

#if !defined (__GNU_LIBRARY__) && !defined (POSIX) && !defined (_POSIX_VERSION) && !defined(WINDOWS32)

long int atol ();
# ifndef VMS
long int lseek ();
# endif

# ifdef  HAVE_GETCWD
#  if !defined(VMS) && !defined(__DECC)
char *getcwd ();
#  endif
# else
char *getwd ();
#  define getcwd(buf, len)       getwd (buf)
# endif

#endif  /* Not GNU C library or POSIX.  */

#if !HAVE_STRCASECMP
# if HAVE_STRICMP
#  define strcasecmp stricmp
# elif HAVE_STRCMPI
#  define strcasecmp strcmpi
# else
/* Create our own, in misc.c */
int strcasecmp (const char *s1, const char *s2);
# endif
#endif

#if !HAVE_STRNCASECMP
# if HAVE_STRNICMP
#  define strncasecmp strnicmp
# elif HAVE_STRNCMPI
#  define strncasecmp strncmpi
# else
/* Create our own, in misc.c */
int strncasecmp (const char *s1, const char *s2, int n);
# endif
#endif

#define OUTPUT_SYNC_NONE    0
#define OUTPUT_SYNC_LINE    1
#define OUTPUT_SYNC_TARGET  2
#define OUTPUT_SYNC_RECURSE 3

/* Non-GNU systems may not declare this in unistd.h.  */
extern char **environ;

extern const floc *reading_file;
extern const floc **expanding_var;

extern unsigned short stopchar_map[];

extern int just_print_flag, run_silent, ignore_errors_flag, keep_going_flag;
extern int print_data_base_flag, question_flag, touch_flag, always_make_flag;
extern int env_overrides, no_builtin_rules_flag, no_builtin_variables_flag;
extern int print_version_flag, print_directory_flag, check_symlink_flag;
extern int warn_undefined_variables_flag, trace_flag, posix_pedantic;
extern int not_parallel, second_expansion, clock_skew_detected;
extern int rebuilding_makefiles, one_shell, output_sync, verify_flag;

extern const char *default_shell;

/* can we run commands via 'sh -c xxx' or must we use batch files? */
extern int batch_mode_shell;

/* Resetting the command script introduction prefix character.  */
#define RECIPEPREFIX_NAME          ".RECIPEPREFIX"
#define RECIPEPREFIX_DEFAULT       '\t'
extern char cmd_prefix;

extern unsigned int job_slots;
extern double max_load_average;

extern const char *program;

#ifdef VMS
const char *vms_command (const char *argv0);
const char *vms_progname (const char *argv0);

void vms_exit (int);
# define _exit(foo) vms_exit(foo)
# define exit(foo) vms_exit(foo)

extern char *program_name;

void
set_program_name (const char *arv0);

int
need_vms_symbol (void);

int
create_foreign_command (const char *command, const char *image);

int
vms_export_dcl_symbol (const char *name, const char *value);

int
vms_putenv_symbol (const char *string);

void
vms_restore_symbol (const char *string);

#endif

void remote_setup (void);
void remote_cleanup (void);
int start_remote_job_p (int);
int start_remote_job (char **, char **, int, int *, pid_t *, int *);
int remote_status (int *, int *, int *, int);
void block_remote_children (void);
void unblock_remote_children (void);
int remote_kill (pid_t id, int sig);
void print_variable_data_base (void);
void print_vpath_data_base (void);

extern char *starting_directory;
extern unsigned int makelevel;
extern char *version_string, *remote_description, *make_host;

extern unsigned int commands_started;

extern int handling_fatal_signal;

#ifndef MIN
#define MIN(_a,_b) ((_a)<(_b)?(_a):(_b))
#endif
#ifndef MAX
#define MAX(_a,_b) ((_a)>(_b)?(_a):(_b))
#endif

#define MAKE_SUCCESS 0
#define MAKE_TROUBLE 1
#define MAKE_FAILURE 2

/* Set up heap debugging library dmalloc.  */

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

#ifndef initialize_main
# ifdef __EMX__
#  define initialize_main(pargc, pargv) \
                          { _wildcard(pargc, pargv); _response(pargc, pargv); }
# else
#  define initialize_main(pargc, pargv)
# endif
#endif

#ifdef __EMX__
# if !defined chdir
#  define chdir _chdir2
# endif
# if !defined getcwd
#  define getcwd _getcwd2
# endif

/* NO_CHDIR2 causes make not to use _chdir2() and _getcwd2() instead of
   chdir() and getcwd(). This avoids some error messages for the
   make testsuite but restricts the drive letter support. */
# ifdef NO_CHDIR2
#  warning NO_CHDIR2: usage of drive letters restricted
#  undef chdir
#  undef getcwd
# endif
#endif

#ifndef initialize_main
# define initialize_main(pargc, pargv)
#endif


/* Some systems (like Solaris, PTX, etc.) do not support the SA_RESTART flag
   properly according to POSIX.  So, we try to wrap common system calls with
   checks for EINTR.  Note that there are still plenty of system calls that
   can fail with EINTR but this, reportedly, gets the vast majority of
   failure cases.  If you still experience failures you'll need to either get
   a system where SA_RESTART works, or you need to avoid -j.  */

#define EINTRLOOP(_v,_c)   while (((_v)=_c)==-1 && errno==EINTR)

/* While system calls that return integers are pretty consistent about
   returning -1 on failure and setting errno in that case, functions that
   return pointers are not always so well behaved.  Sometimes they return
   NULL for expected behavior: one good example is readdir() which returns
   NULL at the end of the directory--and _doesn't_ reset errno.  So, we have
   to do it ourselves here.  */

#define ENULLLOOP(_v,_c)   do { errno = 0; (_v) = _c; } \
                           while((_v)==0 && errno==EINTR)
