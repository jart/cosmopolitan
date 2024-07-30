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
#include "libc/str/str.h"
#include "libc/str/locale.internal.h"
__static_yoink("musl_libc_notice");

static char buf[LC_ALL*(LOCALE_NAME_MAX+1)];

char *setlocale(int cat, const char *name)
{
	const struct __locale_map *lm;

	if ((unsigned)cat > LC_ALL) return 0;

	pthread_mutex_lock(&__locale_lock);

	/* For LC_ALL, setlocale is required to return a string which
	 * encodes the current setting for all categories. The format of
	 * this string is unspecified, and only the following code, which
	 * performs both the serialization and deserialization, depends
	 * on the format, so it can easily be changed if needed. */
	if (cat == LC_ALL) {
		int i;
		if (name) {
			struct __locale_struct tmp_locale;
			char part[LOCALE_NAME_MAX+1] = "C.UTF-8";
			const char *p = name;
			for (i=0; i<LC_ALL; i++) {
				const char *z = strchrnul(p, ';');
				if (z-p <= LOCALE_NAME_MAX) {
					memcpy(part, p, z-p);
					part[z-p] = 0;
					if (*z) p = z+1;
				}
				lm = __get_locale(i, part);
				if (lm == LOC_MAP_FAILED) {
					pthread_mutex_unlock(&__locale_lock);
					return 0;
				}
				tmp_locale.cat[i] = lm;
			}
			__global_locale = tmp_locale;
		}
		char *s = buf;
		const char *part;
		int same = 0;
		for (i=0; i<LC_ALL; i++) {
			const struct __locale_map *lm =
				__global_locale.cat[i];
			if (lm == __global_locale.cat[0]) same++;
			part = lm ? lm->name : "C";
			size_t l = strlen(part);
			memcpy(s, part, l);
			s[l] = ';';
			s += l+1;
		}
		*--s = 0;
		pthread_mutex_unlock(&__locale_lock);
		return same==LC_ALL ? (char *)part : buf;
	}

	if (name) {
		lm = __get_locale(cat, name);
		if (lm == LOC_MAP_FAILED) {
			pthread_mutex_unlock(&__locale_lock);
			return 0;
		}
		__global_locale.cat[cat] = lm;
	} else {
		lm = __global_locale.cat[cat];
	}
	char *ret = lm ? (char *)lm->name : "C";

	pthread_mutex_unlock(&__locale_lock);

	return ret;
}
