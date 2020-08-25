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
#include "libc/bits/popcnt.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/elf/elf.h"
#include "libc/elf/struct/phdr.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/loader.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"

#define DSOLOL "ERROR: ELF not ET_EXEC try `gcc -static -o foo foo.c`\n"

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
  fstart = ROUNDDOWN(felf + phdr->p_offset, align);
  fend = ROUNDUP(felf + phdr->p_offset + phdr->p_filesz, align);
  bsssize = vend - vbss;
  CHECK_GE(vend, vstart);
  CHECK_GE(fend, fstart);
  CHECK_LE(felf, fstart);
  CHECK_GE(vstart, -0x800000000000);
  CHECK_LE(vend, 0x800000000000);
  CHECK_GE(vend - vstart, fstart - fend);
  CHECK_LE(phdr->p_filesz, phdr->p_memsz);
  CHECK_EQ(felf + phdr->p_offset - fstart, phdr->p_vaddr - vstart);
  CHECK_NE(-1, RegisterMemory(m, vstart, (void *)fstart, fend - fstart));
  if (bsssize) {
    CHECK_NE(MAP_FAILED, (rbss = mmap(NULL, bsssize, PROT_READ | PROT_WRITE,
                                      MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
    CHECK_NE(-1, RegisterMemory(m, vbss, rbss, bsssize));
  }
  if (phdr->p_memsz - phdr->p_filesz > bsssize) {
    VirtualSet(m, phdr->p_vaddr + phdr->p_filesz, 0,
               phdr->p_memsz - phdr->p_filesz - bsssize);
  }
}

static void LoadElf(struct Machine *m, struct Elf *elf) {
  unsigned i;
  Elf64_Phdr *phdr;
  if (elf->ehdr->e_type != ET_EXEC) {
    write(STDERR_FILENO, DSOLOL, strlen(DSOLOL));
    exit(1);
  }
  m->ip = elf->base = elf->ehdr->e_entry;
  for (i = 0; i < elf->ehdr->e_phnum; ++i) {
    phdr = getelfsegmentheaderaddress(elf->ehdr, elf->size, i);
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

void LoadProgram(struct Machine *m, const char *prog, char **args, char **vars,
                 struct Elf *elf) {
  int fd;
  struct stat st;
  void *code, *stack;
  size_t codesize, stacksize;
  DCHECK_NOTNULL(prog);
  elf->prog = prog;
  if ((fd = open(prog, O_RDONLY)) == -1 ||
      (fstat(fd, &st) == -1 || !st.st_size) || !S_ISREG(st.st_mode)) {
    fputs(prog, stderr);
    fputs(": not found\n", stderr);
    exit(1);
  }
  codesize = st.st_size;
  stacksize = STACKSIZE;
  CHECK_NE(MAP_FAILED, (stack = mmap(NULL, stacksize, PROT_READ | PROT_WRITE,
                                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  CHECK_NE(MAP_FAILED, (code = mmap(NULL, codesize, PROT_READ | PROT_WRITE,
                                    MAP_PRIVATE, fd, 0)));
  CHECK_NE(-1, close(fd));
  ResetCpu(m);
  Write64(m->sp, 0x800000000000);
  RegisterMemory(m, 0x800000000000 - stacksize, stack, stacksize);
  LoadArgv(m, prog, args, vars);
  if (memcmp(code, "\177ELF", 4) == 0) {
    elf->ehdr = code;
    elf->size = codesize;
    LoadElf(m, elf);
  } else {
    elf->base = IMAGE_BASE_VIRTUAL;
    elf->ehdr = NULL;
    elf->size = 0;
    LoadBin(m, elf->base, prog, code, codesize);
  }
}
