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
#include "libc/bits/popcnt.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/elf/elf.h"
#include "libc/elf/struct/phdr.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "tool/build/lib/argv.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/loader.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"

static void LoadElfLoadSegment(struct Machine *m, void *code, size_t codesize,
                               Elf64_Phdr *phdr) {
  void *rbss;
  int64_t align, bsssize;
  int64_t felf, fstart, fend, vstart, vbss, vend;
  align = MAX(phdr->p_align, PAGESIZE);
  CHECK_EQ(1, popcnt(align));
  CHECK_EQ(0, (phdr->p_vaddr - phdr->p_offset) % align);
  felf = (int64_t)(intptr_t)code;
  vstart = ROUNDDOWN(phdr->p_vaddr, align);
  vbss = ROUNDUP(phdr->p_vaddr + phdr->p_filesz, align);
  vend = ROUNDUP(phdr->p_vaddr + phdr->p_memsz, align);
  fstart = felf + ROUNDDOWN(phdr->p_offset, align);
  fend = felf + phdr->p_offset + phdr->p_filesz;
  bsssize = vend - vbss;
  VERBOSEF("LOADELFLOADSEGMENT"
           " VSTART %#lx VBSS %#lx VEND %#lx"
           " FSTART %#lx FEND %#lx BSSSIZE %#lx",
           vstart, vbss, vend, fstart, fend, bsssize);
  m->brk = MAX(m->brk, vend);
  CHECK_GE(vend, vstart);
  CHECK_GE(fend, fstart);
  CHECK_LE(felf, fstart);
  CHECK_GE(vstart, -0x800000000000);
  CHECK_LE(vend, 0x800000000000);
  CHECK_GE(vend - vstart, fstart - fend);
  CHECK_LE(phdr->p_filesz, phdr->p_memsz);
  CHECK_EQ(felf + phdr->p_offset - fstart, phdr->p_vaddr - vstart);
  CHECK_NE(-1, ReserveVirtual(m, vstart, fend - fstart, 0x0207));
  VirtualRecv(m, vstart, (void *)fstart, fend - fstart);
  if (bsssize) CHECK_NE(-1, ReserveVirtual(m, vbss, bsssize, 0x0207));
  if (phdr->p_memsz - phdr->p_filesz > bsssize) {
    VirtualSet(m, phdr->p_vaddr + phdr->p_filesz, 0,
               phdr->p_memsz - phdr->p_filesz - bsssize);
  }
}

static void LoadElf(struct Machine *m, struct Elf *elf) {
  unsigned i;
  Elf64_Phdr *phdr;
  m->ip = elf->base = elf->ehdr->e_entry;
  VERBOSEF("LOADELF ENTRY %p", m->ip);
  for (i = 0; i < elf->ehdr->e_phnum; ++i) {
    phdr = GetElfSegmentHeaderAddress(elf->ehdr, elf->size, i);
    switch (phdr->p_type) {
      case PT_LOAD:
        elf->base = MIN(elf->base, phdr->p_vaddr);
        LoadElfLoadSegment(m, elf->ehdr, elf->size, phdr);
        break;
      default:
        break;
    }
  }
}

static void LoadBin(struct Machine *m, intptr_t base, const char *prog,
                    void *code, size_t codesize) {
  Elf64_Phdr phdr = {
      .p_type = PT_LOAD,
      .p_flags = PF_X | PF_R | PF_W,
      .p_offset = 0,
      .p_vaddr = base,
      .p_paddr = base,
      .p_filesz = codesize,
      .p_memsz = ROUNDUP(codesize + FRAMESIZE, BIGPAGESIZE),
      .p_align = PAGESIZE,
  };
  LoadElfLoadSegment(m, code, codesize, &phdr);
  m->ip = base;
}

static void BootProgram(struct Machine *m, struct Elf *elf, size_t codesize) {
  m->ip = 0x7c00;
  elf->base = 0x7c00;
  CHECK_NE(-1, ReserveReal(m, 0x00f00000));
  memset(m->real.p, 0, 0x00f00000);
  Write16(m->real.p + 0x400, 0x3F8);
  Write16(m->real.p + 0x40E, 0xb0000 >> 4);
  Write16(m->real.p + 0x413, 0xb0000 / 1024);
  Write16(m->real.p + 0x44A, 80);
  Write64(m->cs, 0);
  Write64(m->dx, 0);
  memcpy(m->real.p + 0x7c00, elf->map, 512);
  if (memcmp(elf->map, "\177ELF", 4) == 0) {
    elf->ehdr = (void *)elf->map;
    elf->size = codesize;
    elf->base = elf->ehdr->e_entry;
  } else {
    elf->base = 0x7c00;
    elf->ehdr = NULL;
    elf->size = 0;
  }
}

void LoadProgram(struct Machine *m, const char *prog, char **args, char **vars,
                 struct Elf *elf) {
  int fd;
  ssize_t rc;
  int64_t sp;
  struct stat st;
  size_t i, mappedsize;
  DCHECK_NOTNULL(prog);
  elf->prog = prog;
  if ((fd = open(prog, O_RDONLY)) == -1 ||
      (fstat(fd, &st) == -1 || !st.st_size)) {
    fputs(prog, stderr);
    fputs(": not found\n", stderr);
    exit(1);
  }
  elf->mapsize = st.st_size;
  CHECK_NE(MAP_FAILED,
           (elf->map = mmap(NULL, elf->mapsize, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE, fd, 0)));
  CHECK_NE(-1, close(fd));
  ResetCpu(m);
  if ((m->mode & 3) == XED_MODE_REAL) {
    BootProgram(m, elf, elf->mapsize);
  } else {
    sp = 0x800000000000;
    Write64(m->sp, sp);
    m->cr3 = AllocateLinearPage(m);
    CHECK_NE(-1, ReserveVirtual(m, sp - STACKSIZE, STACKSIZE, 0x0207));
    LoadArgv(m, prog, args, vars);
    if (memcmp(elf->map, "\177ELF", 4) == 0) {
      elf->ehdr = (void *)elf->map;
      elf->size = elf->mapsize;
      LoadElf(m, elf);
    } else {
      elf->base = IMAGE_BASE_VIRTUAL;
      elf->ehdr = NULL;
      elf->size = 0;
      LoadBin(m, elf->base, prog, elf->map, elf->mapsize);
    }
  }
}
