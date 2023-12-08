/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/memory.h"

__msabi extern typeof(FlushViewOfFile) *const __imp_FlushViewOfFile;

/**
 * Syncs memory created by MapViewOfFileEx().
 *
 * This doesn't wait until the pages are written out to the physical
 * medium. This doesn't update timestamps or file/dir metadata.
 *
 * @note this wrapper takes care of ABI, STRACE(), and __winerr()
 * @note consider buying a ups
 * @see FlushFileBuffers()
 */
textwindows bool32 FlushViewOfFile(const void *lpBaseAddress,
                                   size_t dwNumberOfBytesToFlush) {
  bool32 ok;
  ok = __imp_FlushViewOfFile(lpBaseAddress, dwNumberOfBytesToFlush);
  if (!ok) __winerr();
  NTTRACE("FlushViewOfFile(%p, %'zu) → %hhhd% m", lpBaseAddress,
          dwNumberOfBytesToFlush, ok);
  return ok;
}
