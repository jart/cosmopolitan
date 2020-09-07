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
#include "libc/limits.h"
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

static long DisOne(struct Dis *d, struct Machine *m, int64_t addr) {
  void *r;
  unsigned k;
  uint8_t b[15];
  struct DisOp op;
  long i, n, symbol;
  n = 15;
  if ((symbol = DisFindSym(d, addr)) != -1) {
    if (d->syms.p[symbol].addr <= addr &&
        addr < d->syms.p[symbol].addr + d->syms.p[symbol].size) {
      n = d->syms.p[symbol].size - (addr - d->syms.p[symbol].addr);
    }
    if (addr == d->syms.p[symbol].addr && d->syms.p[symbol].name) {
      op.addr = addr;
      op.size = 0;
      op.active = true;
      DisLabel((struct DisBuilder){d, d->xedd, addr}, d->buf,
               d->syms.stab + d->syms.p[symbol].name);
      if (!(op.s = strdup(d->buf))) return -1;
      APPEND(&d->ops.p, &d->ops.i, &d->ops.n, &op);
    }
  }
  n = MAX(1, MIN(15, n));
  if (!(r = FindReal(m, addr))) return -1;
  k = 0x1000 - (addr & 0xfff);
  if (n <= k) {
    memcpy(b, r, n);
  } else {
    memcpy(b, r, k);
    if ((r = FindReal(m, addr + k))) {
      memcpy(b + k, r, n - k);
    } else {
      n = k;
    }
  }
  xed_decoded_inst_zero_set_mode(d->xedd, m->mode);
  xed_instruction_length_decode(d->xedd, b, n);
  n = d->xedd->op.error ? 1 : d->xedd->length;
  op.addr = addr;
  op.size = n;
  op.active = true;
  op.s = NULL;
  APPEND(&d->ops.p, &d->ops.i, &d->ops.n, &op);
  return n;
}

long Dis(struct Dis *d, struct Machine *m, int64_t addr, int lines) {
  int64_t i, j, symbol;
  DisFreeOps(&d->ops);
  if ((symbol = DisFindSym(d, addr)) != -1 &&
      (d->syms.p[symbol].addr < addr &&
       addr < d->syms.p[symbol].addr + d->syms.p[symbol].size)) {
    for (i = d->syms.p[symbol].addr; i < addr; i += j) {
      if ((j = DisOne(d, m, i)) == -1) return -1;
    }
  }
  for (i = 0; i < lines; ++i, addr += j) {
    if ((j = DisOne(d, m, addr)) == -1) return -1;
  }
  return 0;
}

const char *DisGetLine(struct Dis *d, struct Machine *m, size_t i) {
  char *p;
  void *r[2];
  uint8_t b[15];
  if (i >= d->ops.i) return "";
  if (d->ops.p[i].s) return d->ops.p[i].s;
  DCHECK_LE(d->ops.p[i].size, 15);
  xed_decoded_inst_zero_set_mode(d->xedd, m->mode);
  xed_instruction_length_decode(
      d->xedd, AccessRam(m, d->ops.p[i].addr, d->ops.p[i].size, r, b, true),
      d->ops.p[i].size);
  p = DisLineCode((struct DisBuilder){d, d->xedd, d->ops.p[i].addr}, d->buf);
  CHECK_LT(p - d->buf, sizeof(d->buf));
  return d->buf;
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
