/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/getenv.internal.h"
#include "libc/intrin/promises.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/nt/struct/teb.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"

#define kBufSize 1024
#define kPid     "TracerPid:\t"

static textwindows bool IsBeingDebugged(void) {
#ifdef __x86_64__
  return !!NtGetPeb()->BeingDebugged;
#else
  return false;
#endif
}

/**
 * Determines if gdb, strace, windbg, etc. is controlling process.
 * @return non-zero if attached, otherwise 0
 */
bool32 IsDebuggerPresent(bool32 force) {
  /* asan runtime depends on this function */
  ssize_t got;
  int e, fd, res;
  char *p, buf[1024];
  if (!force && IsGenuineBlink()) return 0;
  if (!force && environ && __getenv(environ, "HEISENDEBUG").s) return 0;
  if (IsWindows()) return IsBeingDebugged();
  if (__isworker) return false;
  if (!PLEDGED(RPATH)) return false;
  res = 0;
  e = errno;
  BLOCK_CANCELATION;
  if ((fd = __sys_openat(AT_FDCWD, "/proc/self/status", O_RDONLY, 0)) >= 0) {
    if ((got = sys_read(fd, buf, sizeof(buf) - 1)) > 0) {
      buf[got] = '\0';
      if ((p = __strstr(buf, kPid))) {
        p += sizeof(kPid) - 1;
        res = __atoul(p);
      }
    }
    sys_close(fd);
  }
  ALLOW_CANCELATION;
  errno = e;
  return res;
}
