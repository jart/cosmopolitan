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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.internal.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/errfuns.h"

int sys_accept4(int server, struct sockaddr_storage *addr, int flags) {
  uint32_t size = sizeof(*addr);
  int olderr, client, file_mode;
  olderr = errno;
  client = __sys_accept4(server, addr, &size, flags);
  if (client == -1 && errno == ENOSYS) {
    // XNU/RHEL5/etc. don't support accept4(), but it's easilly polyfilled
    errno = olderr;
    if (flags & ~(SOCK_CLOEXEC | SOCK_NONBLOCK)) return einval();
    if ((client = __sys_accept(server, addr, &size, 0)) != -1) {
      // __sys_accept() has inconsistent flag inheritence across platforms
      // this is one of the issues that accept4() was invented for solving
      unassert((file_mode = __sys_fcntl(client, F_GETFD)) != -1);
      unassert(!__sys_fcntl(client, F_SETFD,
                            ((file_mode & ~FD_CLOEXEC) |
                             (flags & SOCK_CLOEXEC ? FD_CLOEXEC : 0))));
      unassert((file_mode = __sys_fcntl(client, F_GETFL)) != -1);
      unassert(!__sys_fcntl(client, F_SETFL,
                            ((file_mode & ~O_NONBLOCK) |
                             (flags & SOCK_NONBLOCK ? O_NONBLOCK : 0))));
    }
  }
  return client;
}
