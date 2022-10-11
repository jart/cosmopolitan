/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/metasigaltstack.h"
#include "libc/calls/struct/sigaltstack.h"
#include "libc/calls/struct/sigaltstack.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/ss.h"
#include "libc/sysv/errfuns.h"

static void sigaltstack2bsd(struct sigaltstack_bsd *bsd,
                            const struct sigaltstack *linux) {
  void *sp;
  int flags;
  size_t size;
  sp = linux->ss_sp;
  flags = linux->ss_flags;
  size = linux->ss_size;
  bsd->ss_sp = sp;
  bsd->ss_flags = flags;
  bsd->ss_size = size;
}

static void sigaltstack2linux(struct sigaltstack *linux,
                              const struct sigaltstack_bsd *bsd) {
  void *sp;
  int flags;
  size_t size;
  sp = bsd->ss_sp;
  flags = bsd->ss_flags;
  size = bsd->ss_size;
  linux->ss_sp = sp;
  linux->ss_flags = flags;
  linux->ss_size = size;
}

/**
 * Sets and/or gets alternate signal stack, e.g.
 *
 *     struct sigaction sa;
 *     struct sigaltstack ss;
 *     ss.ss_flags = 0;
 *     ss.ss_size = GetStackSize();
 *     ss.ss_sp = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
 *                     MAP_STACK | MAP_ANONYMOUS, -1, 0);
 *     sa.sa_flags = SA_ONSTACK;
 *     sa.sa_handler = OnStackOverflow;
 *     __cxa_atexit(free, ss[0].ss_sp, 0);
 *     sigemptyset(&sa.ss_mask);
 *     sigaltstack(&ss, 0);
 *     sigaction(SIGSEGV, &sa, 0);
 *
 * It's strongly recommended that you allocate a stack with the same
 * size as GetStackSize() and that it have GetStackSize() alignment.
 * Otherwise some of your runtime support code (e.g. ftrace stack use
 * logging, kprintf() memory safety) won't be able to work as well.
 *
 * @param neu if non-null will install new signal alt stack
 * @param old if non-null will receive current signal alt stack
 * @return 0 on success, or -1 w/ errno
 * @raise EFAULT if bad memory was supplied
 * @raise ENOMEM if `neu->ss_size` is less than `MINSIGSTKSZ`
 */
int sigaltstack(const struct sigaltstack *neu, struct sigaltstack *old) {
  int rc;
  void *b;
  const void *a;
  struct sigaltstack_bsd bsd;
  if (IsAsan() && ((old && __asan_check(old, sizeof(*old)).kind) ||
                   (neu && (__asan_check(neu, sizeof(*neu)).kind ||
                            __asan_check(neu->ss_sp, neu->ss_size).kind)))) {
    rc = efault();
  } else if (neu && neu->ss_size < MINSIGSTKSZ) {
    rc = enomem();
  } else if (IsLinux() || IsBsd()) {
    if (IsLinux()) {
      a = neu;
      b = old;
    } else {
      if (neu) {
        sigaltstack2bsd(&bsd, neu);
        a = &bsd;
      } else {
        a = 0;
      }
      if (old) {
        b = &bsd;
      } else {
        b = 0;
      }
    }
    if ((rc = sys_sigaltstack(a, b)) != -1) {
      if (IsBsd() && old) {
        sigaltstack2linux(old, &bsd);
      }
      rc = 0;
    } else {
      rc = -1;
    }
  } else {
    rc = enosys();
  }
  STRACE("sigaltstack(%s, [%s]) → %d% m", DescribeSigaltstk(0, neu),
         DescribeSigaltstk(0, old), rc);
  return rc;
}
