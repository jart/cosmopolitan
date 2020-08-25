/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/fmt/fmt.h"
#include "libc/macros.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "libc/tzfile.h"

STATIC_YOINK("ntoa");

asm(".ident\t\"\\n\\n\
strftime (BSD-3)\\n\
Copyright 1989 The Regents of the University of California\"");
asm(".include \"libc/disclaimer.inc\"");

static char *strftime_add(char *pt, const char *ptlim, const char *str) {
  while (pt < ptlim && (*pt = *str++) != '\0') ++pt;
  return pt;
}

static char *strftime_conv(char *pt, const char *ptlim, int n,
                           const char *format) {
  char buf[INT_STRLEN_MAXIMUM(int) + 1];
  (snprintf)(buf, sizeof(buf), format, n);
  return strftime_add(pt, ptlim, buf);
}

static char *strftime_secs(char *pt, const char *ptlim, const struct tm *t) {
  static char buf[INT_STRLEN_MAXIMUM(int) + 1];
  struct tm tmp;
  int64_t s;
  tmp = *t; /* Make a copy, mktime(3) modifies the tm struct. */
  s = mktime(&tmp);
  (snprintf)(buf, sizeof(buf), "%ld", s);
  return strftime_add(pt, ptlim, buf);
}

static char *strftime_timefmt(char *pt, const char *ptlim, const char *format,
                              const struct tm *t) {
  int i;
  long diff;
  char const *sign;
  /* size_t z1, z2, z3; */
  for (; *format; ++format) {
    if (*format == '%') {
    label:
      switch (*++format) {
        case '\0':
          --format;
          break;
        case 'A':
          pt = strftime_add(pt, ptlim,
                            (t->tm_wday < 0 || t->tm_wday > 6)
                                ? "?"
                                : kWeekdayName[t->tm_wday]);
          continue;
        case 'a':
          pt = strftime_add(pt, ptlim,
                            (t->tm_wday < 0 || t->tm_wday > 6)
                                ? "?"
                                : kWeekdayNameShort[t->tm_wday]);
          continue;
        case 'B':
          pt = strftime_add(
              pt, ptlim,
              (t->tm_mon < 0 || t->tm_mon > 11) ? "?" : kMonthName[t->tm_mon]);
          continue;
        case 'b':
        case 'h':
          pt = strftime_add(pt, ptlim,
                            (t->tm_mon < 0 || t->tm_mon > 11)
                                ? "?"
                                : kMonthNameShort[t->tm_mon]);
          continue;
        case 'c':
          pt = strftime_timefmt(pt, ptlim, "%D %X", t);
          continue;
        case 'C':
          /*
          ** %C used to do a...
          **	strftime_timefmt("%a %b %e %X %Y", t);
          ** ...whereas now POSIX 1003.2 calls for
          ** something completely different.
          ** (ado, 5/24/93)
          */
          pt = strftime_conv(pt, ptlim, div100int64(t->tm_year + TM_YEAR_BASE),
                             "%02d");
          continue;
        case 'D':
          pt = strftime_timefmt(pt, ptlim, "%m/%d/%y", t);
          continue;
        case 'x':
          /*
          ** Version 3.0 of strftime from Arnold Robbins
          ** (arnold@skeeve.atl.ga.us) does the
          ** equivalent of...
          **	strftime_timefmt("%a %b %e %Y");
          ** ...for %x; since the X3J11 C language
          ** standard calls for "date, using locale's
          ** date format," anything goes.  Using just
          ** numbers (as here) makes Quakers happier.
          ** Word from Paul Eggert (eggert@twinsun.com)
          ** is that %Y-%m-%d is the ISO standard date
          ** format, specified in ISO 2014 and later
          ** ISO 8601:1988, with a summary available in
          ** pub/doc/ISO/english/ISO8601.ps.Z on
          ** ftp.uni-erlangen.de.
          ** (ado, 5/30/93)
          */
          pt = strftime_timefmt(pt, ptlim, "%m/%d/%y", t);
          continue;
        case 'd':
          pt = strftime_conv(pt, ptlim, t->tm_mday, "%02d");
          continue;
        case 'E':
        case 'O':
          /*
          ** POSIX locale extensions, a la
          ** Arnold Robbins' strftime version 3.0.
          ** The sequences
          **	%Ec %EC %Ex %Ey %EY
          **	%Od %oe %OH %OI %Om %OM
          **	%OS %Ou %OU %OV %Ow %OW %Oy
          ** are supposed to provide alternate
          ** representations.
          ** (ado, 5/24/93)
          */
          goto label;
        case 'e':
          pt = strftime_conv(pt, ptlim, t->tm_mday, "%2d");
          continue;
        case 'H':
          pt = strftime_conv(pt, ptlim, t->tm_hour, "%02d");
          continue;
        case 'I':
          pt = strftime_conv(
              pt, ptlim, (t->tm_hour % 12) ? (t->tm_hour % 12) : 12, "%02d");
          continue;
        case 'j':
          pt = strftime_conv(pt, ptlim, t->tm_yday + 1, "%03d");
          continue;
        case 'k':
          /*
          ** This used to be...
          **	strftime_conv(t->tm_hour % 12 ?
          **		t->tm_hour % 12 : 12, 2, ' ');
          ** ...and has been changed to the below to
          ** match SunOS 4.1.1 and Arnold Robbins'
          ** strftime version 3.0.  That is, "%k" and
          ** "%l" have been swapped.
          ** (ado, 5/24/93)
          */
          pt = strftime_conv(pt, ptlim, t->tm_hour, "%2d");
          continue;
#ifdef KITCHEN_SINK
        case 'K':
          /*
          ** After all this time, still unclaimed!
          */
          pt = strftime_add(pt, ptlim, "kitchen sink");
          continue;
#endif /* defined KITCHEN_SINK */
        case 'l':
          /*
          ** This used to be...
          **	strftime_conv(t->tm_hour, 2, ' ');
          ** ...and has been changed to the below to
          ** match SunOS 4.1.1 and Arnold Robbin's
          ** strftime version 3.0.  That is, "%k" and
          ** "%l" have been swapped.
          ** (ado, 5/24/93)
          */
          pt = strftime_conv(pt, ptlim,
                             (t->tm_hour % 12) ? (t->tm_hour % 12) : 12, "%2d");
          continue;
        case 'M':
          pt = strftime_conv(pt, ptlim, t->tm_min, "%02d");
          continue;
        case 'm':
          pt = strftime_conv(pt, ptlim, t->tm_mon + 1, "%02d");
          continue;
        case 'n':
          pt = strftime_add(pt, ptlim, "\n");
          continue;
        case 'p':
          pt = strftime_add(pt, ptlim, t->tm_hour >= 12 ? "PM" : "AM");
          continue;
        case 'R':
          pt = strftime_timefmt(pt, ptlim, "%H:%M", t);
          continue;
        case 'r':
          pt = strftime_timefmt(pt, ptlim, "%I:%M:%S %p", t);
          continue;
        case 'S':
          pt = strftime_conv(pt, ptlim, t->tm_sec, "%02d");
          continue;
        case 's':
          pt = strftime_secs(pt, ptlim, t);
          continue;
        case 'T':
        case 'X':
          pt = strftime_timefmt(pt, ptlim, "%H:%M:%S", t);
          continue;
        case 't':
          pt = strftime_add(pt, ptlim, "\t");
          continue;
        case 'U':
          pt = strftime_conv(pt, ptlim, (t->tm_yday + 7 - t->tm_wday) / 7,
                             "%02d");
          continue;
        case 'u':
          /*
          ** From Arnold Robbins' strftime version 3.0:
          ** "ISO 8601: Weekday as a decimal number
          ** [1 (Monday) - 7]"
          ** (ado, 5/24/93)
          */
          pt = strftime_conv(pt, ptlim, (t->tm_wday == 0) ? 7 : t->tm_wday,
                             "%d");
          continue;
        case 'V':
          /*
          ** From Arnold Robbins' strftime version 3.0:
          ** "the week number of the year (the first
          ** Monday as the first day of week 1) as a
          ** decimal number (01-53).  The method for
          ** determining the week number is as specified
          ** by ISO 8601 (to wit: if the week containing
          ** January 1 has four or more days in the new
          ** year, then it is week 1, otherwise it is
          ** week 53 of the previous year and the next
          ** week is week 1)."
          ** (ado, 5/24/93)
          */
          /*
          ** XXX--If January 1 falls on a Friday,
          ** January 1-3 are part of week 53 of the
          ** previous year.  By analogy, if January
          ** 1 falls on a Thursday, are December 29-31
          ** of the PREVIOUS year part of week 1???
          ** (ado 5/24/93)
          **
          ** You are understood not to expect this.
          */
          i = (t->tm_yday + 10 - (t->tm_wday ? (t->tm_wday - 1) : 6)) / 7;
          if (i == 0) {
            /*
            ** What day of the week does
            ** January 1 fall on?
            */
            i = t->tm_wday - (t->tm_yday - 1);
            /*
            ** Fri Jan 1: 53
            ** Sun Jan 1: 52
            ** Sat Jan 1: 53 if previous
            ** 		 year a leap
            **		 year, else 52
            */
            if (i == TM_FRIDAY)
              i = 53;
            else if (i == TM_SUNDAY)
              i = 52;
            else
              i = isleap(t->tm_year + TM_YEAR_BASE) ? 53 : 52;
#ifdef XPG4_1994_04_09
            /*
            ** As of 4/9/94, though,
            ** XPG4 calls for 53
            ** unconditionally.
            */
            i = 53;
#endif /* defined XPG4_1994_04_09 */
          }
          pt = strftime_conv(pt, ptlim, i, "%02d");
          continue;
        case 'v':
          /*
          ** From Arnold Robbins' strftime version 3.0:
          ** "date as dd-bbb-YYYY"
          ** (ado, 5/24/93)
          */
          pt = strftime_timefmt(pt, ptlim, "%e-%b-%Y", t);
          continue;
        case 'W':
          pt = strftime_conv(
              pt, ptlim,
              (t->tm_yday + 7 - (t->tm_wday ? (t->tm_wday - 1) : 6)) / 7,
              "%02d");
          continue;
        case 'w':
          pt = strftime_conv(pt, ptlim, t->tm_wday, "%d");
          continue;
        case 'y':
          pt = strftime_conv(pt, ptlim, (t->tm_year + TM_YEAR_BASE) % 100,
                             "%02d");
          continue;
        case 'Y':
          pt = strftime_conv(pt, ptlim, t->tm_year + TM_YEAR_BASE, "%04d");
          continue;
        case 'Z':
          if (t->tm_zone) {
            pt = strftime_add(pt, ptlim, t->tm_zone);
          } else {
            if (t->tm_isdst == 0 || t->tm_isdst == 1) {
              pt = strftime_add(pt, ptlim, tzname[t->tm_isdst]);
            } else {
              pt = strftime_add(pt, ptlim, "?");
            }
          }
          continue;
        case 'z':
          if (t->tm_isdst < 0) continue;
#ifdef TM_GMTOFF
          diff = t->TM_GMTOFF;
#else /* !defined TM_GMTOFF */
          if (t->tm_isdst == 0)
#ifdef USG_COMPAT
            diff = -timezone;
#else  /* !defined USG_COMPAT */
            continue;
#endif /* !defined USG_COMPAT */
          else
#ifdef ALTZONE
            diff = -altzone;
#else  /* !defined ALTZONE */
            continue;
#endif /* !defined ALTZONE */
#endif /* !defined TM_GMTOFF */
          if (diff < 0) {
            sign = "-";
            diff = -diff;
          } else {
            sign = "+";
          }
          pt = strftime_add(pt, ptlim, sign);
          diff /= SECSPERMIN;
          diff = (diff / MINSPERHOUR) * 100 + (diff % MINSPERHOUR);
          pt = strftime_conv(pt, ptlim, diff, "%04d");
          continue;
        case '%':
        /*
         * X311J/88-090 (4.12.3.5): if conversion char is
         * undefined, behavior is undefined.  Print out the
         * character itself as printf(3) also does.
         */
        default:
          break;
      }
    }
    if (pt == ptlim) break;
    *pt++ = *format;
  }
  return pt;
}

size_t strftime(char *s, size_t maxsize, const char *format,
                const struct tm *t) {
  char *p;
  p = strftime_timefmt(s, s + maxsize, format, t);
  if (p == s + maxsize) return 0;
  *p = '\0';
  return p - s;
}
