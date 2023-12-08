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
#include "libc/macros.internal.h"
#include "libc/sock/internal.h"
#include "libc/sock/struct/sockaddr.internal.h"
#include "libc/sock/struct/sockaddr6-bsd.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/errfuns.h"

// TODO(jart): DELETE

/**
 * Converts sockaddr_bsd (XNU/BSD) → sockaddr (Linux/Windows).
 */
void sockaddr2linux(const union sockaddr_storage_bsd *addr, uint32_t addrsize,
                    union sockaddr_storage_linux *out_addr,
                    uint32_t *inout_addrsize) {
  uint32_t len, size;
  if (out_addr && inout_addrsize) {
    size = *inout_addrsize;
    bzero(out_addr, size);
    if (addrsize >= sizeof(addr->sa.sa_family)) {
      if (addr->sa.sa_family == AF_INET) {
        if (addrsize >= sizeof(struct sockaddr_in_bsd) &&
            size >= sizeof(struct sockaddr_in)) {
          out_addr->sin.sin_family = AF_INET;
          out_addr->sin.sin_port = addr->sin.sin_port;
          out_addr->sin.sin_addr = addr->sin.sin_addr;
          *inout_addrsize = sizeof(struct sockaddr_in);
        }
      } else if (addr->sa.sa_family == AF_INET6) {
        if (addrsize >= sizeof(struct sockaddr_in6_bsd) &&
            size >= sizeof(struct sockaddr_in6)) {
          out_addr->sin6.sin6_family = AF_INET6;
          out_addr->sin6.sin6_port = addr->sin6.sin6_port;
          out_addr->sin6.sin6_addr = addr->sin6.sin6_addr;
          out_addr->sin6.sin6_flowinfo = addr->sin6.sin6_flowinfo;
          out_addr->sin6.sin6_scope_id = addr->sin6.sin6_scope_id;
          *inout_addrsize = sizeof(struct sockaddr_in6);
        }
      } else if (addr->sa.sa_family == AF_UNIX) {
        if (addrsize >=
                sizeof(addr->sun.sun_len) + sizeof(addr->sun.sun_family) &&
            size >= sizeof(out_addr->sun.sun_family)) {
          len = strnlen(((struct sockaddr_un *)addr)->sun_path,
                        MIN(addrsize - (sizeof(addr->sun.sun_len) +
                                        sizeof(addr->sun.sun_family)),
                            size - sizeof(out_addr->sun.sun_family)));
          out_addr->sun.sun_family = AF_UNIX;
          if (len) memcpy(out_addr->sun.sun_path, addr->sun.sun_path, len);
          *inout_addrsize = sizeof(out_addr->sun.sun_family) + len + 1;
        }
      }
    }
  }
}
