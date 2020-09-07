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
#include "libc/nexgen32e/bsf.h"
#include "libc/str/str.h"
#include "third_party/xed/x86.h"
#include "tool/build/lib/address.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/stats.h"
#include "tool/build/lib/throw.h"

static bool IsOpcodeEqual(struct XedDecodedInst *xedd, uint8_t *a) {
  uint64_t w;
  if (xedd->length) {
    if (xedd->length <= 7) {
      w = Read64(a) ^ Read64(xedd->bytes);
      return !w || bsfl(w) >= (xedd->length << 3);
    } else {
      return memcmp(a, xedd->bytes, xedd->length) == 0;
    }
  } else {
    return false;
  }
}

static void DecodeInstruction(struct Machine *m, uint8_t *p, unsigned n) {
  struct XedDecodedInst xedd[1];
  xed_decoded_inst_zero_set_mode(xedd, m->mode);
  if (!xed_instruction_length_decode(xedd, p, n)) {
    memcpy(m->xedd, xedd, sizeof(m->icache[0]));
  } else {
    HaltMachine(m, kMachineDecodeError);
  }
}

void LoadInstruction(struct Machine *m) {
  uint64_t ip;
  unsigned i, key;
  uint8_t *addr, *toil, copy[15];
  ip = Read64(m->cs) + MaskAddress(m->mode & 3, m->ip);
  key = ip & (ARRAYLEN(m->icache) - 1);
  m->xedd = (struct XedDecodedInst *)m->icache[key];
  if ((i = 0x1000 - (ip & 0xfff)) >= 15) {
    if (ROUNDDOWN(ip, 0x1000) == m->codevirt && ip) {
      addr = m->codereal + (ip & 0xfff);
    } else {
      m->codevirt = ROUNDDOWN(ip, 0x1000);
      m->codereal = ResolveAddress(m, m->codevirt);
      addr = m->codereal + (ip & 0xfff);
    }
    if (!IsOpcodeEqual(m->xedd, addr)) {
      DecodeInstruction(m, addr, 15);
    }
  } else {
    addr = ResolveAddress(m, ip);
    if ((toil = FindReal(m, ip + i))) {
      memcpy(copy, addr, i);
      memcpy(copy + i, toil, 15 - i);
      DecodeInstruction(m, copy, 15);
    } else {
      DecodeInstruction(m, addr, i);
    }
  }
}
