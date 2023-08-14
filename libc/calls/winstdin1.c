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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"

__msabi extern typeof(CloseHandle) *const __imp_CloseHandle;
__msabi extern typeof(CreateFile) *const __imp_CreateFileW;
__msabi extern typeof(CreateNamedPipe) *const __imp_CreateNamedPipeW;
__msabi extern typeof(CreateSemaphore) *const __imp_CreateSemaphoreW;
__msabi extern typeof(CreateThread) *const __imp_CreateThread;
__msabi extern typeof(GetStdHandle) *const __imp_GetStdHandle;
__msabi extern typeof(ReadFile) *const __imp_ReadFile;
__msabi extern typeof(WaitForSingleObject) *const __imp_WaitForSingleObject;
__msabi extern typeof(WriteFile) *const __imp_WriteFile;

__msabi static dontasan dontubsan dontinstrument textwindows uint32_t
WinStdinThread(void *lpParameter) {
  char buf[4096];
  uint32_t i, got, wrote;

  // wait forever for semaphore to exceed 0
  //
  // this semaphore is unlocked the first time read or poll happens. we
  // need it so the prog has time to call functions like SetConsoleMode
  // before we begin performing i/o.
  __imp_WaitForSingleObject(g_fds.stdin.inisem, -1u);
  __imp_CloseHandle(g_fds.stdin.inisem);

  // relay stdin to process
  NTTRACE("<stdin> activated");
  for (;;) {
    if (!__imp_ReadFile(g_fds.stdin.handle, buf, sizeof(buf), &got, 0)) {
      NTTRACE("<stdin> read failed");
      goto Finish;
    }
    if (!got) {
      NTTRACE("<stdin> end of file");
      goto Finish;
    }
    for (i = 0; i < got; i += wrote) {
      if (!__imp_WriteFile(g_fds.stdin.writer, buf + i, got - i, &wrote, 0)) {
        NTTRACE("<stdin> failed to write to pipe");
        goto Finish;
      }
    }
  }

Finish:
  __imp_CloseHandle(g_fds.stdin.writer);
  return 0;
}

// this makes it possible for our read() implementation to periodically
// poll for signals while performing a blocking overlapped io operation
dontasan dontubsan dontinstrument textwindows void WinMainStdin(void) {
  uint32_t mode;
  char16_t pipename[64];
  int64_t hStdin, hWriter, hReader, hThread, hSemaphore;
  if (!SupportsWindows()) return;
  hStdin = __imp_GetStdHandle(kNtStdInputHandle);
  if (hStdin == kNtInvalidHandleValue) {
    NTTRACE("<stdin> GetStdHandle failed");
    return;
  }
  // create non-inherited semaphore with initial value of 0
  hSemaphore = __imp_CreateSemaphoreW(0, 0, 1, 0);
  if (!hSemaphore) {
    NTTRACE("<stdin> CreateSemaphore failed");
    return;
  }
  __create_pipe_name(pipename);
  hReader = __imp_CreateNamedPipeW(
      pipename, kNtPipeAccessInbound | kNtFileFlagOverlapped,
      kNtPipeTypeByte | kNtPipeReadmodeByte, 1, 4096, 4096, 0, 0);
  if (hReader == kNtInvalidHandleValue) {
    NTTRACE("<stdin> CreateNamedPipe failed");
    return;
  }
  hWriter = __imp_CreateFileW(pipename, kNtGenericWrite, 0, 0, kNtOpenExisting,
                              kNtFileFlagOverlapped, 0);
  if (hWriter == kNtInvalidHandleValue) {
    NTTRACE("<stdin> CreateFile failed");
    return;
  }
  hThread = __imp_CreateThread(0, 65536, WinStdinThread, 0, 0, 0);
  if (!hThread) {
    NTTRACE("<stdin> CreateThread failed");
    return;
  }
  __imp_CloseHandle(hThread);
  g_fds.stdin.handle = hStdin;
  g_fds.stdin.reader = hReader;
  g_fds.stdin.writer = hWriter;
  g_fds.stdin.inisem = hSemaphore;
}
