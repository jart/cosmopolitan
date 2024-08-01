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
#include "libc/calls/calls.h"
#include "third_party/musl/mapfile.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/locale.internal.h"
__static_yoink("musl_libc_notice");

#define malloc _mapanon
#define calloc undef
#define realloc undef
#define free undef

#pragma GCC diagnostic ignored "-Wparentheses"

const char *__lctrans_impl(const char *msg, const struct __locale_map *lm)
{
	const char *trans = 0;
	if (lm) trans = __mo_lookup(lm->map, lm->map_size, msg);
	return trans ? trans : msg;
}

static const char envvars[][12] = {
	"LC_CTYPE",
	"LC_NUMERIC",
	"LC_TIME",
	"LC_COLLATE",
	"LC_MONETARY",
	"LC_MESSAGES",
};

const struct __locale_map *__get_locale(int cat, const char *val)
{
	static void *volatile loc_head;
	const struct __locale_map *p;
	struct __locale_map *new = 0;
	const char *path = 0, *z;
	char buf[256];
	size_t l, n;

	if (!*val) {
		(val = getenv("LC_ALL")) && *val ||
		(val = getenv(envvars[cat])) && *val ||
		(val = getenv("LANG")) && *val ||
		(val = "C.UTF-8");
	}

	/* Limit name length and forbid leading dot or any slashes. */
	for (n=0; n<LOCALE_NAME_MAX && val[n] && val[n]!='/'; n++);
	if (val[0]=='.' || val[n]) val = "C.UTF-8";
	int builtin = (val[0]=='C' && !val[1])
		|| !strcmp(val, "C.UTF-8")
		|| !strcmp(val, "POSIX");

	if (builtin) {
		if (cat == LC_CTYPE && val[1]=='.')
			return (void *)&__c_dot_utf8;
		return 0;
	}

	for (p=loc_head; p; p=p->next)
		if (!strcmp(val, p->name)) return p;

	path = secure_getenv("MUSL_LOCPATH");
	/* FIXME: add a default path? */

	if (path) for (; *path; path=z+!!*z) {
		z = strchrnul(path, ':');
		l = z - path;
		if (l >= sizeof buf - n - 2) continue;
		memcpy(buf, path, l);
		buf[l] = '/';
		memcpy(buf+l+1, val, n);
		buf[l+1+n] = 0;
		size_t map_size;
		const void *map = __map_file(buf, &map_size);
		if (map) {
			new = malloc(sizeof *new);
			if (!new) {
				munmap((void *)map, map_size);
				break;
			}
			new->map = map;
			new->map_size = map_size;
			memcpy(new->name, val, n);
			new->name[n] = 0;
			new->next = loc_head;
			loc_head = new;
			break;
		}
	}

	/* If no locale definition was found, make a locale map
	 * object anyway to store the name, which is kept for the
	 * sake of being able to do message translations at the
	 * application level. */
	if (!new && (new = malloc(sizeof *new))) {
		new->map = __c_dot_utf8.map;
		new->map_size = __c_dot_utf8.map_size;
		memcpy(new->name, val, n);
		new->name[n] = 0;
		new->next = loc_head;
		loc_head = new;
	}

	/* For LC_CTYPE, never return a null pointer unless the
	 * requested name was "C" or "POSIX". */
	if (!new && cat == LC_CTYPE) new = (void *)&__c_dot_utf8;

	return new;
}
