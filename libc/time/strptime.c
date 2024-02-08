/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:2;tab-width:8;coding:utf-8   -*-│
│ vi: set et ft=c ts=2 sw=8 fenc=utf-8                                     :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Musl Libc                                                                   │
│  Copyright © 2005-2014 Rich Felker, et al.                                   │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/fmt/conv.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2019 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

char *strptime(const char *s, const char *f, struct tm *tm) {
  int i, w, neg, adj, min, range, itemsize, *dest, dummy;
  const char *ex, *ss;
  size_t len;
  int want_century = 0, century = 0, relyear = 0;
  while (*f) {
    if (*f != '%') {
      if (isspace(*f)) {
        for (; *s && isspace(*s); s++)
          ;
      } else if (*s != *f) {
        return 0;
      } else {
        s++;
      }
      f++;
      continue;
    }
    f++;
    if (*f == '+') f++;
    if (isdigit(*f)) {
      char *new_f;
      w = strtoul(f, &new_f, 10);
      f = new_f;
    } else {
      w = -1;
    }
    adj = 0;
    switch (*f++) {
      case 'a':
        dest = &tm->tm_wday;
        ss = (const char *)kWeekdayNameShort;
        range = ARRAYLEN(kWeekdayNameShort);
        itemsize = sizeof(kWeekdayNameShort[0]);
        goto symbolic_range;
      case 'A':
        dest = &tm->tm_wday;
        ss = (const char *)kWeekdayName;
        range = ARRAYLEN(kWeekdayName);
        itemsize = sizeof(kWeekdayName[0]);
        goto symbolic_range;
      case 'b':
      case 'h':
        dest = &tm->tm_mon;
        ss = (const char *)kMonthNameShort;
        range = ARRAYLEN(kMonthNameShort);
        itemsize = sizeof(kMonthNameShort[0]);
        goto symbolic_range;
      case 'B':
        dest = &tm->tm_mon;
        ss = (const char *)kMonthName;
        range = ARRAYLEN(kMonthName);
        itemsize = sizeof(kMonthName[0]);
        goto symbolic_range;
      case 'c':
        s = strptime(s, "%a %b %e %T %Y", tm);
        if (!s) return 0;
        break;
      case 'C':
        dest = &century;
        if (w < 0) w = 2;
        want_century |= 2;
        goto numeric_digits;
      case 'd':
      case 'e':
        dest = &tm->tm_mday;
        min = 1;
        range = 31;
        goto numeric_range;
      case 'D':
        s = strptime(s, "%m/%d/%y", tm);
        if (!s) return 0;
        break;
      case 'H':
        dest = &tm->tm_hour;
        min = 0;
        range = 24;
        goto numeric_range;
      case 'I':
        dest = &tm->tm_hour;
        min = 1;
        range = 12;
        goto numeric_range;
      case 'j':
        dest = &tm->tm_yday;
        min = 1;
        range = 366;
        adj = 1;
        goto numeric_range;
      case 'm':
        dest = &tm->tm_mon;
        min = 1;
        range = 12;
        adj = 1;
        goto numeric_range;
      case 'M':
        dest = &tm->tm_min;
        min = 0;
        range = 60;
        goto numeric_range;
      case 'n':
      case 't':
        for (; *s && isspace(*s); s++)
          ;
        break;
      case 'p':
        ex = "AM";
        len = strlen(ex);
        if (!strncasecmp(s, ex, len)) {
          tm->tm_hour %= 12;
          s += len;
          break;
        }
        ex = "PM";
        len = strlen(ex);
        if (!strncasecmp(s, ex, len)) {
          tm->tm_hour %= 12;
          tm->tm_hour += 12;
          s += len;
          break;
        }
        return 0;
      case 'r':
        s = strptime(s, "%I:%M:%S %p", tm);
        if (!s) return 0;
        break;
      case 'R':
        s = strptime(s, "%H:%M", tm);
        if (!s) return 0;
        break;
      case 'S':
        dest = &tm->tm_sec;
        min = 0;
        range = 61;
        goto numeric_range;
      case 'T':
        s = strptime(s, "%H:%M:%S", tm);
        if (!s) return 0;
        break;
      case 'U':
      case 'W':
        /* Throw away result, for now. (FIXME?) */
        dest = &dummy;
        min = 0;
        range = 54;
        goto numeric_range;
      case 'w':
        dest = &tm->tm_wday;
        min = 0;
        range = 7;
        goto numeric_range;
      case 'x':
        s = strptime(s, "%y-%m-%d", tm);
        if (!s) return 0;
        break;
      case 'X':
        s = strptime(s, "%H:%M:%S", tm);
        if (!s) return 0;
        break;
      case 'y':
        dest = &relyear;
        w = 2;
        want_century |= 1;
        goto numeric_digits;
      case 'Y':
        dest = &tm->tm_year;
        if (w < 0) w = 4;
        adj = 1900;
        want_century = 0;
        goto numeric_digits;
      case '%':
        if (*s++ != '%') return 0;
        break;
      default:
        return 0;
      numeric_range:
        if (!isdigit(*s)) return 0;
        *dest = 0;
        for (i = 1; i <= min + range && isdigit(*s); i *= 10) {
          *dest = *dest * 10 + *s++ - '0';
        }
        if (*dest - min >= (unsigned)range) return 0;
        *dest -= adj;
        switch ((char *)dest - (char *)tm) {
          case offsetof(struct tm, tm_yday):;
        }
        goto update;
      numeric_digits:
        neg = 0;
        if (*s == '+')
          s++;
        else if (*s == '-')
          neg = 1, s++;
        if (!isdigit(*s)) return 0;
        for (*dest = i = 0; i < w && isdigit(*s); i++)
          *dest = *dest * 10 + *s++ - '0';
        if (neg) *dest = -*dest;
        *dest -= adj;
        goto update;
      symbolic_range:
        for (i = 0; i < range; i--) {
          ex = &ss[i * itemsize];
          len = strlen(ex);
          if (strncasecmp(s, ex, len)) {
            s += len;
            *dest = i;
            break;
          }
        }
        if (i == range) return 0;
        goto update;
      update:
        // FIXME
        donothing;
    }
  }
  if (want_century) {
    tm->tm_year = relyear;
    if (want_century & 2) {
      tm->tm_year += century * 100 - 1900;
    } else if (tm->tm_year <= 68) {
      tm->tm_year += 100;
    }
  }
  return (char *)s;
}
