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
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/cosmotime.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sock.h"
#include "third_party/musl/netdb.h"

[[noreturn]] void Die(const char *thing, const char *reason) {
  fprintf(stderr, "%s: fatal error: %s\n", thing, reason);
  exit(1);
}

[[noreturn]] void DieSys(const char *thing) {
  Die(thing, strerror(errno));
}

void Drift(const char *host) {

  // perform dns lookup
  int err;
  struct addrinfo *addr;
  struct addrinfo hints = {.ai_family = AF_UNSPEC,
                           .ai_socktype = SOCK_DGRAM,
                           .ai_protocol = IPPROTO_UDP,
                           .ai_flags = AI_NUMERICSERV};
  if ((err = getaddrinfo(host, "9110", &hints, &addr)))
    Die(host, gai_strerror(err));

  // create socket
  int sockfd;
  if ((sockfd = socket(addr->ai_family,    //
                       addr->ai_socktype,  //
                       addr->ai_protocol)) == -1)
    DieSys("socket");
  struct sockaddr_in caddr = {.sin_family = AF_INET};
  if (bind(sockfd, (struct sockaddr *)&caddr, sizeof(caddr)))
    DieSys("bind");

  // get timestamp from stampd
  char msg[1];
  struct timespec started;
  clock_gettime(CLOCK_REALTIME, &started);
  if (sendto(sockfd, msg, 1, 0, addr->ai_addr, addr->ai_addrlen) == -1)
    DieSys(host);
  struct timespec stamp;
  struct sockaddr_in saddr;
  uint32_t saddrlen = sizeof(saddr);
  ssize_t got = recvfrom(sockfd, &stamp, sizeof(stamp), 0,
                         (struct sockaddr *)&saddr, &saddrlen);
  struct timespec ended;
  clock_gettime(CLOCK_REALTIME, &ended);
  if (got == -1)
    DieSys(host);
  if (got != sizeof(stamp))
    Die(host, "received bad message");

  // compute drift
  int64_t drift =
      timespec_tonanos(stamp) -
      timespec_tonanos(timespec_add(
          started, timespec_fromnanos(
                       timespec_tonanos(timespec_sub(ended, started)) / 2)));
  printf("%s: %,ld ns\n", host, drift);

  // cleanup
  freeaddrinfo(addr);
  close(sockfd);
}

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i)
    Drift(argv[i]);
}
