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
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pr.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

static errno_t pthread_setname_impl(struct PosixThread *pt, const char *name) {
  char path[128], *p;
  int e, fd, rc, tid, len;

  len = strlen(name);
  tid = _pthread_tid(pt);

  if (IsXnuSilicon()) {
    if (pt == _pthread_self()) {
      __syslib->__pthread_setname_np(name);
      return 0;
    } else {
      return EPERM;
    }
  } else if (IsLinux()) {
    e = errno;
    if (tid == gettid()) {
      if (prctl(PR_SET_NAME, name) == -1) {
        rc = errno;
        errno = e;
        return rc;
      }
    } else {
      p = path;
      p = stpcpy(p, "/proc/self/task/");
      p = FormatUint32(p, tid);
      p = stpcpy(p, "/comm");
      if ((fd = sys_openat(AT_FDCWD, path, O_WRONLY | O_CLOEXEC, 0)) == -1) {
        rc = errno;
        errno = e;
        return rc;
      }
      rc = sys_write(fd, name, len);
      rc |= sys_close(fd);
      if (rc == -1) {
        rc = errno;
        errno = e;
        return rc;
      }
    }
    if (len > 15) {
      // linux is documented as truncating here. we still set the name
      // since the limit might be raised in the future checking return
      // value of this function is a bummer. Grep it for TASK_COMM_LEN
      return ERANGE;
    }
    return 0;

#ifdef __x86_64__
  } else if (IsFreebsd() || IsNetbsd() || IsOpenbsd()) {
    int ax;
    if (IsFreebsd()) {
      ax = 464;  // thr_set_name
    } else if (IsNetbsd()) {
      ax = 323;  // _lwp_setname
    } else {
      ax = 143;  // sys_setthrname
    }
    asm volatile("syscall"
                 : "+a"(ax), "+D"(tid), "+S"(name)
                 : /* no inputs */
                 : "rcx", "rdx", "r8", "r9", "r10", "r11", "memory");
    return ax;
#endif

#ifdef __aarch64__
  } else if (IsFreebsd()) {
    register int x0 asm("x0") = tid;
    register long x1 asm("x1") = (long)name;
    register int x8 asm("x8") = 464;  // thr_set_name
    asm volatile("svc\t0" : "+r"(x0) : "r"(x1), "r"(x8) : "memory");
    return x0;
#endif

  } else {
    return ENOSYS;
  }
}

/**
 * Registers custom name of thread with system, e.g.
 *
 *     void *worker(void *arg) {
 *       pthread_setname_np(pthread_self(), "justine");
 *       pause();
 *       return 0;
 *     }
 *
 *     int main(int argc, char *argv[]) {
 *       pthread_t id;
 *       pthread_create(&id, 0, worker, 0);
 *       pthread_join(id, 0);
 *     }
 *
 * ProTip: The `htop` software is good at displaying thread names.
 *
 * @return 0 on success, or errno on error
 * @raise ERANGE if length of `name` exceeded system limit, in which
 *    case the name may have still been set with os using truncation
 * @raise ENOSYS on Windows and AMD64-XNU
 * @see pthread_getname_np()
 */
errno_t pthread_setname_np(pthread_t thread, const char *name) {
  errno_t err;
  struct PosixThread *pt;
  pt = (struct PosixThread *)thread;
  BLOCK_CANCELATION;
  err = pthread_setname_impl(pt, name);
  ALLOW_CANCELATION;
  STRACE("pthread_setname_np(%d, %#s) → %s", _pthread_tid(pt), name,
         DescribeErrno(err));
  return err;
}
