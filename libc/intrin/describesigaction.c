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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/alloca.h"
#include "libc/sysv/consts/sa.h"

static const char *DescribeSigHandler(char buf[64], void f(int)) {
  if (f == SIG_ERR) return "SIG_ERR";
  if (f == SIG_DFL) return "SIG_DFL";
  if (f == SIG_IGN) return "SIG_IGN";
  ksnprintf(buf, 64, "%t", f);
  return buf;
}

static const char *DescribeSigFlags(char buf[64], int x) {
  const struct DescribeFlags kSigFlags[] = {
      {SA_NOCLDSTOP, "NOCLDSTOP"},  //
      {SA_NOCLDWAIT, "NOCLDWAIT"},  //
      {SA_SIGINFO, "SIGINFO"},      //
      {SA_ONSTACK, "ONSTACK"},      //
      {SA_RESTART, "RESTART"},      //
      {SA_NODEFER, "NODEFER"},      //
      {SA_RESETHAND, "RESETHAND"},  //
      {SA_NOMASK, "NOMASK"},        //
      {SA_ONESHOT, "ONESHOT"},      //
      {0x04000000, "RESTORER"},     //
  };
  return DescribeFlags(buf, 64, kSigFlags, ARRAYLEN(kSigFlags), "SA_", x);
}

#define N 256

#define append(...) o += ksnprintf(buf + o, N - o, __VA_ARGS__)

const char *(DescribeSigaction)(char buf[N], int rc,
                                const struct sigaction *sa) {
  int o = 0;
  char b64[64];

  if (rc == -1) return "n/a";
  if (!sa) return "NULL";
  if ((!IsAsan() && kisdangerous(sa)) ||
      (IsAsan() && !__asan_is_valid(sa, sizeof(*sa)))) {
    ksnprintf(buf, N, "%p", sa);
    return buf;
  }

  append("{.sa_handler=%s", DescribeSigHandler(b64, sa->sa_handler));

  if (sa->sa_flags) {
    append(", .sa_flags=%s", DescribeSigFlags(b64, sa->sa_flags));
  }

  if (!sigisemptyset(&sa->sa_mask)) {
    append(", .sa_mask=%s", DescribeSigset(rc, &sa->sa_mask));
  }

  append("}");

  return buf;
}
