/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2026 Justine Alexandra Roberts Tunney                              │
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
#include "libc/testlib/testlib.h"
#include "net/http/ip.h"

TEST(IsPublicIp6, test) {
  static const uint8_t a[16] = {0x20};
  static const uint8_t b[16] = {0x3f, 0xff};
  static const uint8_t doc[16] = {0x20, 0x01, 0x0d, 0xb8};
  static const uint8_t priv[16] = {0xfc};
  static const uint8_t link[16] = {0xfe, 0x80};
  static const uint8_t loop[16] = {0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 1};
  static const uint8_t multi[16] = {0xff, 0x02};
  EXPECT_TRUE(IsPublicIp6(a));
  EXPECT_TRUE(IsPublicIp6(b));
  EXPECT_FALSE(IsPublicIp6(doc));
  EXPECT_FALSE(IsPublicIp6(priv));
  EXPECT_FALSE(IsPublicIp6(link));
  EXPECT_FALSE(IsPublicIp6(loop));
  EXPECT_FALSE(IsPublicIp6(multi));
}
