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
#include "third_party/xed/x86.h"
#include "tool/build/lib/address.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/throw.h"

uint8_t *GetSegment(struct Machine *m, uint32_t rde, int s) {
  switch (s & 7) {
    case 0:
      return m->es;
    case 1:
      return m->cs;
    case 2:
      return m->ss;
    case 3:
      return m->ds;
    case 4:
      return m->fs;
    case 5:
      return m->gs;
    case 6:
    case 7:
      OpUd(m, rde);
    default:
      unreachable;
  }
}

uint64_t AddSegment(struct Machine *m, uint32_t rde, uint64_t i, uint8_t s[8]) {
  if (!Sego(rde)) {
    i += Read64(s);
  } else {
    i += Read64(GetSegment(m, rde, Sego(rde) - 1));
  }
  return i;
}

uint64_t DataSegment(struct Machine *m, uint32_t rde, uint64_t i) {
  return AddSegment(m, rde, i, m->ds);
}

uint64_t AddressSi(struct Machine *m, uint32_t rde) {
  switch (Eamode(rde)) {
    case XED_MODE_LONG:
      return DataSegment(m, rde, Read64(m->si));
    case XED_MODE_REAL:
      return DataSegment(m, rde, Read16(m->si));
    case XED_MODE_LEGACY:
      return DataSegment(m, rde, Read32(m->si));
    default:
      unreachable;
  }
}

uint64_t AddressDi(struct Machine *m, uint32_t rde) {
  uint64_t i = Read64(m->es);
  switch (Eamode(rde)) {
    case XED_MODE_LONG:
      return i + Read64(m->di);
    case XED_MODE_REAL:
      return i + Read16(m->di);
    case XED_MODE_LEGACY:
      return i + Read32(m->di);
    default:
      unreachable;
  }
}
