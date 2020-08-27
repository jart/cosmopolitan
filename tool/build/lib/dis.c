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
#include "libc/alg/alg.h"
#include "libc/alg/arraylist2.h"
#include "libc/bits/safemacros.h"
#include "libc/conv/itoa.h"
#include "libc/fmt/bing.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/str/tpencode.h"
#include "third_party/xed/x86.h"
#include "tool/build/lib/case.h"
#include "tool/build/lib/dis.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"

#define ADDRLEN 8
#define BYTELEN 11
#define PFIXLEN 4
#define NAMELEN 8
#define CODELEN 40
#define CODELIM 15
#define DATALIM 8
#define PIVOTOP pos_opcode

static char *DisColumn(char *p2, char *p1, long need) {
  char *p;
  unsigned long have;
  DCHECK_GE(p2, p1);
  have = p2 - p1;
  p = p2;
  do {
    *p++ = ' ';
  } while (++have < need);
  *p = '\0';
  return p;
}

static char *DisOctets(char *p, const uint8_t *d, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    if (i) *p++ = ',';
    *p++ = '0';
    *p++ = 'x';
    *p++ = "0123456789abcdef"[(d[i] & 0xf0) >> 4];
    *p++ = "0123456789abcdef"[(d[i] & 0x0f) >> 0];
  }
  *p = '\0';
  return p;
}

static char *DisByte(char *p, const uint8_t *d, size_t n) {
  if (g_dis_high) p = DisHigh(p, g_dis_high->keyword);
  p = DisColumn(stpcpy(p, ".byte"), p, NAMELEN);
  if (g_dis_high) p = DisHigh(p, -1);
  p = DisOctets(p, d, n);
  return p;
}

static char *DisError(struct DisBuilder b, char *p) {
  p = DisColumn(DisByte(p, b.xedd->bytes, MIN(15, b.xedd->length)), p, CODELEN);
  if (g_dis_high) p = DisHigh(p, g_dis_high->comment);
  *p++ = '#';
  *p++ = ' ';
  p = stpcpy(p, indexdoublenulstring(kXedErrorNames, b.xedd->op.error));
  if (g_dis_high) p = DisHigh(p, -1);
  *p = '\0';
  return p;
}

static char *DisAddr(struct DisBuilder b, char *p) {
  if (INT_MIN <= b.addr && b.addr <= INT_MAX) {
    return p + uint64toarray_fixed16(b.addr, p, 32);
  } else {
    return p + uint64toarray_fixed16(b.addr, p, 48);
  }
}

static char *DisRaw(struct DisBuilder b, char *p) {
  long i;
  for (i = 0; i < PFIXLEN - MIN(PFIXLEN, b.xedd->op.PIVOTOP); ++i) {
    *p++ = ' ';
    *p++ = ' ';
  }
  for (i = 0; i < MIN(15, b.xedd->length); ++i) {
    if (i == b.xedd->op.PIVOTOP) *p++ = ' ';
    *p++ = "0123456789abcdef"[(b.xedd->bytes[i] & 0xf0) >> 4];
    *p++ = "0123456789abcdef"[b.xedd->bytes[i] & 0x0f];
  }
  *p = '\0';
  return p;
}

static char *DisCode(struct DisBuilder b, char *p) {
  char optspecbuf[128];
  if (!b.xedd->op.error) {
    return DisInst(b, p, DisSpec(b.xedd, optspecbuf));
  } else {
    return DisError(b, p);
  }
}

static char *DisLineCode(struct DisBuilder b, char *p) {
  p = DisColumn(DisAddr(b, p), p, ADDRLEN);
  p = DisColumn(DisRaw(b, p), p, PFIXLEN * 2 + 1 + BYTELEN * 2);
  p = DisCode(b, p);
  return p;
}

static char *DisLineData(struct DisBuilder b, char *p, const uint8_t *d,
                         size_t n) {
  size_t i;
  p = DisColumn(DisAddr(b, p), p, ADDRLEN);
  p = DisColumn(DisByte(p, d, n), p, 64);
  if (g_dis_high) p = DisHigh(p, g_dis_high->comment);
  *p++ = '#';
  *p++ = ' ';
  for (i = 0; i < n; ++i) p += tpencode(p, 8, bing(d[i], 0), false);
  if (g_dis_high) p = DisHigh(p, -1);
  *p = '\0';
  return p;
}

static char *DisLabel(struct DisBuilder b, char *p, const char *name) {
  p = DisColumn(DisAddr(b, p), p, ADDRLEN);
  if (g_dis_high) p = DisHigh(p, g_dis_high->label);
  p = stpcpy(p, name);
  if (g_dis_high) p = DisHigh(p, -1);
  *p++ = ':';
  *p = '\0';
  return p;
}

long DisFind(struct Dis *d, int64_t addr) {
  long i;
  for (i = 0; i < d->ops.i; ++i) {
    if (addr >= d->ops.p[i].addr &&
        addr < d->ops.p[i].addr + d->ops.p[i].size) {
      return i;
    }
  }
  return -1;
}

void Dis(struct Dis *d, struct Machine *m, int64_t addr) {
  char *p;
  void *r[2];
  bool iscode;
  int64_t unique;
  struct DisOp op;
  long i, j, n, si, max, toto, symbol;
  unique = 0;
  max = 99999;
  DisFreeOps(&d->ops);
  for (i = 0; i < max; ++i) {
    xed_decoded_inst_zero_set_mode(d->xedd, XED_MACHINE_MODE_LONG_64);
    if ((symbol = DisFindSym(d, addr)) != -1) {
      iscode = true; /* d->syms.p[symbol].iscode; */
      n = iscode ? CODELIM : DATALIM;
      if (d->syms.p[symbol].size) {
        n = MIN(n, d->syms.p[symbol].size);
      } else if (symbol + 1 < d->syms.i &&
                 d->syms.p[symbol + 1].addr > d->syms.p[symbol].addr) {
        n = MIN(n, d->syms.p[symbol + 1].addr - d->syms.p[symbol].addr);
      }
      if (addr == d->syms.p[symbol].addr && d->syms.p[symbol].name) {
        op.addr = addr;
        op.unique = unique++;
        op.size = 0;
        op.active = true;
        DisLabel((struct DisBuilder){d, d->xedd, addr}, d->buf,
                 d->syms.stab + d->syms.p[symbol].name);
        if (!(op.s = strdup(d->buf))) break;
        APPEND(&d->ops.p, &d->ops.i, &d->ops.n, &op);
      }
    } else {
      iscode = DisIsText(d, addr);
      n = CODELIM;
    }
    DCHECK_GT(n, 0);
    DCHECK_LE(n, ARRAYLEN(d->raw));
    memset(r, 0, sizeof(r));
    if (!(r[0] = FindReal(m, addr))) {
      max = MIN(100, max);
      n = MIN(DATALIM, 0x1000 - (addr & 0xfff));
      DCHECK_GT(n, 0);
      memset(d->raw, 0xCC, DATALIM);
    } else if ((addr & 0xfff) + n <= 0x1000) {
      memcpy(d->raw, r[0], n);
    } else if ((r[1] = FindReal(m, ROUNDUP(addr, 0x1000)))) {
      si = 0x1000 - (addr & 0xfff);
      memcpy(d->raw, r[0], si);
      memcpy(d->raw + si, r[1], n - si);
    } else {
      n = 0x1000 - (addr & 0xfff);
      DCHECK_GT(n, 0);
      memcpy(d->raw, r[0], n);
    }
    if (!NoDebug()) memset(d->buf, 0x55, sizeof(d->buf));
    if (1 || iscode) {
      xed_instruction_length_decode(d->xedd, d->raw, n);
      DCHECK_GT(n, 0);
      p = DisLineCode((struct DisBuilder){d, d->xedd, addr}, d->buf);
      n = d->xedd->op.error ? 1 : d->xedd->length;
      DCHECK_GT(n, 0);
    } else {
      p = DisLineData((struct DisBuilder){d, d->xedd, addr}, d->buf, d->raw, n);
    }
    DCHECK_LT(p, d->buf + sizeof(d->buf));
    DCHECK_LT(strlen(d->buf), sizeof(d->buf));
    op.addr = addr;
    op.unique = unique++;
    op.size = n;
    op.active = true;
    if (!(op.s = strdup(d->buf))) break;
    APPEND(&d->ops.p, &d->ops.i, &d->ops.n, &op);
    addr += n;
    n = 0;
  }
}

void DisFreeOp(struct DisOp *o) {
  free(o->s);
}

void DisFreeOps(struct DisOps *ops) {
  long i;
  for (i = 0; i < ops->i; ++i) {
    DisFreeOp(&ops->p[i]);
  }
  free(ops->p);
  memset(ops, 0, sizeof(*ops));
}

void DisFree(struct Dis *d) {
  long i;
  DisFreeOps(&d->ops);
  free(d->edges.p);
  free(d->loads.p);
  free(d->syms.p);
  memset(d, 0, sizeof(*d));
}
