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
#include "libc/calls/struct/stat.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/kmalloc.h"
#include "libc/intrin/promises.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/alg.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/thread.h"
#include "libc/zip.internal.h"

#ifdef __x86_64__
__static_yoink(APE_COM_NAME);
#endif

static uint64_t __zipos_get_min_offset(const uint8_t *map,
                                       const uint8_t *cdir) {
  uint64_t i, n, c, r, o;
  c = GetZipCdirOffset(cdir);
  n = GetZipCdirRecords(cdir);
  for (r = c, i = 0; i < n; ++i, c += ZIP_CFILE_HDRSIZE(map + c)) {
    o = GetZipCfileOffset(map + c);
    if (o < r) r = o;
  }
  return r;
}

static void __zipos_munmap_unneeded(const uint8_t *map, const uint8_t *cdir) {
  uint64_t n;
  n = __zipos_get_min_offset(map, cdir);
  n = ROUNDDOWN(n, FRAMESIZE);
  if (n) munmap(map, n);
}

static int __zipos_compare_names(const void *a, const void *b, void *c) {
  const size_t *x = (const size_t *)a;
  const size_t *y = (const size_t *)b;
  struct Zipos *z = (struct Zipos *)c;
  int xn = ZIP_CFILE_NAMESIZE(z->map + *x);
  int yn = ZIP_CFILE_NAMESIZE(z->map + *y);
  int n = MIN(xn, yn);
  if (n) {
    int res =
        memcmp(ZIP_CFILE_NAME(z->map + *x), ZIP_CFILE_NAME(z->map + *y), n);
    if (res) return res;
  }
  return xn - yn;  // xn and yn are 16-bit
}

// creates binary searchable array of file offsets to cdir records
static void __zipos_generate_index(struct Zipos *zipos) {
  size_t c, i;
  zipos->records = GetZipCdirRecords(zipos->cdir);
  zipos->index = kmalloc(zipos->records * sizeof(size_t));
  for (i = 0, c = GetZipCdirOffset(zipos->cdir); i < zipos->records;
       ++i, c += ZIP_CFILE_HDRSIZE(zipos->map + c)) {
    zipos->index[i] = c;
  }
  // smoothsort() isn't the fastest algorithm, but it guarantees
  // o(logn), won't smash the stack and doesn't depend on malloc
  smoothsort_r(zipos->index, zipos->records, sizeof(size_t),
               __zipos_compare_names, zipos);
}

/**
 * Returns pointer to zip central directory of current executable.
 * @asyncsignalsafe
 * @threadsafe
 */
struct Zipos *__zipos_get(void) {
  char *endptr;
  const char *s;
  struct stat st;
  static bool once;
  struct Zipos *res;
  int x, fd, err, msg;
  uint8_t *map, *cdir;
  const char *progpath;
  static struct Zipos zipos;
  __zipos_lock();
  if (!once) {
    if (!(s = getenv("COSMOPOLITAN_DISABLE_ZIPOS"))) {
      // this environment variable may be a filename or file descriptor
      if ((progpath = getenv("COSMOPOLITAN_INIT_ZIPOS")) &&
          (x = strtol(progpath, &endptr, 10)) >= 0 && !*endptr) {
        fd = x;
      } else {
        fd = -1;
      }
      if (fd != -1 || PLEDGED(RPATH)) {
        if (fd == -1) {
          if (!progpath) {
            progpath = GetProgramExecutableName();
          }
          fd = open(progpath, O_RDONLY);
        }
        if (fd != -1) {
          if (!fstat(fd, &st) &&
              (map = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) !=
                  MAP_FAILED) {
            if ((cdir = GetZipEocd(map, st.st_size, &err))) {
              __zipos_munmap_unneeded(map, cdir);
              zipos.map = map;
              zipos.cdir = cdir;
              zipos.dev = st.st_ino;
              __zipos_generate_index(&zipos);
              msg = kZipOk;
            } else {
              munmap(map, st.st_size);
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
    } else {
      progpath = 0;
      msg = -777;
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
