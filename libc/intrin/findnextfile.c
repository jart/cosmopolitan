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
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/win32finddata.h"
#include "libc/nt/thunk/msabi.h"

__msabi extern typeof(FindNextFile) *const __imp_FindNextFileW;

/**
 * Finds more files in directory.
 * @note this wrapper takes care of ABI, STRACE(), and __winerr()
 */
textwindows bool32 FindNextFile(int64_t hFindFile,
                                struct NtWin32FindData *out_lpFindFileData) {
  bool32 ok;
  ok = __imp_FindNextFileW(hFindFile, out_lpFindFileData);
  if (ok) {
    NTTRACE("FindNextFile(%ld, [{"
            ".cFileName=%#hs, "
            ".dwFileAttributes=%s, "
            ".dwFileType=%s"
            "}]) → %hhhd% m",
            hFindFile, out_lpFindFileData->cFileName,
            DescribeNtFileFlagAttr(out_lpFindFileData->dwFileAttributes),
            DescribeNtFiletypeFlags(out_lpFindFileData->dwFileType), ok);
  } else {
    if (GetLastError() != kNtErrorNoMoreFiles) __winerr();
    NTTRACE("FindNextFile(%ld) → %hhhd% m", hFindFile, ok);
  }
  return ok;
}
