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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/errfuns.h"

static bool setsockopt_polyfill(int *optname) {
  if (errno == ENOPROTOOPT && *optname == SO_REUSEPORT /* RHEL5 */) {
    *optname = SO_REUSEADDR; /* close enough */
    return true;             /* fudges errno */
  }
  return false;
}

static textwindows int setsockopt$nt(struct Fd *fd, int level, int optname,
                                     const void *optval, uint32_t optlen) {
  if (__setsockopt$nt(fd->handle, level, optname, optval, optlen) != -1) {
    return 0;
  } else {
    return __winsockerr();
  }
}

/**
 * Modifies socket settings.
 *
 * This function is the ultimate rabbit hole. Basic usage:
 *
 *   int yes = 1;
 *   setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes));
 *
 * @param level can be SOL_SOCKET, SOL_IP, SOL_TCP, etc.
 * @param optname can be SO_{REUSE{PORT,ADDR},KEEPALIVE,etc.} etc.
 * @return 0 on success, or -1 w/ errno
 * @error ENOPROTOOPT for unknown (level,optname)
 * @see libc/sysv/consts.sh for tuning catalogue
 * @see getsockopt()
 */
int setsockopt(int fd, int level, int optname, const void *optval,
               uint32_t optlen) {
  if (!optval) return efault();
  if (!level || !optname) return enoprotoopt(); /* our sysvconsts definition */
  if (optname == -1) return 0;                  /* our sysvconsts definition */
  if (!IsWindows()) {
    do {
      if (setsockopt$sysv(fd, level, optname, optval, optlen) != -1) {
        return 0;
      }
    } while (setsockopt_polyfill(&optname));
    return -1;
  } else if (__isfdkind(fd, kFdSocket)) {
    return setsockopt$nt(&g_fds.p[fd], level, optname, optval, optlen);
  } else {
    return ebadf();
  }
}
