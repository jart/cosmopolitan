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
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/nt/struct/iovec.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/consts/o.h"
#ifdef __x86_64__

struct SendArgs {
  const struct iovec *iov;
  size_t iovlen;
  struct NtIovec iovnt[16];
};

static textwindows int sys_send_nt_start(int64_t handle,
                                         struct NtOverlapped *overlap,
                                         uint32_t *flags, void *arg) {
  struct SendArgs *args = arg;
  return WSASend(handle, args->iovnt,
                 __iovec2nt(args->iovnt, args->iov, args->iovlen), 0, *flags,
                 overlap, 0);
}

textwindows ssize_t sys_send_nt(int fd, const struct iovec *iov, size_t iovlen,
                                uint32_t flags) {
  ssize_t rc;
  struct Fd *f = g_fds.p + fd;
  sigset_t m = __sig_block();
  rc = __winsock_block(f->handle, flags, !!(f->flags & O_NONBLOCK), f->sndtimeo,
                       m, sys_send_nt_start, &(struct SendArgs){iov, iovlen});
  __sig_unblock(m);
  return rc;
}

#endif /* __x86_64__ */
