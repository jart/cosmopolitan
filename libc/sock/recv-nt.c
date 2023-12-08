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
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/nt/struct/iovec.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

#define _MSG_OOB      1
#define _MSG_PEEK     2
#define _MSG_WAITALL  8
#define _MSG_DONTWAIT 64

struct RecvArgs {
  const struct iovec *iov;
  size_t iovlen;
  struct NtIovec iovnt[16];
};

static textwindows int sys_recv_nt_start(int64_t handle,
                                         struct NtOverlapped *overlap,
                                         uint32_t *flags, void *arg) {
  struct RecvArgs *args = arg;
  return WSARecv(handle, args->iovnt,
                 __iovec2nt(args->iovnt, args->iov, args->iovlen), 0, flags,
                 overlap, 0);
}

textwindows ssize_t sys_recv_nt(int fd, const struct iovec *iov, size_t iovlen,
                                uint32_t flags) {
  if (flags & ~(_MSG_DONTWAIT | _MSG_OOB | _MSG_PEEK | _MSG_WAITALL)) {
    return einval();
  }
  ssize_t rc;
  struct Fd *f = g_fds.p + fd;
  sigset_t m = __sig_block();
  bool nonblock = !(flags & _MSG_WAITALL) &&
                  ((f->flags & O_NONBLOCK) || (flags & _MSG_DONTWAIT));
  flags &= ~_MSG_DONTWAIT;
  rc = __winsock_block(f->handle, flags, nonblock, f->rcvtimeo, m,
                       sys_recv_nt_start, &(struct RecvArgs){iov, iovlen});
  __sig_unblock(m);
  return rc;
}

#endif /* __x86_64__ */
