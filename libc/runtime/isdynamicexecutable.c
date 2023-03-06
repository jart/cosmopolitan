/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/errno.h"
#include "libc/intrin/bits.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"

/**
 * Returns true if ELF executable uses dynamic loading magic.
 */
bool _IsDynamicExecutable(const char *prog) {
  bool res;
  Elf64_Ehdr *e;
  Elf64_Phdr *p;
  struct stat st;
  int i, fd, err;
  BLOCK_CANCELLATIONS;
  fd = -1;
  err = errno;
  e = MAP_FAILED;
  if ((fd = open(prog, O_RDONLY)) == -1) {
    res = false;
    goto Finish;
  }
  if (fstat(fd, &st) == -1 || st.st_size < 64) {
    res = false;
    goto Finish;
  }
  if ((e = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    res = false;
    goto Finish;
  }
  if (READ32LE(e->e_ident) != READ32LE(ELFMAG)) {
    res = false;
    goto Finish;
  }
  if (e->e_type == ET_DYN) {
    res = true;
    goto Finish;
  }
  for (i = 0; i < e->e_phnum; ++i) {
    p = GetElfSegmentHeaderAddress(e, st.st_size, i);
    if (p->p_type == PT_INTERP || p->p_type == PT_DYNAMIC) {
      res = true;
      goto Finish;
    }
  }
  res = false;
  goto Finish;
Finish:
  if (e != MAP_FAILED) munmap(e, st.st_size);
  if (fd != -1) close(fd);
  errno = err;
  ALLOW_CANCELLATIONS;
  return res;
}
