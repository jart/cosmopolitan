#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/fmt/conv.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"

/**
 * @fileoverview netcat clone
 *
 * Implemented because BusyBox's netcat doesn't detect remote close and
 * lingers in the CLOSE_WAIT wait possibly due to file descriptor leaks
 *
 * Once upon time we called this command "Telnet"
 */

int main(int argc, char *argv[]) {
  ssize_t rc;
  size_t i, got;
  char buf[1500];
  int err, toto, sock;
  struct linger linger = {true, 1};
  struct sockaddr_in addr = {AF_INET};
  struct pollfd fds[2] = {{-1, POLLIN}, {-1, POLLIN}};

  if (argc != 3) exit(1);
  inet_pton(AF_INET, argv[1], &addr.sin_addr);
  addr.sin_port = htons(atoi(argv[2]));

  if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("socket");
    exit(1);
  }

  if (setsockopt(sock, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)) == -1) {
    perror("setsockopt(SO_LINGER)");
    exit(1);
  }

  if (connect(sock, &addr, sizeof(addr)) == -1) {
    perror("connect");
    exit(1);
  }

  fds[0].fd = 0;
  fds[1].fd = sock;
  for (;;) {
    fds[0].revents = 0;
    fds[1].revents = 0;
    if (poll(fds, ARRAYLEN(fds), -1) == -1) {
      perror("poll");
      exit(1);
    }

    if (fds[0].revents & (POLLIN | POLLERR | POLLHUP)) {
      if ((rc = read(0, buf, 1400)) == -1) {
        perror("read(stdin)");
        exit(1);
      }
      if (!(got = rc)) {
        shutdown(sock, SHUT_WR);
        fds[0].fd = -1;
      }
      for (i = 0; i < got; i += rc) {
        if ((rc = write(sock, buf + i, got - i)) == -1) {
          perror("write(sock)");
          exit(1);
        }
      }
    }

    if (fds[1].revents & (POLLIN | POLLERR | POLLHUP)) {
      if ((rc = read(sock, buf, 1500)) == -1) {
        perror("read(sock)");
        exit(1);
      }
      if (!(got = rc)) {
        break;
      }
      for (i = 0; i < got; i += rc) {
        if ((rc = write(1, buf + i, got - i)) == -1) {
          perror("write(stdout)");
          exit(1);
        }
      }
    }
  }

  if (close(sock) == -1) {
    perror("close");
    exit(1);
  }

  return 0;
}
