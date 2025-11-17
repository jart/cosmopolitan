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
#include <linux/f2fs.h>
#include "libc/fmt/itoa.h"
#include "libc/intrin/describeflags.h"
#include "libc/sysv/consts/fio.h"
#include "libc/sysv/consts/sio.h"
#include "libc/sysv/consts/termios.h"

const char *_DescribeIoctlRequest(char buf[21], unsigned long request) {
  if (request == FIONREAD)
    return "FIONREAD";
  if (request == TIOCGWINSZ)
    return "TIOCGWINSZ";
  if (request == SIOCGIFCONF)
    return "SIOCGIFCONF";
  if (request == SIOCGIFADDR)
    return "SIOCGIFADDR";
  if (request == F2FS_IOC_START_ATOMIC_WRITE)
    return "F2FS_IOC_START_ATOMIC_WRITE";
  if (request == F2FS_IOC_COMMIT_ATOMIC_WRITE)
    return "F2FS_IOC_COMMIT_ATOMIC_WRITE";
  if (request == F2FS_IOC_ABORT_ATOMIC_WRITE)
    return "F2FS_IOC_ABORT_ATOMIC_WRITE";
  if (request == F2FS_IOC_GET_FEATURES)
    return "F2FS_IOC_GET_FEATURES";
  FormatInt32(buf, request);
  return buf;
}
