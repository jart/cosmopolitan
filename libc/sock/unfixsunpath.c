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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/errfuns.h"

// performs inverse of __fixsunpath()
textwindows void __unfixsunpath(void *addr, uint32_t *addrsize,
                                uint32_t addrcapacity) {
  int off = offsetof(struct sockaddr_un, sun_path);

  // ignore recvfrom() case where addr isn't supplied
  if (!addrsize)
    return;

  // ignore empty string case
  if (*addrsize < off + 1)
    return;

  // ignore if it's not a unix domain socket address
  if (*(uint16_t *)addr != AF_UNIX)
    return;

  // create nul-terminated version of sun_path
  const struct sockaddr_un *sunp = addr;
  char sun_path[sizeof(sunp->sun_path) + 1];
  uint32_t bytes = *addrsize - off;
  if (bytes > sizeof(sunp->sun_path))
    return;
  memcpy(sun_path, sunp->sun_path, bytes);
  sun_path[bytes] = 0;

  // convert windows utf-8 sun_path into unix utf-8 sun_path
  // when things get hairy, err on the side of doing nothing
  char16_t path16[PATH_MAX];
  tprecode8to16(path16, PATH_MAX, sun_path);
  int len;
  int e = errno;
  char path8[PATH_MAX];
  if ((len = __mkunixpath(path16, path8)) == -1) {
    errno = e;
    return;
  }
  if (addrcapacity > sizeof(struct sockaddr_un))
    addrcapacity = sizeof(struct sockaddr_un);
  if (addrcapacity < off)
    return;
  if (len + 1 > addrcapacity - off)
    return;
  memcpy((char *)addr + off, path8, len);
  ((char *)addr)[off + len] = 0;
  *addrsize = off + len;
}
