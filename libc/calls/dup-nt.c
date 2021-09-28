/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Implements dup(), dup2(), and dup3() for Windows NT.
 */
textwindows int sys_dup_nt(int oldfd, int newfd, int flags) {
  int64_t proc;
  if (oldfd < 0) return einval();
  if (oldfd >= g_fds.n ||
      (g_fds.p[oldfd].kind != kFdFile && g_fds.p[oldfd].kind != kFdSocket &&
       g_fds.p[oldfd].kind != kFdConsole)) {
    return ebadf();
  }
  if (newfd == -1) {
    if ((newfd = __reservefd()) == -1) return -1;
  } else {
    if (__ensurefds(newfd) == -1) return -1;
    if (g_fds.p[newfd].kind) close(newfd);
    g_fds.p[newfd].kind = kFdReserved;
  }
  proc = GetCurrentProcess();
  if (DuplicateHandle(proc, g_fds.p[oldfd].handle, proc, &g_fds.p[newfd].handle,
                      0, true, kNtDuplicateSameAccess)) {
    g_fds.p[newfd].kind = g_fds.p[oldfd].kind;
    g_fds.p[newfd].extra = g_fds.p[oldfd].extra;
    g_fds.p[newfd].flags = flags;
    return newfd;
  } else {
    __releasefd(newfd);
    return __winerr();
  }
}
