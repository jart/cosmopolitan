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
#include "libc/fmt/conv.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/promises.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/thread.h"
#include "libc/zip.internal.h"
#include "libc/zipos/zipos.internal.h"

#ifdef __x86_64__
__static_yoink(APE_COM_NAME);
#endif

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
  char *endptr;
  ssize_t size;
  int fd, err, msg;
  static bool once;
  struct Zipos *res;
  uint8_t *map, *cdir;
  const char *progpath;
  static struct Zipos zipos;
  __zipos_lock();
  if (!once) {
    progpath = getenv("COSMOPOLITAN_INIT_ZIPOS");
    if (progpath) {
      fd = strtol(progpath, &endptr, 10);
      if (*endptr) fd = -1;
    } else {
      fd = -1;
    }
    if (fd != -1 || PLEDGED(RPATH)) {
      if (fd == -1) {
        progpath = GetProgramExecutableName();
        fd = open(progpath, O_RDONLY);
      }
      if (fd != -1) {
        if ((size = lseek(fd, 0, SEEK_END)) != -1 &&
            (map = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0)) !=
                MAP_FAILED) {
          if ((cdir = GetZipEocd(map, size, &err))) {
            __zipos_munmap_unneeded(map, cdir, map);
            zipos.map = map;
            zipos.cdir = cdir;
            msg = kZipOk;
          } else {
            munmap(map, size);
            msg = !cdir ? err : kZipErrorRaceCondition;
          }
        } else {
          msg = kZipErrorMapFailed;
        }
        close(fd);
      } else {
        msg = kZipErrorOpenFailed;
      }
    } else {
      msg = -666;
    }
    STRACE("__zipos_get(%#s) → %d% m", progpath, msg);
    once = true;
  }
  __zipos_unlock();
  if (zipos.cdir) {
    res = &zipos;
  } else {
    res = 0;
  }
  return res;
}
