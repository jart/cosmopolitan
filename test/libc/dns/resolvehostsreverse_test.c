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
#include "libc/dns/hoststxt.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/testlib/testlib.h"

TEST(ParseHostsTxt, testNotFound) {
  const char kInput[] = "# this is a comment\n"
                        "# IP            HOST1 HOST2\n"
                        "203.0.113.1     lol.example lol\n"
                        "203.0.113.2     cat.example cat\n";
  char name[256];
  uint8_t ip[4] = {127, 0, 113, 1};
  struct HostsTxt *ht = calloc(1, sizeof(struct HostsTxt));
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(1, fwrite(kInput, strlen(kInput), 1, f));
  rewind(f);
  ASSERT_EQ(0, ParseHostsTxt(ht, f));
  ASSERT_EQ(4, ht->entries.i);
  ASSERT_EQ(0, ResolveHostsReverse(ht, AF_INET, ip, name, sizeof(name)));
  FreeHostsTxt(&ht);
  fclose(f);
}

TEST(ParseHostsTxt, testFirstLookup) {
  const char kInput[] = "# this is a comment\n"
                        "# IP            HOST1 HOST2\n"
                        "203.0.113.1     lol.example lol\n"
                        "203.0.113.2     cat.example cat\n";
  char name[256];
  uint8_t ip[4] = {203, 0, 113, 1};
  struct HostsTxt *ht = calloc(1, sizeof(struct HostsTxt));
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(1, fwrite(kInput, strlen(kInput), 1, f));
  rewind(f);
  ASSERT_EQ(0, ParseHostsTxt(ht, f));
  ASSERT_EQ(4, ht->entries.i);
  ASSERT_EQ(1, ResolveHostsReverse(ht, AF_INET, ip, name, sizeof(name)));
  EXPECT_STREQ("lol.example", name);
  FreeHostsTxt(&ht);
  fclose(f);
}

TEST(ParseHostsTxt, testSecondLookup) {
  const char kInput[] = "# this is a comment\n"
                        "# IP            HOST1 HOST2\n"
                        "203.0.113.1     lol.example lol\n"
                        "203.0.113.2     cat.example cat\n";
  char name[256];
  uint8_t ip[4] = {203, 0, 113, 2};
  struct HostsTxt *ht = calloc(1, sizeof(struct HostsTxt));
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(1, fwrite(kInput, strlen(kInput), 1, f));
  rewind(f);
  ASSERT_EQ(0, ParseHostsTxt(ht, f));
  ASSERT_EQ(4, ht->entries.i);
  ASSERT_EQ(1, ResolveHostsReverse(ht, AF_INET, ip, name, sizeof(name)));
  EXPECT_STREQ("cat.example", name);
  FreeHostsTxt(&ht);
  fclose(f);
}
