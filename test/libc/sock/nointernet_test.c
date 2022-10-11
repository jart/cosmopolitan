/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/msghdr.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/testlib.h"

char testlib_enable_tmp_setup_teardown;

void SetUpOnce(void) {
  if (nointernet() == -1) {
    ASSERT_TRUE(errno == EPERM ||  // already traced
                errno == ENOSYS);  // non-linux or ancient linux
    if (errno == ENOSYS) {
      exit(0);
    }
  }
}

TEST(nointernet, testLocalhost_isAllowed) {
  struct sockaddr_in addr = {AF_INET, 0, {htonl(0x7f000001)}};
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(ECONNREFUSED, -1,
             connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0, close(3));
}

TEST(nointernet, testPublicInternet_isDenied) {
  struct sockaddr_in addr = {AF_INET, 0, {htonl(0x06060600)}};
  ASSERT_SYS(EPERM, -1, socket(AF_BLUETOOTH, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(ENOSYS, -1, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(ENOSYS, -1, connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0, close(3));
}

TEST(nointernet, sendmsgPrivateNetwork_doesntGetBlocked) {
  struct msghdr msg = {
      .msg_name = &(struct sockaddr_in){AF_INET, 31337, {htonl(0x0a000001)}},
      .msg_namelen = sizeof(struct sockaddr_in),
      .msg_iov = &(struct iovec){"hi", 2},
      .msg_iovlen = 1,
  };
  ASSERT_SYS(EBADF, -1, sendmsg(-1, &msg, 0));
}

TEST(nointernet, sendmsgPublicNetwork_raisesEnosys_whichPreemptsEbadf) {
  struct msghdr msg = {
      .msg_name = &(struct sockaddr_in){AF_INET, 31337, {htonl(0x06060600)}},
      .msg_namelen = sizeof(struct sockaddr_in),
      .msg_iov = &(struct iovec){"hi", 2},
      .msg_iovlen = 1,
  };
  ASSERT_SYS(ENOSYS, -1, sendmsg(-1, &msg, 0));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
  ASSERT_SYS(ENOSYS, -1, sendmsg(3, &msg, 0));
  ASSERT_SYS(0, 0, close(3));
}
