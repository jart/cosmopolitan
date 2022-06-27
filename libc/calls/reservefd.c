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
#include "libc/calls/state.internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/spinlock.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

static volatile size_t mapsize;

/**
 * Grows file descriptor array memory if needed.
 *
 * @see libc/runtime/memtrack64.txt
 * @see libc/runtime/memtrack32.txt
 * @asyncsignalsafe
 */
int __ensurefds_unlocked(int fd) {
  uint64_t addr;
  int prot, flags;
  size_t size, chunk;
  struct DirectMap dm;
  if (fd < g_fds.n) return fd;
  STRACE("__ensurefds(%d) extending", fd);
  size = mapsize;
  chunk = FRAMESIZE;
  if (IsAsan()) chunk *= 8;
  addr = kMemtrackFdsStart + size;
  prot = PROT_READ | PROT_WRITE;
  flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED;
  dm = sys_mmap((char *)addr, chunk, prot, flags, -1, 0);
  TrackMemoryInterval(&_mmi, addr >> 16, (addr + chunk - 1) >> 16, dm.maphandle,
                      prot, flags, false, false, 0, chunk);
  if (IsAsan()) {
    addr = (addr >> 3) + 0x7fff8000;
    dm = sys_mmap((char *)addr, FRAMESIZE, prot, flags, -1, 0);
    TrackMemoryInterval(&_mmi, addr >> 16, addr >> 16, dm.maphandle, prot,
                        flags, false, false, 0, FRAMESIZE);
  }
  if (!size) {
    g_fds.p = memcpy((char *)kMemtrackFdsStart, g_fds.__init_p,
                     sizeof(g_fds.__init_p));
  }
  g_fds.n = (size + chunk) / sizeof(*g_fds.p);
  mapsize = size + chunk;
  return fd;
}

/**
 * Grows file descriptor array memory if needed.
 * @asyncsignalsafe
 * @threadsafe
 */
int __ensurefds(int fd) {
  __fds_lock();
  fd = __ensurefds_unlocked(fd);
  __fds_unlock();
  return fd;
}

/**
 * Finds open file descriptor slot.
 * @asyncsignalsafe
 */
int __reservefd_unlocked(int start) {
  int fd;
  for (;;) {
    for (fd = MAX(start, g_fds.f); fd < g_fds.n; ++fd) {
      if (!g_fds.p[fd].kind) {
        break;
      }
    }
    fd = __ensurefds_unlocked(fd);
    bzero(g_fds.p + fd, sizeof(*g_fds.p));
    if (_cmpxchg(&g_fds.p[fd].kind, kFdEmpty, kFdReserved)) {
      _cmpxchg(&g_fds.f, fd, fd + 1);
      return fd;
    }
  }
}

/**
 * Finds open file descriptor slot.
 * @asyncsignalsafe
 * @threadsafe
 */
int __reservefd(int start) {
  int fd;
  __fds_lock();
  fd = __reservefd_unlocked(start);
  __fds_unlock();
  return fd;
}
