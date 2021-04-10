/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/sio.h"

int ioctl_siocgifconf_nt(int, struct ifconf *) hidden;

static int ioctl_siocgifconf_sysv(int fd, struct ifconf *ifc) {
  /* Same as the default for now... */
  return sys_ioctl(fd, SIOCGIFCONF, ifc);
  /*
  int rc;

  if ((rc = sys_ioctl(fd, SIOCGIFCONF, &ifc)) != -1) {
    return rc;
  }
  return rc;
  */
}

/**
 * Returns information about network interfaces.
 *
 * @see ioctl(fd, SIOCGIFCONF, tio) dispatches here
 */
int ioctl_siocgifconf(int fd, struct ifconf *ifc) {
  if (!IsWindows()) {
    return ioctl_siocgifconf_sysv(fd, ifc);
  } else {
    return ioctl_siocgifconf_nt(fd, ifc);
  }
}

