/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "third_party/linenoise/linenoise.h"

#define Read32(s) (s[3] << 24 | s[2] << 16 | s[1] << 8 | s[0])
#define EXT(s)    Read32(s "\0\0")

static inline bool IsGraph(wint_t c) {
  return 0x21 <= c && c <= 0x7E;
}

static uint32_t GetFileExtension(const char16_t *s) {
  uint32_t w;
  size_t i, n;
  n = s ? strlen16(s) : 0;
  for (i = w = 0; n--;) {
    wint_t c = s[n];
    if (!IsGraph(c)) return 0;
    if (c == '.') break;
    if (++i > 4) return 0;
    w <<= 8;
    w |= kToLower[c];
  }
  return w;
}

// checks if file should be considered an executable on windows
textwindows int IsWindowsExecutable(int64_t handle, const char16_t *path) {

  // fast path known file extensions
  // shaves away 100ms of gnu make latency in cosmo monorepo
  uint32_t ext;
  if (!IsTiny() && (ext = GetFileExtension(path))) {
    if (ext == EXT("c") ||   // c code
        ext == EXT("cc") ||  // c++ code
        ext == EXT("h") ||   // c/c++ header
        ext == EXT("s") ||   // assembly code
        ext == EXT("o")) {   // object file
      return false;
    }
    if (ext == EXT("com") ||  // mz executable
        ext == EXT("exe") ||  // mz executable
        ext == EXT("sh")) {   // bourne shells
      return true;
    }
  }

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
