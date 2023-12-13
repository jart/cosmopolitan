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
#include "third_party/musl/tempnam.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/clock.h"
#include "libc/time/time.h"

#define MAXTRIES 100

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

static char *
__randname(char *template)
{
	int i;
	struct timespec ts;
	unsigned long r;
	clock_gettime(CLOCK_REALTIME, &ts);
	r = ts.tv_nsec * 65537 ^ ((uintptr_t)&ts / 16 + (uintptr_t) template);
	for (i = 0; i < 6; i++, r >>= 5) template[i] = 'A' + (r & 15) + (r & 16) * 2;
	return template;
}

/**
 * Creates name for temporary file.
 */
char *
tempnam(const char *dir, const char *pfx)
{
	int i, r;
	char s[PATH_MAX];
	size_t l, dl, pl;
	if (!dir) dir = __get_tmpdir();
	if (!pfx) pfx = "temp";
	dl = strlen(dir);
	pl = strlen(pfx);
	l = dl + 1 + pl + 1 + 6;
	if (l >= PATH_MAX) {
		errno = ENAMETOOLONG;
		return 0;
	}
	memcpy(s, dir, dl);
	s[dl] = '/';
	memcpy(s + dl + 1, pfx, pl);
	s[dl + 1 + pl] = '_';
	s[l] = 0;
	for (i = 0; i < MAXTRIES; i++) {
		__randname(s + l - 6);
		r = fstatat(AT_FDCWD, s, &(struct stat){0}, AT_SYMLINK_NOFOLLOW);
		if (r == -ENOENT) return strdup(s);
	}
	return 0;
}
