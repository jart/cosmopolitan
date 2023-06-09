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
#include "libc/assert.h"
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
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.internal.h"
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

#define GETOPTS "ch"

#define USAGE \
  "\
Usage: fixupobj.com [-h] ARGS...\n\
  -?\n\
  -h          show help\n\
  -c          check-only mode\n\
"

#define COSMO_TLS_REG     28
#define MRS_TPIDR_EL0     0xd53bd040u
#define MOV_REG(DST, SRC) (0xaa0003e0u | (SRC) << 16 | (DST))

static const unsigned char kFatNops[8][8] = {
    {},                                          //
    {0x90},                                      // nop
    {0x66, 0x90},                                // xchg %ax,%ax
    {0x0f, 0x1f, 0x00},                          // nopl (%rax)
    {0x0f, 0x1f, 0x40, 0x00},                    // nopl 0x00(%rax)
    {0x0f, 0x1f, 0x44, 0x00, 0x00},              // nopl 0x00(%rax,%rax,1)
    {0x66, 0x0f, 0x1f, 0x44, 0x00, 0x00},        // nopw 0x00(%rax,%rax,1)
    {0x0f, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00},  // nopl 0x00000000(%rax)
};

static int mode;
static char *symstrs;
static char *secstrs;
static ssize_t esize;
static Elf64_Sym *syms;
static const char *epath;
static Elf64_Xword symcount;
static const Elf64_Ehdr *elf;

nullterminated() static void Print(int fd, const char *s, ...) {
  va_list va;
  char buf[2048];
  va_start(va, s);
  buf[0] = 0;
  do {
    strlcat(buf, s, sizeof(buf));
  } while ((s = va_arg(va, const char *)));
  write(fd, buf, strlen(buf));
  va_end(va);
}

static wontreturn void SysExit(const char *func) {
  const char *errstr;
  if (!(errstr = _strerdoc(errno))) errstr = "EUNKNOWN";
  Print(2, epath, ": ", func, " failed with ", errstr, "\n", NULL);
  exit(1);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  mode = O_RDWR;
  while ((opt = getopt(argc, argv, GETOPTS)) != -1) {
    switch (opt) {
      case 'c':
        mode = O_RDONLY;
        break;
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

static Elf64_Shdr *FindElfSectionByName(const char *name) {
  long i;
  Elf64_Shdr *shdr;
  for (i = 0; i < elf->e_shnum; ++i) {
    shdr = GetElfSectionHeaderAddress(elf, esize, i);
    if (!strcmp(GetElfString(elf, esize, secstrs, shdr->sh_name), name)) {
      return shdr;
    }
  }
  return 0;
}

static void CheckPrivilegedCrossReferences(void) {
  long i, x;
  Elf64_Shdr *shdr;
  const char *secname;
  Elf64_Rela *rela, *erela;
  if (!(shdr = FindElfSectionByName(".rela.privileged"))) return;
  rela = GetElfSectionAddress(elf, esize, shdr);
  erela = rela + shdr->sh_size / sizeof(*rela);
  for (; rela < erela; ++rela) {
    if (!ELF64_R_TYPE(rela->r_info)) continue;
    if (!(x = ELF64_R_SYM(rela->r_info))) continue;
    if (syms[x].st_shndx == SHN_ABS) continue;
    if (!syms[x].st_shndx) continue;
    shdr = GetElfSectionHeaderAddress(elf, esize, syms[x].st_shndx);
    if (~shdr->sh_flags & SHF_EXECINSTR) continue;  // data reference
    secname = GetElfString(elf, esize, secstrs, shdr->sh_name);
    if (strcmp(".privileged", secname)) {
      Print(2, epath,
            ": code in .privileged section "
            "references symbol '",
            GetElfString(elf, esize, symstrs, syms[x].st_name),
            "' in unprivileged code section '", secname, "'\n", NULL);
      exit(1);
    }
  }
}

// Modify ARM64 code to use x28 for TLS rather than tpidr_el0.
static void RewriteTlsCode(void) {
  int i, dest;
  Elf64_Shdr *shdr;
  uint32_t *p, *pe;
  for (i = 0; i < elf->e_shnum; ++i) {
    shdr = GetElfSectionHeaderAddress(elf, esize, i);
    if (shdr->sh_type == SHT_PROGBITS &&     //
        (shdr->sh_flags & SHF_ALLOC) &&      //
        (shdr->sh_flags & SHF_EXECINSTR) &&  //
        (p = GetElfSectionAddress(elf, esize, shdr))) {
      for (pe = p + shdr->sh_size / 4; p <= pe; ++p) {
        if ((*p & -32) == MRS_TPIDR_EL0) {
          *p = MOV_REG(*p & 31, COSMO_TLS_REG);
        }
      }
    }
  }
}

/**
 * Improve GCC11 `-fpatchable-function-entry` codegen.
 *
 * When using flags like `-fpatchable-function-entry=9,7` GCC v11 will
 * insert two `nop` instructions, rather than merging them into faster
 * "fat" nops.
 *
 * In order for this to work, the function symbol must be declared as
 * `STT_FUNC` and `st_size` must have the function's byte length.
 */
static void OptimizePatchableFunctionEntries(void) {
#ifdef __x86_64__
  long i, n;
  int nopcount;
  Elf64_Shdr *shdr;
  unsigned char *p, *pe;
  for (i = 0; i < symcount; ++i) {
    if (ELF64_ST_TYPE(syms[i].st_info) == STT_FUNC && syms[i].st_size) {
      shdr = GetElfSectionHeaderAddress(elf, esize, syms[i].st_shndx);
      p = GetElfSectionAddress(elf, esize, shdr);
      p += syms[i].st_value;
      pe = p + syms[i].st_size;
      for (; p + 1 < pe; p += n) {
        if (p[0] != 0x90) break;
        if (p[1] != 0x90) break;
        for (n = 2; p + n < pe && n < ARRAYLEN(kFatNops); ++n) {
          if (p[n] != 0x90) break;
        }
        memcpy(p, kFatNops[n], n);
      }
    }
  }
#endif /* __x86_64__ */
}

static void OptimizeRelocations(void) {
  Elf64_Half i;
  Elf64_Rela *rela;
  unsigned char *code, *p;
  Elf64_Shdr *shdr, *shdrcode;
  for (i = 0; i < elf->e_shnum; ++i) {
    shdr = GetElfSectionHeaderAddress(elf, esize, i);
    if (shdr->sh_type == SHT_RELA) {
      shdrcode = GetElfSectionHeaderAddress(elf, esize, shdr->sh_info);
      if (!(shdrcode->sh_flags & SHF_EXECINSTR)) continue;
      code = GetElfSectionAddress(elf, esize, shdrcode);
      for (rela = GetElfSectionAddress(elf, esize, shdr);
           ((uintptr_t)rela + shdr->sh_entsize <=
            MIN((uintptr_t)elf + esize,
                (uintptr_t)elf + shdr->sh_offset + shdr->sh_size));
           ++rela) {

        /*
         * GCC isn't capable of -mnop-mcount when using -fpie.
         * Let's fix that. It saves ~14 cycles per function call.
         * Then libc/runtime/ftrace.greg.c morphs it back at runtime.
         */
        if (ELF64_R_TYPE(rela->r_info) == R_X86_64_GOTPCRELX &&
            strcmp(GetElfString(elf, esize, symstrs,
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
            strcmp(GetElfString(elf, esize, symstrs,
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

static void FixupObject(void) {
  int fd;
  if ((fd = open(epath, mode)) == -1) {
    SysExit("open");
  }
  if ((esize = lseek(fd, 0, SEEK_END)) == -1) {
    SysExit("lseek");
  }
  if (esize) {
    if ((elf = mmap(0, esize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) ==
        MAP_FAILED) {
      SysExit("mmap");
    }
    if (!IsElf64Binary(elf, esize)) {
      Print(2, epath, ": not an elf64 binary\n", NULL);
      exit(1);
    }
    if (!(syms = GetElfSymbolTable(elf, esize, &symcount))) {
      Print(2, epath, ": missing elf symbol table\n", NULL);
      exit(1);
    }
    if (!(secstrs = GetElfSectionNameStringTable(elf, esize))) {
      Print(2, epath, ": missing elf section string table\n", NULL);
      exit(1);
    }
    if (!(symstrs = GetElfStringTable(elf, esize))) {
      Print(2, epath, ": missing elf symbol string table\n", NULL);
      exit(1);
    }
    CheckPrivilegedCrossReferences();
    if (mode == O_RDWR) {
      if (elf->e_machine == EM_NEXGEN32E) {
        OptimizeRelocations();
        OptimizePatchableFunctionEntries();
      }
      if (elf->e_machine == EM_AARCH64) {
        RewriteTlsCode();
      }
      if (msync(elf, esize, MS_ASYNC | MS_INVALIDATE)) {
        SysExit("msync");
      }
    }
    if (munmap(elf, esize)) {
      SysExit("munmap");
    }
  }
  if (close(fd)) {
    SysExit("close");
  }
}

int main(int argc, char *argv[]) {
  int i, opt;
  if (!IsOptimized()) {
    ShowCrashReports();
  }
  GetOpts(argc, argv);
  for (i = optind; i < argc; ++i) {
    epath = argv[i];
    FixupObject();
  }
}
