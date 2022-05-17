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
#include "libc/assert.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/spinlock.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Grows file descriptor array memory if needed.
 */
int __ensurefds_unlocked(int fd) {
  size_t n1, n2;
  struct Fd *p1, *p2;
  n1 = g_fds.n;
  if (fd >= n1) {
    STRACE("__ensurefds(%d) extending", fd);
    if (weaken(malloc)) {
      // TODO(jart): we need a semaphore for this
      p1 = g_fds.p;
      n2 = fd + (fd >> 1);
      if ((p2 = weaken(malloc)(n2 * sizeof(*p1)))) {
        memcpy(p2, p1, n1 * sizeof(*p1));
        g_fds.p = p2;
        g_fds.n = n2;
        if (p1 != g_fds.__init_p) {
          __cxa_atexit(free, p1, 0);
        }
      } else {
        fd = enomem();
      }
    } else {
      fd = emfile();
    }
  }
  return fd;
}

/**
 * Grows file descriptor array memory if needed.
 */
int __ensurefds(int fd) {
  _spinlock(&__fds_lock);
  fd = __ensurefds_unlocked(fd);
  _spunlock(&__fds_lock);
  return fd;
}

/**
 * Finds open file descriptor slot.
 */
int __reservefd_unlocked(int start) {
  int fd;
  for (fd = g_fds.f; fd < g_fds.n; ++fd) {
    if (!g_fds.p[fd].kind) {
      break;
    }
  }
  fd = __ensurefds_unlocked(fd);
  bzero(g_fds.p + fd, sizeof(*g_fds.p));
  g_fds.p[fd].kind = kFdReserved;
  return fd;
}

/**
 * Finds open file descriptor slot.
 */
int __reservefd(int start) {
  int fd;
  _spinlock(&__fds_lock);
  fd = __reservefd_unlocked(start);
  _spunlock(&__fds_lock);
  return fd;
}

/**
 * Closes non-stdio file descriptors to free dynamic memory.
 */
static void FreeFds(void) {
  int i;
  NTTRACE("FreeFds()");
  _spinlock(&__fds_lock);
  for (i = 3; i < g_fds.n; ++i) {
    if (g_fds.p[i].kind) {
      _spunlock(&__fds_lock);
      close(i);
      _spinlock(&__fds_lock);
    }
  }
  if (g_fds.p != g_fds.__init_p) {
    memcpy(g_fds.__init_p, g_fds.p, sizeof(*g_fds.p) * 3);
    weaken(free)(g_fds.p);
    g_fds.p = g_fds.__init_p;
    g_fds.n = ARRAYLEN(g_fds.__init_p);
  }
  _spunlock(&__fds_lock);
}

static textstartup void FreeFdsInit(void) {
  atexit(FreeFds);
}

const void *const FreeFdsCtor[] initarray = {
    FreeFdsInit,
};
