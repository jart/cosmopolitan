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
#include "libc/dce.h"
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

uint32_t crc32c$pure(uint32_t, const char *, size_t) hidden;
uint32_t crc32c$sse42(uint32_t, const char *, size_t) hidden;
FIXTURE(crc32c, pure) { *(void **)(&crc32c) = (void *)crc32c$pure; }
FIXTURE(crc32c, sse42) {
  if (X86_HAVE(SSE4_2)) {
    *(void **)(&crc32c) = (void *)crc32c$sse42;
  }
}
