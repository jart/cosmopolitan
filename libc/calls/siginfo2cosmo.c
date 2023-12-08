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
#include "libc/calls/struct/siginfo-meta.internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/dce.h"
#include "libc/sysv/consts/sig.h"

privileged void __siginfo2cosmo(struct siginfo *si,
                                const union siginfo_meta *m) {
  void *si_addr;
  int32_t si_signo;
  int32_t si_errno;
  int32_t si_code;
  int32_t si_pid;
  int32_t si_uid;
  int32_t si_status;
  int32_t si_timerid;
  int32_t si_overrun;
  union sigval si_value;

  if (IsLinux()) {
    *si = m->linux;
    return;
  } else if (IsFreebsd()) {
    si_signo = m->freebsd.si_signo;
    si_errno = m->freebsd.si_errno;
    si_code = m->freebsd.si_code;
    si_pid = m->freebsd.si_pid;
    si_uid = m->freebsd.si_uid;
    si_status = m->freebsd.si_status;
    si_timerid = m->freebsd.si_timerid;
    si_overrun = m->freebsd.si_overrun;
    si_addr = m->freebsd.si_addr;
    si_value = m->freebsd.si_value;
  } else if (IsXnu()) {
    si_signo = m->xnu.si_signo;
    si_errno = m->xnu.si_errno;
    si_code = m->xnu.si_code;
    si_pid = m->xnu.si_pid;
    si_uid = m->xnu.si_uid;
    si_status = m->xnu.si_status;
    si_timerid = 0;
    si_overrun = 0;
    si_addr = m->xnu.si_addr;
    si_value = m->xnu.si_value;
  } else if (IsOpenbsd()) {
    si_signo = m->openbsd.si_signo;
    si_errno = m->openbsd.si_errno;
    si_code = m->openbsd.si_code;
    si_pid = m->openbsd.si_pid;
    si_uid = m->openbsd.si_uid;
    si_status = m->openbsd.si_status;
    si_timerid = 0;
    si_overrun = 0;
    si_addr = m->openbsd.si_addr;
    si_value = m->openbsd.si_value;
  } else if (IsNetbsd()) {
    si_signo = m->netbsd.si_signo;
    si_errno = m->netbsd.si_errno;
    si_code = m->netbsd.si_code;
    si_pid = m->netbsd.si_pid;
    si_uid = m->netbsd.si_uid;
    si_status = m->netbsd.si_status;
    si_timerid = 0;
    si_overrun = 0;
    si_addr = m->netbsd.si_addr;
    si_value = m->netbsd.si_value;
  } else {
    notpossible;
  }

  // Turn BUS_OBJERR into BUS_ADRERR for consistency with Linux.
  // See test/libc/calls/sigbus_test.c
  if (IsFreebsd() || IsOpenbsd()) {
    if (si_signo == 10 && si_code == 3) {
      si_code = 2;
    }
  }

  *si = (struct siginfo){0};
  si->si_signo = si_signo;
  si->si_errno = si_errno;
  si->si_code = si_code;
  si->si_value = si_value;
  if (si_signo == SIGILL ||   //
      si_signo == SIGFPE ||   //
      si_signo == SIGSEGV ||  //
      si_signo == SIGBUS ||   //
      si_signo == SIGTRAP) {
    si->si_addr = si_addr;
  } else if (si_signo == SIGCHLD) {
    si->si_status = si_status;
    si->si_pid = si_pid;
    si->si_uid = si_uid;
  } else if (si_signo == SIGALRM) {
    si->si_timerid = si_timerid;
    si->si_overrun = si_overrun;
  } else {
    si->si_pid = si_pid;
    si->si_uid = si_uid;
  }
}
