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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/syslib.internal.h"

/**
 * Obtains memory mapping directly from system.
 *
 * The mmap() function needs to track memory mappings in order to
 * support Windows NT and Address Sanitizer. That memory tracking can be
 * bypassed by calling this function. However the caller is responsible
 * for passing the magic memory handle on Windows NT to CloseHandle().
 *
 * @asyncsignalsafe
 */
struct DirectMap sys_mmap(void *addr, size_t size, int prot, int flags, int fd,
                          int64_t off) {
  struct DirectMap d;
  if (IsXnuSilicon()) {
    long p = _sysret(__syslib->__mmap(addr, size, prot, flags, fd, off));
    d.maphandle = kNtInvalidHandleValue;
    d.addr = (void *)p;
  } else if (!IsWindows() && !IsMetal()) {
    d.addr = __sys_mmap(addr, size, prot, flags, fd, off, off);
    d.maphandle = kNtInvalidHandleValue;
  } else if (IsMetal()) {
    d = sys_mmap_metal(addr, size, prot, flags, fd, off);
  } else {
    d = sys_mmap_nt(addr, size, prot, flags, fd, off);
  }
  KERNTRACE("sys_mmap(%.12p /* %s */, %'zu, %s, %s, %d, %'ld) → {%.12p, %p}% m",
            addr, DescribeFrame((intptr_t)addr >> 16), size,
            DescribeProtFlags(prot), DescribeMapFlags(flags), fd, off, d.addr,
            d.maphandle);
  return d;
}
