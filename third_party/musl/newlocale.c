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
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/str/locale.internal.h"
__static_yoink("musl_libc_notice");

#define malloc _mapanon
#define calloc undef
#define realloc undef
#define free undef

static int default_locale_init_done;
static struct __locale_struct default_locale, default_ctype_locale;

int __loc_is_allocated(locale_t loc)
{
	return loc && loc != C_LOCALE && loc != UTF8_LOCALE
		&& loc != &default_locale && loc != &default_ctype_locale;
}

static locale_t do_newlocale(int mask, const char *name, locale_t loc)
{
	struct __locale_struct tmp;

	for (int i=0; i<LC_ALL; i++) {
		tmp.cat[i] = (!(mask & (1<<i)) && loc) ? loc->cat[i] :
			__get_locale(i, (mask & (1<<i)) ? name : "");
		if (tmp.cat[i] == LOC_MAP_FAILED)
			return 0;
	}

	/* For locales with allocated storage, modify in-place. */
	if (__loc_is_allocated(loc)) {
		*loc = tmp;
		return loc;
	}

	/* Otherwise, first see if we can use one of the builtin locales.
	 * This makes the common usage case for newlocale, getting a C locale
	 * with predictable behavior, very fast, and more importantly, fail-safe. */
	if (!memcmp(&tmp, C_LOCALE, sizeof tmp)) return C_LOCALE;
	if (!memcmp(&tmp, UTF8_LOCALE, sizeof tmp)) return UTF8_LOCALE;

	/* And provide builtins for the initial default locale, and a
	 * variant of the C locale honoring the default locale's encoding. */
	if (!default_locale_init_done) {
		for (int i=0; i<LC_ALL; i++)
			default_locale.cat[i] = __get_locale(i, "");
		default_ctype_locale.cat[LC_CTYPE] = default_locale.cat[LC_CTYPE];
		default_locale_init_done = 1;
	}
	if (!memcmp(&tmp, &default_locale, sizeof tmp)) return &default_locale;
	if (!memcmp(&tmp, &default_ctype_locale, sizeof tmp))
		return &default_ctype_locale;

	/* If no builtin locale matched, attempt to allocate and copy. */
	if ((loc = malloc(sizeof *loc))) *loc = tmp;

	return loc;
}

locale_t newlocale(int mask, const char *name, locale_t loc)
{
	pthread_mutex_lock(&__locale_lock);
	loc = do_newlocale(mask, name, loc);
	pthread_mutex_unlock(&__locale_lock);
	return loc;
}
