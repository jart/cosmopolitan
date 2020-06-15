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
#include "libc/dns/dnsquestion.h"
#include "libc/errno.h"
#include "libc/testlib/testlib.h"

TEST(serializednsquestion, test) {
  uint8_t *buf = tmalloc(1 + 3 + 1 + 3 + 1 + 4);
  char *name = tstrdup("foo.bar");
  struct DnsQuestion dq;
  dq.qname = name;
  dq.qtype = 0x0201;
  dq.qclass = 0x0102;
  EXPECT_EQ(1 + 3 + 1 + 3 + 1 + 4,
            serializednsquestion(buf, 1 + 3 + 1 + 3 + 1 + 4, dq));
  EXPECT_BINEQ(u"♥foo♥bar ☻☺☺☻", buf);
  tfree(name);
  tfree(buf);
}

TEST(serializednsquestion, testNoSpace) {
  uint8_t *buf = tmalloc(1 + 3 + 1 + 3 + 1 + 3);
  char *name = tstrdup("foo.bar");
  struct DnsQuestion dq;
  dq.qname = name;
  dq.qtype = 0x0201;
  dq.qclass = 0x0102;
  EXPECT_EQ(-1, serializednsquestion(buf, 1 + 3 + 1 + 3 + 1 + 3, dq));
  EXPECT_EQ(ENOSPC, errno);
  tfree(name);
  tfree(buf);
}
