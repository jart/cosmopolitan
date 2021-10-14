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
#include "libc/calls/sysdebug.internal.h"
#include "libc/errno.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/str/str.h"

/**
 * Obtains memory mapping directly from system.
 *
 * The mmap() function needs to track memory mappings in order to
 * support Windows NT and Address Sanitizer. That memory tracking can be
 * bypassed by calling this function. However the caller is responsible
 * for passing the magic memory handle on Windows NT to CloseHandle().
 */
noasan struct DirectMap sys_mmap(void *addr, size_t size, int prot, int flags,
                                 int fd, int64_t off) {
  /* asan runtime depends on this function */
  char mode[8];
  struct DirectMap dm;
  if (!IsWindows() && !IsMetal()) {
    dm.addr = __sys_mmap(addr, size, prot, flags, fd, off, off);
    SYSDEBUG("sys_mmap(0x%p%s, 0x%x, %s, %d, %d) -> 0x%p %s", addr,
             DescribeFrame((intptr_t)addr >> 16), size,
             DescribeMapping(prot, flags, mode), (long)fd, off, dm.addr,
             dm.addr != MAP_FAILED ? "" : strerror(errno));
    dm.maphandle = kNtInvalidHandleValue;
    return dm;
  } else if (IsMetal()) {
    return sys_mmap_metal(addr, size, prot, flags, fd, off);
  } else {
    return sys_mmap_nt(addr, size, prot, flags,
                       fd != -1 ? g_fds.p[fd].handle : kNtInvalidHandleValue,
                       off);
  }
}
