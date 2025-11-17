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
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/errfuns.h"

// normalize filename when using unix domain sockets. windows, like
// unix, gives us 108 bytes of utf-8 to work with here. however, on
// windows the current directory is owned by cosmo, so we can't use
// relative paths here.
textwindows int __fixsunpath(struct sockaddr_un *sun, const void **addr,
                             uint32_t *addrsize) {
  int off = offsetof(struct sockaddr_un, sun_path);

  // 1. ignore sendto() case where addr isn't supplied
  // 2. ignore weird case when sun_path is empty
  if (*addrsize < off + 1)
    return 0;

  // ignore if it's not a unix domain socket address
  if (*(uint16_t *)*addr != AF_UNIX)
    return 0;

  // create nul-terminated version of sun_path
  char sun_path[sizeof(sun->sun_path) + 1];
  const struct sockaddr_un *sunp = *addr;
  uint32_t bytes = *addrsize - off;
  if (bytes > sizeof(sun->sun_path))
    bytes = sizeof(sun->sun_path);
  memcpy(sun_path, sunp->sun_path, bytes);
  sun_path[bytes] = 0;

  // once again, ignore empty string case
  if (!sun_path[0])
    return 0;

  // convert unix utf-8 sun_path into absolute windows utf-8 sun_path
  int len;
  char16_t path16[PATH_MAX];
  if ((len = __mkntpath(sunp->sun_path, path16)) == -1)
    return -1;
  int wrote = tprecode16to8(sun->sun_path, sizeof(sun->sun_path), path16).ax;
  if (wrote >= sizeof(sun->sun_path) - 1)
    return enametoolong();

  // mutate arguments to return result
  sun->sun_family = AF_UNIX;
  *addrsize = sizeof(*sun);
  *addr = sun;
  return 0;
}
