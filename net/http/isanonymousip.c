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
#include "net/http/ip.h"

/**
 * Returns true if IPv4 address is anonymous proxy provider.
 */
bool IsAnonymousIp(uint32_t x) {
  return (x & 0xffffffff) == 0x1f0e8527 || (x & 0xffffff00) == 0x2e138900 ||
         (x & 0xffffff00) == 0x2e138f00 || (x & 0xfffffffe) == 0x32074e58 ||
         (x & 0xfffffe00) == 0x3e490800 || (x & 0xffffffff) == 0x3feb9bd2 ||
         (x & 0xffffffff) == 0x400c7617 || (x & 0xffffffff) == 0x400c7658 ||
         (x & 0xffffff00) == 0x432b9c00 || (x & 0xffffff00) == 0x450a8b00 ||
         (x & 0xffffff00) == 0x46e8f500 || (x & 0xffffffff) == 0x4a5209e0 ||
         (x & 0xfffffe00) == 0x50fe4a00 || (x & 0xfffffe00) == 0x5d735200 ||
         (x & 0xfffffe00) == 0x5d735400 || (x & 0xffffffff) == 0x602fe214 ||
         (x & 0xffffff00) == 0x93cb7800 || (x & 0xffffffff) == 0xb0094b2b ||
         (x & 0xffffffff) == 0xb9246491 || (x & 0xffffff00) == 0xc0ee1500 ||
         (x & 0xffffffff) == 0xc16b1147 || (x & 0xffffffff) == 0xc6906958 ||
         (x & 0xffffff00) == 0xc772df00 || (x & 0xffffff00) == 0xc7bcec00 ||
         (x & 0xffffffff) == 0xc8c8c8c8 || (x & 0xffffff00) == 0xce47a200 ||
         (x & 0xffffff00) == 0xcec46700 || (x & 0xffffffff) == 0xd02be134 ||
         (x & 0xffffff00) == 0xd1d8c600 || (x & 0xfffffffc) == 0xd43fa9e8 ||
         (x & 0xffffffff) == 0xd5eaf973 || (x & 0xffffff00) == 0xd897b400;
}
