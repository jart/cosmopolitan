// clang-format off
/* Copyright 2016 Google Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License. */

#ifndef NSYNC_PLATFORM_LINUX_PLATFORM_H_
#define NSYNC_PLATFORM_LINUX_PLATFORM_H_

#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE /* for futexes */
#endif

#include "libc/mem/alg.h"
#include "libc/str/str.h"
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/runtime/pathconf.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "third_party/getopt/getopt.h"
#include "libc/errno.h"
#include "libc/mem/alg.h"
#include "libc/fmt/conv.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/temp.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/gdtoa/gdtoa.h"

#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/weirdtypes.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/sched.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "libc/fmt/conv.h"
#include "libc/inttypes.h"
#include "libc/thread/thread.h"
#include "libc/limits.h"
#include "libc/sysv/consts/_posix.h"
#include "libc/sysv/consts/iov.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/xopen.h"
#include "libc/sysv/consts/futex.h"
#include "libc/sysv/consts/nr.h"
#include "libc/calls/calls.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"
#include "libc/calls/semaphore.internal.h"

#include "libc/calls/calls.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/lock.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"


#endif /*NSYNC_PLATFORM_LINUX_PLATFORM_H_*/
