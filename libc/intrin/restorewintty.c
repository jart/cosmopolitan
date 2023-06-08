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
#include "libc/dce.h"
#include "libc/nt/console.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/internal.h"

__msabi extern typeof(GetCurrentProcessId) *const __imp_GetCurrentProcessId;
__msabi extern typeof(SetConsoleMode) *const __imp_SetConsoleMode;
__msabi extern typeof(GetStdHandle) *const __imp_GetStdHandle;

extern uint32_t __pid_exec;

const unsigned char kConsoleHandles[3] = {
    kNtStdInputHandle,
    kNtStdOutputHandle,
    kNtStdErrorHandle,
};

// Puts cmd.exe gui back the way it was.
privileged noinstrument void _restorewintty(void) {
  if (!IsWindows()) return;
  if (__imp_GetCurrentProcessId() != __pid_exec) return;
  for (int i = 0; i < 3; ++i) {
    __imp_SetConsoleMode(__imp_GetStdHandle(kConsoleHandles[i]),
                         __ntconsolemode[i]);
  }
}
