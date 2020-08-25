/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/log/check.h"
#include "libc/runtime/gc.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/x.h"
#include "tool/build/lib/loader.h"

void LoadDebugSymbols(struct Elf *elf) {
  int fd;
  void *elfmap;
  struct stat st;
  if (elf->ehdr) return;
  DCHECK_NOTNULL(elf->prog);
  if ((fd = open(gc(xstrcat(elf->prog, ".dbg")), O_RDONLY)) != -1) {
    if (fstat(fd, &st) != -1 &&
        (elfmap = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)) !=
            MAP_FAILED) {
      elf->ehdr = elfmap;
      elf->size = st.st_size;
    }
    close(fd);
  }
}
