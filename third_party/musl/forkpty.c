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
#include "libc/calls/weirdtypes.h"
#include "libc/calls/struct/sigset.h"
#include "libc/sysv/consts/sig.h"
#include "libc/calls/struct/sigset.h"
#include "libc/thread/thread.h"
#include "libc/calls/calls.h"
#include "libc/sysv/consts/o.h"
#include "libc/runtime/runtime.h"
#include "libc/errno.h"
#include "libc/calls/termios.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

int forkpty(int *pm, char *name, const struct termios *tio, const struct winsize *ws)
{
	int m, s, ec=0, p[2], cs;
	pid_t pid=-1;
	sigset_t set, oldset;

	if (openpty(&m, &s, name, tio, ws) < 0) return -1;

	sigfillset(&set);
	pthread_sigmask(SIG_BLOCK, &set, &oldset);
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);

	if (pipe2(p, O_CLOEXEC)) {
		close(s);
		goto out;
	}

	pid = fork();
	if (!pid) {
		close(m);
		close(p[0]);
		if (login_tty(s)) {
			write(p[1], &errno, sizeof errno);
			_exit(127);
		}
		close(p[1]);
		pthread_setcancelstate(cs, 0);
		pthread_sigmask(SIG_SETMASK, &oldset, 0);
		return 0;
	}
	close(s);
	close(p[1]);
	if (read(p[0], &ec, sizeof ec) > 0) {
		int status;
		waitpid(pid, &status, 0);
		pid = -1;
		errno = ec;
	}
	close(p[0]);

out:
	if (pid > 0) *pm = m;
	else close(m);

	pthread_setcancelstate(cs, 0);
	pthread_sigmask(SIG_SETMASK, &oldset, 0);

	return pid;
}
