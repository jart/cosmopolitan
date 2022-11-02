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
#include "libc/calls/calls.h"
#include "libc/calls/metalfile.internal.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/promises.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/thread.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.internal.h"

STATIC_YOINK(APE_COM_NAME);

static uint64_t __zipos_get_min_offset(const uint8_t *base,
                                       const uint8_t *cdir) {
  uint64_t i, n, c, r, o;
  c = GetZipCdirOffset(cdir);
  n = GetZipCdirRecords(cdir);
  for (r = c, i = 0; i < n; ++i, c += ZIP_CFILE_HDRSIZE(base + c)) {
    o = GetZipCfileOffset(base + c);
    if (o < r) r = o;
  }
  return r;
}

static void __zipos_munmap_unneeded(const uint8_t *base, const uint8_t *cdir,
                                    const uint8_t *map) {
  uint64_t n;
  n = __zipos_get_min_offset(base, cdir);
  n += base - map;
  n = ROUNDDOWN(n, FRAMESIZE);
  if (n) munmap(map, n);
}

/**
 * Returns pointer to zip central directory of current executable.
 * @asyncsignalsafe
 * @threadsafe
 */
struct Zipos *__zipos_get(void) {
  int fd;
  ssize_t size;
  const char *msg;
  static bool once;
  struct Zipos *res;
  const char *progpath;
  static struct Zipos zipos;
  uint8_t *map, *base, *cdir;
  if (!once && PLEDGED(RPATH)) {
    __zipos_lock();
    progpath = GetProgramExecutableName();
    if ((fd = open(progpath, O_RDONLY)) != -1) {
      if ((size = getfiledescriptorsize(fd)) != -1ul &&
          (map = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0)) != MAP_FAILED) {
        if ((base = FindEmbeddedApe(map, size))) {
          size -= base - map;
        } else {
          base = map;
        }
        if ((cdir = GetZipCdir(base, size)) && _cmpxchg(&zipos.map, 0, base)) {
          __zipos_munmap_unneeded(base, cdir, map);
          zipos.cdir = cdir;
          msg = "ok";
        } else {
          munmap(map, size);
          msg = "eocd not found";
        }
      } else {
        msg = "map failed";
      }
      close(fd);
    } else {
      msg = "open failed";
    }
    once = true;
    __zipos_unlock();
    STRACE("__zipos_get(%#s) → %s% m", progpath, msg);
  }
  if (zipos.cdir) {
    res = &zipos;
  } else {
    res = 0;
  }
  return res;
}
