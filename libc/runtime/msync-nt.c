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
#include "libc/macros.internal.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"

textwindows int sys_msync_nt(char *addr, size_t size, int flags) {
  int i, rc = 0;
  char *a, *b, *x, *y;
  __mmi_lock();
  for (i = __find_memory(&_mmi, (intptr_t)addr >> 16); i < _mmi.i; ++i) {
    x = (char *)ADDR_32_TO_48(_mmi.p[i].x);
    y = x + _mmi.p[i].size;
    if ((x <= addr && addr < y) || (x < addr + size && addr + size <= y) ||
        (addr < x && y < addr + size)) {
      a = MIN(MAX(addr, x), y);
      b = MAX(MIN(addr + size, y), x);
      if (!FlushViewOfFile(a, b - a)) {
        rc = -1;
        break;
      }
      // TODO(jart): FlushFileBuffers too on g_fds handle if MS_SYNC?
    } else {
      break;
    }
  }
  __mmi_unlock();
  return rc;
}
