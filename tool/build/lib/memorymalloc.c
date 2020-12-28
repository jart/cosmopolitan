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
#include "libc/calls/calls.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "libc/x/x.h"
#include "tool/build/lib/buffer.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/pml4t.h"

struct Machine *NewMachine(void) {
  struct Machine *m;
  m = xmemalignzero(alignof(struct Machine), sizeof(struct Machine));
  ResetCpu(m);
  ResetMem(m);
  return m;
}

static void FreeMachineRealFree(struct Machine *m) {
  struct MachineRealFree *rf;
  while ((rf = m->realfree)) {
    m->realfree = rf->next;
    free(rf);
  }
}

void FreeMachine(struct Machine *m) {
  if (m) {
    FreeMachineRealFree(m);
    free(m->real.p);
    free(m);
  }
}

void ResetMem(struct Machine *m) {
  FreeMachineRealFree(m);
  ResetTlb(m);
  memset(&m->memstat, 0, sizeof(m->memstat));
  m->real.i = 0;
  m->cr3 = 0;
}

long AllocateLinearPage(struct Machine *m) {
  long page;
  if ((page = AllocateLinearPageRaw(m)) != -1) {
    memset(m->real.p + page, 0, 0x1000);
  }
  return page;
}

long AllocateLinearPageRaw(struct Machine *m) {
  uint8_t *p;
  size_t i, n;
  struct MachineRealFree *rf;
  if ((rf = m->realfree)) {
    DCHECK(rf->n);
    DCHECK_EQ(0, rf->i & 0xfff);
    DCHECK_EQ(0, rf->n & 0xfff);
    DCHECK_LE(rf->i + rf->n, m->real.i);
    i = rf->i;
    rf->i += 0x1000;
    if (!(rf->n -= 0x1000)) {
      m->realfree = rf->next;
      free(rf);
    }
    --m->memstat.freed;
    ++m->memstat.reclaimed;
  } else {
    i = m->real.i;
    n = m->real.n;
    p = m->real.p;
    if (i == n) {
      if (n) {
        n += n >> 1;
      } else {
        n = 0x10000;
      }
      n = ROUNDUP(n, 0x1000);
      if ((p = realloc(p, n))) {
        m->real.p = p;
        m->real.n = n;
        ResetTlb(m);
        ++m->memstat.resizes;
      } else {
        return -1;
      }
    }
    DCHECK_EQ(0, i & 0xfff);
    DCHECK_EQ(0, n & 0xfff);
    DCHECK_LE(i + 0x1000, n);
    m->real.i += 0x1000;
    ++m->memstat.allocated;
  }
  ++m->memstat.committed;
  return i;
}

static uint64_t MachineRead64(struct Machine *m, unsigned long i) {
  CHECK_LE(i + 8, m->real.n);
  return Read64(m->real.p + i);
}

static void MachineWrite64(struct Machine *m, unsigned long i, uint64_t x) {
  CHECK_LE(i + 8, m->real.n);
  Write64(m->real.p + i, x);
}

int ReserveReal(struct Machine *m, size_t n) {
  uint8_t *p;
  DCHECK_EQ(0, n & 0xfff);
  if (m->real.n < n) {
    if ((p = realloc(m->real.p, n))) {
      m->real.p = p;
      m->real.n = n;
      ResetTlb(m);
      ++m->memstat.resizes;
    } else {
      return -1;
    }
  }
  return 0;
}

int ReserveVirtual(struct Machine *m, int64_t virt, size_t size, uint64_t key) {
  int64_t ti, mi, pt, end, level;
  for (end = virt + size;;) {
    for (pt = m->cr3, level = 39; level >= 12; level -= 9) {
      pt = pt & 0x7ffffffff000;
      ti = (virt >> level) & 511;
      mi = (pt & 0x7ffffffff000) + ti * 8;
      pt = MachineRead64(m, mi);
      if (level > 12) {
        if (!(pt & 1)) {
          if ((pt = AllocateLinearPage(m)) == -1) return -1;
          MachineWrite64(m, mi, pt | 7);
          ++m->memstat.pagetables;
        }
        continue;
      }
      for (;;) {
        if (!(pt & 1)) {
          MachineWrite64(m, mi, key);
          ++m->memstat.reserved;
        }
        if ((virt += 0x1000) >= end) return 0;
        if (++ti == 512) break;
        pt = MachineRead64(m, (mi += 8));
      }
    }
  }
}

int64_t FindVirtual(struct Machine *m, int64_t virt, size_t size) {
  uint64_t i, pt, got;
  got = 0;
  do {
    if (virt >= 0x800000000000) return enomem();
    for (pt = m->cr3, i = 39; i >= 12; i -= 9) {
      pt = MachineRead64(m, (pt & 0x7ffffffff000) + ((virt >> i) & 511) * 8);
      if (!(pt & 1)) break;
    }
    if (i >= 12) {
      got += 1ull << i;
    } else {
      virt += 0x1000;
      got = 0;
    }
  } while (got < size);
  return virt;
}

static void AppendRealFree(struct Machine *m, uint64_t real) {
  struct MachineRealFree *rf;
  if (m->realfree && real == m->realfree->i + m->realfree->n) {
    m->realfree->n += 0x1000;
  } else if ((rf = malloc(sizeof(struct MachineRealFree)))) {
    rf->i = real;
    rf->n = 0x1000;
    rf->next = m->realfree;
    m->realfree = rf;
  }
}

int FreeVirtual(struct Machine *m, int64_t base, size_t size) {
  uint64_t i, mi, pt, end, virt;
  for (virt = base, end = virt + size; virt < end; virt += 1ull << i) {
    for (pt = m->cr3, i = 39;; i -= 9) {
      mi = (pt & 0x7ffffffff000) + ((virt >> i) & 511) * 8;
      pt = MachineRead64(m, mi);
      if (!(pt & 1)) {
        break;
      } else if (i == 12) {
        ++m->memstat.freed;
        if (pt & 0x0e00) {
          --m->memstat.reserved;
        } else {
          --m->memstat.committed;
          AppendRealFree(m, pt & 0x7ffffffff000);
        }
        MachineWrite64(m, mi, 0);
        break;
      }
    }
  }
  ResetTlb(m);
  return 0;
}
