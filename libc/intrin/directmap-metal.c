/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/internal.h"
#include "libc/calls/metalfile.internal.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/pc.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

#define MAP_ANONYMOUS_linux 0x00000020
#define MAP_FIXED_linux     0x00000010

static uint64_t sys_mmap_metal_break;

static struct DirectMap bad_mmap(void) {
  struct DirectMap res;
  res.addr = (void *)-1;
  res.maphandle = -1;
  return res;
}

struct DirectMap sys_mmap_metal(void *vaddr, size_t size, int prot, int flags,
                                int fd, int64_t off) {
  /* asan runtime depends on this function */
  size_t i;
  struct mman *mm;
  struct DirectMap res;
  uint64_t addr, faddr = 0, page, e, *pte, *fdpte, *pml4t;
  mm = __get_mm();
  pml4t = __get_pml4t();
  size = ROUNDUP(size, 4096);
  addr = (uint64_t)vaddr;
  if (!(flags & MAP_ANONYMOUS_linux)) {
    struct Fd *sfd;
    struct MetalFile *file;
    if (off < 0 || fd < 0 || fd >= g_fds.n) return bad_mmap();
    sfd = &g_fds.p[fd];
    if (sfd->kind != kFdFile) return bad_mmap();
    file = (struct MetalFile *)sfd->handle;
    /* TODO: allow mapping partial page at end of file, if file size not
     * multiple of page size */
    if (off > file->size || size > file->size - off) return bad_mmap();
    faddr = (uint64_t)file->base + off;
    if (faddr % 4096 != 0) return bad_mmap();
  }
  if (!(flags & MAP_FIXED_linux)) {
    if (!addr) {
      addr = 4096;
    } else {
      addr = ROUNDUP(addr, 4096);
    }
    for (i = 0; i < size; i += 4096) {
      pte = __get_virtual(mm, pml4t, addr + i, false);
      if (pte && (*pte & (PAGE_V | PAGE_RSRV))) {
        addr = MAX(addr, sys_mmap_metal_break) + i + 4096;
        i = 0;
      }
    }
    sys_mmap_metal_break = MAX(addr + size, sys_mmap_metal_break);
  }
  for (i = 0; i < size; i += 4096) {
    pte = __get_virtual(mm, pml4t, addr + i, true);
    if (pte) {
      if ((flags & MAP_ANONYMOUS_linux)) {
        page = __new_page(mm);
        if (!page) return bad_mmap();
        __clear_page(BANE + page);
        e = page | PAGE_RSRV | PAGE_U;
        if ((prot & PROT_WRITE))
          e |= PAGE_V | PAGE_RW;
        else if ((prot & (PROT_READ | PROT_EXEC)))
          e |= PAGE_V;
        if (!(prot & PROT_EXEC)) e |= PAGE_XD;
      } else {
        fdpte = __get_virtual(mm, pml4t, faddr + i, false);
        e = *fdpte | PAGE_RSRV | PAGE_U;
        page = e & PAGE_TA;
        if (!(prot & PROT_WRITE)) e &= ~PAGE_RW;
        if (!(prot & PROT_EXEC)) e |= PAGE_XD;
      }
      __ref_page(mm, pml4t, page);
      *pte = e;
      invlpg(addr + i);
    } else {
      addr = -1;
      break;
    }
  }
  res.addr = (void *)addr;
  res.maphandle = -1;
  return res;
}

#endif /* __x86_64__ */
