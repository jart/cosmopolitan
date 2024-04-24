/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/errno.h"
#include "libc/serialize.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/sock.h"
#include "net/http/http.h"
#include "net/http/ip.h"

int main(int argc, char *argv[]) {

  if (argc < 2) {
    kprintf("usage: blackhole IP...\n");
    return EX_USAGE;
  }

  int fd;
  struct sockaddr_un addr = {AF_UNIX, "/var/run/blackhole.sock"};
  if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
    kprintf("error: socket(AF_UNIX) failed: %s\n", strerror(errno));
    return 3;
  }

  int rc = 0;
  for (int i = 1; i < argc; ++i) {
    int64_t ip;
    char buf[4];
    if ((ip = ParseIp(argv[i], -1)) != -1) {
      WRITE32BE(buf, ip);
      if (sendto(fd, buf, 4, 0, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        kprintf("error: sendto(%#s) failed: %s\n", addr.sun_path,
                strerror(errno));
        rc |= 2;
      }
    } else {
      kprintf("error: bad ipv4 address: %s\n", argv[i]);
      rc |= 1;
    }
  }

  return rc;
}
