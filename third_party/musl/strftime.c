/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
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
#include "libc/ctype.h"
#include "libc/limits.h"
#include "libc/stdio/stdio.h"
#include "libc/str/langinfo.h"
#include "libc/str/locale.h"
#include "libc/str/locale.internal.h"
#include "libc/str/nltypes.h"
#include "libc/str/str.h"
#include "libc/time.h"
#include "third_party/musl/time_impl.h"
__static_yoink("musl_libc_notice");

static int is_leap(int y)
{
	/* Avoid overflow */
	if (y>INT_MAX-1900) y -= 2000;
	y += 1900;
	return !(y%4) && ((y%100) || !(y%400));
}

static int week_num(const struct tm *tm)
{
	int val = (tm->tm_yday + 7U - (tm->tm_wday+6U)%7) / 7;
	/* If 1 Jan is just 1-3 days past Monday,
	 * the previous week is also in this year. */
	if ((tm->tm_wday + 371U - tm->tm_yday - 2) % 7 <= 2)
		val++;
	if (!val) {
		val = 52;
		/* If 31 December of prev year a Thursday,
		 * or Friday of a leap year, then the
		 * prev year has 53 weeks. */
		int dec31 = (tm->tm_wday + 7U - tm->tm_yday - 1) % 7;
		if (dec31 == 4 || (dec31 == 5 && is_leap(tm->tm_year%400-1)))
			val++;
	} else if (val == 53) {
		/* If 1 January is not a Thursday, and not
		 * a Wednesday of a leap year, then this
		 * year has only 52 weeks. */
		int jan1 = (tm->tm_wday + 371U - tm->tm_yday) % 7;
		if (jan1 != 4 && (jan1 != 3 || !is_leap(tm->tm_year)))
			val = 1;
	}
	return val;
}

const char *__strftime_fmt_1(char (*s)[100], size_t *l, int f, const struct tm *tm, locale_t loc, int pad)
{
	nl_item item;
	long long val;
	const char *fmt = "-";
	int width = 2, def_pad = '0';

	switch (f) {
	case 'a':
		if (tm->tm_wday > 6U) goto string;
		item = ABDAY_1 + tm->tm_wday;
		goto nl_strcat;
	case 'A':
		if (tm->tm_wday > 6U) goto string;
		item = DAY_1 + tm->tm_wday;
		goto nl_strcat;
	case 'h':
	case 'b':
		if (tm->tm_mon > 11U) goto string;
		item = ABMON_1 + tm->tm_mon;
		goto nl_strcat;
	case 'B':
		if (tm->tm_mon > 11U) goto string;
		item = MON_1 + tm->tm_mon;
		goto nl_strcat;
	case 'c':
		item = D_T_FMT;
		goto nl_strftime;
	case 'C':
		val = (1900LL+tm->tm_year) / 100;
		goto number;
	case 'e':
		def_pad = '_';
	case 'd':
		val = tm->tm_mday;
		goto number;
	case 'D':
		fmt = "%m/%d/%y";
		goto recu_strftime;
	case 'F':
		fmt = "%Y-%m-%d";
		goto recu_strftime;
	case 'g':
	case 'G':
		val = tm->tm_year + 1900LL;
		if (tm->tm_yday < 3 && week_num(tm) != 1) val--;
		else if (tm->tm_yday > 360 && week_num(tm) == 1) val++;
		if (f=='g') val %= 100;
		else width = 4;
		goto number;
	case 'H':
		val = tm->tm_hour;
		goto number;
	case 'I':
		val = tm->tm_hour;
		if (!val) val = 12;
		else if (val > 12) val -= 12;
		goto number;
	case 'j':
		val = tm->tm_yday+1;
		width = 3;
		goto number;
	case 'm':
		val = tm->tm_mon+1;
		goto number;
	case 'M':
		val = tm->tm_min;
		goto number;
	case 'n':
		*l = 1;
		return "\n";
	case 'p':
		item = tm->tm_hour >= 12 ? PM_STR : AM_STR;
		goto nl_strcat;
	case 'r':
		item = T_FMT_AMPM;
		goto nl_strftime;
	case 'R':
		fmt = "%H:%M";
		goto recu_strftime;
	case 's':
		val = __tm_to_secs(tm) - tm->tm_gmtoff;
		width = 1;
		goto number;
	case 'S':
		val = tm->tm_sec;
		goto number;
	case 't':
		*l = 1;
		return "\t";
	case 'T':
		fmt = "%H:%M:%S";
		goto recu_strftime;
	case 'u':
		val = tm->tm_wday ? tm->tm_wday : 7;
		width = 1;
		goto number;
	case 'U':
		val = (tm->tm_yday + 7U - tm->tm_wday) / 7;
		goto number;
	case 'W':
		val = (tm->tm_yday + 7U - (tm->tm_wday+6U)%7) / 7;
		goto number;
	case 'V':
		val = week_num(tm);
		goto number;
	case 'w':
		val = tm->tm_wday;
		width = 1;
		goto number;
	case 'x':
		item = D_FMT;
		goto nl_strftime;
	case 'X':
		item = T_FMT;
		goto nl_strftime;
	case 'y':
		val = (tm->tm_year + 1900LL) % 100;
		if (val < 0) val = -val;
		goto number;
	case 'Y':
		val = tm->tm_year + 1900LL;
		if (val >= 10000) {
			*l = snprintf(*s, sizeof *s, "%lld", val);
			return *s;
		}
		width = 4;
		goto number;
	case 'z':
		if (tm->tm_isdst < 0) {
			*l = 0;
			return "";
		}
		*l = snprintf(*s, sizeof *s, "%+.4ld",
			tm->tm_gmtoff/3600*100 + tm->tm_gmtoff%3600/60);
		return *s;
	case 'Z':
		if (tm->tm_isdst < 0 || !tm->tm_zone) {
			*l = 0;
			return "";
		}
		fmt = tm->tm_zone;
		goto string;
	case '%':
		*l = 1;
		return "%";
	default:
		return 0;
	}
number:
	switch (pad ? pad : def_pad) {
	case '-': *l = snprintf(*s, sizeof *s, "%lld", val); break;
	case '_': *l = snprintf(*s, sizeof *s, "%*lld", width, val); break;
	case '0':
	default:  *l = snprintf(*s, sizeof *s, "%0*lld", width, val); break;
	}
	return *s;
nl_strcat:
	fmt = nl_langinfo_l(item, loc);
string:
	*l = strlen(fmt);
	return fmt;
nl_strftime:
	fmt = nl_langinfo_l(item, loc);
recu_strftime:
	*l = strftime_l(*s, sizeof *s, fmt, tm, loc);
	if (!*l) return 0;
	return *s;
}

size_t strftime_l(char *restrict s, size_t n, const char *restrict f, const struct tm *restrict tm, locale_t loc)
{
	size_t l, k;
	char buf[100];
	char *p;
	const char *t;
	int pad, plus;
	unsigned long width;
	for (l=0; l<n; f++) {
		if (!*f) {
			s[l] = 0;
			return l;
		}
		if (*f != '%') {
			s[l++] = *f;
			continue;
		}
		f++;
		pad = 0;
		if (*f == '-' || *f == '_' || *f == '0') pad = *f++;
		if ((plus = (*f == '+'))) f++;
		if (isdigit(*f)) {
			width = strtoul(f, &p, 10);
		} else {
			width = 0;
			p = (void *)f;
		}
		if (*p == 'C' || *p == 'F' || *p == 'G' || *p == 'Y') {
			if (!width && p!=f) width = 1;
		} else {
			width = 0;
		}
		f = p;
		if (*f == 'E' || *f == 'O') f++;
		t = __strftime_fmt_1(&buf, &k, *f, tm, loc, pad);
		if (!t) break;
		if (width) {
			/* Trim off any sign and leading zeros, then
			 * count remaining digits to determine behavior
			 * for the + flag. */
			if (*t=='+' || *t=='-') t++, k--;
			for (; *t=='0' && t[1]-'0'<10U; t++, k--);
			if (width < k) width = k;
			size_t d;
			for (d=0; t[d]-'0'<10U; d++);
			if (tm->tm_year < -1900) {
				s[l++] = '-';
				width--;
			} else if (plus && d+(width-k) >= (*p=='C'?3:5)) {
				s[l++] = '+';
				width--;
			}
			for (; width > k && l < n; width--)
				s[l++] = '0';
		}
		if (k > n-l) k = n-l;
		memcpy(s+l, t, k);
		l += k;
	}
	if (n) {
		if (l==n) l=n-1;
		s[l] = 0;
	}
	return 0;
}

size_t strftime(char *restrict s, size_t n, const char *restrict f, const struct tm *restrict tm)
{
	return strftime_l(s, n, f, tm, CURRENT_LOCALE);
}
