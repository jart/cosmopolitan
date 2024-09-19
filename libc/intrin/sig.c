/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/sysv/consts/sig.h"
#include "libc/atomic.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/dce.h"
#include "libc/fmt/internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
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
#include "libc/nt/runtime.h"
#include "libc/thread/tls.h"

struct Signals __sig;

sigset_t __sig_block(void) {
  if (IsWindows() || IsMetal()) {
    if (__tls_enabled)
      return atomic_exchange_explicit(&__get_tls()->tib_sigmask, -1,
                                      memory_order_acquire);
    else
      return 0;
  } else {
    sigset_t res, neu = -1;
    sys_sigprocmask(SIG_SETMASK, &neu, &res);
    return res;
  }
}

void __sig_unblock(sigset_t m) {
  if (IsWindows() || IsMetal()) {
    if (__tls_enabled) {
      atomic_store_explicit(&__get_tls()->tib_sigmask, m, memory_order_release);
      if (_weaken(__sig_check)) {
        _weaken(__sig_check)();
      }
    }
  } else {
    sys_sigprocmask(SIG_SETMASK, &m, 0);
  }
}

#ifdef __x86_64__

textwindows char16_t *__sig_process_path(char16_t *path, uint32_t pid) {
  char16_t *p = path;
  *p++ = 'C';
  *p++ = ':';
  *p++ = '\\';
  *p++ = 'v';
  *p++ = 'a';
  *p++ = 'r';
  *p = 0;
  CreateDirectory(path, 0);
  *p++ = '\\';
  *p++ = 's';
  *p++ = 'i';
  *p++ = 'g';
  *p = 0;
  CreateDirectory(path, 0);
  *p++ = '\\';
  p = __itoa16(p, (pid & 0x000fff00) >> 8);
  *p = 0;
  CreateDirectory(path, 0);
  *p++ = '\\';
  p = __itoa16(p, pid);
  *p++ = '.';
  *p++ = 'p';
  *p++ = 'i';
  *p++ = 'd';
  *p = 0;
  return path;
}

textwindows static atomic_ulong *__sig_map_process_impl(int pid) {
  char16_t path[128];
  intptr_t hand = CreateFile(__sig_process_path(path, pid),
                             kNtGenericRead | kNtGenericWrite,
                             kNtFileShareRead | kNtFileShareWrite, 0,
                             kNtOpenAlways, kNtFileAttributeNormal, 0);
  if (hand == -1)
    return 0;
  SetFilePointer(hand, 8, 0, kNtFileBegin);
  SetEndOfFile(hand);
  intptr_t map = CreateFileMapping(hand, 0, kNtPageReadwrite, 0, 8, 0);
  if (!map) {
    CloseHandle(hand);
    return 0;
  }
  atomic_ulong *sigs = MapViewOfFileEx(map, kNtFileMapWrite, 0, 0, 8, 0);
  CloseHandle(map);
  CloseHandle(hand);
  return sigs;
}

textwindows atomic_ulong *__sig_map_process(int pid) {
  int e = errno;
  atomic_ulong *res = __sig_map_process_impl(pid);
  errno = e;
  return res;
}

#endif /* __x86_64__ */
