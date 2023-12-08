/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/intrin/safemacros.internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/testlib/testlib.h"

TEST(inet_pton, testLocalhost) {
  uint8_t addr[4] = {255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET, "127.0.0.1", &addr));
  EXPECT_EQ(127, addr[0]);
  EXPECT_EQ(0, addr[1]);
  EXPECT_EQ(0, addr[2]);
  EXPECT_EQ(1, addr[3]);

  uint8_t addrv6[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                        255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "::1", &addrv6));
  EXPECT_EQ(0, addrv6[0]);
  EXPECT_EQ(0, addrv6[1]);
  EXPECT_EQ(0, addrv6[2]);
  EXPECT_EQ(0, addrv6[3]);
  EXPECT_EQ(0, addrv6[4]);
  EXPECT_EQ(0, addrv6[5]);
  EXPECT_EQ(0, addrv6[6]);
  EXPECT_EQ(0, addrv6[7]);
  EXPECT_EQ(0, addrv6[8]);
  EXPECT_EQ(0, addrv6[9]);
  EXPECT_EQ(0, addrv6[10]);
  EXPECT_EQ(0, addrv6[11]);
  EXPECT_EQ(0, addrv6[12]);
  EXPECT_EQ(0, addrv6[13]);
  EXPECT_EQ(0, addrv6[14]);
  EXPECT_EQ(1, addrv6[15]);

  uint8_t addrv6_second[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                               255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "0:0:0:0:0:0:0:1", &addrv6_second));
  EXPECT_EQ(0, addrv6_second[0]);
  EXPECT_EQ(0, addrv6_second[1]);
  EXPECT_EQ(0, addrv6_second[2]);
  EXPECT_EQ(0, addrv6_second[3]);
  EXPECT_EQ(0, addrv6_second[4]);
  EXPECT_EQ(0, addrv6_second[5]);
  EXPECT_EQ(0, addrv6_second[6]);
  EXPECT_EQ(0, addrv6_second[7]);
  EXPECT_EQ(0, addrv6_second[8]);
  EXPECT_EQ(0, addrv6_second[9]);
  EXPECT_EQ(0, addrv6_second[10]);
  EXPECT_EQ(0, addrv6_second[11]);
  EXPECT_EQ(0, addrv6_second[12]);
  EXPECT_EQ(0, addrv6_second[13]);
  EXPECT_EQ(0, addrv6_second[14]);
  EXPECT_EQ(1, addrv6_second[15]);
}

TEST(inet_pton, testAny) {
  uint8_t addr[4] = {255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET, "0.0.0.0", &addr));
  EXPECT_EQ(0, addr[0]);
  EXPECT_EQ(0, addr[1]);
  EXPECT_EQ(0, addr[2]);
  EXPECT_EQ(0, addr[3]);

  uint8_t addrv6[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                        255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "::", &addrv6));
  EXPECT_EQ(0, addrv6[0]);
  EXPECT_EQ(0, addrv6[1]);
  EXPECT_EQ(0, addrv6[2]);
  EXPECT_EQ(0, addrv6[3]);
  EXPECT_EQ(0, addrv6[4]);
  EXPECT_EQ(0, addrv6[5]);
  EXPECT_EQ(0, addrv6[6]);
  EXPECT_EQ(0, addrv6[7]);
  EXPECT_EQ(0, addrv6[8]);
  EXPECT_EQ(0, addrv6[9]);
  EXPECT_EQ(0, addrv6[10]);
  EXPECT_EQ(0, addrv6[11]);
  EXPECT_EQ(0, addrv6[12]);
  EXPECT_EQ(0, addrv6[13]);
  EXPECT_EQ(0, addrv6[14]);
  EXPECT_EQ(0, addrv6[15]);

  uint8_t addrv6_second[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                               255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "0:0:0:0:0:0:0:0", &addrv6_second));
  EXPECT_EQ(0, addrv6_second[0]);
  EXPECT_EQ(0, addrv6_second[1]);
  EXPECT_EQ(0, addrv6_second[2]);
  EXPECT_EQ(0, addrv6_second[3]);
  EXPECT_EQ(0, addrv6_second[4]);
  EXPECT_EQ(0, addrv6_second[5]);
  EXPECT_EQ(0, addrv6_second[6]);
  EXPECT_EQ(0, addrv6_second[7]);
  EXPECT_EQ(0, addrv6_second[8]);
  EXPECT_EQ(0, addrv6_second[9]);
  EXPECT_EQ(0, addrv6_second[10]);
  EXPECT_EQ(0, addrv6_second[11]);
  EXPECT_EQ(0, addrv6_second[12]);
  EXPECT_EQ(0, addrv6_second[13]);
  EXPECT_EQ(0, addrv6_second[14]);
  EXPECT_EQ(0, addrv6_second[15]);
}

TEST(inet_pton, testShortAddress_doesntFillFullValue) {
  uint8_t addr[4] = {255, 255, 255, 255};
  EXPECT_EQ(0, inet_pton(AF_INET, "127.0.0", &addr));
  EXPECT_EQ(127, addr[0]);
  EXPECT_EQ(0, addr[1]);
  EXPECT_EQ(0, addr[2]);
  EXPECT_EQ(255, addr[3]);

  uint8_t addrv6[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                        255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(0, inet_pton(AF_INET6, "0:0:0:0:0:0:0", &addrv6));
}

TEST(inet_pton, testOverflow_stopsParsing) {
  uint8_t addr[4] = {255, 255, 255, 255};
  EXPECT_EQ(0, inet_pton(AF_INET, "0.300.0", &addr));
  EXPECT_EQ(0, addr[0]);
  EXPECT_EQ(255, addr[1]);
  EXPECT_EQ(255, addr[2]);
  EXPECT_EQ(255, addr[3]);
}

TEST(inet_pton, testBadChar_stopsParsing) {
  uint8_t addr[4] = {255, 255, 255, 255};
  EXPECT_EQ(0, inet_pton(AF_INET, "127-.0.0", &addr));
  EXPECT_EQ(127, addr[0]);
  EXPECT_EQ(255, addr[1]);
  EXPECT_EQ(255, addr[2]);
  EXPECT_EQ(255, addr[3]);

  uint8_t addrv6[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                        255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(0, inet_pton(AF_INET6, "0:0:0:0-0:0:0:0", &addrv6));
}

TEST(inet_pton, testBadFamily_returnsNegAndChangesNothing) {
  uint8_t addr[4] = {255, 255, 255, 255};
  EXPECT_EQ(-1, inet_pton(666, "127.0.0.1", &addr));
  EXPECT_EQ(255, addr[0]);
  EXPECT_EQ(255, addr[1]);
  EXPECT_EQ(255, addr[2]);
  EXPECT_EQ(255, addr[3]);
}

TEST(inet_pton, testIPv6BadColon_fails) {
  uint8_t addrv6[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                        255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(0, inet_pton(AF_INET6, "0:0:0:0:0:0:0:0:", &addrv6));

  uint8_t addrv6_second[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                               255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(0, inet_pton(AF_INET6, "0:0:0:0:0:0:0:0:0", &addrv6_second));
}

TEST(inet_pton, testMappedIPv4inIPv6) {
  uint8_t addrv6[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                        255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "::FFFF:204.152.189.116", &addrv6));
  EXPECT_EQ(0, addrv6[0]);
  EXPECT_EQ(0, addrv6[1]);
  EXPECT_EQ(0, addrv6[2]);
  EXPECT_EQ(0, addrv6[3]);
  EXPECT_EQ(0, addrv6[4]);
  EXPECT_EQ(0, addrv6[5]);
  EXPECT_EQ(0, addrv6[6]);
  EXPECT_EQ(0, addrv6[7]);
  EXPECT_EQ(0, addrv6[8]);
  EXPECT_EQ(0, addrv6[9]);
  EXPECT_EQ(0xFF, addrv6[10]);
  EXPECT_EQ(0xFF, addrv6[11]);
  EXPECT_EQ(0xCC, addrv6[12]);
  EXPECT_EQ(0x98, addrv6[13]);
  EXPECT_EQ(0xBD, addrv6[14]);
  EXPECT_EQ(0x74, addrv6[15]);

  uint8_t addrv6_second[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                               255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "1:2:3:4::5.6.7.8", &addrv6_second));
  EXPECT_EQ(0, addrv6_second[0]);
  EXPECT_EQ(1, addrv6_second[1]);
  EXPECT_EQ(0, addrv6_second[2]);
  EXPECT_EQ(2, addrv6_second[3]);
  EXPECT_EQ(0, addrv6_second[4]);
  EXPECT_EQ(3, addrv6_second[5]);
  EXPECT_EQ(0, addrv6_second[6]);
  EXPECT_EQ(4, addrv6_second[7]);
  EXPECT_EQ(0, addrv6_second[8]);
  EXPECT_EQ(0, addrv6_second[9]);
  EXPECT_EQ(0, addrv6_second[10]);
  EXPECT_EQ(0, addrv6_second[11]);
  EXPECT_EQ(0x5, addrv6_second[12]);
  EXPECT_EQ(0x6, addrv6_second[13]);
  EXPECT_EQ(0x7, addrv6_second[14]);
  EXPECT_EQ(0x8, addrv6_second[15]);

  uint8_t addrv6_third[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                              255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "1:2:3:4::56.6.7.8", &addrv6_third));
  EXPECT_EQ(0, addrv6_third[0]);
  EXPECT_EQ(1, addrv6_third[1]);
  EXPECT_EQ(0, addrv6_third[2]);
  EXPECT_EQ(2, addrv6_third[3]);
  EXPECT_EQ(0, addrv6_third[4]);
  EXPECT_EQ(3, addrv6_third[5]);
  EXPECT_EQ(0, addrv6_third[6]);
  EXPECT_EQ(4, addrv6_third[7]);
  EXPECT_EQ(0, addrv6_third[8]);
  EXPECT_EQ(0, addrv6_third[9]);
  EXPECT_EQ(0, addrv6_third[10]);
  EXPECT_EQ(0, addrv6_third[11]);
  EXPECT_EQ(56, addrv6_third[12]);
  EXPECT_EQ(0x6, addrv6_third[13]);
  EXPECT_EQ(0x7, addrv6_third[14]);
  EXPECT_EQ(0x8, addrv6_third[15]);

  uint8_t addrv6_forth[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                              255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "1:2:3:4:5:6:7.8.9.10", &addrv6_forth));
  EXPECT_EQ(0, addrv6_forth[0]);
  EXPECT_EQ(1, addrv6_forth[1]);
  EXPECT_EQ(0, addrv6_forth[2]);
  EXPECT_EQ(2, addrv6_forth[3]);
  EXPECT_EQ(0, addrv6_forth[4]);
  EXPECT_EQ(3, addrv6_forth[5]);
  EXPECT_EQ(0, addrv6_forth[6]);
  EXPECT_EQ(4, addrv6_forth[7]);
  EXPECT_EQ(0, addrv6_forth[8]);
  EXPECT_EQ(5, addrv6_forth[9]);
  EXPECT_EQ(0, addrv6_forth[10]);
  EXPECT_EQ(6, addrv6_forth[11]);
  EXPECT_EQ(7, addrv6_forth[12]);
  EXPECT_EQ(8, addrv6_forth[13]);
  EXPECT_EQ(9, addrv6_forth[14]);
  EXPECT_EQ(10, addrv6_forth[15]);
}

TEST(inet_pton, testVariousIPv6) {
  uint8_t addrv6[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                        255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "1:0:0:0:0:0:0:8", &addrv6));
  EXPECT_EQ(0, addrv6[0]);
  EXPECT_EQ(1, addrv6[1]);
  EXPECT_EQ(0, addrv6[2]);
  EXPECT_EQ(0, addrv6[3]);
  EXPECT_EQ(0, addrv6[4]);
  EXPECT_EQ(0, addrv6[5]);
  EXPECT_EQ(0, addrv6[6]);
  EXPECT_EQ(0, addrv6[7]);
  EXPECT_EQ(0, addrv6[8]);
  EXPECT_EQ(0, addrv6[9]);
  EXPECT_EQ(0, addrv6[10]);
  EXPECT_EQ(0, addrv6[11]);
  EXPECT_EQ(0, addrv6[12]);
  EXPECT_EQ(0, addrv6[13]);
  EXPECT_EQ(0, addrv6[14]);
  EXPECT_EQ(8, addrv6[15]);

  uint8_t addrv6_second[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                               255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "1::8", &addrv6_second));
  EXPECT_EQ(0, addrv6_second[0]);
  EXPECT_EQ(1, addrv6_second[1]);
  EXPECT_EQ(0, addrv6_second[2]);
  EXPECT_EQ(0, addrv6_second[3]);
  EXPECT_EQ(0, addrv6_second[4]);
  EXPECT_EQ(0, addrv6_second[5]);
  EXPECT_EQ(0, addrv6_second[6]);
  EXPECT_EQ(0, addrv6_second[7]);
  EXPECT_EQ(0, addrv6_second[8]);
  EXPECT_EQ(0, addrv6_second[9]);
  EXPECT_EQ(0, addrv6_second[10]);
  EXPECT_EQ(0, addrv6_second[11]);
  EXPECT_EQ(0, addrv6_second[12]);
  EXPECT_EQ(0, addrv6_second[13]);
  EXPECT_EQ(0, addrv6_second[14]);
  EXPECT_EQ(8, addrv6_second[15]);

  uint8_t addrv6_third[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                              255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "1::1:2:3:4", &addrv6_third));
  EXPECT_EQ(0, addrv6_third[0]);
  EXPECT_EQ(1, addrv6_third[1]);
  EXPECT_EQ(0, addrv6_third[2]);
  EXPECT_EQ(0, addrv6_third[3]);
  EXPECT_EQ(0, addrv6_third[4]);
  EXPECT_EQ(0, addrv6_third[5]);
  EXPECT_EQ(0, addrv6_third[6]);
  EXPECT_EQ(0, addrv6_third[7]);
  EXPECT_EQ(0, addrv6_third[8]);
  EXPECT_EQ(1, addrv6_third[9]);
  EXPECT_EQ(0, addrv6_third[10]);
  EXPECT_EQ(2, addrv6_third[11]);
  EXPECT_EQ(0, addrv6_third[12]);
  EXPECT_EQ(3, addrv6_third[13]);
  EXPECT_EQ(0, addrv6_third[14]);
  EXPECT_EQ(4, addrv6_third[15]);

  uint8_t addrv6_forth[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                              255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "1:2:3:4::1", &addrv6_forth));
  EXPECT_EQ(0, addrv6_forth[0]);
  EXPECT_EQ(1, addrv6_forth[1]);
  EXPECT_EQ(0, addrv6_forth[2]);
  EXPECT_EQ(2, addrv6_forth[3]);
  EXPECT_EQ(0, addrv6_forth[4]);
  EXPECT_EQ(3, addrv6_forth[5]);
  EXPECT_EQ(0, addrv6_forth[6]);
  EXPECT_EQ(4, addrv6_forth[7]);
  EXPECT_EQ(0, addrv6_forth[8]);
  EXPECT_EQ(0, addrv6_forth[9]);
  EXPECT_EQ(0, addrv6_forth[10]);
  EXPECT_EQ(0, addrv6_forth[11]);
  EXPECT_EQ(0, addrv6_forth[12]);
  EXPECT_EQ(0, addrv6_forth[13]);
  EXPECT_EQ(0, addrv6_forth[14]);
  EXPECT_EQ(1, addrv6_forth[15]);

  uint8_t addrv6_fifth[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                              255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "1:2F3D:4::1", &addrv6_fifth));
  EXPECT_EQ(0, addrv6_fifth[0]);
  EXPECT_EQ(1, addrv6_fifth[1]);
  EXPECT_EQ(0x2F, addrv6_fifth[2]);
  EXPECT_EQ(0x3D, addrv6_fifth[3]);
  EXPECT_EQ(0, addrv6_fifth[4]);
  EXPECT_EQ(4, addrv6_fifth[5]);
  EXPECT_EQ(0, addrv6_fifth[6]);
  EXPECT_EQ(0, addrv6_fifth[7]);
  EXPECT_EQ(0, addrv6_fifth[8]);
  EXPECT_EQ(0, addrv6_fifth[9]);
  EXPECT_EQ(0, addrv6_fifth[10]);
  EXPECT_EQ(0, addrv6_fifth[11]);
  EXPECT_EQ(0, addrv6_fifth[12]);
  EXPECT_EQ(0, addrv6_fifth[13]);
  EXPECT_EQ(0, addrv6_fifth[14]);
  EXPECT_EQ(1, addrv6_fifth[15]);

  uint8_t addrv6_sixth[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                              255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "AAAA:2:3:4::DDDD", &addrv6_sixth));
  EXPECT_EQ(0xAA, addrv6_sixth[0]);
  EXPECT_EQ(0xAA, addrv6_sixth[1]);
  EXPECT_EQ(0, addrv6_sixth[2]);
  EXPECT_EQ(2, addrv6_sixth[3]);
  EXPECT_EQ(0, addrv6_sixth[4]);
  EXPECT_EQ(3, addrv6_sixth[5]);
  EXPECT_EQ(0, addrv6_sixth[6]);
  EXPECT_EQ(4, addrv6_sixth[7]);
  EXPECT_EQ(0, addrv6_sixth[8]);
  EXPECT_EQ(0, addrv6_sixth[9]);
  EXPECT_EQ(0, addrv6_sixth[10]);
  EXPECT_EQ(0, addrv6_sixth[11]);
  EXPECT_EQ(0, addrv6_sixth[12]);
  EXPECT_EQ(0, addrv6_sixth[13]);
  EXPECT_EQ(0xDD, addrv6_sixth[14]);
  EXPECT_EQ(0xDD, addrv6_sixth[15]);

  uint8_t addrv6_seventh[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                                255, 255, 255, 255, 255, 255, 255, 255};
  EXPECT_EQ(1, inet_pton(AF_INET6, "aaaa:2:3:4CDE::DDDD", &addrv6_seventh));
  EXPECT_EQ(0xAA, addrv6_seventh[0]);
  EXPECT_EQ(0xAA, addrv6_seventh[1]);
  EXPECT_EQ(0, addrv6_seventh[2]);
  EXPECT_EQ(2, addrv6_seventh[3]);
  EXPECT_EQ(0, addrv6_seventh[4]);
  EXPECT_EQ(3, addrv6_seventh[5]);
  EXPECT_EQ(0x4C, addrv6_seventh[6]);
  EXPECT_EQ(0xDE, addrv6_seventh[7]);
  EXPECT_EQ(0, addrv6_seventh[8]);
  EXPECT_EQ(0, addrv6_seventh[9]);
  EXPECT_EQ(0, addrv6_seventh[10]);
  EXPECT_EQ(0, addrv6_seventh[11]);
  EXPECT_EQ(0, addrv6_seventh[12]);
  EXPECT_EQ(0, addrv6_seventh[13]);
  EXPECT_EQ(0xDD, addrv6_seventh[14]);
  EXPECT_EQ(0xDD, addrv6_seventh[15]);
}
