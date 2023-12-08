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
#include "libc/sock/internal.h"
#include "libc/sock/struct/sockaddr.internal.h"
#include "libc/sock/struct/sockaddr6-bsd.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/errfuns.h"

// TODO(jart): DELETE

/**
 * Converts sockaddr (Linux/Windows) → sockaddr_bsd (XNU/BSD).
 */
int sockaddr2bsd(const void *addr, uint32_t addrsize,
                 union sockaddr_storage_bsd *out_addr, uint32_t *out_addrsize) {
  uint32_t len, famsize;
  if (addrsize >= sizeof(((struct sockaddr *)addr)->sa_family)) {
    if (((struct sockaddr *)addr)->sa_family == AF_INET) {
      if (addrsize >= sizeof(struct sockaddr_in)) {
        out_addr->sin.sin_len = 0;
        out_addr->sin.sin_family = AF_INET;
        out_addr->sin.sin_port = ((struct sockaddr_in *)addr)->sin_port;
        out_addr->sin.sin_addr = ((struct sockaddr_in *)addr)->sin_addr;
        bzero(&out_addr->sin.sin_zero, sizeof(out_addr->sin.sin_zero));
        *out_addrsize = sizeof(struct sockaddr_in_bsd);
        return 0;
      } else {
        return einval();
      }
    } else if (((struct sockaddr *)addr)->sa_family == AF_INET6) {
      if (addrsize >= sizeof(struct sockaddr_in6)) {
        out_addr->sin6.sin6_len = 0;
        out_addr->sin6.sin6_family = AF_INET6;
        out_addr->sin6.sin6_port = ((struct sockaddr_in6 *)addr)->sin6_port;
        out_addr->sin6.sin6_flowinfo =
            ((struct sockaddr_in6 *)addr)->sin6_flowinfo;
        out_addr->sin6.sin6_addr = ((struct sockaddr_in6 *)addr)->sin6_addr;
        out_addr->sin6.sin6_scope_id =
            ((struct sockaddr_in6 *)addr)->sin6_scope_id;
        *out_addrsize = sizeof(struct sockaddr_in6_bsd);
        return 0;
      } else {
        return einval();
      }
    } else if (((struct sockaddr *)addr)->sa_family == AF_UNIX) {
      famsize = sizeof(((struct sockaddr_un *)addr)->sun_family);
      if (addrsize >= famsize &&
          (len = strnlen(((struct sockaddr_un *)addr)->sun_path,
                         addrsize - famsize)) <
              sizeof(out_addr->sun.sun_path)) {
        out_addr->sun.sun_len = 0;
        out_addr->sun.sun_family = AF_UNIX;
        memcpy(out_addr->sun.sun_path, ((struct sockaddr_un *)addr)->sun_path,
               len);
        out_addr->sun.sun_path[len] = 0;
        *out_addrsize = sizeof(out_addr->sun.sun_len) +
                        sizeof(out_addr->sun.sun_family) + len;
        return 0;
      } else {
        return einval();
      }
    } else {
      return epfnosupport();
    }
  } else {
    return einval();
  }
}
