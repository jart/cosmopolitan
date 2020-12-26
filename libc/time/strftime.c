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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/fmt/fmt.h"
#include "libc/macros.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "libc/time/tzfile.internal.h"

STATIC_YOINK("ntoa");

asm(".ident\t\"\\n\\n\
strftime (BSD-3)\\n\
Copyright 1989 The Regents of the University of California\"");
asm(".include \"libc/disclaimer.inc\"");

static char *strftime_add(char *p, const char *pe, const char *str) {
  while (p < pe && (*p = *str++) != '\0') ++p;
  return p;
}

static char *strftime_conv(char *p, const char *pe, int n, const char *format) {
  char buf[INT_STRLEN_MAXIMUM(int) + 1];
  (snprintf)(buf, sizeof(buf), format, n);
  return strftime_add(p, pe, buf);
}

static char *strftime_secs(char *p, const char *pe, const struct tm *t) {
  static char buf[INT_STRLEN_MAXIMUM(int) + 1];
  struct tm tmp;
  int64_t s;
  tmp = *t; /* Make a copy, mktime(3) modifies the tm struct. */
  s = mktime(&tmp);
  (snprintf)(buf, sizeof(buf), "%ld", s);
  return strftime_add(p, pe, buf);
}

static char *strftime_timefmt(char *p, const char *pe, const char *format,
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
          p = strftime_add(p, pe,
                           (t->tm_wday < 0 || t->tm_wday > 6)
                               ? "?"
                               : kWeekdayName[t->tm_wday]);
          continue;
        case 'a':
          p = strftime_add(p, pe,
                           (t->tm_wday < 0 || t->tm_wday > 6)
                               ? "?"
                               : kWeekdayNameShort[t->tm_wday]);
          continue;
        case 'B':
          p = strftime_add(
              p, pe,
              (t->tm_mon < 0 || t->tm_mon > 11) ? "?" : kMonthName[t->tm_mon]);
          continue;
        case 'b':
        case 'h':
          p = strftime_add(p, pe,
                           (t->tm_mon < 0 || t->tm_mon > 11)
                               ? "?"
                               : kMonthNameShort[t->tm_mon]);
          continue;
        case 'c':
          p = strftime_timefmt(p, pe, "%D %X", t);
          continue;
        case 'C':
          /*
          ** %C used to do a...
          **	strftime_timefmt("%a %b %e %X %Y", t);
          ** ...whereas now POSIX 1003.2 calls for
          ** something completely different.
          ** (ado, 5/24/93)
          */
          p = strftime_conv(p, pe, div100int64(t->tm_year + TM_YEAR_BASE),
                            "%02d");
          continue;
        case 'D':
          p = strftime_timefmt(p, pe, "%m/%d/%y", t);
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
          p = strftime_timefmt(p, pe, "%m/%d/%y", t);
          continue;
        case 'd':
          p = strftime_conv(p, pe, t->tm_mday, "%02d");
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
          p = strftime_conv(p, pe, t->tm_mday, "%2d");
          continue;
        case 'H':
          p = strftime_conv(p, pe, t->tm_hour, "%02d");
          continue;
        case 'I':
          p = strftime_conv(p, pe, (t->tm_hour % 12) ? (t->tm_hour % 12) : 12,
                            "%02d");
          continue;
        case 'j':
          p = strftime_conv(p, pe, t->tm_yday + 1, "%03d");
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
          p = strftime_conv(p, pe, t->tm_hour, "%2d");
          continue;
#ifdef KITCHEN_SINK
        case 'K':
          /*
          ** After all this time, still unclaimed!
          */
          p = strftime_add(p, pe, "kitchen sink");
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
          p = strftime_conv(p, pe, (t->tm_hour % 12) ? (t->tm_hour % 12) : 12,
                            "%2d");
          continue;
        case 'M':
          p = strftime_conv(p, pe, t->tm_min, "%02d");
          continue;
        case 'm':
          p = strftime_conv(p, pe, t->tm_mon + 1, "%02d");
          continue;
        case 'n':
          p = strftime_add(p, pe, "\n");
          continue;
        case 'p':
          p = strftime_add(p, pe, t->tm_hour >= 12 ? "PM" : "AM");
          continue;
        case 'R':
          p = strftime_timefmt(p, pe, "%H:%M", t);
          continue;
        case 'r':
          p = strftime_timefmt(p, pe, "%I:%M:%S %p", t);
          continue;
        case 'S':
          p = strftime_conv(p, pe, t->tm_sec, "%02d");
          continue;
        case 's':
          p = strftime_secs(p, pe, t);
          continue;
        case 'T':
        case 'X':
          p = strftime_timefmt(p, pe, "%H:%M:%S", t);
          continue;
        case 't':
          p = strftime_add(p, pe, "\t");
          continue;
        case 'U':
          p = strftime_conv(p, pe, (t->tm_yday + 7 - t->tm_wday) / 7, "%02d");
          continue;
        case 'u':
          /*
          ** From Arnold Robbins' strftime version 3.0:
          ** "ISO 8601: Weekday as a decimal number
          ** [1 (Monday) - 7]"
          ** (ado, 5/24/93)
          */
          p = strftime_conv(p, pe, (t->tm_wday == 0) ? 7 : t->tm_wday, "%d");
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
          p = strftime_conv(p, pe, i, "%02d");
          continue;
        case 'v':
          /*
          ** From Arnold Robbins' strftime version 3.0:
          ** "date as dd-bbb-YYYY"
          ** (ado, 5/24/93)
          */
          p = strftime_timefmt(p, pe, "%e-%b-%Y", t);
          continue;
        case 'W':
          p = strftime_conv(
              p, pe, (t->tm_yday + 7 - (t->tm_wday ? (t->tm_wday - 1) : 6)) / 7,
              "%02d");
          continue;
        case 'w':
          p = strftime_conv(p, pe, t->tm_wday, "%d");
          continue;
        case 'y':
          p = strftime_conv(p, pe, (t->tm_year + TM_YEAR_BASE) % 100, "%02d");
          continue;
        case 'Y':
          p = strftime_conv(p, pe, t->tm_year + TM_YEAR_BASE, "%04d");
          continue;
        case 'Z':
          if (t->tm_zone) {
            p = strftime_add(p, pe, t->tm_zone);
          } else {
            if (t->tm_isdst == 0 || t->tm_isdst == 1) {
              p = strftime_add(p, pe, tzname[t->tm_isdst]);
            } else {
              p = strftime_add(p, pe, "?");
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
          p = strftime_add(p, pe, sign);
          diff /= SECSPERMIN;
          diff = (diff / MINSPERHOUR) * 100 + (diff % MINSPERHOUR);
          p = strftime_conv(p, pe, diff, "%04d");
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
    if (p >= pe) break;
    *p++ = *format;
  }
  return p;
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
 */
size_t strftime(char *s, size_t size, const char *f, const struct tm *t) {
  char *p;
  assert(t);
  p = strftime_timefmt(s, s + size, f, t);
  if (p < s + size) {
    *p = '\0';
    return p - s;
  } else {
    s[size - 1] = '\0';
    return 0;
  }
}
