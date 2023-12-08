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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/asmflag.h"
#include "libc/intrin/atomic.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pr.h"
#include "libc/thread/posixthread.internal.h"

static errno_t pthread_getname_impl(struct PosixThread *pt, char *name,
                                    size_t size) {
  int e, fd, rc, tid, len;

  tid = _pthread_tid(pt);
  if (!size) return 0;
  bzero(name, size);
  e = errno;

  if (IsLinux()) {
    // TASK_COMM_LEN is 16 on Linux so we're just being paranoid.
    char buf[256] = {0};
    if (tid == gettid()) {
      if (prctl(PR_GET_NAME, buf) == -1) {
        rc = errno;
        errno = e;
        return rc;
      }
    } else {
      char path[128], *p = path;
      p = stpcpy(p, "/proc/self/task/");
      p = FormatUint32(p, tid);
      p = stpcpy(p, "/comm");
      if ((fd = sys_openat(AT_FDCWD, path, O_RDONLY | O_CLOEXEC, 0)) == -1) {
        rc = errno;
        errno = e;
        return rc;
      }
      rc = sys_read(fd, buf, sizeof(buf) - 1);
      rc |= sys_close(fd);
      if (rc == -1) {
        rc = errno;
        errno = e;
        return rc;
      }
      chomp(buf);
    }
    if ((len = strlen(buf))) {
      memcpy(name, buf, MIN(len, size - 1));
    }
    if (len > size - 1) {
      return ERANGE;
    }
    return 0;

  } else if (IsNetbsd() || IsOpenbsd()) {
    int ax;
    char cf;
    long dx, si;
    if (IsNetbsd()) {
      ax = 324;  // _lwp_getname
    } else {
      ax = 142;  // sys_getthrname
    }
    // NetBSD doesn't document the subtleties of its nul-terminator
    // behavior, so like Linux we shall take the paranoid approach.
    dx = size - 1;
    si = (long)name;
    asm volatile(CFLAG_ASM("syscall")
                 : CFLAG_CONSTRAINT(cf), "+a"(ax), "+D"(tid), "+S"(si), "+d"(dx)
                 : /* no outputs */
                 : "rcx", "r8", "r9", "r10", "r11", "memory");
    if (!cf) {
      // if size + our nul + kernel's nul is the buffer size, then we
      // can't say with absolute confidence truncation didn't happen.
      if (strlen(name) + 1 + 1 <= size) {
        return 0;
      } else {
        return ERANGE;
      }
    } else {
      return ax;
    }

  } else {
    return ENOSYS;
  }
}

/**
 * Gets name of thread registered with system, e.g.
 *
 *     char name[64];
 *     pthread_getname_np(thread, name, sizeof(name));
 *
 * If the thread doesn't have a name, then empty string is returned.
 * This implementation guarantees `buf` is always modified, even on
 * error, and will always be nul-terminated. If `size` is 0 then this
 * function returns 0. Your `buf` is also chomped to remove newlines.
 *
 * @return 0 on success, or errno on error
 * @raise ERANGE if `size` wasn't large enough, in which case your
 *     result will still be returned truncated if possible
 * @raise ENOSYS on MacOS, Windows, and FreeBSD
 */
errno_t pthread_getname_np(pthread_t thread, char *name, size_t size) {
  errno_t rc;
  struct PosixThread *pt;
  pt = (struct PosixThread *)thread;
  BLOCK_CANCELATION;
  rc = pthread_getname_impl(pt, name, size);
  ALLOW_CANCELATION;
  return rc;
}
