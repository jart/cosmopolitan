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
#include "libc/alg/arraylist.internal.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/interruptiblecall.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/msg.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"

/**
 * @fileoverview Asynchronous TCP/UDP Echo Server.
 *
 *   make -j8 o/default/tool/net/echoserver.com
 *   o/default/tool/net/echoserver.com udp:0.0.0.0:7 tcp:0.0.0.0:7
 */

enum SocketKind {
  kSocketServer,
  kSocketClient,
};

struct Message {
  struct iovec data;
  struct sockaddr_in dest;
  uint32_t destsize;
};

struct Messages {
  size_t i, n;
  struct Message *p;
};

struct Socket {
  int64_t fd;
  enum SocketKind kind;
  int type;
  int protocol;
  struct sockaddr_in addr;
  struct Messages egress; /* LIFO */
};

struct Sockets {
  size_t i, n;
  struct Socket *p;
};

struct Polls {
  size_t i, n;
  struct pollfd *p;
};

struct Sockets g_sockets;
struct Polls g_polls;

nodiscard char *DescribeAddress(struct sockaddr_in *addr) {
  char ip4buf[16];
  return xasprintf("%s:%hu",
                   inet_ntop(addr->sin_family, &addr->sin_addr.s_addr, ip4buf,
                             sizeof(ip4buf)),
                   ntohs(addr->sin_port));
}

nodiscard char *DescribeSocket(struct Socket *s) {
  return xasprintf("%s:%s", s->protocol == IPPROTO_UDP ? "udp" : "tcp",
                   gc(DescribeAddress(&s->addr)));
}

wontreturn void ShowUsageAndExit(bool iserror) {
  FILE *f = iserror ? stderr : stdout;
  int rc = iserror ? EXIT_FAILURE : EXIT_SUCCESS;
  fprintf(f, "%s: %s %s\n", "Usage", g_argv[0], "PROTOCOL:ADDR:PORT...");
  exit(rc);
}

void GetFlags(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "h")) != -1) {
    switch (opt) {
      case 'h':
        ShowUsageAndExit(false);
      default:
        ShowUsageAndExit(true);
    }
  }
  if (optind == argc) ShowUsageAndExit(true);
}

void AddSocket(const struct Socket *s) {
  struct pollfd pfd;
  pfd.fd = s->fd;
  pfd.events = POLLIN;
  pfd.revents = 0;
  CHECK_NE(-1L, append(&g_sockets, s));
  CHECK_NE(-1L, append(&g_polls, &pfd));
}

void RemoveSocket(size_t i) {
  DCHECK_LT(i, g_sockets.i);
  LOGF("removing: %s", gc(DescribeSocket(&g_sockets.p[i])));
  CHECK_NE(-1, close(g_sockets.p[i].fd));
  while (g_sockets.p[i].egress.i) {
    free(g_sockets.p[i].egress.p[g_sockets.p[i].egress.i - 1].data.iov_base);
  }
  memcpy(&g_sockets.p[i], &g_sockets.p[i + 1],
         (intptr_t)&g_sockets.p[g_sockets.i] - (intptr_t)&g_sockets.p[i + 1]);
  memcpy(&g_polls.p[i], &g_polls.p[i + 1],
         (intptr_t)&g_polls.p[g_polls.i] - (intptr_t)&g_polls.p[i + 1]);
  g_sockets.i--;
  g_polls.i--;
}

void GetListeningAddressesFromCommandLine(int argc, char *argv[]) {
  int i;
  for (i = optind; i < argc; ++i) {
    struct Socket server;
    memset(&server, 0, sizeof(server));
    char scheme[4];
    unsigned char *ip4 = (unsigned char *)&server.addr.sin_addr.s_addr;
    uint16_t port;
    if (sscanf(argv[i], "%3s:%hhu.%hhu.%hhu.%hhu:%hu", scheme, &ip4[0], &ip4[1],
               &ip4[2], &ip4[3], &port) != 6) {
      fprintf(stderr, "error: bad ip4 uri\n");
      ShowUsageAndExit(true);
    }
    server.fd = -1;
    server.kind = kSocketServer;
    server.addr.sin_family = AF_INET;
    server.addr.sin_port = htons(port);
    if (strcasecmp(scheme, "tcp") == 0) {
      server.type = SOCK_STREAM;
      server.protocol = IPPROTO_TCP;
    } else if (strcasecmp(scheme, "udp") == 0) {
      server.type = SOCK_DGRAM;
      server.protocol = IPPROTO_UDP;
    } else {
      fprintf(stderr, "%s: %s\n", "error", "bad scheme (should be tcp or udp)");
      ShowUsageAndExit(true);
    }
    AddSocket(&server);
  }
}

void BeginListeningForIncomingTraffic(void) {
  size_t i;
  for (i = 0; i < g_sockets.i; ++i) {
    int yes = 1;
    struct Socket *s = &g_sockets.p[i];
    CHECK_NE(-1L,
             (g_polls.p[i].fd = s->fd = socket(
                  s->addr.sin_family, s->type | SOCK_NONBLOCK, s->protocol)));
    CHECK_NE(-1L,
             setsockopt(s->fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)));
    CHECK_NE(-1, bind(s->fd, &s->addr, sizeof(s->addr)));
    if (s->protocol == IPPROTO_TCP) {
      CHECK_NE(-1, listen(s->fd, 1));
    }
    uint32_t addrsize = sizeof(s->addr);
    CHECK_NE(-1, getsockname(s->fd, &s->addr, &addrsize));
    LOGF("listening on %s", gc(DescribeSocket(s)));
  }
}

void AcceptConnection(size_t i) {
  struct Socket *server = &g_sockets.p[i];
  struct Socket client;
  memset(&client, 0, sizeof(client));
  client.kind = kSocketClient;
  client.type = server->type;
  client.protocol = server->protocol;
  uint32_t addrsize = sizeof(client.addr);
  CHECK_NE(-1L, (client.fd = accept4(server->fd, &client.addr, &addrsize,
                                     SOCK_NONBLOCK)));
  LOGF("%s accepted %s", gc(DescribeSocket(server)),
       gc(DescribeSocket(&client)));
  AddSocket(&client);
}

bool ReceiveData(size_t i) {
  ssize_t got;
  struct Message msg;
  bool isudp = g_sockets.p[i].protocol == IPPROTO_UDP;
  memset(&msg, 0, sizeof(msg));
  msg.destsize = sizeof(msg.dest);
  msg.data.iov_len = PAGESIZE;
  msg.data.iov_base = xmalloc(msg.data.iov_len);
  CHECK_NE(-1L, (got = recvfrom(g_sockets.p[i].fd, msg.data.iov_base,
                                msg.data.iov_len, 0, isudp ? &msg.dest : NULL,
                                isudp ? &msg.destsize : NULL)));
  if (0 < got && got <= msg.data.iov_len) {
    LOGF("%s received %lu bytes from %s", gc(DescribeSocket(&g_sockets.p[i])),
         got, gc(DescribeAddress(&msg.dest)));
    msg.data.iov_base = xrealloc(msg.data.iov_base, (msg.data.iov_len = got));
    append(&g_sockets.p[i].egress, &msg);
    g_polls.p[i].events |= POLLOUT;
    return true;
  } else {
    RemoveSocket(i);
    free_s(&msg.data.iov_base);
    return false;
  }
}

void SendData(size_t i) {
  ssize_t sent;
  struct Socket *s = &g_sockets.p[i];
  struct Message *msg = &s->egress.p[s->egress.i - 1];
  bool isudp = s->protocol == IPPROTO_UDP;
  DCHECK(s->egress.i);
  CHECK_NE(-1L, (sent = sendto(s->fd, msg->data.iov_base, msg->data.iov_len, 0,
                               isudp ? &msg->dest : NULL,
                               isudp ? msg->destsize : 0)));
  LOGF("%s sent %lu bytes to %s", gc(DescribeSocket(s)), msg->data.iov_len,
       gc(DescribeAddress(&msg->dest)));
  if (!(msg->data.iov_len -= min((size_t)sent, (size_t)msg->data.iov_len))) {
    free_s(&msg->data.iov_base);
    if (!--s->egress.i) {
      g_polls.p[i].events &= ~POLLOUT;
    }
  }
}

void HandleSomeNetworkTraffic(void) {
  size_t i;
  int eventcount;
  CHECK_GE((eventcount = poll(g_polls.p, g_polls.i, -1)), 0);
  for (i = 0; eventcount && i < g_sockets.i; ++i) {
    if (!g_polls.p[i].revents) continue;
    --eventcount;
    if (g_polls.p[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
      CHECK_EQ(kSocketClient, g_sockets.p[i].kind);
      RemoveSocket(i);
    } else {
      if (g_polls.p[i].revents & POLLIN) {
        if (g_sockets.p[i].kind == kSocketServer &&
            g_sockets.p[i].protocol == IPPROTO_TCP) {
          AcceptConnection(i);
        } else {
          if (!ReceiveData(i)) continue;
        }
      }
      if (g_polls.p[i].revents & POLLOUT) {
        SendData(i);
      }
    }
  }
}

void EchoServer(void) {
  for (;;) HandleSomeNetworkTraffic();
}

int main(int argc, char *argv[]) {
  STATIC_YOINK("isatty");
  GetFlags(argc, argv);
  GetListeningAddressesFromCommandLine(argc, argv);
  BeginListeningForIncomingTraffic();
  struct InterruptibleCall icall;
  memset(&icall, 0, sizeof(icall));
  interruptiblecall(&icall, (void *)EchoServer, 0, 0, 0, 0);
  fputc('\r', stderr);
  LOGF("%s", "shutting down...");
  size_t i;
  for (i = g_sockets.i; i; --i) RemoveSocket(i - 1);
  return 0;
}
