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
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

uint8_t *sha256(const char *s) {
  static uint8_t hash[32];
  struct Sha256Ctx ctx;
  sha256_init(&ctx);
  sha256_update(&ctx, (const uint8_t *)s, strlen(s));
  sha256_final(&ctx, hash);
  return hash;
}

TEST(sha256, testEmpty) {
  EXPECT_BINEQ(
      "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
      sha256(""));
}

TEST(sha256, test) {
  EXPECT_BINEQ(u",≥M║_░ú♫&Φ;*┼╣Γ€←▬▲\\▼ºB^s♦3bôïÿ$", sha256("hello"));
  EXPECT_BINEQ("2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b98",
               sha256("hello"));
}

TEST(sha256, testNontrivialSize) {
  EXPECT_BINEQ(u"╨╒║☺ª↨╨╒ù€»╝∞nfÑ4Æ╒Tn╫╕`eóA¿↑[3╬", sha256(kHyperion));
}
