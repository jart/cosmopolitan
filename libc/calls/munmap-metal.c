/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/directmap.internal.h"
#include "libc/runtime/pc.internal.h"
#ifdef __x86_64__

int sys_munmap_metal(void *addr, size_t size) {
  size_t i;
  uint64_t *e, paddr;
  struct mman *mm = __get_mm();
  uint64_t *pml4t = __get_pml4t();
  for (i = 0; i < size; i += 4096) {
    e = __get_virtual(mm, pml4t, (uint64_t)addr + i, false);
    if (e) {
      paddr = *e & PAGE_TA;
      *e &= ~(PAGE_V | PAGE_RSRV);
      invlpg((uint64_t)addr + i);
      __unref_page(mm, pml4t, paddr);
    }
  }
  return 0;
}

#endif
