/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/makedev.h"
#include "libc/dce.h"

uint64_t(makedev)(uint32_t x, uint32_t y) {
  if (IsXnu()) {
    return x << 24 | y;
  } else if (IsNetbsd()) {
    return ((x << 8) & 0x000fff00) | ((y << 12) & 0xfff00000) |
           (y & 0x000000ff);
  } else if (IsOpenbsd()) {
    return (x & 0xff) << 8 | (y & 0xff) | (y & 0xffff00) << 8;
  } else if (IsFreebsd()) {
    return (uint64_t)(x & 0xffffff00) << 32 | (x & 0x000000ff) << 8 |
           (y & 0x0000ff00) << 24 | (y & 0xffff00ff);
  } else {
    return (uint64_t)(x & 0xfffff000) << 32 | (x & 0x00000fff) << 8 |
           (y & 0xffffff00) << 12 | (y & 0x000000ff);
  }
}
