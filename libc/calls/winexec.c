/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/overlapped.h"

// checks if file should be considered an executable on windows
textwindows int IsWindowsExecutable(int64_t handle) {

  // read first two bytes of file
  // access() and stat() aren't cancelation points
  bool ok;
  char buf[2];
  uint32_t got;
  BLOCK_SIGNALS;
  struct NtOverlapped overlap = {.hEvent = CreateEvent(0, 0, 0, 0)};
  ok = (ReadFile(handle, buf, 2, 0, &overlap) ||
        GetLastError() == kNtErrorIoPending) &&
       GetOverlappedResult(handle, &overlap, &got, true);
  CloseHandle(overlap.hEvent);
  ALLOW_SIGNALS;

  // it's an executable if it starts with `MZ` or `#!`
  return ok && got == 2 &&                     //
         ((buf[0] == 'M' && buf[1] == 'Z') ||  //
          (buf[0] == '#' && buf[1] == '!'));
}
