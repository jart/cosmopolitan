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
#include "libc/calls/struct/stat.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Returns pointer to zip central directory of current executable.
 */
struct Zipos *__zipos_get(void) {
  static struct Zipos zipos;
  static bool once;
  const char *exe, *dir;
  char path[PATH_MAX];
  size_t size;
  uint8_t *map, *base, *cdir;
  if (!once) {
    dir = nulltoempty(getenv("PWD")); /* suboptimal */
    exe = (const char *)getauxval(AT_EXECFN);
    if (!fileexists(exe) && strlen(dir) + 1 + strlen(exe) + 1 <= PATH_MAX) {
      stpcpy(stpcpy(stpcpy(path, dir), "/"), exe);
      exe = path;
    }
    if ((zipos.fd = open(exe, O_RDONLY)) != -1) {
      if ((size = getfiledescriptorsize(zipos.fd)) != SIZE_MAX &&
          (map = mmap(NULL, size, PROT_READ, MAP_SHARED, zipos.fd, 0)) !=
              MAP_FAILED) {
        if (endswith(exe, ".com.dbg")) {
          if ((base = memmem(map, size, "MZqFpD", 6))) {
            size -= base - map;
          } else {
            base = map;
          }
        } else {
          base = map;
        }
        if ((cdir = GetZipCdir(base, size))) {
          zipos.map = base;
          zipos.cdir = cdir;
        } else {
          munmap(map, size);
        }
      }
    }
    once = true;
  }
  return zipos.cdir ? &zipos : NULL;
}
