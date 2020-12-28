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
