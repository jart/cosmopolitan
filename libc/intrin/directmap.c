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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/memtrack.internal.h"

/**
 * Obtains memory mapping directly from system.
 *
 * The mmap() function needs to track memory mappings in order to
 * support Windows NT and Address Sanitizer. That memory tracking can be
 * bypassed by calling this function. However the caller is responsible
 * for passing the magic memory handle on Windows NT to CloseHandle().
 *
 * @asyncsignalsafe
 * @threadsafe
 */
struct DirectMap sys_mmap(void *addr, size_t size, int prot, int flags, int fd,
                          int64_t off) {
#ifdef __x86_64__
  struct DirectMap d;
  if (!IsWindows() && !IsMetal()) {
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
#elif defined(__aarch64__)
  register long r0 asm("x0") = (long)addr;
  register long r1 asm("x1") = (long)size;
  register long r2 asm("x2") = (long)prot;
  register long r3 asm("x3") = (long)flags;
  register long r4 asm("x4") = (long)fd;
  register long r5 asm("x5") = (long)off;
  register long res_x0 asm("x0");
  long res;
  asm volatile("mov\tx8,%1\n\t"
               "mov\tx16,%2\n\t"
               "svc\t0"
               : "=r"(res_x0)
               : "i"(222), "i"(197), "r"(r0), "r"(r1), "r"(r2), "r"(r3),
                 "r"(r4), "r"(r5)
               : "x8", "memory");
  res = res_x0;
  if ((unsigned long)res >= (unsigned long)-4095) {
    errno = (int)-res;
    res = -1;
  }
  return (struct DirectMap){(void *)res, kNtInvalidHandleValue};
#else
#error "arch unsupported"
#endif
}
