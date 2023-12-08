/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/calls.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/check.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sock.h"
#include "net/http/http.h"
#include "net/http/ip.h"

/**
 * @fileoverview tcp/udp echo servers/clients
 * use it to fill your network with junk data
 */

int sock;
char buf[1000];
struct sockaddr_in addr = {0};
uint32_t addrsize = sizeof(struct sockaddr_in);

void PrintUsage(char **argv) {
  kprintf("usage: %s udp server [ip port]%n", argv[0]);
  kprintf("       %s tcp server [ip port]%n", argv[0]);
  kprintf("       %s udp client [ip port]%n", argv[0]);
  kprintf("       %s tcp client [ip port]%n", argv[0]);
  exit(1);
}

void UdpServer(void) {
  int ip;
  ssize_t rc;
  struct sockaddr_in addr2;
  uint32_t addrsize2 = sizeof(struct sockaddr_in);
  CHECK_NE(-1, (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)));
  CHECK_NE(-1, bind(sock, (struct sockaddr *)&addr, addrsize));
  CHECK_NE(-1, getsockname(sock, (struct sockaddr *)&addr2, &addrsize2));
  ip = ntohl(addr2.sin_addr.s_addr);
  kprintf("udp server %hhu.%hhu.%hhu.%hhu %hu%n", ip >> 24, ip >> 16, ip >> 8,
          ip, ntohs(addr2.sin_port));
  for (;;) {
    CHECK_NE(-1, (rc = recvfrom(sock, buf, sizeof(buf), 0,
                                (struct sockaddr *)&addr2, &addrsize2)));
    CHECK_NE(-1,
             sendto(sock, buf, rc, 0, (struct sockaddr *)&addr2, addrsize2));
  }
}

void UdpClient(void) {
  CHECK_NE(-1, (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)));
  CHECK_NE(-1, connect(sock, (struct sockaddr *)&addr, addrsize));
  for (;;) {
    rngset(buf, sizeof(buf), _rand64, -1);
    CHECK_NE(-1, write(sock, &addr, addrsize));
  }
}

void TcpServer(void) {
  ssize_t rc;
  int ip, client;
  struct sockaddr_in addr2;
  uint32_t addrsize2 = sizeof(struct sockaddr_in);
  CHECK_NE(-1, (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)));
  CHECK_NE(-1, bind(sock, (struct sockaddr *)&addr, addrsize));
  CHECK_NE(-1, listen(sock, 10));
  CHECK_NE(-1, getsockname(sock, (struct sockaddr *)&addr2, &addrsize2));
  ip = ntohl(addr2.sin_addr.s_addr);
  kprintf("tcp server %hhu.%hhu.%hhu.%hhu %hu%n", ip >> 24, ip >> 16, ip >> 8,
          ip, ntohs(addr2.sin_port));
  for (;;) {
    addrsize2 = sizeof(struct sockaddr_in);
    CHECK_NE(-1,
             (client = accept(sock, (struct sockaddr *)&addr2, &addrsize2)));
    ip = ntohl(addr2.sin_addr.s_addr);
    kprintf("got client %hhu.%hhu.%hhu.%hhu %hu%n", ip >> 24, ip >> 16, ip >> 8,
            ip, ntohs(addr2.sin_port));
    for (;;) {
      CHECK_NE(-1, (rc = read(client, buf, sizeof(buf))));
      if (!rc) break;
      CHECK_NE(-1, write(client, buf, rc));
    }
  }
}

void TcpClient(void) {
  CHECK_NE(-1, (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)));
  CHECK_NE(-1, connect(sock, (struct sockaddr *)&addr, addrsize));
  for (;;) {
    rngset(buf, sizeof(buf), _rand64, -1);
    CHECK_NE(-1, write(sock, buf, sizeof(buf)));
    CHECK_NE(-1, read(sock, buf, sizeof(buf)));
  }
}

int main(int argc, char *argv[]) {
  int port = 0;
  int64_t ip = 0;
  if (argc < 3) PrintUsage(argv);
  if (argc >= 4) {
    if ((ip = ParseIp(argv[3], -1)) == -1) {
      PrintUsage(argv);
    }
  }
  if (argc >= 5) {
    port = atoi(argv[4]);
    if (port & 0xffff0000) {
      PrintUsage(argv);
    }
  }
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(ip);
  if (!strcmp(argv[1], "udp")) {
    if (!strcmp(argv[2], "server")) {
      UdpServer();
    } else if (!strcmp(argv[2], "client")) {
      UdpClient();
    } else {
      PrintUsage(argv);
    }
  } else if (!strcmp(argv[1], "tcp")) {
    if (!strcmp(argv[2], "server")) {
      TcpServer();
    } else if (!strcmp(argv[2], "client")) {
      TcpClient();
    } else {
      PrintUsage(argv);
    }
  } else {
    PrintUsage(argv);
  }
  return 0;
}
