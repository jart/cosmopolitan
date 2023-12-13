/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ $OpenBSD: ecvt.c,v 1.11 2019/01/25 00:19:25 millert Exp $                    │
│                                                                              │
│ Copyright (c) 2002, 2006 Todd C. Miller <millert@openbsd.org>                │
│                                                                              │
│ Permission to use, copy, modify, and distribute this software for any        │
│ purpose with or without fee is hereby granted, provided that the above       │
│ copyright notice and this permission notice appear in all copies.            │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES     │
│ WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF             │
│ MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR      │
│ ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES       │
│ WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN        │
│ ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF      │
│ OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.               │
│                                                                              │
│ Sponsored in part by the Defense Advanced Research Projects                  │
│ Agency (DARPA) and Air Force Research Laboratory, Air Force                  │
│ Materiel Command, USAF, under agreement number F39502-99-1-0512.             │
│ SUCH DAMAGE.                                                                 │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "third_party/gdtoa/gdtoa.h"

asm(".ident\t\"\\n\\n\
OpenBSD ecvt/gcvt (MIT)\\n\
Copyright (c) 2002, 2006, 2010 Todd C. Miller <millert@openbsd.org>\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

static char *s;

static void
__cvt_atexit(void)
{
	free(s);
	s = 0;
}

static void __attribute__((__constructor__))
__cvt_init(void)
{
	atexit(__cvt_atexit);
}

static char *
__cvt(double value, int ndigit, int *decpt, int *sign, int fmode, int pad)
{
	char *p, *rve, c;
	size_t siz;

	if (ndigit == 0) {
		*sign = value < 0.0;
		*decpt = 0;
		return ("");
	}

	free(s);
	s = NULL;

	if (ndigit < 0)
		siz = -ndigit + 1;
	else
		siz = ndigit + 1;


	/* __dtoa() doesn't allocate space for 0 so we do it by hand */
	if (value == 0.0) {
		*decpt = 1 - fmode;	/* 1 for 'e', 0 for 'f' */
		*sign = 0;
		if ((rve = s = malloc(siz)) == NULL)
			return(NULL);
		*rve++ = '0';
		*rve = '\0';
	} else {
		p = dtoa(value, fmode + 2, ndigit, decpt, sign, &rve);
		if (p == NULL)
			return (NULL);
		if (*decpt == 9999) {
			/* Infinity or Nan, convert to inf or nan like printf */
			*decpt = 0;
			c = *p;
			freedtoa(p);
			return(c == 'I' ? "inf" : "nan");
		}
		/* Make a local copy and adjust rve to be in terms of s */
		if (pad && fmode)
			siz += *decpt;
		if ((s = malloc(siz)) == NULL) {
			freedtoa(p);
			return(NULL);
		}
		(void) strlcpy(s, p, siz);
		rve = s + (rve - p);
		freedtoa(p);
	}

	/* Add trailing zeros */
	if (pad) {
		siz -= rve - s;
		while (--siz)
			*rve++ = '0';
		*rve = '\0';
	}

	return(s);
}

char *
ecvt(double value, int ndigit, int *decpt, int *sign)
{
	return(__cvt(value, ndigit, decpt, sign, 0, 1));
}

char *
fcvt(double value, int ndigit, int *decpt, int *sign)
{
	return(__cvt(value, ndigit, decpt, sign, 1, 1));
}
