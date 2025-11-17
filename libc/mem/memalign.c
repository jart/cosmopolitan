/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/dlmalloc/dlmalloc.h"

__static_yoink("free");

/**
 * Allocates aligned memory.
 *
 * Returns a pointer to a newly allocated chunk of n bytes, aligned in
 * accord with the alignment argument. The alignment argument shall be
 * rounded up to the nearest two power and higher 2 powers may be used
 * if the allocator imposes a minimum alignment requirement.
 *
 * @param align is alignment in bytes, coerced to 1+ w/ 2-power roundup
 * @param bytes is number of bytes needed, coerced to 1+
 * @return rax is memory address, or NULL w/ errno
 * @see valloc(), pvalloc()
 */
void *memalign(size_t align, size_t bytes) {
#ifdef COSMO_MEM_DEBUG
  static_assert(sizeof(size_t) == sizeof(uintptr_t));
  if (!bytes)
    ++bytes;
  if (align < 16)
    align = 16;
  while (!IS2POW(align))
    ++align;
  void *page = mmap(0, align + bytes, PROT_READ | PROT_WRITE,
                    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (page == MAP_FAILED)
    return 0;
  size_t *res =
      (size_t *)(((((uintptr_t)page + align + bytes + __pagesize - 1) &
                   -__pagesize) -
                  bytes) &
                 -align);
  res[-1] = -bytes;
  res[-2] = -(uintptr_t)page;
  return res;
#elifdef MODE_DBG
  char *p = dlmemalign(align, bytes);
  if (p)
    memset(p, 0xa8, dlmalloc_usable_size(p));
  return p;
#else
  return dlmemalign(align, bytes);
#endif
}
