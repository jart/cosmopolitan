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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/intrin/describebacktrace.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"

/**
 * Exits process with crash report.
 *
 * The `cosmoaddr2line` command may be copied and pasted into the shell
 * to obtain further details such as function calls and source lines in
 * the backtrace. Unlike abort() this function doesn't depend on signal
 * handling infrastructure. If tcsetattr() was called earlier to change
 * terminal settings, then they'll be restored automatically. Your exit
 * handlers won't be called. The `KPRINTF_LOG` environment variable may
 * configure the output location of these reports, defaulting to stderr
 * which is duplicated at startup, in case the program closes the file.
 *
 * @see __minicrash() for signal handlers, e.g. handling abort()
 * @asyncsignalsafe
 * @vforksafe
 */
relegated wontreturn void __die(void) {
  char host[128];
  __restore_tty();
  strcpy(host, "unknown");
  gethostname(host, sizeof(host));
  kprintf("%serror: %s on %s pid %d tid %d has perished%s\n"
          "cosmoaddr2line %s %s\n",
          __nocolor ? "" : "\e[1;31m", program_invocation_short_name, host,
          getpid(), gettid(), __nocolor ? "" : "\e[0m", FindDebugBinary(),
          DescribeBacktrace(__builtin_frame_address(0)));
  ShowBacktrace(2, __builtin_frame_address(0));
  _Exit(77);
}
