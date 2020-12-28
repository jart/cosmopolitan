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
#include "libc/dns/resolvconf.h"
#include "libc/mem/mem.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/testlib/testlib.h"

static const char *FormatIp(struct sockaddr_in *ip) {
  static char g_ipbuf[16];
  return inet_ntop(ip->sin_family, &ip->sin_addr.s_addr, g_ipbuf, 16);
}

TEST(parseresolvconf, testEmpty) {
  struct ResolvConf *rv = calloc(1, sizeof(struct ResolvConf));
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(0, parseresolvconf(rv, f));
  ASSERT_EQ(0, rv->nameservers.i);
  freeresolvconf(&rv);
  fclose(f);
}

TEST(parseresolvconf, testCorrectlyTokenizes) {
  const char kInput[] = "# this is a comment\n"
                        "nameserver  203.0.113.2 \n"
                        " nameserver  203.0.113.1\n";
  struct ResolvConf *rv = calloc(1, sizeof(struct ResolvConf));
  FILE *f = fmemopen(kInput, strlen(kInput), "r+");
  ASSERT_EQ(2, parseresolvconf(rv, f));
  ASSERT_EQ(2, rv->nameservers.i);
  EXPECT_EQ(AF_INET, rv->nameservers.p[0].sin_family);
  EXPECT_EQ(DNS_PORT, ntohs(rv->nameservers.p[0].sin_port));
  EXPECT_STREQ("203.0.113.2", FormatIp(&rv->nameservers.p[0]));
  EXPECT_EQ(AF_INET, rv->nameservers.p[1].sin_family);
  EXPECT_EQ(DNS_PORT, ntohs(rv->nameservers.p[1].sin_port));
  EXPECT_STREQ("203.0.113.1", FormatIp(&rv->nameservers.p[1]));
  freeresolvconf(&rv);
  fclose(f);
}

TEST(parseresolvconf, testMulticastDnsThing_getsIgnored) {
  const char kInput[] = "search local # boop\n";
  struct ResolvConf *rv = calloc(1, sizeof(struct ResolvConf));
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(strlen(kInput), fwrite(kInput, 1, strlen(kInput), f));
  ASSERT_EQ(0, parseresolvconf(rv, f));
  ASSERT_EQ(0, rv->nameservers.i);
  freeresolvconf(&rv);
  fclose(f);
}
