/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "third_party/musl/crypt.h"

TEST(crypt, test) {
  // consistent with python crypt.crypt()
  EXPECT_STREQ("thXpd0YFlQG2w", crypt("hello", "there"));
  EXPECT_STREQ("$1$there$3B/lVCUlX8R18HDBuijby1", crypt("hello", "$1$there"));
  EXPECT_STREQ("$2aVOhEz8P7i6", crypt("hello", "$2$there"));
  EXPECT_STREQ("$5$there$.u5mdR0jvLs0jEf7qHTG98t8la1KVhEBH3eOFZ7ztL0",
               crypt("hello", "$5$there"));
  EXPECT_STREQ(
      "$6$there$y0TGuPcNSR23fFWCwYUj6VUfhnc9nlnkm6Y8waSLnANwCUcxK6esd7xm7.Jl."
      "jjr1/sPTaRK7igDNSxC.BhgX/",
      crypt("hello", "$6$there"));
}
