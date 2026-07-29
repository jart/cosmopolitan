/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/fds.h"
#include "libc/intrin/strace.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/errno.h"
#include "libc/sysv/pib.h"

/**
 * Retrieves socket setting.
 *
 * @param level can be SOL_SOCKET, IPPROTO_TCP, etc.
 * @param optname can be SO_{REUSE{PORT,ADDR},KEEPALIVE,etc.} etc.
 * @return 0 on success, or -1 w/ errno
 * @error ENOPROTOOPT for unknown (level,optname)
 * @error EINVAL if `out_optlen` is invalid somehow
 * @error ENOTSOCK if `fd` is valid but not a socket
 * @error EBADF if `fd` isn't valid
 * @error EFAULT if optval memory isn't valid
 * @see libc/sysv/consts.sh for tuning catalogue
 * @see setsockopt()
 */
int getsockopt(int fd, int level, int optname, void* out_opt_optval,
               uint32_t* out_optlen) {
  int rc;

  if (level == -1 || !optname) {
    rc = enoprotoopt();  // see libc/sysv/consts.sh
  } else if (__isfdkind(fd, kFdZip)) {
    rc = enotsock();
  } else if (!IsWindows()) {
    rc = sys_getsockopt(fd, level, optname, out_opt_optval, out_optlen);
    if (level == SOL_SOCKET && optname == SO_ERROR) {
      assert(*out_optlen == sizeof(int));
      *(int*)out_opt_optval = __errno_host2linux(*((int*)out_opt_optval));
    }
  } else if (!__isfdopen(fd)) {
    rc = ebadf();
  } else if (__isfdkind(fd, kFdSocket)) {
    rc = sys_getsockopt_nt(&__get_pib()->fds.p[fd], level, optname,
                           out_opt_optval, out_optlen);
  } else {
    rc = enotsock();
  }

#if SYSDEBUG
  if (out_opt_optval && out_optlen && rc != -1) {
    STRACE("getsockopt(%d, %s, %s, [%#.*hhs], [%d]) → %d% lm", fd,
           DescribeSockLevel(level), DescribeSockOptname(level, optname),
           *out_optlen, out_opt_optval, *out_optlen, rc);
  } else {
    STRACE("getsockopt(%d, %s, %s, %p, %p) → %d% lm", fd,
           DescribeSockLevel(level), DescribeSockOptname(level, optname),
           out_opt_optval, out_optlen, rc);
  }
#endif

  return rc;
}
