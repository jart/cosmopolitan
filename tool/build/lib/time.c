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
#include "libc/calls/calls.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/sysv/consts/clock.h"
#include "libc/time/time.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/time.h"

/**
 * @fileoverview i am the timelorde
 */

void OpPause(struct Machine *m, uint32_t rde) {
  sched_yield();
}

void OpRdtsc(struct Machine *m, uint32_t rde) {
  uint64_t c;
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  c = ts.tv_sec * 1000000000 + ts.tv_nsec;
  Write64(m->ax, (c >> 000) & 0xffffffff);
  Write64(m->dx, (c >> 040) & 0xffffffff);
}

void OpRdtscp(struct Machine *m, uint32_t rde) {
  uint32_t core, node, tscaux;
  OpRdtsc(m, rde);
  core = 0;
  node = 0;
  tscaux = (node & 0xfff) << 12 | (core & 0xfff);
  Write64(m->cx, tscaux & 0xffffffff);
}
