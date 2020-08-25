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
#include "libc/log/check.h"
#include "libc/runtime/runtime.h"
#include "tool/build/lib/throw.h"

static bool IsHaltingInitialized(struct Machine *m) {
  jmp_buf zb;
  memset(zb, 0, sizeof(zb));
  return memcmp(m->onhalt, zb, sizeof(m->onhalt)) != 0;
}

void HaltMachine(struct Machine *m, int code) {
  CHECK(IsHaltingInitialized(m));
  longjmp(m->onhalt, code);
}

void ThrowDivideError(struct Machine *m) {
  HaltMachine(m, kMachineDivideError);
}

void ThrowSegmentationFault(struct Machine *m, int64_t va) {
  m->faultaddr = va;
  if (m->xedd) m->ip -= m->xedd->length;
  HaltMachine(m, kMachineSegmentationFault);
}

void ThrowProtectionFault(struct Machine *m) {
  HaltMachine(m, kMachineProtectionFault);
}

void OpUd(struct Machine *m) {
  m->ip -= m->xedd->length;
  HaltMachine(m, kMachineUndefinedInstruction);
}

void OpHlt(struct Machine *m) {
  HaltMachine(m, kMachineHalt);
}

void OpInterrupt(struct Machine *m, int i) {
  HaltMachine(m, i);
}
