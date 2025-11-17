/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "third_party/getopt/getopt.internal.h"

#define HELP "usage: stampd [FLAGS]\n"

[[noreturn]] void Usage(int rc, FILE* f) {
  fputs(HELP, f);
  exit(rc);
}

[[noreturn]] void Die(const char* thing, const char* reason) {
  fputs(thing, stderr);
  fputs(": fatal error: ", stderr);
  fputs(reason, stderr);
  fputs("\n", stderr);
  exit(1);
}

[[noreturn]] void DieSys(const char* thing) {
  Die(thing, strerror(errno));
}

int main(int argc, char* argv[]) {

  // parse flags
  int opt;
  bool daemonize = false;
  while ((opt = getopt(argc, argv, "dh")) != -1) {
    switch (opt) {
      case 'd':
        daemonize = true;
        break;
      case 'h':
        Usage(0, stdout);
      default:
        Usage(1, stderr);
    }
  }

  // create socket
  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    DieSys("socket");
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
    DieSys("setsockopt");
  struct sockaddr_in saddr = {.sin_family = AF_INET, .sin_port = htons(9110)};
  if (bind(sockfd, (struct sockaddr*)&saddr, sizeof(saddr)))
    DieSys("bind");

  // send to background
  if (daemonize)
    if (daemon(0, 0))
      DieSys("daemon");

  // serve requests
  for (;;) {
    char msg[1500];
    struct sockaddr_in caddr;
    uint32_t caddrlen = sizeof(caddr);
    if (recvfrom(sockfd, msg, sizeof(msg), 0, (struct sockaddr*)&caddr,
                 &caddrlen) == -1)
      DieSys("recvfrom");
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    if (sendto(sockfd, &now, sizeof(now), 0, (struct sockaddr*)&caddr,
               sizeof(caddr)) == -1)
      DieSys("sendto");
  }
}
