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
#include "libc/macros.h"
#include "libc/math.h"
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
  memset(m->xmmtype, 0, sizeof(m->xmmtype));
}

void ResetCpu(struct Machine *m) {
  InitMachine(m);
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
  ResetSse(m);
  ResetFpu(m);
}
