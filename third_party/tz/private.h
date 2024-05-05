#ifndef TZ_PRIVATE_H
#define TZ_PRIVATE_H
#include "libc/errno.h"
#include "libc/calls/weirdtypes.h"
#include "libc/time.h"
#include "libc/limits.h"

#define ALL_STATE
#define HAVE_TZNAME 1
#define HAVE_MALLOC_ERRNO 1
#define TM_ZONE tm_zone
#define TM_GMTOFF tm_gmtoff

/* The maximum size of any created object, as a signed integer.
   Although the C standard does not outright prohibit larger objects,
   behavior is undefined if the result of pointer subtraction does not
   fit into ptrdiff_t, and the code assumes in several places that
   pointer subtraction works.  As a practical matter it's OK to not
   support objects larger than this.  */
#define INDEX_MAX ((ptrdiff_t)min(PTRDIFF_MAX, SIZE_MAX))

#define ATTRIBUTE_MALLOC       __attribute__((__malloc__))
#define ATTRIBUTE_FORMAT(spec) __attribute__((__format__ spec))
#define ATTRIBUTE_FALLTHROUGH  __attribute__((__fallthrough__))
#define ATTRIBUTE_MAYBE_UNUSED __attribute__((__unused__))
#define ATTRIBUTE_NORETURN     __attribute__((__noreturn__))
#define ATTRIBUTE_REPRODUCIBLE __attribute__((__pure__))
#define ATTRIBUTE_UNSEQUENCED  __attribute__((__const__))

#define unreachable() __builtin_unreachable()

/*
** Finally, some convenience items.
*/

#define TYPE_BIT(type)     (CHAR_BIT * (ptrdiff_t)sizeof(type))
#define TYPE_SIGNED(type)  (((type) - 1) < 0)
#define TWOS_COMPLEMENT(t) ((t) ~(t)0 < 0)

/* Minimum and maximum of two values.  Use lower case to avoid
   naming clashes with standard include files.  */
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

/* Max and min values of the integer type T, of which only the bottom
   B bits are used, and where the highest-order used bit is considered
   to be a sign bit if T is signed.  */
#define MAXVAL(t, b)                              \
  ((t)(((t)1 << ((b) - 1 - TYPE_SIGNED(t))) - 1 + \
       ((t)1 << ((b) - 1 - TYPE_SIGNED(t)))))
#define MINVAL(t, b) \
  ((t)(TYPE_SIGNED(t) ? -TWOS_COMPLEMENT(t) - MAXVAL(t, b) : 0))

/* The extreme time values, assuming no padding.  */
#define TIME_T_MIN_NO_PADDING MINVAL(time_t, TYPE_BIT(time_t))
#define TIME_T_MAX_NO_PADDING MAXVAL(time_t, TYPE_BIT(time_t))

/*
** 302 / 1000 is log10(2.0) rounded up.
** Subtract one for the sign bit if the type is signed;
** add one for integer division truncation;
** add one more for a minus sign if the type is signed.
*/
#define INT_STRLEN_MAXIMUM(type) \
  ((TYPE_BIT(type) - TYPE_SIGNED(type)) * 302 / 1000 + 1 + TYPE_SIGNED(type))

/*
** INITIALIZE(x)
*/

/* Whether memory access must strictly follow the C standard.
   If 0, it's OK to read uninitialized storage so long as the value is
   not relied upon.  Defining it to 0 lets mktime access parts of
   struct tm that might be uninitialized, as a heuristic when the
   standard doesn't say what to return and when tm_gmtoff can help
   mktime likely infer a better value.  */
#ifndef UNINIT_TRAP
#define UNINIT_TRAP 0
#endif

#if !defined TZ_DOMAIN && defined HAVE_GETTEXT
#define TZ_DOMAIN "tz"
#endif

/* Handy macros that are independent of tzfile implementation.  */

enum {
  SECSPERMIN = 60,
  MINSPERHOUR = 60,
  SECSPERHOUR = SECSPERMIN * MINSPERHOUR,
  HOURSPERDAY = 24,
  DAYSPERWEEK = 7,
  DAYSPERNYEAR = 365,
  DAYSPERLYEAR = DAYSPERNYEAR + 1,
  MONSPERYEAR = 12,
  YEARSPERREPEAT = 400 /* years before a Gregorian repeat */
};

#define SECSPERDAY ((int_fast32_t)SECSPERHOUR * HOURSPERDAY)

#define DAYSPERREPEAT  ((int_fast32_t)400 * 365 + 100 - 4 + 1)
#define SECSPERREPEAT  ((int_fast64_t)DAYSPERREPEAT * SECSPERDAY)
#define AVGSECSPERYEAR (SECSPERREPEAT / YEARSPERREPEAT)

/* How many years to generate (in zic.c) or search through (in localtime.c).
   This is two years larger than the obvious 400, to avoid edge cases.
   E.g., suppose a non-POSIX.1-2017 rule applies from 2012 on with transitions
   in March and September, plus one-off transitions in November 2013.
   If zic looked only at the last 400 years, it would set max_year=2413,
   with the intent that the 400 years 2014 through 2413 will be repeated.
   The last transition listed in the tzfile would be in 2413-09,
   less than 400 years after the last one-off transition in 2013-11.
   Two years is not overkill for localtime.c, as a one-year bump
   would mishandle 2023d's America/Ciudad_Juarez for November 2422.  */
enum { years_of_observations = YEARSPERREPEAT + 2 };

enum {
  TM_SUNDAY,
  TM_MONDAY,
  TM_TUESDAY,
  TM_WEDNESDAY,
  TM_THURSDAY,
  TM_FRIDAY,
  TM_SATURDAY
};

enum {
  TM_JANUARY,
  TM_FEBRUARY,
  TM_MARCH,
  TM_APRIL,
  TM_MAY,
  TM_JUNE,
  TM_JULY,
  TM_AUGUST,
  TM_SEPTEMBER,
  TM_OCTOBER,
  TM_NOVEMBER,
  TM_DECEMBER
};

enum {
  TM_YEAR_BASE = 1900,
  TM_WDAY_BASE = TM_MONDAY,
  EPOCH_YEAR = 1970,
  EPOCH_WDAY = TM_THURSDAY
};

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

#define isleap_sum(a, b) isleap((a) % 400 + (b) % 400)

#endif /* !defined TZ_PRIVATE_H */
