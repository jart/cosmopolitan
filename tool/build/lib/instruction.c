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

static noinline void LoadInstructionSlow(struct Machine *m, uint64_t ip) {
  unsigned i;
  uint8_t *addr;
  uint8_t copy[15], *toil;
  i = 0x1000 - (ip & 0xfff);
  addr = ResolveAddress(m, ip);
  if ((toil = FindReal(m, ip + i))) {
    memcpy(copy, addr, i);
    memcpy(copy + i, toil, 15 - i);
    DecodeInstruction(m, copy, 15);
  } else {
    DecodeInstruction(m, addr, i);
  }
}

void LoadInstruction(struct Machine *m) {
  uint64_t ip;
  unsigned key;
  uint8_t *addr;
  ip = Read64(m->cs) + MaskAddress(m->mode & 3, m->ip);
  key = ip & (ARRAYLEN(m->icache) - 1);
  m->xedd = (struct XedDecodedInst *)m->icache[key];
  if ((ip & 0xfff) < 0x1000 - 15) {
    if (ip - (ip & 0xfff) == m->codevirt && m->codehost) {
      addr = m->codehost + (ip & 0xfff);
    } else {
      m->codevirt = ip - (ip & 0xfff);
      m->codehost = ResolveAddress(m, m->codevirt);
      addr = m->codehost + (ip & 0xfff);
    }
    if (!IsOpcodeEqual(m->xedd, addr)) {
      DecodeInstruction(m, addr, 15);
    }
  } else {
    LoadInstructionSlow(m, ip);
  }
}
