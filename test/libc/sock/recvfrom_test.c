/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

// two clients send a udp packet containing their local address
// server verifies content of packet matches the peer's address
TEST(recvfrom, test) {
  uint32_t addrsize = sizeof(struct sockaddr_in);
  struct sockaddr_in server = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&server, sizeof(server)));
  ASSERT_SYS(0, 0, getsockname(3, (struct sockaddr *)&server, &addrsize));

  ////////////////////////////////////////////////////////////////////////////////
  SPAWN(fork);
  struct sockaddr_in data, addr;
  ASSERT_SYS(
      0, sizeof(data),
      recvfrom(3, &data, sizeof(data), 0, (struct sockaddr *)&addr, &addrsize));
  ASSERT_EQ(sizeof(data), addrsize);
  ASSERT_EQ(0, memcmp(&data, &addr, addrsize));
  ASSERT_SYS(
      0, sizeof(data),
      recvfrom(3, &data, sizeof(data), 0, (struct sockaddr *)&addr, &addrsize));
  ASSERT_EQ(sizeof(data), addrsize);
  ASSERT_EQ(0, memcmp(&data, &addr, addrsize));

  ////////////////////////////////////////////////////////////////////////////////
  PARENT();
  EXPECT_SYS(0, 0, close(3));

  int client1;
  struct sockaddr_in client1_addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  ASSERT_NE(-1, (client1 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)));
  ASSERT_SYS(0, 0, bind(client1, (struct sockaddr *)&client1_addr, addrsize));
  ASSERT_SYS(0, 0,
             getsockname(client1, (struct sockaddr *)&client1_addr, &addrsize));

  int client2;
  struct sockaddr_in client2_addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  ASSERT_NE(-1, (client2 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)));
  ASSERT_SYS(0, 0, bind(client2, (struct sockaddr *)&client2_addr, addrsize));
  ASSERT_SYS(0, 0,
             getsockname(client2, (struct sockaddr *)&client2_addr, &addrsize));

  ASSERT_SYS(0, addrsize,
             sendto(client1, (struct sockaddr *)&client1_addr, addrsize, 0,
                    (struct sockaddr *)&server, addrsize));
  ASSERT_SYS(0, addrsize,
             sendto(client2, (struct sockaddr *)&client2_addr, addrsize, 0,
                    (struct sockaddr *)&server, addrsize));

  EXPECT_SYS(0, 0, close(client2));
  EXPECT_SYS(0, 0, close(client1));
  WAIT(exit, 0);
}

// server listens for connections, accepts a connection, and sends data
// client connects to server recieves with recvfrom and verifies addrsize
// is 0 as the sender info isn't available on connection sockets.
TEST(recvfrom, tcp) {
  uint32_t addrsize = sizeof(struct sockaddr_in);
  struct sockaddr_in server = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&server, sizeof(server)));
  ASSERT_SYS(0, 0, getsockname(3, (struct sockaddr *)&server, &addrsize));
  ASSERT_SYS(0, 0, listen(3, 5));

  ////////////////////////////////////////////////////////////////////////////////
  SPAWN(fork);
  struct sockaddr_in data, addr;
  uint32_t addrsize = sizeof(struct sockaddr_in);
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, connect(3, (struct sockaddr *)&server, sizeof(server)));
  ASSERT_SYS(
      0, sizeof(data),
      recvfrom(3, &data, sizeof(data), 0, (struct sockaddr *)&addr, &addrsize));
  ASSERT_EQ(0, addrsize);
  EXPECT_SYS(0, 0, close(3));

  ////////////////////////////////////////////////////////////////////////////////
  PARENT();
  int client;
  struct sockaddr client_sockaddr;
  uint32_t sockaddr_size = sizeof(client_sockaddr);
  ASSERT_NE(-1, (client = accept(3, &client_sockaddr, &sockaddr_size)));
  ASSERT_SYS(0, sizeof(client_sockaddr),
             sendto(client, &client_sockaddr, sizeof(client_sockaddr), 0,
                    (struct sockaddr *)&server, sizeof(server)));
  EXPECT_SYS(0, 0, close(client));
  WAIT(exit, 0);
  EXPECT_SYS(0, 0, close(3));
}
