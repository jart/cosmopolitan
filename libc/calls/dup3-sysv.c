/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

#define F_DUP2FD         10
#define F_DUP2FD_CLOEXEC 18

static struct Dup3 {
  _Atomic(uint32_t) once;
  bool demodernize;
} g_dup3;

static void sys_dup3_test(void) {
  int e = errno;
  __sys_dup3(-1, -1, 0);
  if ((g_dup3.demodernize = errno == ENOSYS)) {
    STRACE("demodernizing %s() due to %s", "dup3", "ENOSYS");
  }
  errno = e;
}

int32_t sys_dup3(int32_t oldfd, int32_t newfd, int flags) {
  int how;
  unassert(oldfd >= 0);
  unassert(newfd >= 0);
  unassert(!(flags & ~O_CLOEXEC));

  if (IsFreebsd()) {
    if (flags & O_CLOEXEC) {
      how = F_DUP2FD_CLOEXEC;
    } else {
      how = F_DUP2FD;
    }
    return __sys_fcntl(oldfd, how, newfd);
  }

  cosmo_once(&g_dup3.once, sys_dup3_test);

  if (!g_dup3.demodernize) {
    return __sys_dup3(oldfd, newfd, flags);
  } else {
    return __fixupnewfd(sys_dup2(oldfd, newfd, 0), flags);
  }
}
