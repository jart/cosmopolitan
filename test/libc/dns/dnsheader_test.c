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
#include "libc/dns/dns.h"
#include "libc/dns/dnsheader.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(serializednsheader, test) {
  struct DnsHeader header;
  memset(&header, 0, sizeof(header));
  header.id = 255;
  header.bf1 = true;
  header.qdcount = 1;
  uint8_t *buf = tmalloc(12);
  ASSERT_EQ(12, serializednsheader(buf, 12, header));
  EXPECT_BINEQ(u" λ☺  ☺      ", buf);
  tfree(buf);
}

TEST(serializednsheader, fuzzSymmetry) {
  uint8_t *buf = tmalloc(12);
  struct DnsHeader *in = tmalloc(sizeof(struct DnsHeader));
  struct DnsHeader *out = tmalloc(sizeof(struct DnsHeader));
  ASSERT_EQ(12, serializednsheader(buf, 12, *in));
  ASSERT_EQ(12, deserializednsheader(out, buf, 12));
  ASSERT_EQ(0, memcmp(in, out, 12));
  tfree(out);
  tfree(in);
  tfree(buf);
}
