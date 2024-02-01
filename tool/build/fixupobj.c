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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/rela.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/stdalign.internal.h"
#include "libc/stdckdint.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/zip.internal.h"
#include "third_party/getopt/getopt.internal.h"

/**
 * @fileoverview GCC Codegen Fixer-Upper.
 */

#define COSMO_TLS_REG 28
#define MRS_TPIDR_EL0 0xd53bd040u
#define IFUNC_SECTION ".init.202.ifunc"

#define MOV_REG(DST, SRC) (0xaa0003e0u | (SRC) << 16 | (DST))

static int mode;
static int fildes;
static char *symstrs;
static char *secstrs;
static ssize_t esize;
static Elf64_Sym *syms;
static Elf64_Ehdr *elf;
static const char *epath;
static Elf64_Xword symcount;

static wontreturn void Die(const char *reason) {
  tinyprint(2, epath, ": ", reason, "\n", NULL);
  exit(1);
}

static wontreturn void SysExit(const char *func) {
  const char *errstr;
  if (!(errstr = _strerdoc(errno))) errstr = "EUNKNOWN";
  tinyprint(2, epath, ": ", func, " failed with ", errstr, "\n", NULL);
  exit(1);
}

static wontreturn void PrintUsage(int fd, int exitcode) {
  tinyprint(fd, "\n\
NAME\n\
\n\
  Cosmopolitan Object Fixer\n\
\n\
SYNOPSIS\n\
\n\
  ",
            program_invocation_name, " [FLAGS] OBJECT...\n\
\n\
DESCRIPTION\n\
\n\
  This program applies fixups to ELF object files and executables that\n\
  at build time whenever they're created by the toolchain. It's needed\n\
  so that zip assets work correctly, plus this'll make code go faster.\n\
  This program is also able to spot some coding errors like privileged\n\
  functions calling unprivileged ones.\n\
\n\
  Multiple binary files may be specified, which are modified in-place.\n\
\n\
FLAGS\n\
\n\
  -h            show this help\n\
  -c            checks only mode\n\
\n\
",
            NULL);
  exit(exitcode);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  mode = O_RDWR;
  while ((opt = getopt(argc, argv, "ch")) != -1) {
    switch (opt) {
      case 'c':
        mode = O_RDONLY;
        break;
      case 'h':
        PrintUsage(1, 0);
      default:
        PrintUsage(2, 1);
    }
  }
  if (optind == argc) {
    tinyprint(2,
              "error: no elf object files specified\n"
              "run ",
              program_invocation_name, " -h for usage\n", NULL);
    exit(1);
  }
}

// Official Intel Multibyte No-Operation Instructions. See
// Intel's Six Thousand Page Manual, Volume 2, Table 4-12:
// On "Recommended Multi-Byte Sequence of NOP Instruction"
static const unsigned char kNops[10][10] = {
    {},                                          //
    {/***/ /***/ 0x90},                          // nop
    {0x66, /***/ 0x90},                          // xchg %ax,%ax
    {/***/ 0x0f, 0x1f, 0000},                    // nopl (%rax)
    {/***/ 0x0f, 0x1f, 0100, /***/ 0},           // nopl 0x00(%rax)
    {/***/ 0x0f, 0x1f, 0104, 0000, 0},           // nopl 0x00(%rax,%rax,1)
    {0x66, 0x0f, 0x1f, 0104, 0000, 0},           // nopw 0x00(%rax,%rax,1)
    {/***/ 0x0f, 0x1f, 0200, 0000, 0, 0, 0},     // nopl 0x00000000(%rax)
    {/***/ 0x0F, 0x1F, 0204, 0000, 0, 0, 0, 0},  // nopl 0x00000000(%rax,%rax,1)
    {0x66, 0x0F, 0x1F, 0204, 0000, 0, 0, 0, 0},  // nopw 0x00000000(%rax,%rax,1)
    // osz  map  op   modrm  sib   displacement  //
};

/**
 * Rewrites leading NOP instructions to have fewer instructions.
 *
 * For example, the following code:
 *
 *     nop
 *     nop
 *     nop
 *     nop
 *     nop
 *     nop
 *     nop
 *     nop
 *     nop
 *     nop
 *     nop
 *     nop
 *     ret
 *     nop
 *     nop
 *
 * Would be morphed into the following:
 *
 *     nopw 0x00000000(%rax,%rax,1)
 *     xchg %ax,%ax
 *     ret
 *     nop
 *     nop
 *
 * @param p points to memory region that shall be modified
 * @param e points to end of memory region, i.e. `p + #bytes`
 * @return p advanced past last morphed byte
 */
static unsigned char *CoalesceNops(unsigned char *p, const unsigned char *e) {
  long n;
  for (; p + 1 < e; p += n) {
    if (p[0] != 0x90) break;
    if (p[1] != 0x90) break;
    for (n = 2; p + n < e; ++n) {
      if (p[n] != 0x90) break;
      if (n == ARRAYLEN(kNops) - 1) break;
    }
    memcpy(p, kNops[n], n);
  }
  return p;
}

static void CheckPrivilegedCrossReferences(void) {
  unsigned long x;
  const char *secname;
  const Elf64_Shdr *shdr;
  const Elf64_Rela *rela, *erela;
  shdr = FindElfSectionByName(elf, esize, secstrs, ".rela.privileged");
  if (!shdr || !(rela = GetElfSectionAddress(elf, esize, shdr))) return;
  erela = rela + shdr->sh_size / sizeof(*rela);
  for (; rela < erela; ++rela) {
    if (!ELF64_R_TYPE(rela->r_info)) continue;
    if (!(x = ELF64_R_SYM(rela->r_info))) continue;
    if (x >= symcount) continue;
    if (syms[x].st_shndx == SHN_ABS) continue;
    if (!syms[x].st_shndx) continue;
    if ((shdr = GetElfSectionHeaderAddress(elf, esize, syms[x].st_shndx))) {
      if (~shdr->sh_flags & SHF_EXECINSTR) continue;  // data reference
      if ((secname = GetElfString(elf, esize, secstrs, shdr->sh_name)) &&
          strcmp(".privileged", secname)) {
        tinyprint(2, epath,
                  ": code in .privileged section "
                  "references symbol '",
                  GetElfString(elf, esize, symstrs, syms[x].st_name),
                  "' in unprivileged code section '", secname, "'\n", NULL);
        exit(1);
      }
    }
  }
}

// Modify ARM64 code to use x28 for TLS rather than tpidr_el0.
static void RewriteTlsCode(void) {
  int i;
  Elf64_Shdr *shdr;
  uint32_t *p, *pe;
  for (i = 0; i < elf->e_shnum; ++i) {
    if (!(shdr = GetElfSectionHeaderAddress(elf, esize, i))) {
      Die("elf header overflow #1");
    }
    if (shdr->sh_type == SHT_PROGBITS &&  //
        (shdr->sh_flags & SHF_ALLOC) &&   //
        (shdr->sh_flags & SHF_EXECINSTR)) {
      if (!(p = GetElfSectionAddress(elf, esize, shdr))) {
        Die("elf header overflow #2");
      }
      for (pe = p + shdr->sh_size / 4; p <= pe; ++p) {
        if ((*p & -32) == MRS_TPIDR_EL0) {
          *p = MOV_REG(*p & 31, COSMO_TLS_REG);
        }
      }
    }
  }
}

static void UseFreebsdOsAbi(void) {
  elf->e_ident[EI_OSABI] = ELFOSABI_FREEBSD;
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
  long i;
  Elf64_Shdr *shdr;
  unsigned char *p;
  Elf64_Addr sym_rva;
  if (elf->e_machine == EM_NEXGEN32E) {
    for (i = 0; i < symcount; ++i) {
      if (!syms[i].st_size) continue;
      if (ELF64_ST_TYPE(syms[i].st_info) != STT_FUNC) continue;
      if (!(shdr = GetElfSectionHeaderAddress(elf, esize, syms[i].st_shndx))) {
        Die("elf header overflow #3");
      }
      if (shdr->sh_type != SHT_PROGBITS) continue;
      if (!(p = GetElfSectionAddress(elf, esize, shdr))) {
        Die("elf section overflow");
      }
      if (ckd_sub(&sym_rva, syms[i].st_value, shdr->sh_addr)) {
        Die("elf symbol beneath section");
      }
      if (sym_rva > esize - shdr->sh_offset ||               //
          (p += sym_rva) >= (unsigned char *)elf + esize ||  //
          syms[i].st_size >= esize - sym_rva) {
        Die("elf symbol overflow");
      }
      CoalesceNops(p, p + syms[i].st_size);
    }
  }
}

/**
 * Converts PKZIP recs from PC-relative to RVA-relative.
 */
static void RelinkZipFiles(void) {
  int rela, recs;
  unsigned long cdsize, cdoffset;
  unsigned char foot[kZipCdirHdrMinSize];
  unsigned char *base, *xeof, *stop, *eocd, *cdir, *lfile, *cfile;
  base = (unsigned char *)elf;
  xeof = (unsigned char *)elf + esize;
  eocd = xeof - kZipCdirHdrMinSize;
  stop = base;
  // scan backwards for zip eocd todo record
  // that was created by libc/nexgen32e/zip.S
  for (;;) {
    if (eocd < stop) return;
    if (READ32LE(eocd) == kZipCdirHdrMagicTodo &&  //
        ZIP_CDIR_SIZE(eocd) &&                     //
        !ZIP_CDIR_OFFSET(eocd) &&                  //
        !ZIP_CDIR_RECORDS(eocd) &&                 //
        !ZIP_CDIR_RECORDSONDISK(eocd)) {
      break;
    }
    eocd = memrchr(stop, 'P', eocd - base);
  }
  // apply fixups to zip central directory recs
  recs = 0;
  cdir = (stop = eocd) - (cdsize = ZIP_CDIR_SIZE(eocd));
  for (cfile = cdir; cfile < stop; cfile += ZIP_CFILE_HDRSIZE(cfile)) {
    if (++recs >= 65536) {
      Die("too many zip central directory records");
    }
    if (cfile < base ||                        //
        cfile + kZipCfileHdrMinSize > xeof ||  //
        cfile + ZIP_CFILE_HDRSIZE(cfile) > xeof) {
      Die("zip central directory entry overflows image");
    }
    if (READ32LE(cfile) != kZipCfileHdrMagic) {
      Die("bad __zip_cdir_size or zip central directory corrupted");
    }
    if ((rela = ZIP_CFILE_OFFSET(cfile)) < 0) {
      lfile = cfile + kZipCfileOffsetOffset + rela;
    } else {
      lfile = base + rela;  // earlier fixup failed partway?
    }
    if (lfile < base ||                        //
        lfile + kZipLfileHdrMinSize > xeof ||  //
        lfile + ZIP_LFILE_SIZE(lfile) > xeof) {
      Die("zip local file overflows image");
    }
    if (READ32LE(lfile) != kZipLfileHdrMagic) {
      Die("zip central directory offset to local file corrupted");
    }
    if (rela < 0) {
      WRITE32LE(cfile + kZipCfileOffsetOffset, lfile - base);
    }
  }
  // append new eocd record to program image
  if (esize > INT_MAX - sizeof(foot) ||
      (cdoffset = esize) > INT_MAX - sizeof(foot)) {
    Die("the time has come to adopt zip64");
  }
  bzero(foot, sizeof(foot));
  WRITE32LE(foot, kZipCdirHdrMagic);
  WRITE32LE(foot + kZipCdirSizeOffset, cdsize);
  WRITE16LE(foot + kZipCdirRecordsOffset, recs);
  WRITE32LE(foot + kZipCdirOffsetOffset, cdoffset);
  WRITE16LE(foot + kZipCdirRecordsOnDiskOffset, recs);
  if (pwrite(fildes, cdir, cdsize, esize) != cdsize) {
    SysExit("cdir pwrite");
  }
  if (pwrite(fildes, foot, sizeof(foot), esize + cdsize) != sizeof(foot)) {
    SysExit("eocd pwrite");
  }
  eocd = foot;
}

// when __attribute__((__target_clones__(...))) is used, the compiler
// will generate multiple implementations of a function for different
// microarchitectures as well as a resolver function that tells which
// function is appropriate to call. however the compiler doesn't make
// code for the actual function. it also doesn't record where resolve
// functions are located in the binary so we've reverse eng'd it here
static void GenerateIfuncInit(void) {
  char *name, *s;
  long code_i = 0;
  long relas_i = 0;
  static char code[16384];
  static Elf64_Rela relas[1024];
  Elf64_Shdr *symtab_shdr = GetElfSymbolTable(elf, esize, SHT_SYMTAB, 0);
  if (!symtab_shdr) Die("symbol table section header not found");
  Elf64_Word symtab_shdr_index =
      ((char *)symtab_shdr - ((char *)elf + elf->e_shoff)) / elf->e_shentsize;
  for (Elf64_Xword i = 0; i < symcount; ++i) {
    if (syms[i].st_shndx == SHN_UNDEF) continue;
    if (syms[i].st_shndx >= SHN_LORESERVE) continue;
    if (ELF64_ST_TYPE(syms[i].st_info) != STT_GNU_IFUNC) continue;
    if (!(name = GetElfString(elf, esize, symstrs, syms[i].st_name)))
      Die("could not get symbol name of ifunc");
    static char resolver_name[65536];
    strlcpy(resolver_name, name, sizeof(resolver_name));
    if (strlcat(resolver_name, ".resolver", sizeof(resolver_name)) >=
        sizeof(resolver_name))
      Die("ifunc name too long");
    Elf64_Xword function_sym_index = i;
    Elf64_Xword resolver_sym_index = -1;
    for (Elf64_Xword i = 0; i < symcount; ++i) {
      if (syms[i].st_shndx == SHN_UNDEF) continue;
      if (syms[i].st_shndx >= SHN_LORESERVE) continue;
      if (ELF64_ST_TYPE(syms[i].st_info) != STT_FUNC) continue;
      if (!(s = GetElfString(elf, esize, symstrs, syms[i].st_name))) continue;
      if (strcmp(s, resolver_name)) continue;
      resolver_sym_index = i;
      break;
    }
    if (resolver_sym_index == -1)
      // this can happen if a function with __target_clones() also has a
      // __weak_reference() defined, in which case GCC shall only create
      // one resolver function for the two of them so we can ignore this
      // HOWEVER the GOT will still have an entry for each two functions
      continue;

    // call the resolver (using cosmo's special .init abi)
    static const char chunk1[] = {
        0x57,                          // push %rdi
        0x56,                          // push %rsi
        0xe8, 0x00, 0x00, 0x00, 0x00,  // call f.resolver
    };
    if (code_i + sizeof(chunk1) > sizeof(code) || relas_i + 1 > ARRAYLEN(relas))
      Die("too many ifuncs");
    memcpy(code + code_i, chunk1, sizeof(chunk1));
    relas[relas_i].r_info = ELF64_R_INFO(resolver_sym_index, R_X86_64_PLT32);
    relas[relas_i].r_offset = code_i + 1 + 1 + 1;
    relas[relas_i].r_addend = -4;
    code_i += sizeof(chunk1);
    relas_i += 1;

    // move the resolved function address into the GOT slot. it's very
    // important that this happen, because the linker by default makes
    // self-referencing PLT functions whose execution falls through oh
    // no. we need to repeat this process for any aliases this defines
    static const char chunk2[] = {
        0x48, 0x89, 0x05, 0x00, 0x00, 0x00, 0x00,  // mov %rax,f@gotpcrel(%rip)
    };
    for (Elf64_Xword i = 0; i < symcount; ++i) {
      if (i == function_sym_index ||
          (ELF64_ST_TYPE(syms[i].st_info) == STT_GNU_IFUNC &&
           syms[i].st_shndx == syms[function_sym_index].st_shndx &&
           syms[i].st_value == syms[function_sym_index].st_value)) {
        if (code_i + sizeof(chunk2) > sizeof(code) ||
            relas_i + 1 > ARRAYLEN(relas))
          Die("too many ifuncs");
        memcpy(code + code_i, chunk2, sizeof(chunk2));
        relas[relas_i].r_info = ELF64_R_INFO(i, R_X86_64_GOTPCREL);
        relas[relas_i].r_offset = code_i + 3;
        relas[relas_i].r_addend = -4;
        code_i += sizeof(chunk2);
        relas_i += 1;
      }
    }

    static const char chunk3[] = {
        0x5e,  // pop %rsi
        0x5f,  // pop %rdi
    };
    if (code_i + sizeof(chunk3) > sizeof(code)) Die("too many ifuncs");
    memcpy(code + code_i, chunk3, sizeof(chunk3));
    code_i += sizeof(chunk3);
  }
  if (!code_i) return;

  // prepare to mutate elf
  // remap file so it has more space
  if (elf->e_shnum + 2 > 65535) Die("too many sections");
  size_t reserve_size = esize + 32 * 1024 * 1024;
  if (ftruncate(fildes, reserve_size)) SysExit("ifunc ftruncate #1");
  elf = mmap((char *)elf, reserve_size, PROT_READ | PROT_WRITE,
             MAP_FIXED | MAP_SHARED, fildes, 0);
  if (elf == MAP_FAILED) SysExit("ifunc mmap");

  // duplicate section name strings table to end of file
  Elf64_Shdr *shdrstr_shdr = (Elf64_Shdr *)((char *)elf + elf->e_shoff +
                                            elf->e_shstrndx * elf->e_shentsize);
  memcpy((char *)elf + esize, (char *)elf + shdrstr_shdr->sh_offset,
         shdrstr_shdr->sh_size);
  shdrstr_shdr->sh_offset = esize;
  esize += shdrstr_shdr->sh_size;

  // append strings for the two sections we're creating
  const char *code_section_name = IFUNC_SECTION;
  Elf64_Word code_section_name_offset = shdrstr_shdr->sh_size;
  memcpy((char *)elf + esize, code_section_name, strlen(code_section_name) + 1);
  shdrstr_shdr->sh_size += strlen(code_section_name) + 1;
  esize += strlen(code_section_name) + 1;
  const char *rela_section_name = ".rela" IFUNC_SECTION;
  Elf64_Word rela_section_name_offset = shdrstr_shdr->sh_size;
  memcpy((char *)elf + esize, rela_section_name, strlen(rela_section_name) + 1);
  shdrstr_shdr->sh_size += strlen(rela_section_name) + 1;
  esize += strlen(rela_section_name) + 1;
  unassert(esize == shdrstr_shdr->sh_offset + shdrstr_shdr->sh_size);
  ++esize;

  // duplicate section headers to end of file
  esize = (esize + alignof(Elf64_Shdr) - 1) & -alignof(Elf64_Shdr);
  memcpy((char *)elf + esize, (char *)elf + elf->e_shoff,
         elf->e_shnum * elf->e_shentsize);
  elf->e_shoff = esize;
  esize += elf->e_shnum * elf->e_shentsize;
  unassert(esize == elf->e_shoff + elf->e_shnum * elf->e_shentsize);

  // append code section header
  Elf64_Shdr *code_shdr = (Elf64_Shdr *)((char *)elf + esize);
  Elf64_Word code_shdr_index = elf->e_shnum++;
  esize += elf->e_shentsize;
  code_shdr->sh_name = code_section_name_offset;
  code_shdr->sh_type = SHT_PROGBITS;
  code_shdr->sh_flags = SHF_ALLOC | SHF_EXECINSTR;
  code_shdr->sh_addr = 0;
  code_shdr->sh_link = 0;
  code_shdr->sh_info = 0;
  code_shdr->sh_entsize = 1;
  code_shdr->sh_addralign = 1;
  code_shdr->sh_size = code_i;

  // append code's rela section header
  Elf64_Shdr *rela_shdr = (Elf64_Shdr *)((char *)elf + esize);
  esize += elf->e_shentsize;
  rela_shdr->sh_name = rela_section_name_offset;
  rela_shdr->sh_type = SHT_RELA;
  rela_shdr->sh_flags = SHF_INFO_LINK;
  rela_shdr->sh_addr = 0;
  rela_shdr->sh_info = code_shdr_index;
  rela_shdr->sh_link = symtab_shdr_index;
  rela_shdr->sh_entsize = sizeof(Elf64_Rela);
  rela_shdr->sh_addralign = alignof(Elf64_Rela);
  rela_shdr->sh_size = relas_i * sizeof(Elf64_Rela);
  elf->e_shnum++;

  // append relas
  esize = (esize + 63) & -64;
  rela_shdr->sh_offset = esize;
  memcpy((char *)elf + esize, relas, relas_i * sizeof(Elf64_Rela));
  esize += relas_i * sizeof(Elf64_Rela);
  unassert(esize == rela_shdr->sh_offset + rela_shdr->sh_size);

  // append code
  esize = (esize + 63) & -64;
  code_shdr->sh_offset = esize;
  memcpy((char *)elf + esize, code, code_i);
  esize += code_i;
  unassert(esize == code_shdr->sh_offset + code_shdr->sh_size);

  if (ftruncate(fildes, esize)) SysExit("ifunc ftruncate #1");
}

// when __attribute__((__target_clones__(...))) is used, static binaries
// become poisoned with rela IFUNC relocations, which the linker refuses
// to remove. even if we objcopy the ape executable as binary the linker
// preserves its precious ifunc code and puts them before the executable
// header. the good news is that the linker actually does link correctly
// which means we can delete the broken rela sections in the elf binary.
static void PurgeIfuncSections(void) {
  Elf64_Shdr *shdrs = (Elf64_Shdr *)((char *)elf + elf->e_shoff);
  for (Elf64_Word i = 0; i < elf->e_shnum; ++i) {
    char *name;
    if (shdrs[i].sh_type == SHT_RELA ||
        ((name = GetElfSectionName(elf, esize, shdrs + i)) &&
         !strcmp(name, ".init.202.ifunc"))) {
      shdrs[i].sh_type = SHT_NULL;
      shdrs[i].sh_flags &= ~SHF_ALLOC;
    }
  }
}

static void FixupObject(void) {
  if ((fildes = open(epath, mode)) == -1) {
    SysExit("open");
  }
  if ((esize = lseek(fildes, 0, SEEK_END)) == -1) {
    SysExit("lseek");
  }
  if (esize) {
    if ((elf = mmap((void *)0x003210000000, esize, PROT_READ | PROT_WRITE,
                    MAP_FIXED | MAP_SHARED, fildes, 0)) == MAP_FAILED) {
      SysExit("mmap");
    }
    if (!IsElf64Binary(elf, esize)) {
      Die("not an elf64 binary");
    }
    if (!(syms = GetElfSymbols(elf, esize, SHT_SYMTAB, &symcount))) {
      Die("missing elf symbol table");
    }
    if (!(secstrs = GetElfSectionNameStringTable(elf, esize))) {
      Die("missing elf section string table");
    }
    if (!(symstrs = GetElfStringTable(elf, esize, ".strtab"))) {
      Die("missing elf symbol string table");
    }
    CheckPrivilegedCrossReferences();
    if (mode == O_RDWR) {
      if (elf->e_machine == EM_NEXGEN32E) {
        OptimizePatchableFunctionEntries();
        GenerateIfuncInit();
      } else if (elf->e_machine == EM_AARCH64) {
        RewriteTlsCode();
        if (elf->e_type != ET_REL) {
          UseFreebsdOsAbi();
        }
      }
      if (elf->e_type != ET_REL) {
        PurgeIfuncSections();
        RelinkZipFiles();
      }
      if (msync(elf, esize, MS_ASYNC | MS_INVALIDATE)) {
        SysExit("msync");
      }
    }
    if (munmap(elf, esize)) {
      SysExit("munmap");
    }
  }
  if (close(fildes)) {
    SysExit("close");
  }
}

int main(int argc, char *argv[]) {
  int i;
  if (!IsOptimized()) {
    ShowCrashReports();
  }
  GetOpts(argc, argv);
  for (i = optind; i < argc; ++i) {
    epath = argv[i];
    FixupObject();
  }
}
