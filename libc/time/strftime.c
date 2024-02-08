/*-*- mode:c; indent-tabs-mode:t; tab-width:8; coding:utf-8                 -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright (c) 1989 The Regents of the University of California.              │
│ All rights reserved.                                                         │
│                                                                              │
│ Redistribution and use in source and binary forms are permitted              │
│ provided that the above copyright notice and this paragraph are              │
│ duplicated in all such forms and that any documentation,                     │
│ advertising materials, and other materials related to such                   │
│ distribution and use acknowledge that the software was developed             │
│ by the University of California, Berkeley.  The name of the                  │
│ University may not be used to endorse or promote products derived            │
│ from this software without specific prior written permission.                │
│ THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR               │
│ IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED               │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.          │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/inttypes.h"
#include "libc/stdio/stdio.h"
#include "libc/str/locale.h"
#include "libc/time/time.h"
#include "libc/time/tz.internal.h"
// clang-format off
// converts broken-down timestamp to string

#define DIVISOR	100

asm(".ident\t\"\\n\\n\
strftime (BSD-3)\\n\
Copyright 1989 The Regents of the University of California\"");
asm(".include \"libc/disclaimer.inc\"");

/*
** Based on the UCB version with the copyright notice appearing above.
**
** This is ANSIish only when "multibyte character == plain character".
*/

struct lc_time_T {
	const char *	mon[MONSPERYEAR];
	const char *	month[MONSPERYEAR];
	const char *	wday[DAYSPERWEEK];
	const char *	weekday[DAYSPERWEEK];
	const char *	X_fmt;
	const char *	x_fmt;
	const char *	c_fmt;
	const char *	am;
	const char *	pm;
	const char *	date_fmt;
};

#define Locale	(&C_time_locale)

static const struct lc_time_T	C_time_locale = {
	{
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	}, {
		"January", "February", "March", "April", "May", "June",
		"July", "August", "September", "October", "November", "December"
	}, {
		"Sun", "Mon", "Tue", "Wed",
		"Thu", "Fri", "Sat"
	}, {
		"Sunday", "Monday", "Tuesday", "Wednesday",
		"Thursday", "Friday", "Saturday"
	},

	/* X_fmt */
	"%H:%M:%S",

	/*
	** x_fmt
	** C99 and later require this format.
	** Using just numbers (as here) makes Quakers happier;
	** it's also compatible with SVR4.
	*/
	"%m/%d/%y",

	/*
	** c_fmt
	** C99 and later require this format.
	** Previously this code used "%D %X", but we now conform to C99.
	** Note that
	**	"%a %b %d %H:%M:%S %Y"
	** is used by Solaris 2.3.
	*/
	"%a %b %e %T %Y",

	/* am */
	"AM",

	/* pm */
	"PM",

	/* date_fmt */
	"%a %b %e %H:%M:%S %Z %Y"
};

enum warn { IN_NONE, IN_SOME, IN_THIS, IN_ALL };

#ifndef YEAR_2000_NAME
#define YEAR_2000_NAME	"CHECK_STRFTIME_FORMATS_FOR_TWO_DIGIT_YEARS"
#endif /* !defined YEAR_2000_NAME */

static char *
strftime_add(const char *str, char *pt, const char *ptlim)
{
	while (pt < ptlim && (*pt = *str++) != '\0')
		++pt;
	return pt;
}

static char *
strftime_conv(int n, const char *format, char *pt, const char *ptlim)
{
	char	buf[INT_STRLEN_MAXIMUM(int) + 1];
	(sprintf)(buf, format, n);
	return strftime_add(buf, pt, ptlim);
}

/*
** POSIX and the C Standard are unclear or inconsistent about
** what %C and %y do if the year is negative or exceeds 9999.
** Use the convention that %C concatenated with %y yields the
** same output as %Y, and that %Y contains at least 4 bytes,
** with more only if necessary.
*/

static char *
strftime_yconv(
	int		a,
	int		b,
	bool		convert_top,
	bool		convert_yy,
	char		*pt,
	const char	*ptlim)
{
	register int	lead;
	register int	trail;

	trail = a % DIVISOR + b % DIVISOR;
	lead = a / DIVISOR + b / DIVISOR + trail / DIVISOR;
	trail %= DIVISOR;
	if (trail < 0 && lead > 0) {
		trail += DIVISOR;
		--lead;
	} else if (lead < 0 && trail > 0) {
		trail -= DIVISOR;
		++lead;
	}
	if (convert_top) {
		if (lead == 0 && trail < 0)
			pt = strftime_add("-0", pt, ptlim);
		else	pt = strftime_conv(lead, "%02d", pt, ptlim);
	}
	if (convert_yy)
		pt = strftime_conv(((trail < 0) ? -trail : trail), "%02d", pt, ptlim);
	return pt;
}

static char *
strftime_fmt(const char *format, const struct tm *t, char *pt,
	     const char *ptlim, enum warn *warnp)
{
	for ( ; *format; ++format) {
		if (*format == '%') {
label:
			switch (*++format) {
			case '\0':
				--format;
				break;
			case 'A':
				pt = strftime_add(
					(t->tm_wday < 0 ||
					t->tm_wday >= DAYSPERWEEK) ?
					"?" : Locale->weekday[t->tm_wday],
					pt, ptlim);
				continue;
			case 'a':
				pt = strftime_add(
					(t->tm_wday < 0 ||
					 t->tm_wday >= DAYSPERWEEK) ?
					"?" : Locale->wday[t->tm_wday],
					pt, ptlim);
				continue;
			case 'B':
				pt = strftime_add(
					(t->tm_mon < 0 ||
					 t->tm_mon >= MONSPERYEAR) ?
					"?" : Locale->month[t->tm_mon],
					pt, ptlim);
				continue;
			case 'b':
			case 'h':
				pt = strftime_add(
					(t->tm_mon < 0 ||
					 t->tm_mon >= MONSPERYEAR) ?
					"?" : Locale->mon[t->tm_mon],
					pt, ptlim);
				continue;
			case 'C':
				/*
				** %C used to do a...
				**	_fmt("%a %b %e %X %Y", t);
				** ...whereas now POSIX 1003.2 calls for
				** something completely different.
				** (ado, 1993-05-24)
				*/
				pt = strftime_yconv(t->tm_year, TM_YEAR_BASE,
						    true, false, pt, ptlim);
				continue;
			case 'c':
				{
				enum warn warn2 = IN_SOME;

				pt = strftime_fmt(Locale->c_fmt, t, pt, ptlim, &warn2);
				if (warn2 == IN_ALL)
					warn2 = IN_THIS;
				if (warn2 > *warnp)
					*warnp = warn2;
				}
				continue;
			case 'D':
				pt = strftime_fmt("%m/%d/%y", t, pt, ptlim, warnp);
				continue;
			case 'd':
				pt = strftime_conv(t->tm_mday, "%02d", pt, ptlim);
				continue;
			case 'E':
			case 'O':
				/*
				** Locale modifiers of C99 and later.
				** The sequences
				**	%Ec %EC %Ex %EX %Ey %EY
				**	%Od %oe %OH %OI %Om %OM
				**	%OS %Ou %OU %OV %Ow %OW %Oy
				** are supposed to provide alternative
				** representations.
				*/
				goto label;
			case 'e':
				pt = strftime_conv(t->tm_mday, "%2d", pt, ptlim);
				continue;
			case 'F':
				pt = strftime_fmt("%Y-%m-%d", t, pt, ptlim, warnp);
				continue;
			case 'H':
				pt = strftime_conv(t->tm_hour, "%02d", pt, ptlim);
				continue;
			case 'I':
				pt = strftime_conv((t->tm_hour % 12) ?
						   (t->tm_hour % 12) : 12,
						   "%02d", pt, ptlim);
				continue;
			case 'j':
				pt = strftime_conv(t->tm_yday + 1, "%03d", pt, ptlim);
				continue;
			case 'k':
				/*
				** This used to be...
				**	_conv(t->tm_hour % 12 ?
				**		t->tm_hour % 12 : 12, 2, ' ');
				** ...and has been changed to the below to
				** match SunOS 4.1.1 and Arnold Robbins'
				** strftime version 3.0. That is, "%k" and
				** "%l" have been swapped.
				** (ado, 1993-05-24)
				*/
				pt = strftime_conv(t->tm_hour, "%2d",
						   pt, ptlim);
				continue;
#ifdef KITCHEN_SINK
			case 'K':
				/*
				** After all this time, still unclaimed!
				*/
				pt = strftime_add("kitchen sink", pt, ptlim);
				continue;
#endif /* defined KITCHEN_SINK */
			case 'l':
				/*
				** This used to be...
				**	_conv(t->tm_hour, 2, ' ');
				** ...and has been changed to the below to
				** match SunOS 4.1.1 and Arnold Robbin's
				** strftime version 3.0. That is, "%k" and
				** "%l" have been swapped.
				** (ado, 1993-05-24)
				*/
				pt = strftime_conv((t->tm_hour % 12) ?
						   (t->tm_hour % 12) : 12,
						   "%2d", pt, ptlim);
				continue;
			case 'M':
				pt = strftime_conv(t->tm_min, "%02d",
						   pt, ptlim);
				continue;
			case 'm':
				pt = strftime_conv(t->tm_mon + 1, "%02d",
						   pt, ptlim);
				continue;
			case 'n':
				pt = strftime_add("\n", pt, ptlim);
				continue;
			case 'p':
				pt = strftime_add(
					(t->tm_hour >= (HOURSPERDAY / 2)) ?
					Locale->pm :
					Locale->am,
					pt, ptlim);
				continue;
			case 'R':
				pt = strftime_fmt("%H:%M", t, pt, ptlim, warnp);
				continue;
			case 'r':
				pt = strftime_fmt("%I:%M:%S %p", t, pt,
						  ptlim, warnp);
				continue;
			case 'S':
				pt = strftime_conv(t->tm_sec, "%02d", pt,
						   ptlim);
				continue;
			case 's':
				{
					struct tm	tm;
					char		buf[INT_STRLEN_MAXIMUM(
								time_t) + 1];
					time_t		mkt;

					tm = *t;
					tm.tm_yday = -1;
					mkt = mktime(&tm);
					if (mkt == (time_t) -1) {
					  /* Fail unless this -1 represents
					     a valid time.  */
					  struct tm tm_1;
					  if (!localtime_r(&mkt, &tm_1))
					    return NULL;
					  if (!(tm.tm_year == tm_1.tm_year
						&& tm.tm_yday == tm_1.tm_yday
						&& tm.tm_hour == tm_1.tm_hour
						&& tm.tm_min == tm_1.tm_min
						&& tm.tm_sec == tm_1.tm_sec))
					    return NULL;
					}
					if (TYPE_SIGNED(time_t)) {
					  intmax_t n = mkt;
					  (sprintf)(buf, "%"PRIdMAX, n);
					} else {
					  uintmax_t n = mkt;
					  (sprintf)(buf, "%"PRIuMAX, n);
					}
					pt = strftime_add(buf, pt, ptlim);
				}
				continue;
			case 'T':
				pt = strftime_fmt("%H:%M:%S", t, pt, ptlim, warnp);
				continue;
			case 't':
				pt = strftime_add("\t", pt, ptlim);
				continue;
			case 'U':
				pt = strftime_conv((t->tm_yday + DAYSPERWEEK -
						    t->tm_wday) / DAYSPERWEEK,
						   "%02d", pt, ptlim);
				continue;
			case 'u':
				/*
				** From Arnold Robbins' strftime version 3.0:
				** "ISO 8601: Weekday as a decimal number
				** [1 (Monday) - 7]"
				** (ado, 1993-05-24)
				*/
				pt = strftime_conv((t->tm_wday == 0) ?
						   DAYSPERWEEK : t->tm_wday,
						   "%d", pt, ptlim);
				continue;
			case 'V':	/* ISO 8601 week number */
			case 'G':	/* ISO 8601 year (four digits) */
			case 'g':	/* ISO 8601 year (two digits) */
/*
** From Arnold Robbins' strftime version 3.0: "the week number of the
** year (the first Monday as the first day of week 1) as a decimal number
** (01-53)."
** (ado, 1993-05-24)
**
** From <https://www.cl.cam.ac.uk/~mgk25/iso-time.html> by Markus Kuhn:
** "Week 01 of a year is per definition the first week which has the
** Thursday in this year, which is equivalent to the week which contains
** the fourth day of January. In other words, the first week of a new year
** is the week which has the majority of its days in the new year. Week 01
** might also contain days from the previous year and the week before week
** 01 of a year is the last week (52 or 53) of the previous year even if
** it contains days from the new year. A week starts with Monday (day 1)
** and ends with Sunday (day 7). For example, the first week of the year
** 1997 lasts from 1996-12-30 to 1997-01-05..."
** (ado, 1996-01-02)
*/
				{
					int	year;
					int	base;
					int	yday;
					int	wday;
					int	w;

					year = t->tm_year;
					base = TM_YEAR_BASE;
					yday = t->tm_yday;
					wday = t->tm_wday;
					for ( ; ; ) {
						int	len;
						int	bot;
						int	top;

						len = isleap_sum(year, base) ?
							DAYSPERLYEAR :
							DAYSPERNYEAR;
						/*
						** What yday (-3 ... 3) does
						** the ISO year begin on?
						*/
						bot = ((yday + 11 - wday) %
							DAYSPERWEEK) - 3;
						/*
						** What yday does the NEXT
						** ISO year begin on?
						*/
						top = bot -
							(len % DAYSPERWEEK);
						if (top < -3)
							top += DAYSPERWEEK;
						top += len;
						if (yday >= top) {
							++base;
							w = 1;
							break;
						}
						if (yday >= bot) {
							w = 1 + ((yday - bot) /
								DAYSPERWEEK);
							break;
						}
						--base;
						yday += isleap_sum(year, base) ?
							DAYSPERLYEAR :
							DAYSPERNYEAR;
					}
					if (*format == 'V')
						pt = strftime_conv(w, "%02d",
								   pt, ptlim);
					else if (*format == 'g') {
						*warnp = IN_ALL;
						pt = strftime_yconv(year, base,
								    false, true,
								    pt, ptlim);
					} else	pt = strftime_yconv(year, base,
								    true, true,
								    pt, ptlim);
				}
				continue;
			case 'v':
				/*
				** From Arnold Robbins' strftime version 3.0:
				** "date as dd-bbb-YYYY"
				** (ado, 1993-05-24)
				*/
				pt = strftime_fmt("%e-%b-%Y", t, pt, ptlim, warnp);
				continue;
			case 'W':
				pt = strftime_conv(
					(t->tm_yday + DAYSPERWEEK -
					 (t->tm_wday ?
					  (t->tm_wday - 1) :
					  (DAYSPERWEEK - 1))) / DAYSPERWEEK,
					"%02d", pt, ptlim);
				continue;
			case 'w':
				pt = strftime_conv(t->tm_wday, "%d", pt, ptlim);
				continue;
			case 'X':
				pt = strftime_fmt(Locale->X_fmt, t, pt, ptlim, warnp);
				continue;
			case 'x':
				{
				enum warn warn2 = IN_SOME;

				pt = strftime_fmt(Locale->x_fmt, t, pt, ptlim, &warn2);
				if (warn2 == IN_ALL)
					warn2 = IN_THIS;
				if (warn2 > *warnp)
					*warnp = warn2;
				}
				continue;
			case 'y':
				*warnp = IN_ALL;
				pt = strftime_yconv(t->tm_year, TM_YEAR_BASE,
						    false, true,
						    pt, ptlim);
				continue;
			case 'Y':
				pt = strftime_yconv(t->tm_year, TM_YEAR_BASE,
						    true, true,
						    pt, ptlim);
				continue;
			case 'Z':
				pt = strftime_add(t->tm_zone, pt, ptlim);
				/*
				** C99 and later say that %Z must be
				** replaced by the empty string if the
				** time zone abbreviation is not
				** determinable.
				*/
				continue;
			case 'z':
				{
				long		diff;
				char const *	sign;
				bool negative;

				diff = t->tm_gmtoff;
				negative = diff < 0;
				if (diff == 0) {
				  negative = t->tm_zone[0] == '-';
				}
				if (negative) {
					sign = "-";
					diff = -diff;
				} else	sign = "+";
				pt = strftime_add(sign, pt, ptlim);
				diff /= SECSPERMIN;
				diff = (diff / MINSPERHOUR) * 100 +
					(diff % MINSPERHOUR);
				pt = strftime_conv(diff, "%04d", pt, ptlim);
				}
				continue;
			case '+':
				pt = strftime_fmt(Locale->date_fmt, t, pt,
						  ptlim, warnp);
				continue;
			case '%':
			/*
			** X311J/88-090 (4.12.3.5): if conversion char is
			** undefined, behavior is undefined. Print out the
			** character itself as printf(3) also does.
			*/
			default:
				break;
			}
		}
		if (pt == ptlim)
			break;
		*pt++ = *format;
	}
	return pt;
}

/**
 * Converts time to string, e.g.
 *
 *     char b[64];
 *     int64_t sec;
 *     struct tm tm;
 *     time(&sec);
 *     localtime_r(&sec, &tm);
 *     strftime(b, sizeof(b), "%Y-%m-%dT%H:%M:%S%z", &tm);       // ISO8601
 *     strftime(b, sizeof(b), "%a, %d %b %Y %H:%M:%S %Z", &tm);  // RFC1123
 *
 * @return bytes copied excluding nul, or 0 on error
 * @see FormatHttpDateTime()
 */
size_t
strftime(char *s, size_t maxsize, const char *format, const struct tm *t)
{
	char *	p;
	int saved_errno = errno;
	enum warn warn = IN_NONE;

	tzset();
	p = strftime_fmt(format, t, s, s + maxsize, &warn);
	if (!p) {
	  errno = EOVERFLOW;
	  return 0;
	}
	if (p == s + maxsize) {
		errno = ERANGE;
		return 0;
	}
	*p = '\0';
	errno = saved_errno;
	return p - s;
}

size_t
strftime_l(char *s, size_t maxsize, char const *format, struct tm const *t,
	   locale_t locale)
{
	/* Just call strftime, as only the C locale is supported.  */
	return strftime(s, maxsize, format, t);
}
