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

#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>

#ifndef SSIZE_MAX
#define SSIZE_MAX ((ssize_t)((size_t)-1 >> 1))
#endif

static inline void sigclearmask(void)
{
#if defined(HAVE_SIGSETMASK) && \
    (!defined(__GLIBC__) || \
     (defined(__GNUC__) && (__GNUC__ * 1000 + __GNUC_MINOR__) >= 4006))
#ifdef __GLIBC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
	sigsetmask(0);
#ifdef __GLIBC__
#pragma GCC diagnostic pop
#endif
#else
	sigset_t set;
	sigemptyset(&set);
	sigprocmask(SIG_SETMASK, &set, 0);
#endif
}

#ifndef HAVE_MEMFD_CREATE
static inline int memfd_create(const char *name, unsigned int flags)
{
	return -1;
}
#endif

#ifndef HAVE_MEMPCPY
void *mempcpy(void *, const void *, size_t);
#endif

#ifndef HAVE_STPCPY
char *stpcpy(char *, const char *);
#endif

#ifndef HAVE_STRCHRNUL
char *strchrnul(const char *, int);
#endif

#ifndef HAVE_STRSIGNAL
char *strsignal(int);
#endif

#ifndef HAVE_STRTOD
static inline double strtod(const char *nptr, char **endptr)
{
	*endptr = (char *)nptr;
	return 0;
}
#endif

#ifndef HAVE_STRTOIMAX
#define strtoimax strtoll
#endif

#ifndef HAVE_STRTOUMAX
#define strtoumax strtoull
#endif

#ifndef HAVE_BSEARCH
void *bsearch(const void *, const void *, size_t, size_t,
	      int (*)(const void *, const void *));
#endif

#ifndef HAVE_KILLPG
static inline int killpg(pid_t pid, int signal)
{
#ifdef DEBUG
	if (pid < 0)
		abort();
#endif
	return kill(-pid, signal);
}
#endif

#ifndef HAVE_SYSCONF
#define _SC_CLK_TCK 2
long sysconf(int) __attribute__((__noreturn__));
#endif

#if !HAVE_DECL_ISBLANK
int isblank(int c);
#endif

#ifndef HAVE_TEE
static inline ssize_t tee(int fd_in, int fd_out, size_t len, unsigned int flags)
{
	return -1;
}
#endif

#ifndef HAVE_FNMATCH
static inline int fnmatch(const char *pattern, const char *string, int flags)
{
	return -1;
}
#endif

#ifndef HAVE_GLOB
#define GLOB_ERR	(1 << 0)/* Return on read errors.  */
#define GLOB_MARK	(1 << 1)/* Append a slash to each name.  */
#define GLOB_NOSORT	(1 << 2)/* Don't sort the names.  */
#define GLOB_DOOFFS	(1 << 3)/* Insert PGLOB->gl_offs NULLs.  */
#define GLOB_NOCHECK	(1 << 4)/* If nothing matches, return the pattern.  */
#define GLOB_APPEND	(1 << 5)/* Append to results of a previous call.  */
#define GLOB_NOESCAPE	(1 << 6)/* Backslashes don't quote metacharacters.  */
#define GLOB_PERIOD	(1 << 7)/* Leading `.' can be matched by metachars.  */
#define GLOB_MAGCHAR	(1 << 8)/* Set in gl_flags if any metachars seen.  */
#define GLOB_ALTDIRFUNC (1 << 9)/* Use gl_opendir et al functions.  */
#define GLOB_BRACE	(1 << 10)/* Expand "{a,b}" to "a" "b".  */
#define GLOB_NOMAGIC	(1 << 11)/* If no magic chars, return the pattern.  */
#define GLOB_TILDE	(1 << 12)/* Expand ~user and ~ to home directories. */
#define GLOB_ONLYDIR	(1 << 13)/* Match only directories.  */
#define GLOB_TILDE_CHECK (1 << 14)/* Like GLOB_TILDE but return an error
				     if the user name is not available.  */

#define GLOB_NOSPACE	1	/* Ran out of memory.  */
#define GLOB_ABORTED	2	/* Read error.  */
#define GLOB_NOMATCH	3	/* No matches found.  */
#define GLOB_NOSYS	4	/* Not implemented.  */

struct dirent64;
struct stat64;

typedef struct {
	size_t gl_pathc;
	char **gl_pathv;
	size_t gl_offs;
	int gl_flags;

	void (*gl_closedir)(void *);
	struct dirent64 *(*gl_readdir)(void *);
	void *(*gl_opendir)(const char *);
	int (*gl_lstat)(const char *, struct stat64 *);
	int (*gl_stat)(const char *, struct stat64 *);
} glob64_t;

static inline int glob64(const char *pattern, int flags,
			 int (*errfunc)(const char *epath, int eerrno),
			 glob64_t *restrict pglob)
{
	return -1;
}

static inline void globfree64(glob64_t *pglob)
{
}
#endif

/*
 * A trick to suppress uninitialized variable warning without generating any
 * code
 */
#define uninitialized_var(x) x = x

unsigned conv_escape(char *str, char *out, bool mbchar);
