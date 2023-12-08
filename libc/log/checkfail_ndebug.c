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
static relegated wontreturn void __check_fail_ndebug(uint64_t want,       //
                                                     uint64_t got,        //
                                                     const char *file,    //
                                                     int line,            //
                                                     const char *opchar,  //
                                                     const char *fmt,     //
                                                     va_list va) {
  __restore_tty();
  kprintf("%rerror:%s:%d: check failed: %'ld %s %'ld% m", file, line, want,
          opchar, got);
  if (fmt && *fmt) {
    kprintf(": ");
    kvprintf(fmt, va);
  }
  kprintf("\n");
  if (_weaken(__die)) _weaken(__die)();
  _Exit(68);
}

void __check_fail_eq(uint64_t want, uint64_t got, const char *file, int line,
                     const char *opchar, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  __check_fail_ndebug(want, got, file, line, opchar, fmt, va);
  va_end(va);
}

void __check_fail_ne(uint64_t want, uint64_t got, const char *file, int line,
                     const char *opchar, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  __check_fail_ndebug(want, got, file, line, opchar, fmt, va);
  va_end(va);
}

void __check_fail_le(uint64_t want, uint64_t got, const char *file, int line,
                     const char *opchar, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  __check_fail_ndebug(want, got, file, line, opchar, fmt, va);
  va_end(va);
}

void __check_fail_lt(uint64_t want, uint64_t got, const char *file, int line,
                     const char *opchar, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  __check_fail_ndebug(want, got, file, line, opchar, fmt, va);
  va_end(va);
}

void __check_fail_ge(uint64_t want, uint64_t got, const char *file, int line,
                     const char *opchar, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  __check_fail_ndebug(want, got, file, line, opchar, fmt, va);
  va_end(va);
}

void __check_fail_gt(uint64_t want, uint64_t got, const char *file, int line,
                     const char *opchar, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  __check_fail_ndebug(want, got, file, line, opchar, fmt, va);
  va_end(va);
}
