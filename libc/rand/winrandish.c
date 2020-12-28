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
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nt/dll.h"
#include "libc/nt/events.h"
#include "libc/nt/struct/point.h"
#include "libc/nt/struct/teb.h"
#include "libc/rand/rand.h"

/**
 * Returns somewhat randomish number on Windows.
 */
textwindows int64_t winrandish(void) {
  int64_t res;
  struct NtPoint point;
  res = ((int64_t)NtGetPid() << 17) ^ NtGetTid() ^ rdtsc();
  /*
   * This function is intended for older CPUs built before 2012, so
   * let's avoid having our CUI apps yoink USER32.DLL until we're
   * certain we need it, thus avoiding a hundred lines of noise in
   * NtTrace.exe output.
   */
  typeof(GetCursorPos) *GetCursorPos_ =
      GetProcAddress(GetModuleHandle("user32.dll"), "GetCursorPos");
  if (GetCursorPos_ && GetCursorPos_(&point)) res ^= point.x * point.y;
  return res;
}
