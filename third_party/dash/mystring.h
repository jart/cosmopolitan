/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) 1997-2005
 *	Herbert Xu <herbert@gondor.apana.org.au>.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Kenneth Almquist.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
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
 *
 *	@(#)mystring.h	8.2 (Berkeley) 5/4/95
 */

#include <inttypes.h>
#include <string.h>

#ifdef HAVE_FNMATCH
#define FNMATCH_IS_ENABLED 1
#ifdef HAVE_GLOB
#define GLOB_IS_ENABLED 1
#else
#define GLOB_IS_ENABLED 0
#endif
#else
#define FNMATCH_IS_ENABLED 0
#define GLOB_IS_ENABLED 0
#endif

extern const char snlfmt[];
extern const char spcstr[];
extern const char dolatstr[];
#define DOLATSTRLEN 6
extern const char cqchars[];
#define qchars (cqchars + 1)
extern const char illnum[];
extern const char homestr[];
extern const char dotdir[];

#if 0
void scopyn(const char *, char *, int);
#endif
char *prefix(const char *, const char *);
void badnum(const char *s) __attribute__ ((noreturn));
intmax_t atomax(const char *, int);
intmax_t atomax10(const char *);
int number(const char *);
int is_number(const char *);
char *single_quote(const char *);
char *sstrdup(const char *);
int pstrcmp(const void *, const void *);
const char *const *findstring(const char *, const char *const *, size_t);

#define equal(s1, s2)	(strcmp(s1, s2) == 0)
#define scopy(s1, s2)	((void)strcpy(s2, s1))
