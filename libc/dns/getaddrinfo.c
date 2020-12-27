/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/dns/dns.h"
#include "libc/dns/hoststxt.h"
#include "libc/dns/resolvconf.h"
#include "libc/fmt/conv.h"
#include "libc/mem/mem.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ai.h"
#include "libc/sysv/consts/eai.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/errfuns.h"

/**
 * Resolves address for internet name.
 *
 * @param name is either an ip string or a utf-8 hostname
 * @param service is the port number as a string
 * @param hints may be passed to specialize behavior (optional)
 * @param res receives a pointer that must be freed with freeaddrinfo(),
 *     and won't be modified if -1 is returned
 * @return 0 on success or EAI_xxx value
 */
int getaddrinfo(const char *name, const char *service,
                const struct addrinfo *hints, struct addrinfo **res) {
  int rc, port;
  const char *canon;
  struct addrinfo *ai;
  port = 0;
  if (!name && !service) return EAI_NONAME;
  if (service && (port = parseport(service)) == -1) return EAI_NONAME;
  if (!name && (hints->ai_flags & AI_CANONNAME)) return EAI_BADFLAGS;
  if (!(ai = newaddrinfo(port))) return EAI_MEMORY;
  if (service) ai->ai_addr4->sin_port = htons(port);
  if (hints) {
    ai->ai_socktype = hints->ai_socktype;
    ai->ai_protocol = hints->ai_protocol;
  }
  if (!name) {
    ai->ai_addr4->sin_addr.s_addr =
        (hints && (hints->ai_flags & AI_PASSIVE) == AI_PASSIVE)
            ? INADDR_ANY
            : INADDR_LOOPBACK;
    return 0;
  }
  if (inet_pton(AF_INET, name, &ai->ai_addr4->sin_addr.s_addr) == 1) {
    *res = ai;
    return 0;
  } else if (hints && (hints->ai_flags & AI_NUMERICHOST) == AI_NUMERICHOST) {
    freeaddrinfo(ai);
    return EAI_NONAME;
  } else if (resolvehoststxt(gethoststxt(), AF_INET, name, ai->ai_addr,
                             sizeof(ai->ai_addr4), &canon) > 0) {
    memcpy(ai->ai_canonname, canon, min(strlen(canon), DNS_NAME_MAX) + 1);
    *res = ai;
    return 0;
  } else {
    rc = resolvedns(getresolvconf(), AF_INET, name, ai->ai_addr,
                    sizeof(ai->ai_addr4));
    if (rc > 0) {
      *res = ai;
      return 0;
    }
    freeaddrinfo(ai);
    if (rc == 0) {
      return EAI_NONAME;
    } else {
      return EAI_SYSTEM;
    }
  }
}
