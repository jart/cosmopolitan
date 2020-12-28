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
#include "libc/str/sha256.h"
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
