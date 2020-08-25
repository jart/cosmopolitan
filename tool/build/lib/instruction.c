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
#include "libc/str/str.h"
#include "third_party/xed/x86.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/stats.h"
#include "tool/build/lib/throw.h"

static bool IsOpcodeEqual(uint8_t *a, uint8_t b[16], size_t size) {
  if (likely(size)) {
    return memcmp(a, b, size) == 0;
  } else {
    return false;
  }
}

void LoadInstruction(struct Machine *m) {
  unsigned i;
  enum XedError err;
  uint8_t *addr, *toil, copy[15];
  if ((i = 0x1000 - (m->ip & 0xfff)) >= 15) {
    if (ROUNDDOWN(m->ip, 0x1000) == m->codevirt && m->ip) {
      addr = m->codereal + (m->ip & 0xfff);
    } else {
      m->codevirt = ROUNDDOWN(m->ip, 0x1000);
      m->codereal = ResolveAddress(m, m->codevirt);
      addr = m->codereal + (m->ip & 0xfff);
    }
    m->xedd = m->icache + (m->ip & (ARRAYLEN(m->icache) - 1));
    if (IsOpcodeEqual(addr, m->xedd->bytes, m->xedd->length)) {
      taken++;
    } else {
      ntaken++;
      xed_decoded_inst_zero_set_mode(m->xedd, XED_MACHINE_MODE_LONG_64);
      if (xed_instruction_length_decode(m->xedd, addr, 15)) {
        HaltMachine(m, kMachineDecodeError);
      }
    }
  } else {
    m->xedd = m->icache;
    xed_decoded_inst_zero_set_mode(m->xedd, XED_MACHINE_MODE_LONG_64);
    addr = ResolveAddress(m, m->ip);
    if ((toil = FindReal(m, m->ip + i))) {
      memcpy(copy, addr, i);
      memcpy(copy + i, toil, 15 - i);
      if ((err = xed_instruction_length_decode(m->xedd, copy, 15))) {
        HaltMachine(m, kMachineDecodeError);
      }
    } else {
      if ((err = xed_instruction_length_decode(m->xedd, addr, i))) {
        HaltMachine(m, kMachineDecodeError);
      }
    }
  }
}
