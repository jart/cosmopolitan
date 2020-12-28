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
#include "libc/calls/calls.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "tool/build/lib/flags.h"
#include "tool/build/lib/machine.h"

#define LDBL 3
#define RINT 0

static void ResetFpu(struct Machine *m) {
  long i;
  long double fval;
  fval = -NAN;
  m->fpu.sw = 0;
  m->fpu.tw = -1;
  m->fpu.cw = 0;
  m->fpu.im = true;
  m->fpu.dm = true;
  m->fpu.zm = true;
  m->fpu.om = true;
  m->fpu.um = true;
  m->fpu.pm = true;
  m->fpu.pc = LDBL;
  m->fpu.rc = RINT;
  for (i = 0; i < ARRAYLEN(m->fpu.st); ++i) {
    memcpy(&m->fpu.st[i], &fval, sizeof(fval));
  }
}

static void ResetSse(struct Machine *m) {
  m->sse.mxcsr = 0;
  m->sse.daz = false;
  m->sse.im = true;
  m->sse.dm = true;
  m->sse.zm = true;
  m->sse.om = true;
  m->sse.um = true;
  m->sse.pm = true;
  m->sse.rc = RINT;
  m->sse.ftz = false;
  memset(m->xmm, 0, sizeof(m->xmm));
}

void ResetInstructionCache(struct Machine *m) {
  memset(m->icache, -1, sizeof(m->icache));
}

void ResetCpu(struct Machine *m) {
  m->faultaddr = 0;
  m->stashsize = 0;
  m->stashaddr = 0;
  m->writeaddr = 0;
  m->readaddr = 0;
  m->writesize = 0;
  m->readsize = 0;
  m->flags = SetFlag(m->flags, FLAGS_DF, false);
  m->flags = SetFlag(m->flags, FLAGS_CF, false);
  m->flags = SetFlag(m->flags, FLAGS_ZF, false);
  m->flags = SetFlag(m->flags, FLAGS_SF, false);
  m->flags = SetFlag(m->flags, FLAGS_IF, true);
  m->flags = SetFlag(m->flags, FLAGS_F1, true);
  m->flags = SetFlag(m->flags, FLAGS_F0, false);
  m->flags = SetFlag(m->flags, FLAGS_IOPL, 3);
  memset(m->reg, 0, sizeof(m->reg));
  memset(m->bofram, 0, sizeof(m->bofram));
  memset(&m->freelist, 0, sizeof(m->freelist));
  ResetSse(m);
  ResetFpu(m);
}

void ResetTlb(struct Machine *m) {
  m->tlbindex = 0;
  memset(m->tlb, 0, sizeof(m->tlb));
  m->codevirt = 0;
  m->codehost = NULL;
}
