/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright (c) 1993                                                           │
│      The Regents of the University of California.  All rights reserved.      │
│                                                                              │
│ Redistribution and use in source and binary forms, with or without           │
│ modification, are permitted provided that the following conditions           │
│ are met:                                                                     │
│  1. Redistributions of source code must retain the above copyright           │
│     notice, this list of conditions and the following disclaimer.            │
│  2. Redistributions in binary form must reproduce the above copyright        │
│     notice, this list of conditions and the following disclaimer in the      │
│     documentation and/or other materials provided with the distribution.     │
│  3. Neither the name of the University nor the names of its contributors     │
│     may be used to endorse or promote products derived from this software    │
│     without specific prior written permission.                               │
│                                                                              │
│ THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND      │
│ ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE        │
│ IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   │
│ ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE     │
│ FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL   │
│ DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS      │
│ OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)        │
│ HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT   │
│ LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    │
│ OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF       │
│ SUCH DAMAGE.                                                                 │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/errno.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/log/bsd.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
// clang-format off

asm(".ident\t\"\\n\\n\
FreeBSD Err (BSD-3 License)\\n\
Copyright (c) 1993\\n\
\tThe Regents of the University of California.\\n\
\tAll rights reserved.\"");
asm(".include \"libc/disclaimer.inc\"");

static FILE *err_file; /* file to use for error output */
static void (*err_exit)(int);

/*
 * This is declared to take a `void *' so that the caller is not required
 * to include <stdio.h> first.  However, it is really a `FILE *', and the
 * manual page documents it as such.
 */
void
err_set_file(void *fp)
{
	if (fp)
		err_file = fp;
	else
		err_file = stderr;
}

void
err_set_exit(void (*ef)(int))
{
	err_exit = ef;
}

void
err(int eval, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	verrc(eval, errno, fmt, ap);
	va_end(ap);
}

void
verr(int eval, const char *fmt, va_list ap)
{
	verrc(eval, errno, fmt, ap);
}

void
errc(int eval, int code, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	verrc(eval, code, fmt, ap);
	va_end(ap);
}

void
verrc(int eval, int code, const char *fmt, va_list ap)
{
	if (err_file == NULL)
		err_set_file(NULL);
	(fprintf)(err_file, "%s: ", program_invocation_name);
	if (fmt != NULL) {
		(vfprintf)(err_file, fmt, ap);
		(fprintf)(err_file, ": ");
	}
	(fprintf)(err_file, "%s\n", _strerdoc(code));
	if (err_exit)
		err_exit(eval);
	exit(eval);
}

void
errx(int eval, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	verrx(eval, fmt, ap);
	va_end(ap);
}

void
verrx(int eval, const char *fmt, va_list ap)
{
	if (err_file == NULL)
		err_set_file(NULL);
	(fprintf)(err_file, "%s: ", program_invocation_name);
	if (fmt != NULL)
		(vfprintf)(err_file, fmt, ap);
	(fprintf)(err_file, "\n");
	if (err_exit)
		err_exit(eval);
	exit(eval);
}

void
warn(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vwarnc(errno, fmt, ap);
	va_end(ap);
}

void
vwarn(const char *fmt, va_list ap)
{
	vwarnc(errno, fmt, ap);
}

void
warnc(int code, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vwarnc(code, fmt, ap);
	va_end(ap);
}

void
vwarnc(int code, const char *fmt, va_list ap)
{
	int saved_errno;

	saved_errno = errno;
	if (err_file == NULL)
		err_set_file(NULL);
	(fprintf)(err_file, "%s: ", program_invocation_name);
	if (fmt != NULL) {
		(vfprintf)(err_file, fmt, ap);
		(fprintf)(err_file, ": ");
	}
	(fprintf)(err_file, "%s\n", strerror(code));
	errno = saved_errno;
}

void
warnx(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vwarnx(fmt, ap);
	va_end(ap);
}

void
vwarnx(const char *fmt, va_list ap)
{
	int saved_errno;

	saved_errno = errno;
	if (err_file == NULL)
		err_set_file(NULL);
	(fprintf)(err_file, "%s: ", program_invocation_name);
	if (fmt != NULL)
		(vfprintf)(err_file, fmt, ap);
	(fprintf)(err_file, "\n");
	errno = saved_errno;
}
