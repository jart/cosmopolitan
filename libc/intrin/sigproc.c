/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/atomic.h"
#include "libc/calls/sig.internal.h"
#include "libc/fmt/internal.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/filemovemethod.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thunk/msabi.h"
#ifdef __x86_64__

#define ABI __msabi textwindows dontinstrument

// cut back on code size and avoid setting errno
// this code is a mandatory dependency of winmain
__msabi extern typeof(CloseHandle) *const __imp_CloseHandle;
__msabi extern typeof(CreateDirectory) *const __imp_CreateDirectoryW;
__msabi extern typeof(CreateFile) *const __imp_CreateFileW;
__msabi extern typeof(CreateFileMapping) *const __imp_CreateFileMappingW;
__msabi extern typeof(MapViewOfFileEx) *const __imp_MapViewOfFileEx;
__msabi extern typeof(SetEndOfFile) *const __imp_SetEndOfFile;
__msabi extern typeof(SetFilePointer) *const __imp_SetFilePointer;
__msabi extern typeof(GetEnvironmentVariable)
    *const __imp_GetEnvironmentVariableW;

// Generates C:\ProgramData\cosmo\sig\x\y.pid like path
ABI char16_t *__sig_process_path(char16_t *path, uint32_t pid,
                                 int create_directories) {
  char16_t buf[3];
  char16_t *p = path;
  uint32_t vlen = __imp_GetEnvironmentVariableW(u"SYSTEMDRIVE", buf, 3);
  *p++ = vlen == 2 ? buf[0] : 'C';
  *p++ = ':';
  *p++ = '\\';
  *p++ = 'P';
  *p++ = 'r';
  *p++ = 'o';
  *p++ = 'g';
  *p++ = 'r';
  *p++ = 'a';
  *p++ = 'm';
  *p++ = 'D';
  *p++ = 'a';
  *p++ = 't';
  *p++ = 'a';
  *p = 0;
  if (create_directories)
    __imp_CreateDirectoryW(path, 0);
  *p++ = '\\';
  *p++ = 'c';
  *p++ = 'o';
  *p++ = 's';
  *p++ = 'm';
  *p++ = 'o';
  *p = 0;
  if (create_directories)
    __imp_CreateDirectoryW(path, 0);
  *p++ = '\\';
  *p++ = 's';
  *p++ = 'i';
  *p++ = 'g';
  *p = 0;
  if (create_directories)
    __imp_CreateDirectoryW(path, 0);
  *p++ = '\\';
  p = __itoa16(p, (pid & 0x000ff800) >> 11);
  *p = 0;
  if (create_directories)
    __imp_CreateDirectoryW(path, 0);
  *p++ = '\\';
  p = __itoa16(p, pid);
  *p++ = '.';
  *p++ = 'p';
  *p++ = 'i';
  *p++ = 'd';
  *p = 0;
  return path;
}

ABI atomic_ulong *__sig_map_process(int pid, int disposition) {
  char16_t path[128];
  __sig_process_path(path, pid, disposition == kNtOpenAlways);
  intptr_t hand = __imp_CreateFileW(path, kNtGenericRead | kNtGenericWrite,
                                    kNtFileShareRead | kNtFileShareWrite, 0,
                                    disposition, kNtFileAttributeNormal, 0);
  if (hand == -1)
    return 0;
  __imp_SetFilePointer(hand, 8, 0, kNtFileBegin);
  __imp_SetEndOfFile(hand);
  intptr_t map = __imp_CreateFileMappingW(hand, 0, kNtPageReadwrite, 0, 8, 0);
  if (!map) {
    __imp_CloseHandle(hand);
    return 0;
  }
  atomic_ulong *sigs = __imp_MapViewOfFileEx(map, kNtFileMapWrite, 0, 0, 8, 0);
  __imp_CloseHandle(map);
  __imp_CloseHandle(hand);
  return sigs;
}

#endif /* __x86_64__ */
