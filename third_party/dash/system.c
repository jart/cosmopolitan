/*
 * Copyright (c) 2004
 *	Herbert Xu <herbert@gondor.apana.org.au>.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef HAVE_ISALPHA
#define isalnum _isalnum
#define iscntrl _iscntrl
#define islower _islower
#define isspace _isspace
#define isalpha _isalpha
#define isdigit _isdigit
#define isprint _isprint
#define isupper _isupper
#define isblank _isblank
#define isgraph _isgraph
#define ispunct _ispunct
#define isxdigit _isxdigit
#include <ctype.h>
#undef isalnum
#undef iscntrl
#undef islower
#undef isspace
#undef isalpha
#undef isdigit
#undef isprint
#undef isupper
#undef isblank
#undef isgraph
#undef ispunct
#undef isxdigit
#endif

#include <signal.h>
#include <string.h>

#include "error.h"
#include "output.h"
#include "system.h"

#ifndef HAVE_MEMPCPY
void *mempcpy(void *dest, const void *src, size_t n)
{
	return memcpy(dest, src, n) + n;
}
#endif

#ifndef HAVE_STPCPY
char *stpcpy(char *dest, const char *src)
{
	size_t len = strlen(src);
	dest[len] = 0;
	return mempcpy(dest, src, len);
}
#endif

#ifndef HAVE_STRCHRNUL
char *strchrnul(const char *s, int c)
{
	char *p = strchr(s, c);
	if (!p)
		p = (char *)s + strlen(s);
	return p;
}
#endif

#ifndef HAVE_STRSIGNAL
char *strsignal(int sig)
{
	static char buf[19];

	if ((unsigned)sig < NSIG && sys_siglist[sig])
		return (char *)sys_siglist[sig];
	fmtstr(buf, sizeof(buf), "Signal %d", sig); 
	return buf;
}
#endif

#ifndef HAVE_BSEARCH
void *bsearch(const void *key, const void *base, size_t nmemb,
	      size_t size, int (*cmp)(const void *, const void *))
{
	while (nmemb) {
		size_t mididx = nmemb / 2;
		const void *midobj = base + mididx * size;
		int diff = cmp(key, midobj);

		if (diff == 0)
			return (void *)midobj;

		if (diff > 0) {
			base = midobj + size;
			nmemb -= mididx + 1;
		} else
			nmemb = mididx;
	}

	return 0;
}
#endif

#ifndef HAVE_SYSCONF
long sysconf(int name)
{
	sh_error("no sysconf for: %d", name);
}
#endif

#ifndef HAVE_ISALPHA
int isalnum(int c) {
	return _isalnum(c);
}


int iscntrl(int c) {
	return _iscntrl(c);
}


int islower(int c) {
	return _islower(c);
}


int isspace(int c) {
	return _isspace(c);
}


int isalpha(int c) {
	return _isalpha(c);
}


int isdigit(int c) {
	return _isdigit(c);
}


int isprint(int c) {
	return _isprint(c);
}


int isupper(int c) {
	return _isupper(c);
}


#if HAVE_DECL_ISBLANK
int isblank(int c) {
	return _isblank(c);
}
#endif


int isgraph(int c) {
	return _isgraph(c);
}


int ispunct(int c) {
	return _ispunct(c);
}


int isxdigit(int c) {
	return _isxdigit(c);
}
#endif

#if !HAVE_DECL_ISBLANK
int isblank(int c) {
	return c == ' ' || c == '\t';
}
#endif
