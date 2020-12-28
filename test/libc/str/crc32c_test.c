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
#include "libc/dce.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

#define FANATICS "Fanatics"

static const char hyperion[] =
    FANATICS " have their dreams, wherewith they weave / "
             "A paradise for a sect; the savage too / "
             "From forth the loftiest fashion of his sleep / "
             "...";

TEST(crc32c, test) {
  EXPECT_EQ(0, crc32c(0, "", 0));
  EXPECT_EQ(crc32c(0, "hello", 5), crc32c(0, "hello", 5));
  EXPECT_EQ(0xe3069283, crc32c(0, "123456789", 9));
  EXPECT_EQ(0x6d6eefba, crc32c(0, hyperion, strlen(hyperion)));
  EXPECT_EQ(0x6d6eefba, crc32c(crc32c(0, FANATICS, strlen(FANATICS)),
                               hyperion + strlen(FANATICS),
                               strlen(hyperion) - strlen(FANATICS)));
}

FIXTURE(crc32c, pure) {
  *(void **)(&crc32c) = (void *)crc32c$pure;
}

FIXTURE(crc32c, sse42) {
  if (X86_HAVE(SSE4_2)) {
    *(void **)(&crc32c) = (void *)crc32c$sse42;
  }
}
