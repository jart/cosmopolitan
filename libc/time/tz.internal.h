#ifndef COSMOPOLITAN_THIRD_PARTY_TZ_PRIVATE_H_
#define COSMOPOLITAN_THIRD_PARTY_TZ_PRIVATE_H_
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/inttypes.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/ok.h"
COSMOPOLITAN_C_START_

/* clang-format off */
/* Private header for tzdb code.  */

/*
** This file is in the public domain, so clarified as of
** 1996-06-05 by Arthur David Olson.
*/

/*
** This header is for use ONLY with the time conversion code.
** There is no guarantee that it will remain unchanged,
** or that it will remain at all.
** Do NOT copy it to any system include directory.
** Thank you!
*/

/*
** zdump has been made independent of the rest of the time
** conversion package to increase confidence in the verification it provides.
** You can use zdump to help in verifying other implementations.
** To do this, compile with -DUSE_LTZ=0 and link without the tz library.
*/
#ifndef USE_LTZ
# define USE_LTZ 1
#endif

/* This string was in the Factory zone through version 2016f.  */
#define GRANDPARENTED	"Local time zone must be set--see zic manual page"

/*
** Defaults for preprocessor symbols.
** You can override these in your C compiler options, e.g. '-DHAVE_GETTEXT=1'.
*/

#ifndef HAVE_DECL_ASCTIME_R
#define HAVE_DECL_ASCTIME_R 1
#endif

#if !defined HAVE_GENERIC && defined __has_extension
# if __has_extension(c_generic_selections)
#  define HAVE_GENERIC 1
# else
#  define HAVE_GENERIC 0
# endif
#endif
/* _Generic is buggy in pre-4.9 GCC.  */
#if !defined HAVE_GENERIC && defined __GNUC__
# define HAVE_GENERIC (4 < __GNUC__ + (9 <= __GNUC_MINOR__))
#endif
#ifndef HAVE_GENERIC
# define HAVE_GENERIC (201112 <= __STDC_VERSION__)
#endif

#ifndef HAVE_GETTEXT
#define HAVE_GETTEXT		0
#endif /* !defined HAVE_GETTEXT */

#ifndef HAVE_INCOMPATIBLE_CTIME_R
#define HAVE_INCOMPATIBLE_CTIME_R	0
#endif

#ifndef HAVE_LINK
#define HAVE_LINK		1
#endif /* !defined HAVE_LINK */

#ifndef HAVE_MALLOC_ERRNO
#define HAVE_MALLOC_ERRNO 1
#endif

#ifndef HAVE_POSIX_DECLS
#define HAVE_POSIX_DECLS 1
#endif

#ifndef HAVE_STRTOLL
#define HAVE_STRTOLL 1
#endif

#ifndef HAVE_SYMLINK
#define HAVE_SYMLINK		1
#endif /* !defined HAVE_SYMLINK */

#if HAVE_INCOMPATIBLE_CTIME_R
#define asctime_r _incompatible_asctime_r
#define ctime_r _incompatible_ctime_r
#endif /* HAVE_INCOMPATIBLE_CTIME_R */

/*
** Nested includes
*/

/* Avoid clashes with NetBSD by renaming NetBSD's declarations.
   If defining the 'timezone' variable, avoid a clash with FreeBSD's
   'timezone' function by renaming its declaration.  */
#define localtime_rz sys_localtime_rz
#define mktime_z sys_mktime_z
#define posix2time_z sys_posix2time_z
#define time2posix_z sys_time2posix_z
#if defined USG_COMPAT && USG_COMPAT == 2
# define timezone sys_timezone
#endif
#define timezone_t sys_timezone_t
#define tzalloc sys_tzalloc
#define tzfree sys_tzfree
#undef localtime_rz
#undef mktime_z
#undef posix2time_z
#undef time2posix_z
#if defined USG_COMPAT && USG_COMPAT == 2
# undef timezone
#endif
#undef timezone_t
#undef tzalloc
#undef tzfree

#if HAVE_GETTEXT
#include <libintl.h>
#endif /* HAVE_GETTEXT */

#ifndef HAVE_STRFTIME_L
# if _POSIX_VERSION < 200809
#  define HAVE_STRFTIME_L 0
# else
#  define HAVE_STRFTIME_L 1
# endif
#endif

#ifndef USG_COMPAT
# ifndef _XOPEN_VERSION
#  define USG_COMPAT 0
# else
#  define USG_COMPAT 1
# endif
#endif

#ifndef HAVE_TZNAME
# if _POSIX_VERSION < 198808 && !USG_COMPAT
#  define HAVE_TZNAME 0
# else
#  define HAVE_TZNAME 1
# endif
#endif

#ifndef ALTZONE
# if defined __sun || defined _M_XENIX
#  define ALTZONE 1
# else
#  define ALTZONE 0
# endif
#endif

#ifndef R_OK
#define R_OK	4
#endif /* !defined R_OK */

#if 3 <= __GNUC__
# define ATTRIBUTE_FORMAT(spec) __attribute__((__format__ spec))
#else
# define ATTRIBUTE_FORMAT(spec) /* empty */
#endif

/*
** Workarounds for compilers/systems.
*/

#ifndef EPOCH_LOCAL
# define EPOCH_LOCAL 0
#endif
#ifndef EPOCH_OFFSET
# define EPOCH_OFFSET 0
#endif

/*
** Compile with -Dtime_tz=T to build the tz package with a private
** int64_t type equivalent to T rather than the system-supplied int64_t.
** This debugging feature can test unusual design decisions
** (e.g., int64_t wider than 'long', or unsigned int64_t) even on
** typical platforms.
*/
#if defined time_tz || EPOCH_LOCAL || EPOCH_OFFSET != 0
# define TZ_INT64_T 1
#else
# define TZ_INT64_T 0
#endif

#if defined LOCALTIME_IMPLEMENTATION && TZ_INT64_T
static int64_t sys_time(int64_t *x) { return time(x); }
#endif

#if TZ_INT64_T

typedef time_tz tz_int64_t;

# undef  asctime
# define asctime tz_asctime
# undef  asctime_r
# define asctime_r tz_asctime_r
# undef  ctime
# define ctime tz_ctime
# undef  ctime_r
# define ctime_r tz_ctime_r
# undef  difftime
# define difftime tz_difftime
# undef  gmtime
# define gmtime tz_gmtime
# undef  gmtime_r
# define gmtime_r tz_gmtime_r
# undef  localtime
# define localtime tz_localtime
# undef  localtime_r
# define localtime_r tz_localtime_r
# undef  localtime_rz
# define localtime_rz tz_localtime_rz
# undef  mktime
# define mktime tz_mktime
# undef  mktime_z
# define mktime_z tz_mktime_z
# undef  offtime
# define offtime tz_offtime
# undef  posix2time
# define posix2time tz_posix2time
# undef  posix2time_z
# define posix2time_z tz_posix2time_z
# undef  strftime
# define strftime tz_strftime
# undef  time
# define time tz_time
# undef  time2posix
# define time2posix tz_time2posix
# undef  time2posix_z
# define time2posix_z tz_time2posix_z
# undef  int64_t
# define int64_t tz_int64_t
# undef  timegm
# define timegm tz_timegm
# undef  timelocal
# define timelocal tz_timelocal
# undef  timeoff
# define timeoff tz_timeoff
# undef  tzalloc
# define tzalloc tz_tzalloc
# undef  tzfree
# define tzfree tz_tzfree
# undef  tzset
# define tzset tz_tzset
# if HAVE_STRFTIME_L
#  undef  strftime_l
#  define strftime_l tz_strftime_l
# endif
# if HAVE_TZNAME
#  undef  tzname
#  define tzname tz_tzname
# endif
# if USG_COMPAT
#  undef  daylight
#  define daylight tz_daylight
#  undef  timezone
#  define timezone tz_timezone
# endif
# if ALTZONE
#  undef  altzone
#  define altzone tz_altzone
# endif

char *asctime(struct tm const *) libcesque;
char *asctime_r(struct tm const *restrict, char *restrict) libcesque;
char *ctime(int64_t const *) libcesque;
char *ctime_r(int64_t const *, char *) libcesque;
double difftime(int64_t, int64_t) libcesque pureconst;
size_t strftime(char *restrict, size_t, char const *restrict,
		struct tm const *restrict) libcesque;
size_t strftime_l(char *restrict, size_t, char const *restrict,
		  struct tm const *restrict, locale_t) libcesque;
struct tm *gmtime(int64_t const *) libcesque;
struct tm *gmtime_r(int64_t const *restrict, struct tm *restrict) libcesque;
struct tm *localtime(int64_t const *) libcesque;
struct tm *localtime_r(int64_t const *restrict, struct tm *restrict) libcesque;
int64_t mktime(struct tm *) libcesque;
int64_t time(int64_t *) libcesque;
void tzset(void) libcesque;
#endif

#if !HAVE_DECL_ASCTIME_R && !defined asctime_r
extern char *asctime_r(struct tm const *restrict, char *restrict) libcesque;
#endif

#ifndef HAVE_DECL_ENVIRON
# if defined environ || defined __USE_GNU
#  define HAVE_DECL_ENVIRON 1
# else
#  define HAVE_DECL_ENVIRON 0
# endif
#endif

#if 2 <= HAVE_TZNAME + (TZ_INT64_T || !HAVE_POSIX_DECLS)
extern char *tzname[];
#endif
#if 2 <= USG_COMPAT + (TZ_INT64_T || !HAVE_POSIX_DECLS)
extern long timezone;
extern int daylight;
#endif
#if 2 <= ALTZONE + (TZ_INT64_T || !HAVE_POSIX_DECLS)
extern long altzone;
#endif

/*
** The STD_INSPIRED functions are similar, but most also need
** declarations if time_tz is defined.
*/

#ifdef STD_INSPIRED
# if TZ_INT64_T || !defined offtime
struct tm *offtime(int64_t const *, long);
# endif
# if TZ_INT64_T || !defined timegm
int64_t timegm(struct tm *);
# endif
# if TZ_INT64_T || !defined timelocal
int64_t timelocal(struct tm *);
# endif
# if TZ_INT64_T || !defined timeoff
int64_t timeoff(struct tm *, long);
# endif
# if TZ_INT64_T || !defined time2posix
int64_t time2posix(int64_t);
# endif
# if TZ_INT64_T || !defined posix2time
int64_t posix2time(int64_t);
# endif
#endif

/* Infer TM_ZONE on systems where this information is known, but suppress
   guessing if NO_TM_ZONE is defined.  Similarly for TM_GMTOFF.  */
#define TM_GMTOFF tm_gmtoff
#define TM_ZONE tm_zone

/*
** Define functions that are ABI compatible with NetBSD but have
** better prototypes.  NetBSD 6.1.4 defines a pointer type timezone_t
** and labors under the misconception that 'const timezone_t' is a
** pointer to a constant.  This use of 'const' is ineffective, so it
** is not done here.  What we call 'struct state' NetBSD calls
** 'struct __state', but this is a private name so it doesn't matter.
*/
#if NETBSD_INSPIRED
typedef struct state *timezone_t;
struct tm *localtime_rz(timezone_t restrict, int64_t const *restrict,
			struct tm *restrict);
int64_t mktime_z(timezone_t restrict, struct tm *restrict);
timezone_t tzalloc(char const *);
void tzfree(timezone_t);
# ifdef STD_INSPIRED
#  if TZ_INT64_T || !defined posix2time_z
int64_t posix2time_z(timezone_t, int64_t) nosideeffect;
#  endif
#  if TZ_INT64_T || !defined time2posix_z
int64_t time2posix_z(timezone_t, int64_t) nosideeffect;
#  endif
# endif
#endif

/*
** Finally, some convenience items.
*/

#define TWOS_COMPLEMENT(t) ((t) ~ (t) 0 < 0)

/* Max and min values of the integer type T, of which only the bottom
   B bits are used, and where the highest-order used bit is considered
   to be a sign bit if T is signed.  */
#define MAXVAL(t, b)						\
  ((t) (((t) 1 << ((b) - 1 - TYPE_SIGNED(t)))			\
	- 1 + ((t) 1 << ((b) - 1 - TYPE_SIGNED(t)))))
#define MINVAL(t, b)						\
  ((t) (TYPE_SIGNED(t) ? - TWOS_COMPLEMENT(t) - MAXVAL(t, b) : 0))

/* The extreme time values, assuming no padding.  */
#define INT64_T_MIN_NO_PADDING MINVAL(int64_t, TYPE_BIT(int64_t))
#define INT64_T_MAX_NO_PADDING MAXVAL(int64_t, TYPE_BIT(int64_t))

/* The extreme time values.  These are macros, not constants, so that
   any portability problems occur only when compiling .c files that use
   the macros, which is safer for applications that need only zdump and zic.
   This implementation assumes no padding if int64_t is signed and
   either the compiler lacks support for _Generic or int64_t is not one
   of the standard signed integer types.  */
#if HAVE_GENERIC
# define INT64_T_MIN \
    _Generic((int64_t) 0, \
	     signed char: SCHAR_MIN, short: SHRT_MIN, \
	     int: INT_MIN, long: LONG_MIN, long long: LLONG_MIN, \
	     default: INT64_T_MIN_NO_PADDING)
# define INT64_T_MAX \
    (TYPE_SIGNED(int64_t) \
     ? _Generic((int64_t) 0, \
		signed char: SCHAR_MAX, short: SHRT_MAX, \
		int: INT_MAX, long: LONG_MAX, long long: LLONG_MAX, \
		default: INT64_T_MAX_NO_PADDING)			    \
     : (int64_t) -1)
#else
# define INT64_T_MIN INT64_T_MIN_NO_PADDING
# define INT64_T_MAX INT64_T_MAX_NO_PADDING
#endif

/*
** 302 / 1000 is log10(2.0) rounded up.
** Subtract one for the sign bit if the type is signed;
** add one for integer division truncation;
** add one more for a minus sign if the type is signed.
*/
#define INT_STRLEN_MAXIMUM(type) \
	((TYPE_BIT(type) - TYPE_SIGNED(type)) * 302 / 1000 + \
	1 + TYPE_SIGNED(type))

/*
** INITIALIZE(x)
*/

#define INITIALIZE(x)	((x) = 0)

/* Whether memory access must strictly follow the C standard.
   If 0, it's OK to read uninitialized storage so long as the value is
   not relied upon.  Defining it to 0 lets mktime access parts of
   struct tm that might be uninitialized, as a heuristic when the
   standard doesn't say what to return and when tm_gmtoff can help
   mktime likely infer a better value.  */
#ifndef UNINIT_TRAP
# define UNINIT_TRAP 0
#endif

#ifdef DEBUG
# define UNREACHABLE() abort()
#elif 4 < __GNUC__ + (5 <= __GNUC_MINOR__)
# define UNREACHABLE() __builtin_unreachable()
#elif defined __has_builtin
# if __has_builtin(__builtin_unreachable)
#  define UNREACHABLE() __builtin_unreachable()
# endif
#endif
#ifndef UNREACHABLE
# define UNREACHABLE() ((void) 0)
#endif

/*
** For the benefit of GNU folk...
** '_(MSGID)' uses the current locale's message library string for MSGID.
** The default is to use gettext if available, and use MSGID otherwise.
*/

#if HAVE_GETTEXT
#define _(msgid) gettext(msgid)
#else /* !HAVE_GETTEXT */
#define _(msgid) msgid
#endif /* !HAVE_GETTEXT */

#if !defined TZ_DOMAIN && defined HAVE_GETTEXT
# define TZ_DOMAIN "tz"
#endif

#if HAVE_INCOMPATIBLE_CTIME_R
#undef asctime_r
#undef ctime_r
char *asctime_r(struct tm const *, char *);
char *ctime_r(int64_t const *, char *);
#endif /* HAVE_INCOMPATIBLE_CTIME_R */

/* Handy macros that are independent of tzfile implementation.  */

#define SECSPERMIN	60
#define MINSPERHOUR	60
#define HOURSPERDAY	24
#define DAYSPERWEEK	7
#define DAYSPERNYEAR	365
#define DAYSPERLYEAR	366
#define SECSPERHOUR	(SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY	((int32_t) SECSPERHOUR * HOURSPERDAY)
#define MONSPERYEAR	12

#define YEARSPERREPEAT		400	/* years before a Gregorian repeat */
#define DAYSPERREPEAT		((int32_t) 400 * 365 + 100 - 4 + 1)
#define SECSPERREPEAT		((int64_t) DAYSPERREPEAT * SECSPERDAY)
#define AVGSECSPERYEAR		(SECSPERREPEAT / YEARSPERREPEAT)

#define TM_SUNDAY	0
#define TM_MONDAY	1
#define TM_TUESDAY	2
#define TM_WEDNESDAY	3
#define TM_THURSDAY	4
#define TM_FRIDAY	5
#define TM_SATURDAY	6

#define TM_JANUARY	0
#define TM_FEBRUARY	1
#define TM_MARCH	2
#define TM_APRIL	3
#define TM_MAY		4
#define TM_JUNE		5
#define TM_JULY		6
#define TM_AUGUST	7
#define TM_SEPTEMBER	8
#define TM_OCTOBER	9
#define TM_NOVEMBER	10
#define TM_DECEMBER	11

#define TM_YEAR_BASE	1900
#define TM_WDAY_BASE	TM_MONDAY

#define EPOCH_YEAR	1970
#define EPOCH_WDAY	TM_THURSDAY

#define isleap(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))

/*
** Since everything in isleap is modulo 400 (or a factor of 400), we know that
**	isleap(y) == isleap(y % 400)
** and so
**	isleap(a + b) == isleap((a + b) % 400)
** or
**	isleap(a + b) == isleap(a % 400 + b % 400)
** This is true even if % means modulo rather than Fortran remainder
** (which is allowed by C89 but not by C99 or later).
** We use this to avoid addition overflow problems.
*/

#define isleap_sum(a, b)	isleap((a) % 400 + (b) % 400)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_TZ_PRIVATE_H_ */
