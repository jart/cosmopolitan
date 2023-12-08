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
#include "libc/assert.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/memory.h"

__msabi extern typeof(MapViewOfFileEx) *const __imp_MapViewOfFileEx;

/**
 * Maps view of file mapping into memory on the New Technology.
 *
 * @param hFileMappingObject was returned by CreateFileMapping()
 * @param dwDesiredAccess has kNtFileMap... flags
 * @param opt_lpDesiredBaseAddress may be NULL to let o/s choose
 * @return base address, or NULL on failure
 * @note this wrapper takes care of ABI, STRACE(), and __winerr()
 */
textwindows void *MapViewOfFileEx(int64_t hFileMappingObject,
                                  uint32_t dwDesiredAccess,
                                  uint32_t dwFileOffsetHigh,
                                  uint32_t dwFileOffsetLow,
                                  size_t dwNumberOfBytesToMap,
                                  void *opt_lpDesiredBaseAddress) {
  void *pStartingAddress;
  pStartingAddress = __imp_MapViewOfFileEx(
      hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow,
      dwNumberOfBytesToMap, opt_lpDesiredBaseAddress);
  if (!pStartingAddress) __winerr();
  NTTRACE("MapViewOfFileEx(%ld, %s, %'ld, %'zu, %p) → %p% m",
          hFileMappingObject, DescribeNtFileMapFlags(dwDesiredAccess),
          (uint64_t)dwFileOffsetHigh << 32 | dwFileOffsetLow,
          dwNumberOfBytesToMap, opt_lpDesiredBaseAddress, pStartingAddress);
  return pStartingAddress;
}
