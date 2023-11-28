/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define this label if your system uses case-insensitive file names */
/* #undef CASE_INSENSITIVE_FILENAMES */

/* Define this label if you wish to check the regcomp() function at run time
   for correct behavior. This function is currently broken on Cygwin. */
/* #undef CHECK_REGCOMP */

/* You can define this label to be a string containing the name of a
   site-specific configuration file containing site-wide default options. The
   files /etc/ctags.conf and /usr/local/etc/ctags.conf are already checked, so
   only define one here if you need a file somewhere else. */
/* #undef CUSTOM_CONFIGURATION_FILE */


/* Define this as desired.
 * 1:  Original ctags format
 * 2:  Extended ctags format with extension flags in EX-style comment.
 */
#define DEFAULT_FILE_FORMAT 2



/* Define this label to use the system sort utility (which is probably more
*  efficient) over the internal sorting algorithm.
*/
#ifndef INTERNAL_SORT
# define EXTERNAL_SORT 1
#endif


/* Define to 1 if you have the `chsize' function. */
/* #undef HAVE_CHSIZE */

/* Define to 1 if you have the `clock' function. */
#define HAVE_CLOCK 1

/* Define to 1 if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `fgetpos' function. */
#define HAVE_FGETPOS 1

/* Define to 1 if you have the `findfirst' function. */
/* #undef HAVE_FINDFIRST */

/* Define to 1 if you have the `fnmatch' function. */
#define HAVE_FNMATCH 1

/* Define to 1 if you have the <fnmatch.h> header file. */
#define HAVE_FNMATCH_H 1

/* Define to 1 if you have the `ftruncate' function. */
/* #undef HAVE_FTRUNCATE */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define to 1 if you have the `opendir' function. */
#define HAVE_OPENDIR 1

/* Define to 1 if you have the `putenv' function. */
/* #undef HAVE_PUTENV */

/* Define to 1 if you have the `regcomp' function. */
#define HAVE_REGCOMP 1
#define HAVE_REGEX 1

/* Define to 1 if you have the `remove' function. */
#define HAVE_REMOVE 1

/* Define to 1 if you have the `setenv' function. */
#define HAVE_SETENV 1

/* Define to 1 if you have the <stat.h> header file. */
/* #undef HAVE_STAT_H */

/* Define this macro if the field "st_ino" exists in struct stat in
   <sys/stat.h>. */
#define HAVE_STAT_ST_INO 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the `stricmp' function. */
/* #undef HAVE_STRICMP */

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strnicmp' function. */
/* #undef HAVE_STRNICMP */

/* Define to 1 if you have the `strstr' function. */
#define HAVE_STRSTR 1

/* Define to 1 if you have the <sys/dir.h> header file. */
#define HAVE_SYS_DIR_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/times.h> header file. */
#define HAVE_SYS_TIMES_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the `tempnam' function. */
/* #undef HAVE_TEMPNAM */

/* Define to 1 if you have the `times' function. */
/* #undef HAVE_TIMES */

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if you have the `truncate' function. */
#define HAVE_TRUNCATE 1

/* Define to 1 if you have the <types.h> header file. */
/* #undef HAVE_TYPES_H */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `_findfirst' function. */
/* #undef HAVE__FINDFIRST */

/* Define as the maximum integer on your system if not defined <limits.h>. */
/* #undef INT_MAX */

/* Define to the appropriate size for tmpnam() if <stdio.h> does not define
   this. */
#define L_tmpnam 20

/* Define this label if you want macro tags (defined lables) to use patterns
   in the EX command by default (original ctags behavior is to use line
   numbers). */
/* #undef MACROS_USE_PATTERNS */

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
/* #undef NEED_PROTO_FGETPOS */

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
/* #undef NEED_PROTO_FTRUNCATE */

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
/* #undef NEED_PROTO_GETENV */

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
/* #undef NEED_PROTO_LSTAT */

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
/* #undef NEED_PROTO_MALLOC */

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
/* #undef NEED_PROTO_REMOVE */

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
/* #undef NEED_PROTO_STAT */

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
/* #undef NEED_PROTO_TRUNCATE */

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
/* #undef NEED_PROTO_UNLINK */

/* Define this is you have a prototype for putenv() in <stdlib.h>, but doesn't
   declare its argument as "const char *". */
/* #undef NON_CONST_PUTENV_PROTOTYPE */

/* Package name. */
/* #undef PACKAGE */

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define this label if regcomp() is broken. */
/* #undef REGCOMP_BROKEN */

/* Define this value used by fseek() appropriately if <stdio.h> (or <unistd.h>
   on SunOS 4.1.x) does not define them. */
/* #undef SEEK_SET */

/* Define to 1 if all of the C90 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#define STDC_HEADERS 1

/* Define this label if your system supports starting scripts with a line of
   the form "#! /bin/sh" to select the interpreter to use for the script. */
#define SYS_INTERPRETER 1

/* If you wish to change the directory in which temporary files are stored,
   define this label to the directory desired. */
#define TMPDIR "/tmp"

/* Package version. */
/* #undef VERSION */

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* This corrects the problem of missing prototypes for certain functions in
   some GNU installations (e.g. SunOS 4.1.x). */
/* #undef __USE_FIXED_PROTOTYPES__ */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to long if <stdio.h> does not define this. */
/* #undef fpos_t */

/* Define to `long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define remove to unlink if you have unlink(), but not remove(). */
/* #undef remove */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */
