/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/flock.h"
#include "libc/nt/enum/filelockflags.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/pib.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

textwindows static bool32 LockFileNt(intptr_t hand) {
  intptr_t event = CreateEventTls();
  struct NtOverlapped overlap = {.hEvent = event, .Pointer = 0};
  bool32 ok = LockFileEx(hand, kNtLockfileExclusiveLock, 0, 1, 0, &overlap);
  if (ok) {
    uint32_t exchanged;
    ok = GetOverlappedResult(hand, &overlap, &exchanged, true);
  }
  CloseEventTls(event);
  return ok;
}

textwindows static bool32 UnlockFileNt(int64_t hFile) {
  intptr_t event;
  event = CreateEventTls();
  struct NtOverlapped overlap = {.hEvent = event, .Pointer = 0};
  bool32 ok = UnlockFileEx(hFile, 0, 1, 0, &overlap);
  CloseEventTls(event);
  return ok;
}

static void LockUnlockFileSingleThreaded(void) {
  ASSERT_SYS(0, 0, fcntl(3, F_SETLKW, &(struct flock){.l_type = F_WRLCK}));
  ASSERT_SYS(0, 0, fcntl(3, F_SETLKW, &(struct flock){.l_type = F_UNLCK}));
}

static void LockUnlockFileSingleThreadedOptimalWindows(void) {
  ASSERT_TRUE(LockFileNt(__get_pib()->fds.p[3].handle));
  ASSERT_TRUE(UnlockFileNt(__get_pib()->fds.p[3].handle));
}

TEST(flocks, test) {
  ASSERT_SYS(0, 3, open("foo", O_RDWR | O_CREAT | O_TRUNC, 0644));
  BENCHMARK(100, 2, LockUnlockFileSingleThreaded());
  if (IsWindows())
    BENCHMARK(100, 2, LockUnlockFileSingleThreadedOptimalWindows());
  ASSERT_SYS(0, 0, close(3));
}
