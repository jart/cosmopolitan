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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/fmt/internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/filemovemethod.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/str/str.h"
#ifdef __x86_64__

#define COSMO_PID_UPPER_BOUND 0x100000  // about one million
#define COSMO_PID_LOWER_BOUND 0x10000   // avoid pids windows likes

// cut back on code size and avoid setting errno
// this code is a mandatory dependency of winmain
__msabi extern typeof(CloseHandle) *const __imp_CloseHandle;
__msabi extern typeof(CreateDirectory) *const __imp_CreateDirectoryW;
__msabi extern typeof(CreateFile) *const __imp_CreateFileW;
__msabi extern typeof(CreateFileMapping) *const __imp_CreateFileMappingW;
__msabi extern typeof(GetLastError) *const __imp_GetLastError;
__msabi extern typeof(MapViewOfFileEx) *const __imp_MapViewOfFileEx;
__msabi extern typeof(SetEndOfFile) *const __imp_SetEndOfFile;
__msabi extern typeof(SetFilePointerEx) *const __imp_SetFilePointerEx;

textwindows static uint32_t ProcessPrng32(void) {
  uint32_t r;
  ProcessPrng(&r, sizeof(r));
  return r;
}

// Daniel Lemire, "Fast Random Integer Generation in an Interval",
// Association for Computing Machinery, ACM Trans. Model. Comput.
// Simul., no. 1, vol. 29, pp. 1--12, New York, NY, USA, January 2019.
textwindows static uint32_t ProcessPrngUniform(uint32_t upper_bound) {
  if (upper_bound <= 1)
    return 0;
  uint64_t product = upper_bound * (uint64_t)ProcessPrng32();
  if ((uint32_t)product < upper_bound) {
    uint32_t threshold = -upper_bound % upper_bound;
    while ((uint32_t)product < threshold)
      product = upper_bound * (uint64_t)ProcessPrng32();
  }
  return product >> 32;
}

textwindows int __generate_pid(atomic_ulong **sigpending) {
  for (;;) {
    int pid =
        ProcessPrngUniform(COSMO_PID_UPPER_BOUND - COSMO_PID_LOWER_BOUND - 1) +
        COSMO_PID_LOWER_BOUND + 1;
    if ((*sigpending = __sig_map_process(pid, kNtCreateNew)))
      return pid;
  }
}

textwindows static bool __sig_makedirs(char16_t *p, char16_t *e) {
  while (e > p) {
    --e;
    if (*e == '\\')
      break;
  }
  if (e - p < 3)
    return true;
  for (;;) {
    char16_t c = *e;
    *e = 0;
    bool32 ok = __imp_CreateDirectoryW(p, 0);
    *e = c;
    if (ok)
      return true;
    uint32_t err = __imp_GetLastError();
    if (err == kNtErrorAlreadyExists)
      return true;
    if (err != kNtErrorPathNotFound)
      return false;
    if (!__sig_makedirs(p, e))
      return false;
  }
}

// Generates C:\ProgramData\cosmo\sig\x\y.pid like path
textwindows char16_t *__sig_process_path(char16_t *path, uint32_t pid) {
  char16_t *p = path;
  *p++ = __getcosmosdrive();
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
  *p++ = '\\';
  *p++ = 'c';
  *p++ = 'o';
  *p++ = 's';
  *p++ = 'm';
  *p++ = 'o';
  *p++ = '\\';
  *p++ = 's';
  *p++ = 'i';
  *p++ = 'g';
  *p++ = '\\';
  p = __itoa16(p, (pid & 0x000ff800) >> 11);
  *p++ = '\\';
  p = __itoa16(p, pid);
  *p++ = '.';
  *p++ = 'p';
  *p++ = 'i';
  *p++ = 'd';
  *p = 0;
  return path;
}

textwindows atomic_ulong *__sig_map_process(int pid, int disposition) {
  char16_t path[128];
  __sig_process_path(path, pid);
  intptr_t hand;
  for (;;) {
    hand = __imp_CreateFileW(path, kNtGenericRead | kNtGenericWrite,
                             kNtFileShareRead | kNtFileShareWrite, 0,
                             disposition, kNtFileAttributeNormal, 0);
    if (hand != -1)
      break;
    if (disposition == kNtOpenAlways || disposition == kNtCreateNew) {
      uint32_t err = __imp_GetLastError();
      if (err == kNtErrorPathNotFound)
        if (__sig_makedirs(path, path + strlen16(path)))
          continue;
    }
    return 0;
  }
  __imp_SetFilePointerEx(hand, 8, 0, kNtFileBegin);
  __imp_SetEndOfFile(hand);
  intptr_t map = __imp_CreateFileMappingW(hand, 0, kNtPageReadwrite, 0, 8, 0);
  if (!map) {
    __imp_CloseHandle(hand);
    return 0;
  }
  atomic_ulong *sigs =
      __imp_MapViewOfFileEx(map, kNtFileMapRead | kNtFileMapWrite, 0, 0, 8, 0);
  __imp_CloseHandle(map);
  __imp_CloseHandle(hand);
  return sigs;
}

#endif /* __x86_64__ */
