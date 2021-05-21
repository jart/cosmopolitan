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
  if (ResolveHostsTxt(ht, AF_INET, name, (void *)&addr4,
                      sizeof(struct sockaddr_in), NULL) > 0) {
    static char g_ipbuf[16];
    return inet_ntop(AF_INET, &addr4.sin_addr, g_ipbuf, sizeof(g_ipbuf));
  } else {
    return NULL;
  }
}

static const char *EzCanonicalize(const struct HostsTxt *ht, const char *name) {
  const char *res;
  return ResolveHostsTxt(ht, AF_INET, name, NULL, 0, &res) > 0 ? res : NULL;
}

static const char kInput[] = "127.0.0.1	localhost\n"
                             "203.0.113.1     lol.example. lol\n"
                             "203.0.113.2     cat.example. cat\n";

TEST(ResolveHostsTxt, testBasicLookups) {
  struct HostsTxt *ht = calloc(1, sizeof(struct HostsTxt));
  FILE *f = fmemopen(kInput, strlen(kInput), "r+");
  ASSERT_EQ(0, ParseHostsTxt(ht, f));
  SortHostsTxt(ht, HOSTSTXT_SORTEDBYNAME);
  ASSERT_EQ(5, ht->entries.i);
  EXPECT_STREQ("127.0.0.1", EzIp4Lookup(ht, "localhost"));
  EXPECT_STREQ("203.0.113.1", EzIp4Lookup(ht, "lol"));
  EXPECT_STREQ("203.0.113.1", EzIp4Lookup(ht, "lol.example"));
  EXPECT_STREQ("203.0.113.1", EzIp4Lookup(ht, "lol.example."));
  EXPECT_STREQ("203.0.113.2", EzIp4Lookup(ht, "cat"));
  EXPECT_STREQ("203.0.113.2", EzIp4Lookup(ht, "cat.example."));
  EXPECT_EQ(NULL, EzIp4Lookup(ht, "boop"));
  FreeHostsTxt(&ht);
  fclose(f);
}

TEST(ResolveHostsTxt, testCanonicalize) {
  struct HostsTxt *ht = calloc(1, sizeof(struct HostsTxt));
  FILE *f = fmemopen(kInput, strlen(kInput), "r+");
  ASSERT_EQ(0, ParseHostsTxt(ht, f));
  SortHostsTxt(ht, HOSTSTXT_SORTEDBYNAME);
  ASSERT_EQ(5, ht->entries.i);
  EXPECT_STREQ("localhost", EzCanonicalize(ht, "localhost"));
  EXPECT_STREQ("lol.example.", EzCanonicalize(ht, "lol"));
  EXPECT_STREQ("lol.example.", EzCanonicalize(ht, "lol.example"));
  EXPECT_STREQ("lol.example.", EzCanonicalize(ht, "lol.example."));
  EXPECT_STREQ("cat.example.", EzCanonicalize(ht, "cat"));
  EXPECT_STREQ("cat.example.", EzCanonicalize(ht, "cat.example."));
  EXPECT_EQ(NULL, EzCanonicalize(ht, "boop"));
  FreeHostsTxt(&ht);
  fclose(f);
}
