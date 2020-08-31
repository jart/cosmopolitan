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
#include "tool/build/lib/endian.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/word.h"

void SetMemoryShort(struct Machine *m, int64_t v, int16_t i) {
  void *p[2];
  uint8_t b[2];
  Write16(BeginStore(m, v, 2, p, b), i);
  EndStore(m, v, 2, p, b);
}

void SetMemoryInt(struct Machine *m, int64_t v, int32_t i) {
  void *p[2];
  uint8_t b[4];
  Write32(BeginStore(m, v, 4, p, b), i);
  EndStore(m, v, 4, p, b);
}

void SetMemoryLong(struct Machine *m, int64_t v, int64_t i) {
  void *p[2];
  uint8_t b[8];
  Write64(BeginStore(m, v, 8, p, b), i);
  EndStore(m, v, 8, p, b);
}

void SetMemoryFloat(struct Machine *m, int64_t v, float f) {
  void *p[2];
  uint8_t b[4];
  memcpy(BeginStore(m, v, 4, p, b), &f, 4);
  EndStore(m, v, 4, p, b);
}

void SetMemoryDouble(struct Machine *m, int64_t v, double f) {
  void *p[2];
  uint8_t b[8];
  memcpy(BeginStore(m, v, 8, p, b), &f, 8);
  EndStore(m, v, 8, p, b);
}

void SetMemoryLdbl(struct Machine *m, int64_t v, long double f) {
  void *p[2];
  uint8_t b[10];
  memcpy(BeginStore(m, v, 10, p, b), &f, 10);
  EndStore(m, v, 10, p, b);
}
