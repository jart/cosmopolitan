/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
  m->mode = XED_MACHINE_MODE_LONG_64;
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
  m->real.i = 0;
  m->cr3 = AllocateLinearPage(m);
}

long AllocateLinearPage(struct Machine *m) {
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
      } else {
        return -1;
      }
    }
    DCHECK_EQ(0, i & 0xfff);
    DCHECK_EQ(0, n & 0xfff);
    DCHECK_LE(i + 0x1000, n);
    m->real.i += 0x1000;
  }
  memset(m->real.p + i, 0, 0x1000); /* TODO: lazy page clearing */
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

int ReserveVirtual(struct Machine *m, int64_t virt, size_t size) {
  int64_t level, pt, ti, mi, end;
  for (end = virt + size; virt < end; virt += 0x1000) {
    for (pt = m->cr3, level = 39; level >= 12; level -= 9) {
      pt = pt & 0x00007ffffffff000;
      ti = (virt >> level) & 511;
      DEBUGF("reserve %p level %d table %p index %ld", virt, level, pt, ti);
      mi = pt + ti * 8;
      pt = MachineRead64(m, mi);
      if (!(pt & 1)) {
        if ((pt = AllocateLinearPage(m)) == -1) return -1;
        MachineWrite64(m, mi, pt | 7);
      }
    }
  }
  return 0;
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

int FreeVirtual(struct Machine *m, int64_t base, size_t size) {
  struct MachineRealFree *rf;
  uint64_t i, mi, pt, la, end, virt;
  for (virt = base, end = virt + size; virt < end;) {
    for (pt = m->cr3, i = 39;; i -= 9) {
      mi = (pt & 0x7ffffffff000) + ((virt >> i) & 511) * 8;
      pt = MachineRead64(m, mi);
      if (!(pt & 1)) {
        break;
      } else if (i == 12) {
        MachineWrite64(m, mi, 0);
        la = pt & 0x7ffffffff000;
        if (m->realfree && la == m->realfree->i + m->realfree->n) {
          m->realfree->n += 0x1000;
        } else if ((rf = malloc(sizeof(struct MachineRealFree)))) {
          rf->i = la;
          rf->n = 0x1000;
          rf->next = m->realfree;
          m->realfree = rf;
        }
        break;
      }
    }
    virt += 1ull << i;
  }
  return 0;
}
