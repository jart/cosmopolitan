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
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/pml4t.h"
#include "tool/build/lib/stats.h"
#include "tool/build/lib/throw.h"

void SetReadAddr(struct Machine *m, int64_t addr, uint32_t size) {
  if (size) {
    m->readaddr = addr;
    m->readsize = size;
  }
}

void SetWriteAddr(struct Machine *m, int64_t addr, uint32_t size) {
  if (size) {
    m->writeaddr = addr;
    m->writesize = size;
  }
}

long HandlePageFault(struct Machine *m, uint64_t entry, uint64_t table,
                     unsigned index) {
  long page;
  if ((page = AllocateLinearPage(m)) != -1) {
    --m->memstat.reserved;
    *(uint64_t *)(m->real.p + table + index * 8) =
        page | entry & ~0x7ffffffffe00;
  }
  return page;
}

void *FindReal(struct Machine *m, int64_t virt) {
  long page;
  uint64_t table, entry;
  unsigned skew, level, index, i;
  if ((m->mode & 3) != XED_MODE_REAL) {
    if (-0x800000000000 <= virt && virt < 0x800000000000) {
      skew = virt & 0xfff;
      virt &= -0x1000;
      for (i = 0; i < ARRAYLEN(m->tlb); ++i) {
        if (m->tlb[i].virt == virt && m->tlb[i].host) {
          return m->tlb[i].host + skew;
        }
      }
      level = 39;
      entry = m->cr3;
      do {
        table = entry & 0x7ffffffff000;
        CHECK_LT(table, m->real.n);
        index = (virt >> level) & 511;
        entry = *(uint64_t *)(m->real.p + table + index * 8);
        if (!(entry & 1)) return NULL;
      } while ((level -= 9) >= 12);
      if (!(entry & 0x0e00)) {
        page = entry & 0x7ffffffff000;
        CHECK_LT(page, m->real.n);
      } else if ((page = HandlePageFault(m, entry, table, index)) == -1) {
        return NULL;
      }
      m->tlbindex = (m->tlbindex + 1) & (ARRAYLEN(m->tlb) - 1);
      m->tlb[m->tlbindex] = m->tlb[0];
      m->tlb[0].virt = virt;
      m->tlb[0].host = m->real.p + page;
      return m->real.p + page + skew;
    } else {
      return NULL;
    }
  } else if (0 <= virt && virt + 0xfff < m->real.n) {
    return m->real.p + virt;
  } else {
    return NULL;
  }
}

void *ResolveAddress(struct Machine *m, int64_t v) {
  void *r;
  if ((r = FindReal(m, v))) return r;
  ThrowSegmentationFault(m, v);
}

void VirtualSet(struct Machine *m, int64_t v, char c, uint64_t n) {
  char *p;
  uint64_t k;
  k = 0x1000 - (v & 0xfff);
  while (n) {
    k = MIN(k, n);
    p = ResolveAddress(m, v);
    memset(p, c, k);
    n -= k;
    v += k;
    k = 0x1000;
  }
}

void VirtualCopy(struct Machine *m, int64_t v, char *r, uint64_t n, bool d) {
  char *p;
  uint64_t k;
  k = 0x1000 - (v & 0xfff);
  while (n) {
    k = MIN(k, n);
    p = ResolveAddress(m, v);
    if (d) {
      memcpy(r, p, k);
    } else {
      memcpy(p, r, k);
    }
    n -= k;
    r += k;
    v += k;
    k = 0x1000;
  }
}

void *VirtualSend(struct Machine *m, void *dst, int64_t src, uint64_t n) {
  VirtualCopy(m, src, dst, n, true);
  return dst;
}

void VirtualSendRead(struct Machine *m, void *dst, int64_t addr, uint64_t n) {
  VirtualSend(m, dst, addr, n);
  SetReadAddr(m, addr, n);
}

void VirtualRecv(struct Machine *m, int64_t dst, void *src, uint64_t n) {
  VirtualCopy(m, dst, src, n, false);
}

void VirtualRecvWrite(struct Machine *m, int64_t addr, void *src, uint64_t n) {
  VirtualRecv(m, addr, src, n);
  SetWriteAddr(m, addr, n);
}

void *ReserveAddress(struct Machine *m, int64_t v, size_t n) {
  void *r;
  DCHECK_LE(n, sizeof(m->stash));
  if ((v & 0xfff) + n <= 0x1000) return ResolveAddress(m, v);
  m->stashaddr = v;
  m->stashsize = n;
  r = m->stash;
  VirtualSend(m, r, v, n);
  return r;
}

void *AccessRam(struct Machine *m, int64_t v, size_t n, void *p[2],
                uint8_t *tmp, bool copy) {
  unsigned k;
  uint8_t *a, *b;
  DCHECK_LE(n, 0x1000);
  if ((v & 0xfff) + n <= 0x1000) return ResolveAddress(m, v);
  k = 0x1000;
  k -= v & 0xfff;
  DCHECK_LE(k, 0x1000);
  a = ResolveAddress(m, v);
  b = ResolveAddress(m, v + k);
  if (copy) {
    memcpy(tmp, a, k);
    memcpy(tmp + k, b, n - k);
  }
  p[0] = a;
  p[1] = b;
  return tmp;
}

void *Load(struct Machine *m, int64_t v, size_t n, uint8_t *b) {
  void *p[2];
  SetReadAddr(m, v, n);
  return AccessRam(m, v, n, p, b, true);
}

void *BeginStore(struct Machine *m, int64_t v, size_t n, void *p[2],
                 uint8_t *b) {
  SetWriteAddr(m, v, n);
  return AccessRam(m, v, n, p, b, false);
}

void *BeginStoreNp(struct Machine *m, int64_t v, size_t n, void *p[2],
                   uint8_t *b) {
  if (!v) return NULL;
  return BeginStore(m, v, n, p, b);
}

void *BeginLoadStore(struct Machine *m, int64_t v, size_t n, void *p[2],
                     uint8_t *b) {
  SetWriteAddr(m, v, n);
  return AccessRam(m, v, n, p, b, true);
}

void EndStore(struct Machine *m, int64_t v, size_t n, void *p[2], uint8_t *b) {
  uint8_t *a;
  unsigned k;
  DCHECK_LE(n, 0x1000);
  if ((v & 0xfff) + n <= 0x1000) return;
  k = 0x1000;
  k -= v & 0xfff;
  DCHECK_GT(k, n);
  DCHECK_NOTNULL(p[0]);
  DCHECK_NOTNULL(p[1]);
  memcpy(p[0], b, k);
  memcpy(p[1], b + k, n - k);
}

void EndStoreNp(struct Machine *m, int64_t v, size_t n, void *p[2],
                uint8_t *b) {
  if (v) EndStore(m, v, n, p, b);
}

void *LoadStr(struct Machine *m, int64_t addr) {
  size_t have;
  char *copy, *page, *p;
  have = 0x1000 - (addr & 0xfff);
  if (!addr) return NULL;
  if (!(page = FindReal(m, addr))) return NULL;
  if ((p = memchr(page, '\0', have))) {
    SetReadAddr(m, addr, p - page + 1);
    return page;
  }
  CHECK_LT(m->freelist.i, ARRAYLEN(m->freelist.p));
  if (!(copy = malloc(have + 0x1000))) return NULL;
  memcpy(copy, page, have);
  for (;;) {
    if (!(page = FindReal(m, addr + have))) break;
    if ((p = memccpy(copy + have, page, '\0', 0x1000))) {
      SetReadAddr(m, addr, have + (p - (copy + have)) + 1);
      return (m->freelist.p[m->freelist.i++] = copy);
    }
    have += 0x1000;
    if (!(p = realloc(copy, have + 0x1000))) break;
    copy = p;
  }
  free(copy);
  return NULL;
}

void *LoadBuf(struct Machine *m, int64_t addr, size_t size) {
  size_t have, need;
  char *buf, *copy, *page;
  have = 0x1000 - (addr & 0xfff);
  if (!addr) return NULL;
  if (!(buf = FindReal(m, addr))) return NULL;
  if (size > have) {
    CHECK_LT(m->freelist.i, ARRAYLEN(m->freelist.p));
    if (!(copy = malloc(size))) return NULL;
    buf = memcpy(copy, buf, have);
    do {
      need = MIN(0x1000, size - have);
      if ((page = FindReal(m, addr + have))) {
        memcpy(copy + have, page, need);
        have += need;
      } else {
        free(copy);
        return NULL;
      }
    } while (have < size);
  }
  SetReadAddr(m, addr, size);
  return buf;
}
