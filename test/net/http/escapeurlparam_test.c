/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/mem/mem.h"
#include "libc/runtime/gc.internal.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "net/http/escape.h"

char *escapeparam(const char *s) {
  struct EscapeResult r;
  r = EscapeUrlParam(s, -1);
  ASSERT_EQ(strlen(r.data), r.size);
  return r.data;
}

TEST(EscapeUrlParam, test) {
  EXPECT_STREQ("abc%20%26%3C%3E%22%27%01%02",
               gc(escapeparam("abc &<>\"'\1\2")));
}

TEST(EscapeUrlParam, testLargeGrowth) {
  EXPECT_STREQ("%22%22%22", gc(escapeparam("\"\"\"")));
}

TEST(EscapeUrlParam, testEmpty) {
  EXPECT_STREQ("", gc(escapeparam("")));
}

TEST(EscapeUrlParam, testAstralPlanes_usesUtf8HexEncoding) {
  EXPECT_STREQ("%F0%90%8C%B0", escapeparam("𐌰"));
}

BENCH(EscapeUrlParam, bench) {
  EZBENCH2("EscapeUrlParam", donothing,
           free(EscapeUrlParam(kHyperion, kHyperionSize).data));
}
