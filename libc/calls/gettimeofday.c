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
#include "libc/assert.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/itimerval.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/timeval.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"
#include "libc/time/struct/timezone.h"

typedef axdx_t gettimeofday_f(struct timeval *, struct timezone *, void *);
static gettimeofday_f __gettimeofday_init;
static gettimeofday_f *__gettimeofday = __gettimeofday_init;

/**
 * Returns system wall time in microseconds, e.g.
 *
 *     int64_t t;
 *     char p[20];
 *     struct tm tm;
 *     struct timeval tv;
 *     gettimeofday(&tv, 0);
 *     t = tv.tv_sec;
 *     gmtime_r(&t, &tm);
 *     iso8601(p, &tm);
 *     printf("%s\n", p);
 *
 * @param tv points to timeval that receives result if non-NULL
 * @param tz receives UTC timezone if non-NULL
 * @see	clock_gettime() for nanosecond precision
 * @see	strftime() for string formatting
 */
int gettimeofday(struct timeval *tv, struct timezone *tz) {
  int rc = __gettimeofday(tv, tz, 0).ax;
#if SYSDEBUG
  if (__tls_enabled && !(__get_tls()->tib_flags & TIB_FLAG_TIME_CRITICAL)) {
    POLLTRACE("gettimeofday([%s], %p) → %d% m", DescribeTimeval(rc, tv), tz,
              rc);
  }
#endif
  return rc;
}

/**
 * Returns pointer to fastest gettimeofday().
 */
gettimeofday_f *__gettimeofday_get(bool *opt_out_isfast) {
  bool isfast;
  gettimeofday_f *res;
  if (IsLinux() && (res = __vdsosym("LINUX_2.6", "__vdso_gettimeofday"))) {
    isfast = true;
  } else if (IsWindows()) {
    isfast = true;
    res = sys_gettimeofday_nt;
  } else if (IsXnu()) {
#ifdef __x86_64__
    res = sys_gettimeofday_xnu;
    isfast = false;
#elif defined(__aarch64__)
    res = sys_gettimeofday_m1;
    isfast = true;
#else
#error "unsupported architecture"
#endif
  } else if (IsMetal()) {
    isfast = false;
    res = sys_gettimeofday_metal;
  } else {
    isfast = false;
    res = sys_gettimeofday;
  }
  if (opt_out_isfast) {
    *opt_out_isfast = isfast;
  }
  return res;
}

static axdx_t __gettimeofday_init(struct timeval *tv,   //
                                  struct timezone *tz,  //
                                  void *arg) {
  gettimeofday_f *gettime;
  __gettimeofday = gettime = __gettimeofday_get(0);
  return gettime(tv, tz, 0);
}
