/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/weaken.h"
#include "libc/log/backtrace.internal.h"
#include "libc/runtime/symbols.internal.h"

static _Thread_local bool noreentry;

/**
 * Shows backtrace if crash reporting facilities are linked.
 */
void _bt(const char *fmt, ...) {
  int e;
  va_list va;

  if (!noreentry) {
    noreentry = true;
  } else {
    return;
  }

  if (fmt) {
    va_start(va, fmt);
    kvprintf(fmt, va);
    va_end(va);
  }

  if (_weaken(ShowBacktrace) && _weaken(GetSymbolTable)) {
    e = errno;
    _weaken(ShowBacktrace)(2, __builtin_frame_address(0));
    errno = e;
  } else {
    kprintf("_bt() can't show backtrace because you need:\n"
            "\tSTATIC_YOINK(\"ShowBacktrace\");\n"
            "to be linked.\n");
    if (_weaken(PrintBacktraceUsingSymbols) && _weaken(GetSymbolTable)) {
      e = errno;
      _weaken(PrintBacktraceUsingSymbols)(2, __builtin_frame_address(0),
                                          _weaken(GetSymbolTable)());
      errno = e;
    } else {
      kprintf("_bt() can't show backtrace because you need:\n"
              "\tSTATIC_YOINK(\"PrintBacktraceUsingSymbols\");\n"
              "\tSTATIC_YOINK(\"GetSymbolTable\");\n"
              "to be linked.\n");
    }
  }

  noreentry = false;
}
