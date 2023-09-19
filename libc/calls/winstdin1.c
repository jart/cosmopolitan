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
#include "libc/dce.h"
#include "libc/nt/console.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/thread/tls.h"

static textwindows dontinstrument wontreturn void WinStdinThread(void) {
  char buf[4];
  struct CosmoTib tls;
  uint32_t i, got, wrote;
  __bootstrap_tls(&tls, __builtin_frame_address(0));
  for (;;) {
    WaitForSingleObject(g_fds.stdin.inisem, -1u);
    if (!ReadFile(g_fds.stdin.handle, buf, sizeof(buf), &got, 0)) goto Finish;
    if (!got) goto Finish;
    for (i = 0; i < got; i += wrote) {
      if (!WriteFile(g_fds.stdin.writer, buf + i, got - i, &wrote, 0)) {
        goto Finish;
      }
    }
  }
Finish:
  CloseHandle(g_fds.stdin.writer);
  ExitThread(0);
}

textwindows static char16_t *FixCpy(char16_t p[17], uint64_t x, uint8_t k) {
  while (k > 0) *p++ = "0123456789abcdef"[(x >> (k -= 4)) & 15];
  *p = '\0';
  return p;
}

textwindows static char16_t *CreateStdinPipeName(char16_t *a, int64_t h) {
  char16_t *p = a;
  const char *q = "\\\\?\\pipe\\cosmo\\stdin\\";
  while (*q) *p++ = *q++;
  p = FixCpy(p, h, 64);
  *p = 0;
  return a;
}

textwindows void WinMainStdin(void) {
  uint32_t conmode;
  char16_t pipename[64];
  int64_t hStdin, hWriter, hReader, hThread, hSemaphore;
  if (!SupportsWindows()) return;
  hStdin = GetStdHandle(kNtStdInputHandle);
  if (!GetConsoleMode(hStdin, &conmode)) return;
  CreateStdinPipeName(pipename, hStdin);
  hWriter = CreateNamedPipe(
      pipename, kNtPipeAccessOutbound | kNtFileFlagOverlapped,
      kNtPipeTypeMessage | kNtPipeReadmodeMessage | kNtPipeNowait,
      kNtPipeUnlimitedInstances, 4096, 4096, 0, 0);
  if (hWriter == kNtInvalidHandleValue) return;
  hReader = CreateFile(pipename, kNtGenericRead, 0, 0, kNtOpenExisting,
                       kNtFileFlagOverlapped, 0);
  if (hReader == kNtInvalidHandleValue) return;
  hSemaphore = CreateSemaphore(0, 0, 1, 0);
  if (!hSemaphore) return;
  hThread = CreateThread(0, 65536, (void *)WinStdinThread, 0,
                         kNtStackSizeParamIsAReservation, 0);
  if (!hThread) return;
  g_fds.stdin.handle = hStdin;
  g_fds.stdin.thread = hThread;
  g_fds.stdin.reader = hReader;
  g_fds.stdin.writer = hWriter;
  g_fds.stdin.inisem = hSemaphore;
}
