/*-*- mode:c;indent-tabs-mode:nil;c-bansic-offset:2;tab-width:8;coding:utf-8
-*-│ │vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8 :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "tool/build/lib/buffer.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/pml4t.h"

struct Pml4tFormater {
  bool t;
  int64_t start;
  struct Buffer b;
  long lines;
};

static int64_t MakeAddress(unsigned short a[4]) {
  uint64_t x;
  x = 0;
  x |= a[0];
  x <<= 9;
  x |= a[1];
  x <<= 9;
  x |= a[2];
  x <<= 9;
  x |= a[3];
  x <<= 12;
  return x;
}

static void FormatStartPage(struct Pml4tFormater *pp, int64_t start) {
  pp->t = true;
  pp->start = start;
  if (pp->lines++) AppendChar(&pp->b, '\n');
  AppendFmt(&pp->b, "%p-", start);
}

static void FormatEndPage(struct Pml4tFormater *pp, int64_t end) {
  int64_t size;
  pp->t = false;
  size = end - pp->start;
  AppendFmt(&pp->b, "%p %p %,ld bytes", end - 1, size, size);
}

char *FormatPml4t(uint64_t pml4t[512]) {
  uint64_t *pd[4];
  unsigned short i, a[4];
  struct Pml4tFormater pp = {0};
  unsigned short range[][2] = {{256, 512}, {0, 256}};
  pd[0] = pml4t;
  for (i = 0; i < ARRAYLEN(range); ++i) {
    a[0] = range[i][0];
    do {
      a[1] = a[2] = a[3] = 0;
      if (!IsValidPage(pd[0][a[0]])) {
        if (pp.t) FormatEndPage(&pp, MakeAddress(a));
      } else {
        pd[1] = UnmaskPageAddr(pd[0][a[0]]);
        do {
          a[2] = a[3] = 0;
          if (!IsValidPage(pd[1][a[1]])) {
            if (pp.t) FormatEndPage(&pp, MakeAddress(a));
          } else {
            pd[2] = UnmaskPageAddr(pd[1][a[1]]);
            do {
              a[3] = 0;
              if (!IsValidPage(pd[2][a[2]])) {
                if (pp.t) FormatEndPage(&pp, MakeAddress(a));
              } else {
                pd[3] = UnmaskPageAddr(pd[2][a[2]]);
                do {
                  if (!IsValidPage(pd[3][a[3]])) {
                    if (pp.t) FormatEndPage(&pp, MakeAddress(a));
                  } else {
                    if (!pp.t) {
                      FormatStartPage(&pp, MakeAddress(a));
                    }
                  }
                } while (++a[3] != 512);
              }
            } while (++a[2] != 512);
          }
        } while (++a[1] != 512);
      }
    } while (++a[0] != range[i][1]);
  }
  if (pp.t) {
    FormatEndPage(&pp, 0x800000000000);
  }
  if (pp.b.p) {
    realloc(pp.b.p, pp.b.i + 1);
    return pp.b.p;
  } else {
    return strdup("");
  }
}
