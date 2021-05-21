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

TEST(ParseHostsTxt, testEmpty) {
  struct HostsTxt *ht = calloc(1, sizeof(struct HostsTxt));
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(0, ParseHostsTxt(ht, f));
  ASSERT_EQ(0, ht->entries.i);
  FreeHostsTxt(&ht);
  fclose(f);
}

TEST(ParseHostsTxt, testCorrectlyTokenizesAndSorts) {
  const char kInput[] = "# this is a comment\n"
                        "# IP            HOST1 HOST2\n"
                        "203.0.113.1     lol.example. lol\n"
                        "203.0.113.2     cat.example. cat\n";
  struct HostsTxt *ht = calloc(1, sizeof(struct HostsTxt));
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(1, fwrite(kInput, strlen(kInput), 1, f));
  rewind(f);
  ASSERT_EQ(0, ParseHostsTxt(ht, f));
  SortHostsTxt(ht, HOSTSTXT_SORTEDBYNAME);
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
  FreeHostsTxt(&ht);
  fclose(f);
}

TEST(ParseHostsTxt, testIpv6_isIgnored) {
  const char kInput[] = "::1             boop\n"
                        "203.0.113.2     cat     # ignore me\n";
  struct HostsTxt *ht = calloc(1, sizeof(struct HostsTxt));
  FILE *f = fmemopen(kInput, strlen(kInput), "r+");
  ASSERT_EQ(0, ParseHostsTxt(ht, f));
  ASSERT_EQ(1, ht->entries.i);
  EXPECT_STREQ("cat", &ht->strings.p[ht->entries.p[0].name]);
  EXPECT_STREQ("cat", &ht->strings.p[ht->entries.p[0].canon]);
  EXPECT_STREQ("203.0.113.2", ParseIp(ht->entries.p[0].ip));
  FreeHostsTxt(&ht);
  fclose(f);
}
