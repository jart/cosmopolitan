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
#include "libc/dns/hoststxt.h"
#include "libc/mem/mem.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/testlib/testlib.h"

static const char *ParseIp(unsigned char ip[4]) {
  static char g_ipbuf[16];
  return inet_ntop(AF_INET, ip, g_ipbuf, sizeof(g_ipbuf));
}

TEST(parsehoststxt, testEmpty) {
  struct HostsTxt *ht = calloc(1, sizeof(struct HostsTxt));
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(0, parsehoststxt(ht, f));
  ASSERT_EQ(0, ht->entries.i);
  freehoststxt(&ht);
  fclose(f);
}

TEST(parsehoststxt, testCorrectlyTokenizesAndSorts) {
  const char kInput[] =
      "# this is a comment\n"
      "# IP            HOST1 HOST2\n"
      "203.0.113.1     lol.example. lol\n"
      "203.0.113.2     cat.example. cat\n";
  struct HostsTxt *ht = calloc(1, sizeof(struct HostsTxt));
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(strlen(kInput), fwrite(kInput, 1, strlen(kInput), f));
  ASSERT_EQ(0, parsehoststxt(ht, f));
  sorthoststxt(ht);
  ASSERT_EQ(4, ht->entries.i);
  EXPECT_STREQ("cat.example.", &ht->strings.p[ht->entries.p[0].name]);
  EXPECT_STREQ("cat.example.", &ht->strings.p[ht->entries.p[0].canon]);
  EXPECT_STREQ("203.0.113.2", ParseIp(ht->entries.p[0].ip));
  EXPECT_STREQ("lol.example.", &ht->strings.p[ht->entries.p[1].name]);
  EXPECT_STREQ("lol.example.", &ht->strings.p[ht->entries.p[1].canon]);
  EXPECT_STREQ("203.0.113.1", ParseIp(ht->entries.p[1].ip));
  EXPECT_STREQ("cat", &ht->strings.p[ht->entries.p[2].name]);
  EXPECT_STREQ("cat.example.", &ht->strings.p[ht->entries.p[2].canon]);
  EXPECT_STREQ("203.0.113.2", ParseIp(ht->entries.p[2].ip));
  EXPECT_STREQ("lol", &ht->strings.p[ht->entries.p[3].name]);
  EXPECT_STREQ("lol.example.", &ht->strings.p[ht->entries.p[3].canon]);
  EXPECT_STREQ("203.0.113.1", ParseIp(ht->entries.p[3].ip));
  freehoststxt(&ht);
  fclose(f);
}

TEST(parsehoststxt, testIpv6_isIgnored) {
  const char kInput[] =
      "::1             boop\n"
      "203.0.113.2     cat     # ignore me\n";
  struct HostsTxt *ht = calloc(1, sizeof(struct HostsTxt));
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(strlen(kInput), fwrite(kInput, 1, strlen(kInput), f));
  ASSERT_EQ(0, parsehoststxt(ht, f));
  ASSERT_EQ(1, ht->entries.i);
  EXPECT_STREQ("cat", &ht->strings.p[ht->entries.p[0].name]);
  EXPECT_STREQ("cat", &ht->strings.p[ht->entries.p[0].canon]);
  EXPECT_STREQ("203.0.113.2", ParseIp(ht->entries.p[0].ip));
  freehoststxt(&ht);
  fclose(f);
}
