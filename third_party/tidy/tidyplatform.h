#ifndef __TIDY_PLATFORM_H__
#define __TIDY_PLATFORM_H__
COSMOPOLITAN_C_START_

#define LINUX_OS
#define PLATFORM_NAME              "Cosmopolitan"
#define TIDY_CONFIG_FILE           "/zip/.tidyrc"
#define TIDY_USER_CONFIG_FILE      "~/.tidyrc"
#define SUPPORT_LOCALIZATIONS      0
#define SUPPORT_CONSOLE_APP        1
#define FILENAMES_CASE_SENSITIVE   1
#define PRESERVE_FILE_TIMES        1
#define HAS_FUTIME                 0
#define UTIME_NEEDS_CLOSED_FILE    1
#define HAS_VSNPRINTF              1
#define SUPPORT_POSIX_MAPPED_FILES 1
#define TIDY_EXPORT
#define TIDY_STRUCT
#define TIDY_THREAD_LOCAL
#define TIDY_INDENTATION_LIMIT 50
#define TIDY_CALL
/* #define SUPPORT_GETPWNAM */

#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#define ARG_UNUSED(x) x __attribute__((__unused__))
#define FUNC_UNUSED   __attribute__((__unused__))
#else
#define ARG_UNUSED(x) x
#define FUNC_UNUSED
#endif

typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned char byte;
typedef uint tchar;   /* single, full character */
typedef char tmbchar; /* single, possibly partial character */
typedef enum { no, yes } Bool;

typedef tmbchar* tmbstr; /* pointer to buffer of possibly partial chars */
typedef const tmbchar* ctmbstr; /* Ditto, but const */
#define NULLSTR (tmbstr) ""
#define TMBSTR_DEFINED

/* Opaque data structure.
 *  Cast to implementation type struct within lib.
 *  This will reduce inter-dependencies/conflicts w/ application code.
 */
#if 1
#define opaque_type(typenam) \
  struct _##typenam {        \
    int _opaque;             \
  };                         \
  typedef struct _##typenam const* typenam
#else
#define opaque_type(typenam) typedef const void* typenam
#endif

/* Opaque data structure used to pass back
** and forth to keep current position in a
** list or other collection.
*/
opaque_type(TidyIterator);

COSMOPOLITAN_C_END_
#endif /* __TIDY_PLATFORM_H__ */
