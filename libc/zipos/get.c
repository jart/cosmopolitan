/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/mem/alloca.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Returns pointer to zip central directory of current executable.
 */
struct Zipos *__zipos_get(void) {
  static bool once;
  static struct Zipos zipos;
  int fd;
  size_t n;
  char *path;
  sigset_t neu, old;
  uint8_t *p, *base, *cdir;
  if (!once) {
    sigfillset(&neu);
    sigprocmask(SIG_BLOCK, &neu, &old);
    if ((fd = open(program_executable_name, O_RDONLY)) != -1) {
      if ((n = getfiledescriptorsize(fd)) != SIZE_MAX &&
          (p = mmap(0, n, PROT_READ, MAP_SHARED, fd, 0)) != MAP_FAILED) {
        if (endswith(program_executable_name, ".com.dbg")) {
          if ((base = memmem(p, n, "MZqFpD", 6))) {
            n -= base - p;
          } else {
            base = p;
          }
        } else {
          base = p;
        }
        if ((cdir = GetZipCdir(base, n))) {
          zipos.map = base;
          zipos.cdir = cdir;
        } else {
          munmap(p, n);
          ZTRACE("__zipos_get(%`'s) → eocd not found", program_executable_name);
        }
      } else {
        ZTRACE("__zipos_get(%`'s) → stat/mmap %m", program_executable_name);
      }
      close(fd);
    } else {
      ZTRACE("__zipos_get(%`'s) → open %m", program_executable_name);
    }
    once = true;
    sigprocmask(SIG_SETMASK, &old, 0);
  }
  return zipos.cdir ? &zipos : 0;
}
