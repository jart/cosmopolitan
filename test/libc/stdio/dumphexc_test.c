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
#include "libc/mem/mem.h"
#include "libc/mem/gc.h"
#include "libc/stdio/hex.internal.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

TEST(DumpHexc, test) {
  EXPECT_STREQ("\"\\\n\
\\x68\\x65\\x6c\\x6c\\x6f\\xe2\\x86\\x92\\x0a\\x01\\x02\\x74\\x68\\x65\\x65\\x72\\\n\
\\x68\\x75\\x72\\x63\\x65\\x6f\\x61\\x68\\x72\\x63\\x75\\x6f\\x65\\x61\\x75\\x68\\\n\
\\x63\\x72\"",
               gc(DumpHexc("hello→\n\1\2theerhurceoahrcuoeauhcr", -1, 0)));
}

BENCH(DumpHexc, bench) {
  EZBENCH2("dumphexc", donothing, free(DumpHexc(kHyperion, kHyperionSize, 0)));
}
