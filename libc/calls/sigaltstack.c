/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/sigaltstack.h"
#include "libc/calls/struct/metasigaltstack.h"
#include "libc/calls/struct/sigaltstack.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/consts/ss.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"

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

static void sigaltstack_setnew(const struct sigaltstack *neu) {
  if (neu) {
    struct CosmoTib *tib = __get_tls();
    tib->tib_sigstack_addr = (char *)ROUNDUP((uintptr_t)neu->ss_sp, 16);
    tib->tib_sigstack_size = ROUNDDOWN(neu->ss_size, 16);
    tib->tib_sigstack_flags = neu->ss_flags & SS_DISABLE;
  }
}

static textwindows int sigaltstack_cosmo(const struct sigaltstack *neu,
                                         struct sigaltstack *old) {
  struct CosmoTib *tib = __get_tls();
  char *bp = __builtin_frame_address(0);
  if (old) {
    old->ss_sp = tib->tib_sigstack_addr;
    old->ss_size = tib->tib_sigstack_size;
    old->ss_flags = tib->tib_sigstack_flags & ~SS_ONSTACK;
  }
  sigaltstack_setnew(neu);
  if (tib->tib_sigstack_addr <= bp &&
      bp <= tib->tib_sigstack_addr + tib->tib_sigstack_size) {
    if (old) old->ss_flags |= SS_ONSTACK;
    tib->tib_sigstack_flags = SS_ONSTACK;  // can't disable if on it
  } else if (!tib->tib_sigstack_size) {
    if (old) old->ss_flags = SS_DISABLE;
    tib->tib_sigstack_flags = SS_DISABLE;
  }
  return 0;
}

static int sigaltstack_bsd(const struct sigaltstack *neu,
                           struct sigaltstack *old) {
  int rc;
  struct sigaltstack_bsd oldbsd, neubsd, *neup = 0;
  if (neu) sigaltstack2bsd(&neubsd, neu), neup = &neubsd;
  if (IsXnuSilicon()) {
    rc = _sysret(__syslib->__sigaltstack(neup, &oldbsd));
  } else {
    rc = sys_sigaltstack(neup, &oldbsd);
  }
  if (rc == -1) {
    return -1;
  }
  if (old) sigaltstack2linux(old, &oldbsd);
  return 0;
}

/**
 * Sets and/or gets alternate signal stack, e.g.
 *
 *     struct sigaction sa;
 *     struct sigaltstack ss;
 *     ss.ss_flags = 0;
 *     ss.ss_size = sysconf(_SC_MINSIGSTKSZ) + 8192;
 *     ss.ss_sp = malloc(ss.ss_size);
 *     sigaltstack(&ss, 0);
 *     sigemptyset(&sa.ss_mask);
 *     sa.sa_flags = SA_ONSTACK;
 *     sa.sa_handler = OnStackOverflow;
 *     sigaction(SIGSEGV, &sa, 0);
 *
 * @param neu if non-null will install new signal alt stack
 * @param old if non-null will receive current signal alt stack
 * @return 0 on success, or -1 w/ errno
 * @raise EFAULT if bad memory was supplied
 * @raise ENOMEM if `neu->ss_size` is less than `MINSIGSTKSZ`
 */
int sigaltstack(const struct sigaltstack *neu, struct sigaltstack *old) {
  int rc;
  if (IsAsan() && ((old && !__asan_is_valid(old, sizeof(*old))) ||
                   (neu && !__asan_is_valid(neu, sizeof(*neu))))) {
    rc = efault();
  } else if (neu && ((neu->ss_size >> 32) ||  //
                     (neu->ss_flags & ~(SS_ONSTACK | SS_DISABLE)))) {
    rc = einval();
  } else if (neu && neu->ss_size < __get_minsigstksz()) {
    rc = enomem();
  } else if (IsLinux()) {
    rc = sys_sigaltstack(neu, old);
    if (!rc) sigaltstack_setnew(neu);
  } else if (IsBsd()) {
    rc = sigaltstack_bsd(neu, old);
    if (!rc) sigaltstack_setnew(neu);
  } else {
    rc = sigaltstack_cosmo(neu, old);
  }
  STRACE("sigaltstack(%s, [%s]) → %d% m", DescribeSigaltstk(0, neu),
         DescribeSigaltstk(0, old), rc);
  return rc;
}
