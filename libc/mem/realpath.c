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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/serialize.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/log/backtrace.internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

#define SYMLOOP_MAX 40

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

static size_t GetSlashLen(const char *s)
{
	const char *s0 = s;
	while (*s == '/') s++;
	return s-s0;
}

static char *ResolvePath(char *d, const char *s, size_t n)
{
	if (d || (d = malloc(n+1))) {
		return memmove(d, s, n+1);
	} else {
		enomem();
		return 0;
	}
}

/**
 * Returns absolute pathname.
 *
 * This function removes `/./` and `/../` components. If any individual
 * path component is a symbolic link, then it'll be resolved. Any slash
 * characters that repeat (e.g. `//`) will collapse into one (i.e. `/`)
 *
 * This implementation is consistent with glibc, in that `"//"` becomes
 * `"/"` unlike Musl Libc, which considers that special (not sure why?)
 * This is the only change Cosmopolitan Libc made vs. Musl's realpath()
 * aside from also being permissive about backslashes, to help Windows.
 *
 * @param filename is the path that needs to be resolved
 * @param resolved needs PATH_MAX bytes, or NULL to use malloc()
 * @return resolved path, or NULL w/ errno
 * @raise EINVAL if `filename` is NULL
 * @raise ENOENT if `filename` is an empty string
 * @raise ENOMEM if `resolved` is NULL and malloc() failed
 * @raise ENOENT if `filename` didn't exist
 * @raise ENOTDIR if directory component existed that's not a directory
 * @raise ENOTDIR if base component ends with slash and is not a dir
 * @raise ENAMETOOLONG if filename resolution exceeded `PATH_MAX`
 * @raise ELOOP if too many symlinks were encountered
 */
char *realpath(const char *filename, char *resolved)
{
	ssize_t rc;
	int e, up, check_dir=0;
	size_t k, p, q, l, l0, cnt=0, nup=0;
	char output[PATH_MAX], stack[PATH_MAX+1], *z;

	if (!filename) {
		einval();
		return 0;
	}
	l = strnlen(filename, sizeof stack);
	if (!l) {
		enoent();
		return 0;
	}
	if (l >= PATH_MAX) goto toolong;
	if (l >= 4 && READ32LE(filename) == READ32LE("/zip") &&
	    (!filename[4] || filename[4] == '/')) {
		return ResolvePath(resolved, filename, l);
	}
	p = sizeof stack - l - 1;
	q = 0;
	memcpy(stack+p, filename, l+1);

	/* Main loop. Each iteration pops the next part from stack of
	 * remaining path components and consumes any slashes that follow.
	 * If not a link, it's moved to output; if a link, contents are
	 * pushed to the stack. */
restart:
	for (; ; p+=GetSlashLen(stack+p)) {
		/* If stack starts with /, the whole component is / or //
		 * and the output state must be reset. */
		if (stack[p] == '/') {
			check_dir=0;
			nup=0;
			q=0;
			output[q++] = '/';
			p++;
			continue;
		}

		z = (char *)min((intptr_t)strchrnul(stack+p, '/'),
				(intptr_t)strchrnul(stack+p, '\\'));
		l0 = l = z-(stack+p);

		if (!l && !check_dir) break;

		/* Skip any . component but preserve check_dir status. */
		if (l==1 && stack[p]=='.') {
			p += l;
			continue;
		}

		/* Copy next component onto output at least temporarily, to
		 * call readlink, but wait to advance output position until
		 * determining it's not a link. */
		if (q && output[q-1] != '/') {
			if (!p) goto toolong;
			stack[--p] = '/';
			l++;
		}
		if (q+l >= PATH_MAX) goto toolong;
		if (l) memcpy(output+q, stack+p, l);
		output[q+l] = 0;
		p += l;

		up = 0;
		if (l0==2 && stack[p-2]=='.' && stack[p-1]=='.') {
			up = 1;
			/* Any non-.. path components we could cancel start
			 * after nup repetitions of the 3-byte string "../";
			 * if there are none, accumulate .. components to
			 * later apply to cwd, if needed. */
			if (q <= 3*nup) {
				nup++;
				q += l;
				continue;
			}
			/* When previous components are already known to be
			 * directories, processing .. can skip readlink. */
			if (!check_dir) goto skip_readlink;
		}
		e = errno;
		if ((rc = readlink(output, stack, p)) == -1) {
			if (errno != EINVAL) return 0;
			errno = e; /* [jart] undirty errno if not a symlink */
skip_readlink:
			check_dir = 0;
			if (up) {
				while(q && output[q-1] != '/') q--;
				if (q>1 && (q>2 || output[0] != '/')) q--;
				continue;
			}
			if (l0) q += l;
			check_dir = stack[p];
			continue;
		}
		k = rc;
		npassert(k <= p);
		if (k==p)
			goto toolong;
		if (!k) {
			errno = ENOENT;
			return 0;
		}
		if (++cnt == SYMLOOP_MAX) {
			errno = ELOOP;
			return 0;
		}

		/* If link contents end in /, strip any slashes already on
		 * stack to avoid /->// or //->/// or spurious toolong. */
		if (stack[k-1] == '/') {
			while (stack[p] == '/')
				p++;
		}
		p -= k;
		memmove(stack+p, stack, k);

		/* Skip the stack advancement in case we have a new
		 * absolute base path. */
		goto restart;
	}

 	output[q] = 0;

	if (output[0] != '/') {
		if (__getcwd(stack, sizeof(stack)) == -1) return 0;
		l = strlen(stack);
		/* Cancel any initial .. components. */
		p = 0;
		while (nup--) {
			while(l>1 && stack[l-1] != '/') l--;
			if (l>1) l--;
			p += 2;
			if (p<q) p++;
		}
		if (q-p && stack[l-1] != '/') stack[l++] = '/';
		if (l + (q-p) + 1 >= PATH_MAX) goto toolong;
		memmove(output + l, output + p, q - p + 1);
		if (l) memcpy(output, stack, l);
		q = l + q-p;
	}

	return ResolvePath(resolved, output, q);

toolong:
	enametoolong();
	return 0;
}
