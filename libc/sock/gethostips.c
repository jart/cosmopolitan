/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/serialize.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/errors.h"
#include "libc/nt/iphlpapi.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sio.h"
#include "libc/sysv/consts/sock.h"

static uint32_t *GetUnixIps(void) {
  int fd, n;
  uint64_t z;
  uint32_t *a;
  char *b, *p, *e, c[16];
  if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) == -1) return 0;
  a = 0;
  n = 0;
  z = 15000;
  b = malloc(z);
  memcpy(c, &z, 8);
  memcpy(c + (IsXnu() ? 4 : 8), &b, 8);
  if (sys_ioctl(fd, SIOCGIFCONF, &c) != -1) {
    for (p = b, e = p + MIN(z, READ32LE(c)); p + 16 + 16 <= e;
         p += IsBsd() ? 16 + MAX(16, p[16] & 255) : 40) {
      if ((p[IsBsd() ? 17 : 16] & 255) != AF_INET) continue;
      a = realloc(a, ++n * sizeof(*a));
      a[n - 1] = READ32BE(p + 20);
    }
    a = realloc(a, ++n * sizeof(*a));
    a[n - 1] = 0;
  }
  close(fd);
  free(b);
  return a;
}

static textwindows uint32_t *GetWindowsIps(void) {
  uint32_t i, z, n, rc, *a;
  struct NtIpAdapterUnicastAddress *u;
  struct NtIpAdapterAddresses *p, *ifaces;
  i = 0;
  z = 15000;
  do {
    if (!(ifaces = malloc(z))) return 0;
    rc = GetAdaptersAddresses(AF_INET,
                              kNtGaaFlagSkipAnycast | kNtGaaFlagSkipMulticast |
                                  kNtGaaFlagSkipDnsServer |
                                  kNtGaaFlagSkipFriendlyName,
                              0, ifaces, &z);
    if (rc != kNtErrorBufferOverflow) break;
    free(ifaces);
    ifaces = 0;
  } while (++i < 3);
  if (rc == kNtErrorNoData) {
    a = calloc(1, sizeof(*a));
  } else if (rc == kNtNoError) {
    for (a = 0, n = 0, p = ifaces; p; p = p->Next) {
      if (p->OperStatus != kNtIfOperStatusUp) continue;
      for (u = p->FirstUnicastAddress; u; u = u->Next) {
        if (u->Address.lpSockaddr->sa_family != AF_INET) continue;
        a = realloc(a, ++n * sizeof(*a));
        a[n - 1] = ntohl(
            ((struct sockaddr_in *)u->Address.lpSockaddr)->sin_addr.s_addr);
      }
    }
    a = realloc(a, ++n * sizeof(*a));
    a[n - 1] = 0;
  } else {
    __winerr();
    a = 0;
  }
  free(ifaces);
  return a;
}

/**
 * Returns IP addresses of system.
 *
 * Normally return values will look like `{0x7f000001, 0x0a0a0a7c, 0}`
 * which means the same thing as `{"127.0.0.1", "10.10.10.124", 0}`.
 * Returned IPs will IPv4 anycast addresses bound to network interfaces
 * which come in a NULL-terminated array with no particular ordering.
 *
 *     uint32_t *ip, *ips = GetIps();
 *     for (ip = ips; *ip; ++ip) {
 *       printf("%hhu.%hhu.%hhu.%hhu\n", *ip >> 24, *ip >> 16, *ip >> 8, *ip);
 *     }
 *
 * This function supports Windows, Linux, XNU, FreeBSD, NetBSD, OpenBSD.
 *
 * @return null-terminated ip array on success, or null w/ errno
 */
uint32_t *GetHostIps(void) {
  if (!IsWindows()) {
    return GetUnixIps();
  } else {
    return GetWindowsIps();
  }
}
