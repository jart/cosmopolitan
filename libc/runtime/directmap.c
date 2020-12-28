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
#include "libc/nt/runtime.h"
#include "libc/runtime/directmap.h"

/**
 * Obtains memory mapping directly from system.
 *
 * The mmap() function needs to track memory mappings in order to
 * support Windows NT and Address Sanitizer. That memory tracking can be
 * bypassed by calling this function. However the caller is responsible
 * for passing the magic memory handle on Windows NT to CloseHandle().
 */
struct DirectMap __mmap(void *addr, size_t size, unsigned prot, unsigned flags,
                        int fd, int64_t off) {
  if (!IsWindows()) {
    return (struct DirectMap){mmap$sysv(addr, size, prot, flags, fd, off),
                              kNtInvalidHandleValue};
  } else {
    return __mmap$nt(addr, size, prot, flags, fd, off);
  }
}
