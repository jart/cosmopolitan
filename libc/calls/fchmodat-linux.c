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
#include "libc/calls/struct/stat.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/errno.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

static void __procfdname(char *buf, unsigned fd) {
	unsigned i, j;
	for (i = 0; (buf[i] = "/proc/self/fd/"[i]); i++);
	if (!fd) {
		buf[i] = '0';
		buf[i + 1] = 0;
		return;
	}
	for (j = fd; j; j /= 10, i++);
	buf[i] = 0;
	for (; fd; fd /= 10) buf[--i] = '0' + fd % 10;
}

int sys_fchmodat_linux(int fd, const char *path, unsigned mode, int flag) {
	struct stat st;
	int ret, fd2;
	char proc[15 + 3 * sizeof(int)];
	if (flag != AT_SYMLINK_NOFOLLOW)
		return einval();
	if (sys_fstatat(fd, path, &st, flag))
		return -1;
	if (S_ISLNK(st.st_mode))
		return eopnotsupp();
	if ((fd2 = sys_openat(fd, path, (O_RDONLY | O_PATH | O_NOFOLLOW |
					 O_NOCTTY | O_CLOEXEC), 0)) < 0) {
		if (fd2 == -ELOOP)
			return eopnotsupp();
		return fd2;
	}
	__procfdname(proc, fd2);
	ret = sys_fstatat(AT_FDCWD, proc, &st, 0);
	if (!ret) {
		if (S_ISLNK(st.st_mode))
			ret = eopnotsupp();
		else
			ret = sys_fchmodat(AT_FDCWD, proc, mode, 0);
	}
	sys_close(fd2);
	return ret;
}
