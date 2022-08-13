/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/mem/alg.h"
#include "libc/intrin/bits.h"
#include "libc/dns/consts.h"
#include "libc/dns/dns.h"
#include "libc/dns/hoststxt.h"
#include "libc/fmt/fmt.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/errfuns.h"

/**
 * Finds name associated with address in HOSTS.TXT table.
 *
 * @param ht can be GetHostsTxt()
 * @param af can be AF_INET
 * @param ip is IP address in binary (sin_addr)
 * @param buf is buffer to store the name
 * @param bufsize is length of buf
 * @return 1 if found, 0 if not found, or -1 w/ errno
 * @error EAFNOSUPPORT
 */
int ResolveHostsReverse(const struct HostsTxt *ht, int af, const uint8_t *ip,
                        char *buf, size_t bufsize) {
  size_t i;
  if (af != AF_INET && af != AF_UNSPEC) return eafnosupport();
  for (i = 0; i < ht->entries.i; ++i) {
    if (READ32LE(ip) == READ32LE(ht->entries.p[i].ip)) {
      if (memccpy(buf, ht->strings.p + ht->entries.p[i].name, '\0', bufsize)) {
        return 1;
      }
    }
  }
  return 0;
}
