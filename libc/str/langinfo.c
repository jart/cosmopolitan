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
#include "libc/str/langinfo.h"
#include "libc/str/locale.h"
#include "libc/str/nltypes.h"
#include "libc/thread/tls.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

static const char c_time[] =
	"Sun\0" "Mon\0" "Tue\0" "Wed\0" "Thu\0" "Fri\0" "Sat\0"
	"Sunday\0" "Monday\0" "Tuesday\0" "Wednesday\0"
	"Thursday\0" "Friday\0" "Saturday\0"
	"Jan\0" "Feb\0" "Mar\0" "Apr\0" "May\0" "Jun\0"
	"Jul\0" "Aug\0" "Sep\0" "Oct\0" "Nov\0" "Dec\0"
	"January\0"   "February\0" "March\0"    "April\0"
	"May\0"       "June\0"     "July\0"     "August\0"
	"September\0" "October\0"  "November\0" "December\0"
	"AM\0" "PM\0"
	"%a %b %e %T %Y\0"
	"%m/%d/%y\0"
	"%H:%M:%S\0"
	"%I:%M:%S %p\0"
	"\0"
	"\0"
	"%m/%d/%y\0"
	"0123456789\0"
	"%a %b %e %T %Y\0"
	"%H:%M:%S";

static const char c_messages[] = "^[yY]\0" "^[nN]\0" "yes\0" "no";
static const char c_numeric[] = ".\0" "";

char *nl_langinfo_l(nl_item item, locale_t loc)
{
	int cat = item >> 16;
	int idx = item & 65535;
	const char *str;

	if (!loc)
		return "";

	if (item == CODESET) return loc->cat[LC_CTYPE] ? "UTF-8" : "ASCII";

	/* _NL_LOCALE_NAME extension */
	if (idx == 65535 && cat < LC_ALL)
		return loc->cat[cat] ? (char *)loc->cat[cat]->name : "C";
	
	switch (cat) {
	case LC_NUMERIC:
		if (idx > 1) return "";
		str = c_numeric;
		break;
	case LC_TIME:
		if (idx > 0x31) return "";
		str = c_time;
		break;
	case LC_MONETARY:
		if (idx > 0) return "";
		str = "";
		break;
	case LC_MESSAGES:
		if (idx > 3) return "";
		str = c_messages;
		break;
	default:
		return "";
	}

	for (; idx; idx--, str++) for (; *str; str++);
	// if (cat != LC_NUMERIC && *str) str = LCTRANS(str, cat, loc);
	return (char *)str;
}

char *nl_langinfo(nl_item item)
{
	return nl_langinfo_l(item, (locale_t)__get_tls()->tib_locale);
}
