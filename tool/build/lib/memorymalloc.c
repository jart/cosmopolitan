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
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/pml4t.h"

void *MallocPage(void) {
  void *p;
  size_t n;
  if ((p = memalign(4096, 4096))) {
    memset(p, 0, 4096);
  }
  return p;
}

int RegisterMemory(struct Machine *m, int64_t v, void *r, size_t n) {
  return RegisterPml4t(m->cr3, v, (int64_t)(intptr_t)r, n, MallocPage);
}

void ResetRam(struct Machine *m) {
  FreePml4t(m->cr3, -0x800000000000, 0x800000000000, free, munmap);
}

struct Machine *NewMachine(void) {
  struct Machine *m;
  m = memalign(alignof(struct Machine), sizeof(struct Machine));
  memset(m, 0, sizeof(struct Machine));
  m->mode = XED_MACHINE_MODE_LONG_64;
  InitMachine(m);
  return m;
}
