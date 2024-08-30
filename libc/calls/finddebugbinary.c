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
#include "ape/sections.internal.h"
#include "libc/atomic.h"
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/elf/def.h"
#include "libc/elf/tinyelf.internal.h"
#include "libc/errno.h"
#include "libc/intrin/directmap.h"
#include "libc/intrin/promises.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"

static struct {
  atomic_uint once;
  const char *res;
  char buf[PATH_MAX];
} g_comdbg;

static const char *kDbgExts[] = {
    ".dbg",
    ".com.dbg",
#ifdef __aarch64__
    ".aarch64.elf",
#elif defined(__powerpc64__)
    ".ppc64.elf",
#endif
};

static int GetElfMachine(void) {
#ifdef __x86_64__
  return EM_NEXGEN32E;
#elif defined(__aarch64__)
  return EM_AARCH64;
#elif defined(__powerpc64__)
  return EM_PPC64;
#elif defined(__riscv)
  return EM_RISCV;
#elif defined(__s390x__)
  return EM_S390;
#else
#error "unsupported architecture"
#endif
}

static bool IsMyDebugBinary(const char *path) {
  void *addr;
  int64_t size;
  uintptr_t value;
  bool res = false;
  int fd, e = errno;
  if ((fd = open(path, O_RDONLY | O_CLOEXEC, 0)) != -1) {
    // sanity test that this .com.dbg file (1) is an elf image, and (2)
    // contains the same number of bytes of code as our .com executable
    // which is currently running in memory.
    if ((size = lseek(fd, 0, SEEK_END)) != -1 &&
        (addr = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0)) != MAP_FAILED) {
      if (READ32LE((char *)addr) == READ32LE("\177ELF") &&
          ((Elf64_Ehdr *)addr)->e_machine == GetElfMachine() &&
          GetElfSymbolValue(addr, "_etext", &value)) {
        res = !_etext || value == (uintptr_t)_etext;
      }
      munmap(addr, size);
    }
    close(fd);
  }
  errno = e;
  return res;
}

static void FindDebugBinaryInit(void) {
  const char *comdbg;
  if (issetugid())
    return;
  if ((comdbg = getenv("COMDBG"))) {
    g_comdbg.res = comdbg;
    return;
  }
  char *prog = GetProgramExecutableName();
  for (int i = 0; i < ARRAYLEN(kDbgExts); ++i) {
    strlcpy(g_comdbg.buf, prog, sizeof(g_comdbg.buf));
    strlcat(g_comdbg.buf, kDbgExts[i], sizeof(g_comdbg.buf));
    if (IsMyDebugBinary(g_comdbg.buf)) {
      g_comdbg.res = g_comdbg.buf;
      return;
    }
  }
  g_comdbg.res = prog;
}

/**
 * Returns path of binary with the debug information, or null.
 *
 * You can specify the COMDBG environment variable, with the path of the
 * debug binary, in case the automatic heuristics fail. What we look for
 * is GetProgramExecutableName() with ".dbg", ".com.dbg", etc. appended.
 *
 * @return path to debug binary, or NULL if we couldn't find it
 * @asyncsignalsafe
 */
const char *FindDebugBinary(void) {
  cosmo_once(&g_comdbg.once, FindDebugBinaryInit);
  return g_comdbg.res;
}

// pay startup cost to make this signal safe from the user's perspective
__attribute__((__constructor__(10))) static void FindDebugBinaryCtor(void) {
  cosmo_once(&g_comdbg.once, FindDebugBinaryInit);
}
