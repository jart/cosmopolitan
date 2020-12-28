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
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/errfuns.h"

#define IP(X)        (intptr_t)(X)
#define ALIGNED(p)   (!(IP(p) & (FRAMESIZE - 1)))
#define CANONICAL(p) (-0x800000000000 <= IP(p) && IP(p) <= 0x7fffffffffff)

/**
 * Releases memory pages.
 *
 * @param addr is a pointer within any memory mapped region the process
 *     has permission to control, such as address ranges returned by
 *     mmap(), the program image itself, etc.
 * @param size is the amount of memory to unmap, which needn't be a
 *     multiple of FRAMESIZE, and may be a subset of that which was
 *     mapped previously, and may punch holes in existing mappings,
 *     but your mileage may vary on windows
 * @return 0 on success, or -1 w/ errno
 */
int munmap(void *addr, size_t size) {
  int rc;
  if (!ALIGNED(addr) || !CANONICAL(addr) || !size) return einval();
  size = ROUNDUP(size, FRAMESIZE);
  if (UntrackMemoryIntervals(addr, size) == -1) return -1;
  if (IsWindows()) return 0;
  return munmap$sysv(addr, size);
}
