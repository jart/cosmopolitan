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
#include "libc/alg/arraylist2.internal.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/pml4t.h"

static void AppendContiguousMemoryRange(struct ContiguousMemoryRanges *ranges,
                                        int64_t a, int64_t b) {
  APPEND(&ranges->p, &ranges->i, &ranges->n,
         (&(struct ContiguousMemoryRange){a, b}));
}

static void FindContiguousMemoryRangesImpl(
    struct Machine *m, struct ContiguousMemoryRanges *ranges, int64_t addr,
    unsigned level, int64_t pt, int64_t a, int64_t b) {
  int64_t i, page, entry;
  for (i = a; i < b; ++i) {
    entry = Read64(m->real.p + pt + i * 8);
    if (!(entry & 1)) continue;
    entry &= 0x7ffffffff000;
    page = (addr | i << level) << 16 >> 16;
    if (level == 12) {
      if (ranges->i && page == ranges->p[ranges->i - 1].b) {
        ranges->p[ranges->i - 1].b += 0x1000;
      } else {
        AppendContiguousMemoryRange(ranges, page, page + 0x1000);
      }
    } else if (entry + 512 * 8 <= m->real.n) {
      FindContiguousMemoryRangesImpl(m, ranges, page, level - 9, entry, 0, 512);
    }
  }
}

void FindContiguousMemoryRanges(struct Machine *m,
                                struct ContiguousMemoryRanges *ranges) {
  uint64_t cr3;
  ranges->i = 0;
  if ((m->mode & 3) == XED_MODE_LONG) {
    cr3 = m->cr3 & 0x7ffffffff000;
    FindContiguousMemoryRangesImpl(m, ranges, 0, 39, cr3, 256, 512);
    FindContiguousMemoryRangesImpl(m, ranges, 0, 39, cr3, 0, 256);
  } else {
    AppendContiguousMemoryRange(ranges, 0, m->real.n);
  }
}
