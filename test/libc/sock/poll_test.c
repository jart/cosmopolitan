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
#include "libc/log/libfatal.internal.h"
#include "libc/runtime/gc.internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "tool/decode/lib/flagger.h"
#include "tool/decode/lib/pollnames.h"

nodiscard char *FormatPollFd(struct pollfd p[2]) {
  return xasprintf("fd:%d revents:%s\n"
                   "fd:%d revents:%s\n",
                   p[0].fd, gc(RecreateFlags(kPollNames, p[0].revents)),
                   p[1].fd, gc(RecreateFlags(kPollNames, p[1].revents)));
}

TEST(poll, testNegativeOneFd_isIgnored) {
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  struct sockaddr_in addr = {AF_INET, 0, {htonl(INADDR_LOOPBACK)}};
  ASSERT_SYS(0, 0, bind(3, &addr, sizeof(addr)));
  ASSERT_SYS(0, 0, listen(3, 10));
  struct pollfd fds[] = {{-1}, {3}};
  EXPECT_SYS(0, 0, poll(fds, ARRAYLEN(fds), 1));
  EXPECT_STREQ("fd:-1 revents:0\n"
               "fd:3 revents:0\n",
               gc(FormatPollFd(&fds[0])));
  ASSERT_SYS(0, 0, close(3));
}
