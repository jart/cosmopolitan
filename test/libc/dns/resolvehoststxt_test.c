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
#include "libc/dns/hoststxt.h"
#include "libc/mem/mem.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/testlib/testlib.h"

static const char *EzIp4Lookup(const struct HostsTxt *ht, const char *name) {
  struct sockaddr_in addr4;
  if (resolvehoststxt(ht, AF_INET, name, (void *)&addr4,
                      sizeof(struct sockaddr_in), NULL) > 0) {
    static char g_ipbuf[16];
    return inet_ntop(AF_INET, &addr4.sin_addr, g_ipbuf, sizeof(g_ipbuf));
  } else {
    return NULL;
  }
}

static const char *EzCanonicalize(const struct HostsTxt *ht, const char *name) {
  const char *res;
  return resolvehoststxt(ht, AF_INET, name, NULL, 0, &res) > 0 ? res : NULL;
}

static const char kInput[] = "127.0.0.1	localhost\n"
                             "203.0.113.1     lol.example. lol\n"
                             "203.0.113.2     cat.example. cat\n";

TEST(resolvehoststxt, testBasicLookups) {
  struct HostsTxt *ht = calloc(1, sizeof(struct HostsTxt));
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(strlen(kInput), fwrite(kInput, 1, strlen(kInput), f));
  ASSERT_EQ(0, parsehoststxt(ht, f));
  sorthoststxt(ht);
  ASSERT_EQ(5, ht->entries.i);
  EXPECT_STREQ("127.0.0.1", EzIp4Lookup(ht, "localhost"));
  EXPECT_STREQ("203.0.113.1", EzIp4Lookup(ht, "lol"));
  EXPECT_STREQ("203.0.113.1", EzIp4Lookup(ht, "lol.example"));
  EXPECT_STREQ("203.0.113.1", EzIp4Lookup(ht, "lol.example."));
  EXPECT_STREQ("203.0.113.2", EzIp4Lookup(ht, "cat"));
  EXPECT_STREQ("203.0.113.2", EzIp4Lookup(ht, "cat.example."));
  EXPECT_EQ(NULL, EzIp4Lookup(ht, "boop"));
  freehoststxt(&ht);
  fclose(f);
}

TEST(resolvehoststxt, testCanonicalize) {
  struct HostsTxt *ht = calloc(1, sizeof(struct HostsTxt));
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(strlen(kInput), fwrite(kInput, 1, strlen(kInput), f));
  ASSERT_EQ(0, parsehoststxt(ht, f));
  sorthoststxt(ht);
  ASSERT_EQ(5, ht->entries.i);
  EXPECT_STREQ("localhost", EzCanonicalize(ht, "localhost"));
  EXPECT_STREQ("lol.example.", EzCanonicalize(ht, "lol"));
  EXPECT_STREQ("lol.example.", EzCanonicalize(ht, "lol.example"));
  EXPECT_STREQ("lol.example.", EzCanonicalize(ht, "lol.example."));
  EXPECT_STREQ("cat.example.", EzCanonicalize(ht, "cat"));
  EXPECT_STREQ("cat.example.", EzCanonicalize(ht, "cat.example."));
  EXPECT_EQ(NULL, EzCanonicalize(ht, "boop"));
  freehoststxt(&ht);
  fclose(f);
}
