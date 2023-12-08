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
#include "libc/calls/struct/timeval.h"
#include "libc/nt/struct/linger.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/linger.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

__msabi extern typeof(__sys_getsockopt_nt) *const __imp_getsockopt;

textwindows int sys_getsockopt_nt(struct Fd *fd, int level, int optname,
                                  void *out_opt_optval,
                                  uint32_t *inout_optlen) {
  uint64_t ms;
  uint32_t in_optlen;
  struct linger_nt linger;
  npassert(fd->kind == kFdSocket);

  if (out_opt_optval && inout_optlen) {
    in_optlen = *inout_optlen;
  } else {
    in_optlen = 0;
  }

  if (level == SOL_SOCKET &&
      (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO)) {
    if (in_optlen >= sizeof(struct timeval)) {
      if (optname == SO_RCVTIMEO) {
        ms = fd->rcvtimeo;
      } else {
        ms = fd->sndtimeo;
      }
      ((struct timeval *)out_opt_optval)->tv_sec = ms / 1000;
      ((struct timeval *)out_opt_optval)->tv_usec = ms % 1000 * 1000;
      *inout_optlen = sizeof(struct timeval);
      return 0;
    } else {
      return einval();
    }
  }

  // TODO(jart): Use WSAIoctl?
  if (__imp_getsockopt(fd->handle, level, optname, out_opt_optval,
                       inout_optlen) == -1) {
    return __winsockerr();
  }

  if (level == SOL_SOCKET) {
    if (optname == SO_LINGER && in_optlen == sizeof(struct linger)) {
      linger = *(struct linger_nt *)out_opt_optval;
      ((struct linger *)out_opt_optval)->l_onoff = !!linger.l_onoff;
      ((struct linger *)out_opt_optval)->l_linger = linger.l_linger;
      *inout_optlen = sizeof(struct linger);
    }
  }

  if (in_optlen == 4 && *inout_optlen == 1) {
    // handle cases like this
    // getsockopt(8, SOL_TCP, TCP_FASTOPEN, [u"☺"], [1]) → 0
    int32_t wut = *(signed char *)out_opt_optval;
    memcpy(out_opt_optval, &wut, 4);
    *inout_optlen = 4;
  }

  return 0;
}

#endif /* __x86_64__ */
