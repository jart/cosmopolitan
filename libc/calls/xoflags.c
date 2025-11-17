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
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

int __xoflags(int flags) {
  int flags2 = 0;

  // we assume O_UNLINK was removed by openat() wrapper
  if (flags & ~(O_ACCMODE | O_CREAT | O_EXCL | O_NOCTTY | O_TRUNC | O_APPEND |
                O_NONBLOCK | O_DSYNC | O_DIRECT | O_LARGEFILE | O_DIRECTORY |
                O_NOFOLLOW | O_NOATIME | O_CLOEXEC | O_SYNC | O_RSYNC |
                _O_PATH | _O_TMPFILE))
    return einval();

  if (IsLinux()) {
    flags2 = flags;
  } else {
    flags2 = flags & O_ACCMODE;
    if (flags & O_CREAT)
      flags2 |= 0x00000200;
    if (flags & O_EXCL)
      flags2 |= 0x00000800;
    if (flags & O_NOCTTY) {
      if (IsXnu()) {
        flags2 |= 0x00020000;
      } else {
        flags2 |= 0x00008000;
      }
    }
    if (flags & O_TRUNC)
      flags2 |= 0x00000400;
    if (flags & O_APPEND)
      flags2 |= 8;
    if (flags & O_NONBLOCK)
      flags2 |= 4;
    if (flags & O_DSYNC) {
      if (IsXnu()) {
        flags2 |= 0x00400000;
      } else if (IsFreebsd()) {
        flags2 |= 0x01000000;
      } else if (IsOpenbsd()) {
        flags2 |= 0x00000080;
      } else if (IsNetbsd()) {
        flags2 |= 0x00010000;
      } else {
        return einval();
      }
    }
    if (flags & O_DIRECT) {
      if (IsFreebsd()) {
        flags2 |= 0x00010000;
      } else if (IsNetbsd()) {
        flags2 |= 0x00080000;
      } else {
        return einval();
      }
    }
    if (flags & O_LARGEFILE) {
      // do nothing
    }
    if (flags & O_DIRECTORY) {
      if (IsXnu()) {
        flags2 |= 0x00100000;
      } else if (IsFreebsd() || IsOpenbsd()) {
        flags2 |= 0x00020000;
      } else if (IsNetbsd()) {
        flags2 |= 0x00200000;
      } else {
        return einval();
      }
    }
    if (flags & O_NOFOLLOW)
      flags2 |= 0x00000100;
    if (flags & O_NOATIME) {
      // do nothing
    }
    if (flags & O_CLOEXEC) {
      if (IsXnu()) {
        flags2 |= 0x01000000;
      } else if (IsFreebsd()) {
        flags2 |= 0x00100000;
      } else if (IsOpenbsd()) {
        flags2 |= 0x00010000;
      } else if (IsNetbsd()) {
        flags2 |= 0x00400000;
      } else {
        return einval();
      }
    }
    if ((flags & O_SYNC) == O_SYNC)
      flags2 |= 0x00000080;
    if (flags & _O_PATH)
      return einval();
    if (flags & _O_TMPFILE)
      return einval();
  }

  return flags2;
}
