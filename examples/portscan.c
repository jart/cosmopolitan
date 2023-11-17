#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/timeval.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/mem/alloca.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "net/http/ip.h"

/**
 * @fileoverview fast local network port scanner, e.g.
 *
 *     make -j8 o//examples/portscan.com
 *     o//examples/portscan.com 10.10.10.0/24 22
 */

const char *FormatIp(char buf[16], uint32_t ip) {
  snprintf(buf, 16, "%hhu.%hhu.%hhu.%hhu", ip >> 24, ip >> 16, ip >> 8, ip);
  return buf;
}

#define FormatIp(x) FormatIp(alloca(16), x)

int main(int argc, char *argv[]) {
  int port;
  struct Cidr in;
  uint32_t netmask;
  const char *status;
  struct timeval timeout;
  uint32_t network_address;
  uint32_t last_host_address;
  uint32_t first_host_address;

  if (argc != 3) {
  PrintUsage:
    fprintf(stderr,
            "usage:   %s IP/CIDR PORT\n"
            "example: %s 192.168.0.0/24 22\n",
            argv[0], argv[0]);
    return 1;
  }

  in = ParseCidr(argv[1], -1);
  if (in.addr == -1) {
    fprintf(stderr, "error: bad ip/cidr\n");
    goto PrintUsage;
  }

  port = atoi(argv[2]);
  if (!(1 <= port && port <= 65535)) {
    fprintf(stderr, "error: bad port\n");
    goto PrintUsage;
  }

  if (in.cidr > 30) {
    fprintf(stderr, "error: maximum supported cidr is 30\n");
    goto PrintUsage;
  }

  if (in.cidr < 22) {
    fprintf(stderr, "error: minimum cidr support right now is 22\n");
    goto PrintUsage;
  }

  netmask = -1u << (32 - in.cidr);
  network_address = in.addr & netmask;
  first_host_address = network_address + 1;
  last_host_address = (network_address | ~netmask) - 1;

  assert(last_host_address > first_host_address);
  assert(last_host_address - first_host_address < 1024);

  fprintf(stderr, "scanning %s through %s\n", FormatIp(first_host_address),
          FormatIp(last_host_address));

  timeout = timeval_frommillis(100);
  for (int64_t ip = first_host_address; ip <= last_host_address; ++ip) {
    if (!fork()) {
      socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      setsockopt(3, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
      setsockopt(3, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
      if (!connect(3,
                   (struct sockaddr *)&(struct sockaddr_in){
                       .sin_family = AF_INET,
                       .sin_addr.s_addr = htonl(ip),
                       .sin_port = htons(port),
                   },
                   sizeof(struct sockaddr_in))) {
        status = "open";
      } else if (errno == ECONNREFUSED) {
        status = "closed";
      } else if (errno != EINPROGRESS) {
        status = _strerrno(errno);
      } else {
        status = 0;
      }
      if (status) {
        printf("%-15s %s\n", FormatIp(ip), status);
      }
      _Exit(0);
    }
  }
  for (;;) {
    if (wait(0) == -1 && errno == ECHILD) {
      break;
    }
  }
}
