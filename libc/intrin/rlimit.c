// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "libc/intrin/rlimit.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/intrin/strace.h"
#include "libc/nt/accounting.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/threadpriority.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/pib.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

/**
 * Resource Limits for Windows.
 *
 * This thread is created by WinMain() and it has a single purpose which
 * is enforcing `RLIMIT_CPU`. This module also serializes process limits
 * across execve() boundaries using an environment variable.
 *
 * The resource limits are stored in our Process Information Block. They
 * use bit-reflected values, which means the PIB can be allocated as BSS
 * memory initialized to zero that's interpreted as RLIM_INFINITY (-1L).
 * This means our environment variable looks like COSMO_RLIMIT=:0:0:0...
 * when encoded as a string of base-10 uint64_t values. However when the
 * user actually uses setrlimit() they'll be specifying numbers that are
 * much smaller than RLIM_INFINITY and would therefore waste space in an
 * environment variable. So what we do is zig-zag encode the values when
 * we serialize to the environment.
 *
 * The numbers in COSMO_RLIMIT variable are always base-10 and tokenized
 * on non-numeric characters. They come in {cur,max} max pairs. They use
 * the same ordering as the Linux RLIMIT_FOO constants. If you use fewer
 * than the full set of supported resource limits, then remaining values
 * are initialized to zero. If too many numbers are encountered then the
 * extra ones will be ignored.
 */

#define STACK_SIZE 65536

/* these apply zig-zag encoding to bit-reflected rlim_t values so that
   both really large numbers and really small numbers, will be encoded
   efficiently as really small values, saving space in the environment */
#define RLIMIT_SERIALIZE(x)   (((uint64_t)(x) << 1) ^ ((int64_t)(x) >> 63))
#define RLIMIT_DESERIALIZE(x) (((uint64_t)(x) >> 1) ^ -((uint64_t)(x) & 1))

__msabi extern typeof(CreateThread) *const __imp_CreateThread;
__msabi extern typeof(GetCurrentThread) *const __imp_GetCurrentThread;
__msabi extern typeof(GetProcessTimes) *const __imp_GetProcessTimes;
__msabi extern typeof(SleepEx) *const __imp_SleepEx;

textwindows dontinstrument static uint32_t __rlimit_worker(void *arg) {
  struct CosmoTib tls;
  __bootstrap_tls(&tls, __builtin_frame_address(0));
  STRACE("__rlimit_worker() started");
  long next_sigxcpu_signal = 0;
  for (long i = 0;; ++i) {
    __imp_SleepEx(POLL_INTERVAL_MS, 0);
    struct NtFileTime birth, exit, k, u;
    if (__imp_GetProcessTimes(GetCurrentProcess(), &birth, &exit, &k, &u)) {
      struct timespec cputime =
          WindowsDurationToTimeSpec(ReadFileTime(k) + ReadFileTime(u));
      if (cputime.tv_sec >= ~__get_pib()->rlimit[RLIMIT_CPU].rlim_max) {
        STRACE("process exceeded cpu hard limit");
        TerminateThisProcess(SIGKILL);
      }
      if (i >= next_sigxcpu_signal &&
          cputime.tv_sec >= ~__get_pib()->rlimit[RLIMIT_CPU].rlim_cur) {
        next_sigxcpu_signal = i + 1000 / POLL_INTERVAL_MS;
        atomic_fetch_or(__get_pib()->sigpending, 1ull << (SIGXCPU - 1));
      }
    }
  }
  __builtin_unreachable();
}

textwindows static bool __rlimit_atoi(const char **str, rlim_t *res) {
  int c, g = 0;
  uint64_t x = 0;
  for (; (c = **str); ++(*str)) {
    if ('0' <= c && c <= '9') {
      g = 1;
      x *= 10;
      x += c - '0';
    } else if (g) {
    ReturnResult:
      *res = RLIMIT_DESERIALIZE(x);
      return true;
    }
  }
  if (g)
    goto ReturnResult;
  return false;
}

// this is called after fork in child
textwindows void __rlimit_launch(void) {
  __imp_CreateThread(0, STACK_SIZE, __rlimit_worker, 0,
                     kNtStackSizeParamIsAReservation, 0);
}

// this is called upon initialization of executed processes
__attribute__((__constructor__(1))) textwindows void __rlimit_init(void) {
  if (!IsWindows())
    return;
  __rlimit_launch();
  const char *str;
  if ((str = getenv("_COSMO_RLIMIT"))) {
    rlim_t *rlims = (rlim_t *)__get_pib()->rlimit;
    for (size_t i = 0; i < RLIM_NLIMITS * 2; ++i)
      if (!__rlimit_atoi(&str, &rlims[i]))
        break;
  }
}

// this is called from execve()
textwindows void __rlimit_serialize(char buf[RLIMIT_VAR_BUF_SIZE]) {
  size_t n = RLIM_NLIMITS * 2;
  char *p = stpcpy(buf, "_COSMO_RLIMIT=");
  rlim_t *rlims = (rlim_t *)__get_pib()->rlimit;
  while (n && !rlims[n - 1])
    --n;
  for (size_t i = 0; i < n; ++i) {
    *p++ = ':';
    p = FormatUint64(p, RLIMIT_SERIALIZE(rlims[i]));
  }
}

#endif /* __x86_64__ */
