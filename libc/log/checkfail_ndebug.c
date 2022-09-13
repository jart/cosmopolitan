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
#include "libc/intrin/kprintf.h"
#include "libc/intrin/weaken.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"

/**
 * Handles failure of CHECK_xx() macros in -DNDEBUG mode.
 *
 * This handler (1) makes binaries smaller by not embedding source code;
 * and therefore (2) less likely to leak sensitive information. This can
 * still print backtraces with function names if the .com.dbg file is in
 * the same folder.
 *
 * @see libc/log/thunks/__check_fail_ndebug.S
 */
relegated wontreturn void __check_fail_ndebug(uint64_t want, uint64_t got,
                                              const char *file, int line,
                                              const char *opchar,
                                              const char *fmt, ...) {
  va_list va;
  __restore_tty();
  kprintf("%rerror:%s:%d: check failed: %'ld %s %'ld% m", file, line, want,
          opchar, got);
  if (*fmt) {
    kprintf(": ");
    va_start(va, fmt);
    kvprintf(fmt, va);
    va_end(va);
  }
  kprintf("\n");
  if (_weaken(__die)) _weaken(__die)();
  _Exitr(68);
}
