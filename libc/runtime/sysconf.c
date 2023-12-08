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
#include "libc/runtime/sysconf.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sysinfo.h"
#include "libc/calls/struct/sysinfo.internal.h"
#include "libc/dce.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/runtime/clktck.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/_posix.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/ss.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"

/**
 * Returns configuration value about system.
 *
 * The following parameters are supported:
 *
 * - `_SC_CLK_TCK` returns number of clock ticks per second
 * - `_SC_ARG_MAX` will perform expensive rlimit calculations
 * - `_SC_SIGSTKSZ` returns host platform's preferred SIGSTKSZ
 * - `_SC_MINSIGSTKSZ` returns host platform's required MINSIGSTKSZ
 * - `_SC_PAGESIZE` currently always returns 65536 due to Windows
 * - `_SC_AVPHYS_PAGES` returns average physical memory pages
 * - `_SC_PHYS_PAGES` returns physical memory pages available
 * - `_SC_NPROCESSORS_ONLN` returns number of effective CPUs
 * - `_SC_CHILD_MAX` returns maximum number of processes
 * - `_SC_OPEN_MAX` returns maximum number of open files
 *
 * @return value on success, or -1 w/ errno
 * @raise EINVAL if `name` isn't valid
 */
long sysconf(int name) {
  switch (name) {
    case _SC_CLK_TCK:
      return CLK_TCK;
    case _SC_PAGESIZE:
      return FRAMESIZE;
    case _SC_ARG_MAX:
      return __get_arg_max();
    case _SC_SIGSTKSZ:
      return _SIGSTKSZ;
    case _SC_MINSIGSTKSZ:
      return __get_minsigstksz();
    case _SC_CHILD_MAX:
      return __get_rlimit(RLIMIT_NPROC);
    case _SC_OPEN_MAX:
      return __get_rlimit(RLIMIT_NOFILE);
    case _SC_NPROCESSORS_CONF:
    case _SC_NPROCESSORS_ONLN:
      return __get_cpu_count();
    case _SC_PHYS_PAGES:
      return __get_phys_pages();
    case _SC_AVPHYS_PAGES:
      return __get_avphys_pages();
    case _SC_THREADS:
      return _POSIX_THREADS;
    case _SC_THREAD_DESTRUCTOR_ITERATIONS:
      return PTHREAD_DESTRUCTOR_ITERATIONS;
    case _SC_THREAD_KEYS_MAX:
      return PTHREAD_KEYS_MAX;
    case _SC_THREAD_STACK_MIN:
      return PTHREAD_STACK_MIN;
    case _SC_THREAD_THREADS_MAX:
      return LONG_MAX;
    case _SC_LINE_MAX:
      return _POSIX2_LINE_MAX;
    case _SC_MONOTONIC_CLOCK:
      return _POSIX_MONOTONIC_CLOCK;
    case _SC_HOST_NAME_MAX:
      return _POSIX_HOST_NAME_MAX;
    case _SC_SPAWN:
      return _POSIX_SPAWN;
    case _SC_SYMLOOP_MAX:
      return _POSIX_SYMLOOP_MAX;
    default:
      return einval();
  }
}
