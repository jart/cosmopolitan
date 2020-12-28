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
#include "libc/alg/alg.h"
#include "libc/alg/arraylist2.internal.h"
#include "libc/bits/safemacros.h"
#include "libc/fmt/bing.internal.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/str/tpencode.internal.h"
#include "third_party/xed/x86.h"
#include "tool/build/lib/case.h"
#include "tool/build/lib/demangle.h"
#include "tool/build/lib/dis.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/high.h"
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
  p = HighStart(p, g_high.keyword);
  p = DisColumn(stpcpy(p, ".byte"), p, NAMELEN);
  p = HighEnd(p);
  p = DisOctets(p, d, n);
  return p;
}

static char *DisError(struct Dis *d, char *p) {
  p = DisColumn(DisByte(p, d->xedd->bytes, MIN(15, d->xedd->length)), p,
                CODELEN);
  p = HighStart(p, g_high.comment);
  *p++ = '#';
  *p++ = ' ';
  p = stpcpy(p, IndexDoubleNulString(kXedErrorNames, d->xedd->op.error));
  p = HighEnd(p);
  *p = '\0';
  return p;
}

static char *DisAddr(struct Dis *d, char *p) {
  if (-0x80000000 <= d->addr && d->addr <= 0x7fffffff) {
    return p + uint64toarray_fixed16(d->addr, p, 32);
  } else {
    return p + uint64toarray_fixed16(d->addr, p, 48);
  }
}

static char *DisRaw(struct Dis *d, char *p) {
  long i;
  for (i = 0; i < PFIXLEN - MIN(PFIXLEN, d->xedd->op.PIVOTOP); ++i) {
    *p++ = ' ';
    *p++ = ' ';
  }
  for (i = 0; i < MIN(15, d->xedd->length); ++i) {
    if (i == d->xedd->op.PIVOTOP) *p++ = ' ';
    *p++ = "0123456789abcdef"[(d->xedd->bytes[i] & 0xf0) >> 4];
    *p++ = "0123456789abcdef"[d->xedd->bytes[i] & 0x0f];
  }
  *p = '\0';
  return p;
}

static char *DisCode(struct Dis *d, char *p) {
  char optspecbuf[128];
  if (!d->xedd->op.error) {
    return DisInst(d, p, DisSpec(d->xedd, optspecbuf));
  } else {
    return DisError(d, p);
  }
}

static char *DisLineCode(struct Dis *d, char *p) {
  p = DisColumn(DisAddr(d, p), p, ADDRLEN);
  p = DisColumn(DisRaw(d, p), p, PFIXLEN * 2 + 1 + BYTELEN * 2);
  p = DisCode(d, p);
  return p;
}

static char *DisLineData(struct Dis *d, char *p, const uint8_t *b, size_t n) {
  size_t i;
  p = DisColumn(DisAddr(d, p), p, ADDRLEN);
  p = DisColumn(DisByte(p, b, n), p, 64);
  p = HighStart(p, g_high.comment);
  *p++ = '#';
  *p++ = ' ';
  for (i = 0; i < n; ++i) p += tpencode(p, 8, bing(b[i], 0), false);
  p = HighEnd(p);
  *p = '\0';
  return p;
}

static char *DisLabel(struct Dis *d, char *p, const char *name) {
  p = DisColumn(DisAddr(d, p), p, ADDRLEN);
  p = HighStart(p, g_high.label);
  p = Demangle(p, name, DIS_MAX_SYMBOL_LENGTH);
  p = HighEnd(p);
  *p++ = ':';
  *p = '\0';
  return p;
}

long DisFind(struct Dis *d, int64_t addr) {
  int l, r, m, i;
  l = 0;
  r = d->ops.i - 1;
  while (l <= r) {
    m = (l + r) >> 1;
    if (d->ops.p[m].addr < addr) {
      l = m + 1;
    } else if (d->ops.p[m].addr > addr) {
      r = m - 1;
    } else {
      return m;
    }
  }
  return -1;
}

static long DisAppendOpLines(struct Dis *d, struct Machine *m, int64_t addr) {
  void *r;
  int64_t ip;
  unsigned k;
  struct DisOp op;
  long i, n, symbol;
  uint8_t *p, b[15];
  n = 15;
  ip = addr - Read64(m->cs);
  if ((symbol = DisFindSym(d, ip)) != -1) {
    if (d->syms.p[symbol].addr <= ip &&
        ip < d->syms.p[symbol].addr + d->syms.p[symbol].size) {
      n = d->syms.p[symbol].size - (ip - d->syms.p[symbol].addr);
    }
    if (ip == d->syms.p[symbol].addr && d->syms.p[symbol].name) {
      op.addr = addr;
      op.size = 0;
      op.active = true;
      d->addr = addr;
      DisLabel(d, d->buf, d->syms.stab + d->syms.p[symbol].name);
      if (!(op.s = strdup(d->buf))) return -1;
      APPEND(&d->ops.p, &d->ops.i, &d->ops.n, &op);
    }
  }
  n = MAX(1, MIN(15, n));
  if (!(r = FindReal(m, addr))) return -1;
  k = 0x1000 - (addr & 0xfff);
  if (n <= k) {
    p = r;
  } else {
    p = b;
    memcpy(b, r, k);
    if ((r = FindReal(m, addr + k))) {
      memcpy(b + k, r, n - k);
    } else {
      n = k;
    }
  }
  xed_decoded_inst_zero_set_mode(d->xedd, m->mode);
  xed_instruction_length_decode(d->xedd, p, n);
  n = d->xedd->op.error ? 1 : d->xedd->length;
  op.addr = addr;
  op.size = n;
  op.active = true;
  op.s = NULL;
  APPEND(&d->ops.p, &d->ops.i, &d->ops.n, &op);
  return n;
}

long Dis(struct Dis *d, struct Machine *m, uint64_t addr, uint64_t ip,
         int lines) {
  int64_t i, j, symbol;
  DisFreeOps(&d->ops);
  if ((symbol = DisFindSym(d, addr)) != -1 &&
      (d->syms.p[symbol].addr < addr &&
       addr < d->syms.p[symbol].addr + d->syms.p[symbol].size)) {
    for (i = d->syms.p[symbol].addr; i < addr; i += j) {
      if ((j = DisAppendOpLines(d, m, i)) == -1) return -1;
    }
  }
  for (i = 0; i < lines; ++i, addr += j) {
    if ((j = DisAppendOpLines(d, m, addr)) == -1) return -1;
  }
  return 0;
}

const char *DisGetLine(struct Dis *d, struct Machine *m, size_t i) {
  void *r[2];
  uint8_t b[15];
  if (i >= d->ops.i) return "";
  if (d->ops.p[i].s) return d->ops.p[i].s;
  DCHECK_LE(d->ops.p[i].size, 15);
  xed_decoded_inst_zero_set_mode(d->xedd, m->mode);
  xed_instruction_length_decode(
      d->xedd, AccessRam(m, d->ops.p[i].addr, d->ops.p[i].size, r, b, true),
      d->ops.p[i].size);
  d->m = m;
  d->addr = d->ops.p[i].addr;
  CHECK_LT(DisLineCode(d, d->buf) - d->buf, sizeof(d->buf));
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
