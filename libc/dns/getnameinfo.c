/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/dns/consts.h"
#include "libc/dns/dns.h"
#include "libc/dns/hoststxt.h"
#include "libc/dns/resolvconf.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/mem/mem.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/errfuns.h"

/**
 * Resolves name/service for socket address.
 *
 * @param addr
 * @param addrlen
 * @param name
 * @param namelen
 * @param service
 * @param servicelen
 * @param flags
 *
 * @return 0 on success or EAI_xxx value
 */
int getnameinfo(const struct sockaddr *addr, socklen_t addrlen, char *name,
                socklen_t namelen, char *service, socklen_t servicelen,
                int flags) {
  char rdomain[1 + sizeof "255.255.255.255.in-addr.arpa"];
  char info[512];
  int rc, port;
  uint8_t *ip;
  unsigned int valid_flags;

  valid_flags =
      (NI_NAMEREQD | NI_NUMERICHOST | NI_NUMERICSERV | NI_NOFQDN | NI_DGRAM);

  if (flags & ~(valid_flags)) return EAI_BADFLAGS;
  if (!name && !service) return EAI_NONAME;
  if (addr->sa_family != AF_INET || addrlen < sizeof(struct sockaddr_in))
    return EAI_FAMILY;

  ip = (uint8_t *)&(((struct sockaddr_in *)addr)->sin_addr);
  sprintf(rdomain, "%d.%d.%d.%d.in-addr.arpa", ip[3], ip[2], ip[1], ip[0]);
  info[0] = '\0';
  if (name != NULL && namelen != 0) {
    if ((flags & NI_NUMERICHOST) && (flags & NI_NAMEREQD)) return EAI_NONAME;

    if ((flags & NI_NUMERICHOST) &&
        inet_ntop(AF_INET, ip, info, sizeof(info)) == NULL)
      return EAI_SYSTEM;
    else if (!info[0] && ResolveHostsReverse(GetHostsTxt(), AF_INET, ip, info,
                                             sizeof(info)) < 0)
      return EAI_SYSTEM;
    else if (!info[0] && ResolveDnsReverse(GetResolvConf(), AF_INET, rdomain,
                                           info, sizeof(info)) < 0)
      return EAI_SYSTEM;
    else if (!info[0] && (flags & NI_NAMEREQD))
      return EAI_NONAME;
    else if (!info[0] && inet_ntop(AF_INET, ip, info, sizeof(info)) == NULL)
      return EAI_SYSTEM;

    if (strlen(info) + 1 > namelen) return EAI_OVERFLOW;
    strcpy(name, info);
  }

  port = ntohs(((struct sockaddr_in *)addr)->sin_port);
  info[0] = '\0';
  if (service != NULL && servicelen != 0) {
    itoa(port, info, 10);
    /* TODO: reverse lookup on /etc/services to get name of service */
    if (strlen(info) + 1 > servicelen) return EAI_OVERFLOW;
    strcpy(service, info);
  }

  return 0;
}
