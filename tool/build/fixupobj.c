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
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/elf/elf.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/rela.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/getopt/getopt.h"

/**
 * @fileoverview GCC Codegen Fixer-Upper.
 */

#define GETOPTS "h"

#define USAGE \
  "\
Usage: fixupobj.com [-h] ARGS...\n\
  -?\n\
  -h          show help\n\
"

void Write(const char *s, ...) {
  va_list va;
  va_start(va, s);
  do {
    write(2, s, strlen(s));
  } while ((s = va_arg(va, const char *)));
  va_end(va);
}

wontreturn void SysExit(int rc, const char *call, const char *thing) {
  int err;
  char ibuf[12];
  const char *estr;
  err = errno;
  FormatInt32(ibuf, err);
  estr = _strerdoc(err);
  if (!estr) estr = "EUNKNOWN";
  Write(thing, ": ", call, "() failed: ", estr, " (", ibuf, ")\n", 0);
  exit(rc);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, GETOPTS)) != -1) {
    switch (opt) {
      case 'h':
      case '?':
        write(1, USAGE, sizeof(USAGE) - 1);
        exit(0);
      default:
        write(2, USAGE, sizeof(USAGE) - 1);
        exit(64);
    }
  }
}

void OptimizeRelocations(Elf64_Ehdr *elf, size_t elfsize) {
  char *strs;
  Elf64_Half i;
  struct Op *op;
  Elf64_Sym *syms;
  Elf64_Rela *rela;
  Elf64_Xword symcount;
  unsigned char *code, *p;
  Elf64_Shdr *shdr, *shdrcode;
  CHECK_NOTNULL((strs = GetElfStringTable(elf, elfsize)));
  CHECK_NOTNULL((syms = GetElfSymbolTable(elf, elfsize, &symcount)));
  for (i = 0; i < elf->e_shnum; ++i) {
    shdr = GetElfSectionHeaderAddress(elf, elfsize, i);
    if (shdr->sh_type == SHT_RELA) {
      CHECK_EQ(sizeof(struct Elf64_Rela), shdr->sh_entsize);
      CHECK_NOTNULL(
          (shdrcode = GetElfSectionHeaderAddress(elf, elfsize, shdr->sh_info)));
      if (!(shdrcode->sh_flags & SHF_EXECINSTR)) continue;
      CHECK_NOTNULL((code = GetElfSectionAddress(elf, elfsize, shdrcode)));
      for (rela = GetElfSectionAddress(elf, elfsize, shdr);
           ((uintptr_t)rela + shdr->sh_entsize <=
            min((uintptr_t)elf + elfsize,
                (uintptr_t)elf + shdr->sh_offset + shdr->sh_size));
           ++rela) {

        /*
         * GCC isn't capable of -mnop-mcount when using -fpie.
         * Let's fix that. It saves ~14 cycles per function call.
         * Then libc/runtime/ftrace.greg.c morphs it back at runtime.
         */
        if (ELF64_R_TYPE(rela->r_info) == R_X86_64_GOTPCRELX &&
            strcmp(GetElfString(elf, elfsize, strs,
                                syms[ELF64_R_SYM(rela->r_info)].st_name),
                   "mcount") == 0) {
          rela->r_info = R_X86_64_NONE;
          p = code + rela->r_offset - 2;
          p[0] = 0x66; /* nopw 0x00(%rax,%rax,1) */
          p[1] = 0x0f;
          p[2] = 0x1f;
          p[3] = 0x44;
          p[4] = 0x00;
          p[5] = 0x00;
        }

        /*
         * Let's just try to nop mcount calls in general due to the above.
         */
        if ((ELF64_R_TYPE(rela->r_info) == R_X86_64_PC32 ||
             ELF64_R_TYPE(rela->r_info) == R_X86_64_PLT32) &&
            strcmp(GetElfString(elf, elfsize, strs,
                                syms[ELF64_R_SYM(rela->r_info)].st_name),
                   "mcount") == 0) {
          rela->r_info = R_X86_64_NONE;
          p = code + rela->r_offset - 1;
          p[0] = 0x0f; /* nopl 0x00(%rax,%rax,1) */
          p[1] = 0x1f;
          p[2] = 0x44;
          p[3] = 0x00;
          p[4] = 0x00;
        }
      }
    }
  }
}

void RewriteObject(const char *path) {
  int fd;
  struct stat st;
  Elf64_Ehdr *elf;
  if ((fd = open(path, O_RDWR)) == -1) {
    SysExit(__COUNTER__ + 1, "open", path);
  }
  if (fstat(fd, &st) == -1) {
    SysExit(__COUNTER__ + 1, "fstat", path);
  }
  if (st.st_size >= 64) {
    if ((elf = mmap(0, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                    0)) == MAP_FAILED) {
      SysExit(__COUNTER__ + 1, "mmap", path);
    }
    OptimizeRelocations(elf, st.st_size);
    if (msync(elf, st.st_size, MS_ASYNC | MS_INVALIDATE)) {
      SysExit(__COUNTER__ + 1, "msync", path);
    }
    if (munmap(elf, st.st_size)) {
      SysExit(__COUNTER__ + 1, "munmap", path);
    }
  }
  if (close(fd)) {
    SysExit(__COUNTER__ + 1, "close", path);
  }
}

int main(int argc, char *argv[]) {
  int i, opt;
  if (IsModeDbg()) ShowCrashReports();
  GetOpts(argc, argv);
  for (i = optind; i < argc; ++i) {
    RewriteObject(argv[i]);
  }
}
