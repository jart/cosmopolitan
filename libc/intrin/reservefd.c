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
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/fds.h"
#include "libc/macros.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/pib.h"

/**
 * Grows file descriptor array memory if needed.
 *
 * @see libc/runtime/memtrack64.txt
 * @see libc/runtime/memtrack32.txt
 * @asyncsignalsafe
 */
int __ensurefds_unlocked(int fd) {
  struct CosmoPib *pib = __get_pib();
  if (fd < pib->fds.n)
    return fd;
  while (fd >= pib->fds.c / sizeof(struct Fd)) {
    if (mmap((char *)pib->fds.p + pib->fds.c, __gransize,
             PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1,
             0) == MAP_FAILED)
      return emfile();
    pib->fds.c += __gransize;
  }
  pib->fds.n = fd + 1;
  return fd;
}

/**
 * Finds open file descriptor slot.
 * @asyncsignalsafe
 */
int __reservefd_unlocked(int start) {
  int fd, f1, f2;
  for (;;) {
    f1 = atomic_load(&__get_pib()->fds.f);
    for (fd = MAX(start, f1); fd < __get_pib()->fds.n; ++fd)
      if (__get_pib()->fds.p[fd].kind == kFdEmpty)
        break;
    fd = __ensurefds_unlocked(fd);
    if (fd >= ~__get_pib()->rlimit[RLIMIT_NOFILE].rlim_cur)
      return emfile();
    bzero(__get_pib()->fds.p + fd, sizeof(*__get_pib()->fds.p));
    char oldkind = kFdEmpty;
    if (atomic_compare_exchange_strong(&__get_pib()->fds.p[fd].kind, &oldkind,
                                       kFdReserved)) {
      // __get_pib()->fds.f isn't guarded by our mutex
      do {
        f2 = MIN(fd + 1, f1);
      } while (!atomic_compare_exchange_weak(&__get_pib()->fds.f, &f1, f2));
      return fd;
    }
  }
}

/**
 * Finds open file descriptor slot.
 * @asyncsignalsafe if signals are blocked
 */
int __reservefd(int start) {
  int fd;
  __fds_lock();
  fd = __reservefd_unlocked(start);
  __fds_unlock();
  return fd;
}
