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
#include "libc/intrin/atomic.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/thread/tls.h"
#include "libc/thread/tls2.internal.h"

/**
 * @fileoverview makes windows stdin handle capable of being poll'd
 *
 * 1. On Windows, there's no way to check how many bytes of input are
 *    available from the cmd.exe console. The only thing you can do is a
 *    blocking read that can't be interrupted.
 *
 * 2. On Windows, it's up to the parent process whether or not the
 *    handles it passes us are capable of non-blocking overlapped i/o
 *    reads (which we need for busy polling to check for interrupts).
 *
 * So we solve this by creating a thread which just does naive reads on
 * standard input, and then relays the data to the process via a named
 * pipe, which we explicitly creaete with overlapped i/o enabled.
 *
 * This code runs very early during process initialization, at the
 * beginning of WinMain(). This module is only activated if the app
 * links any one of: poll(), select(), or ioctl(FIONREAD).
 */

__msabi extern typeof(CloseHandle) *const __imp_CloseHandle;
__msabi extern typeof(CreateFile) *const __imp_CreateFileW;
__msabi extern typeof(CreateNamedPipe) *const __imp_CreateNamedPipeW;
__msabi extern typeof(CreateSemaphore) *const __imp_CreateSemaphoreW;
__msabi extern typeof(CreateThread) *const __imp_CreateThread;
__msabi extern typeof(GetCurrentThreadId) *const __imp_GetCurrentThreadId;
__msabi extern typeof(GetStdHandle) *const __imp_GetStdHandle;
__msabi extern typeof(ReadFile) *const __imp_ReadFile;
__msabi extern typeof(WaitForSingleObject) *const __imp_WaitForSingleObject;
__msabi extern typeof(WriteFile) *const __imp_WriteFile;

static unsigned long StrLen(const char *s) {
  unsigned long n = 0;
  while (*s++) ++n;
  return n;
}

static void Log(const char *s) {
#if 0
  __imp_WriteFile(__imp_GetStdHandle(kNtStdErrorHandle), s, StrLen(s), 0, 0);
#endif
}

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
  Log("<stdin> activated\n");
  for (;;) {
    if (!__imp_ReadFile(g_fds.stdin.handle, buf, sizeof(buf), &got, 0)) {
      Log("<stdin> read failed\n");
      goto Finish;
    }
    if (!got) {
      Log("<stdin> end of file\n");
      goto Finish;
    }
    for (i = 0; i < got; i += wrote) {
      if (!__imp_WriteFile(g_fds.stdin.writer, buf + i, got - i, &wrote, 0)) {
        Log("<stdin> failed to write to pipe\n");
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
  char16_t pipename[64];
  int64_t hStdin, hWriter, hReader, hThread, hSemaphore;
  if (!SupportsWindows()) return;
  hStdin = __imp_GetStdHandle(kNtStdInputHandle);
  if (hStdin == kNtInvalidHandleValue) {
    Log("<stdin> GetStdHandle failed\n");
    return;
  }
  // create non-inherited semaphore with initial value of 0
  hSemaphore = __imp_CreateSemaphoreW(0, 0, 1, 0);
  if (!hSemaphore) {
    Log("<stdin> CreateSemaphore failed\n");
    return;
  }
  __create_pipe_name(pipename);
  hReader = __imp_CreateNamedPipeW(
      pipename, kNtPipeAccessInbound | kNtFileFlagOverlapped,
      kNtPipeTypeByte | kNtPipeReadmodeByte, 1, 4096, 4096, 0, 0);
  if (hReader == kNtInvalidHandleValue) {
    Log("<stdin> CreateNamedPipe failed\n");
    return;
  }
  hWriter = __imp_CreateFileW(pipename, kNtGenericWrite, 0, 0, kNtOpenExisting,
                              kNtFileFlagOverlapped, 0);
  if (hWriter == kNtInvalidHandleValue) {
    Log("<stdin> CreateFile failed\n");
    return;
  }
  hThread = __imp_CreateThread(0, 65536, WinStdinThread, 0, 0, 0);
  if (!hThread) {
    Log("<stdin> CreateThread failed\n");
    return;
  }
  g_fds.stdin.handle = hStdin;
  g_fds.stdin.thread = hThread;
  g_fds.stdin.reader = hReader;
  g_fds.stdin.writer = hWriter;
  g_fds.stdin.inisem = hSemaphore;
}
