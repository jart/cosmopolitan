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
#include "ape/sections.internal.h"
#include "libc/atomic.h"
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/cosmo.h"
#include "libc/elf/tinyelf.internal.h"
#include "libc/errno.h"
#include "libc/intrin/bits.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"

static struct {
  atomic_uint once;
  const char *res;
  char buf[PATH_MAX];
} g_comdbg;

static bool IsMyDebugBinary(const char *path) {
  void *map;
  int64_t size;
  uintptr_t value;
  bool res = false;
  int fd, e = errno;
  BLOCK_CANCELATION;
  if ((fd = open(path, O_RDONLY | O_CLOEXEC, 0)) != -1) {
    // sanity test that this .com.dbg file (1) is an elf image, and (2)
    // contains the same number of bytes of code as our .com executable
    // which is currently running in memory.
    if ((size = lseek(fd, 0, SEEK_END)) != -1 &&
        (map = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0)) != MAP_FAILED) {
      if (READ32LE((char *)map) == READ32LE("\177ELF") &&
          GetElfSymbolValue(map, "_etext", &value)) {
        res = !_etext || value == (uintptr_t)_etext;
      }
      munmap(map, size);
    }
    close(fd);
  }
  ALLOW_CANCELATION;
  errno = e;
  return res;
}

static void FindDebugBinaryInit(void) {
  char *p = GetProgramExecutableName();
  size_t n = strlen(p);
  if ((n > 4 && READ32LE(p + n - 4) == READ32LE(".dbg")) ||
      IsMyDebugBinary(p)) {
    g_comdbg.res = p;
  } else if (n + 4 < ARRAYLEN(g_comdbg.buf)) {
    mempcpy(mempcpy(g_comdbg.buf, p, n), ".dbg", 5);
    if (IsMyDebugBinary(g_comdbg.buf)) {
      g_comdbg.res = g_comdbg.buf;
    }
  }
  if (!g_comdbg.res) {
    g_comdbg.res = getenv("COMDBG");
  }
}

/**
 * Returns path of binary with the debug information, or null.
 *
 * @return path to debug binary, or NULL
 */
const char *FindDebugBinary(void) {
  cosmo_once(&g_comdbg.once, FindDebugBinaryInit);
  return g_comdbg.res;
}
