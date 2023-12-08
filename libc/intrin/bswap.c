/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/bswap.h"
#include "libc/intrin/newbie.h"
#include "libc/sock/sock.h"

uint16_t(bswap_16)(uint16_t x) {
  return (0x00ff & x) << 010 | (0xff00 & x) >> 010;
}

__strong_reference(bswap_16, ntohs);
__strong_reference(bswap_16, htons);

uint32_t(bswap_32)(uint32_t x) {
  return (0x000000ffu & x) << 030 | (0x0000ff00u & x) << 010 |
         (0x00ff0000u & x) >> 010 | (0xff000000u & x) >> 030;
}

__strong_reference(bswap_32, ntohl);
__strong_reference(bswap_32, htonl);

uint64_t(bswap_64)(uint64_t x) {
  return (0x00000000000000ffull & x) << 070 |
         (0x000000000000ff00ull & x) << 050 |
         (0x0000000000ff0000ull & x) << 030 |
         (0x00000000ff000000ull & x) << 010 |
         (0x000000ff00000000ull & x) >> 010 |
         (0x0000ff0000000000ull & x) >> 030 |
         (0x00ff000000000000ull & x) >> 050 |
         (0xff00000000000000ull & x) >> 070;
}
