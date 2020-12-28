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
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/elf/elf.h"
#include "libc/elf/struct/rela.h"
#include "libc/elf/struct/shdr.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/x.h"
#include "tool/decode/lib/asmcodegen.h"
#include "tool/decode/lib/elfidnames.h"
#include "tool/decode/lib/flagger.h"
#include "tool/decode/lib/titlegen.h"

/**
 * @fileoverview ELF executable metadata disassembler.
 */

static const char *path;
static struct stat st[1];
static Elf64_Ehdr *elf;

static void startfile(void) {
  showtitle("αcτµαlly pδrταblε εxεcµταblε", "tool/decode/elf", basename(path),
            NULL, &kModelineAsm);
  printf("#include \"libc/elf.h\"\n\n", path);
}

static void printelfehdr(void) {
  show(".ascii", format(b1, "%`'.*s", 4, (const char *)&elf->e_ident[0]),
       "magic");
  show(".byte",
       firstnonnull(findnamebyid(&kElfClassNames[0], elf->e_ident[EI_CLASS]),
                    format(b1, "%d", elf->e_ident[EI_CLASS])),
       "elf->e_ident[EI_CLASS]");
  show(".byte",
       firstnonnull(findnamebyid(kElfDataNames, elf->e_ident[EI_DATA]),
                    format(b1, "%d", elf->e_ident[EI_DATA])),
       "elf->e_ident[EI_DATA]");
  show(".byte", format(b1, "%d", elf->e_ident[EI_VERSION]),
       "elf->e_ident[EI_VERSION]");
  show(".byte",
       firstnonnull(findnamebyid(kElfOsabiNames, elf->e_ident[EI_OSABI]),
                    format(b1, "%d", elf->e_ident[EI_OSABI])),
       "elf->e_ident[EI_OSABI]");
  show(".byte", format(b1, "%d", elf->e_ident[EI_ABIVERSION]),
       "elf->e_ident[EI_ABIVERSION]");
  show(".byte",
       format(b1, "%d,%d,%d,%d,%d,%d,%d", elf->e_ident[EI_PAD + 0],
              elf->e_ident[EI_PAD + 1], elf->e_ident[EI_PAD + 2],
              elf->e_ident[EI_PAD + 3], elf->e_ident[EI_PAD + 4],
              elf->e_ident[EI_PAD + 5], elf->e_ident[EI_PAD + 6]),
       "padding");
  show(".org", "0x10", NULL);
  show(".short",
       firstnonnull(findnamebyid(kElfTypeNames, elf->e_type),
                    format(b1, "%hd", elf->e_type)),
       "elf->e_type");
  show(".short",
       firstnonnull(findnamebyid(kElfMachineNames, elf->e_machine),
                    format(b1, "%hd", elf->e_machine)),
       "elf->e_machine");
  show(".long", format(b1, "%d", elf->e_version), "elf->e_version");
  show(".quad", format(b1, "%#x", elf->e_entry), "elf->e_entry");
  show(".quad", format(b1, "%#x", elf->e_phoff), "elf->e_phoff");
  show(".quad", format(b1, "%#x", elf->e_shoff), "elf->e_shoff");
  show(".long", format(b1, "%#x", elf->e_flags), "elf->e_flags");
  show(".short", format(b1, "%hd", elf->e_ehsize), "elf->e_ehsize");
  show(".short", format(b1, "%hd", elf->e_phentsize), "elf->e_phentsize");
  show(".short", format(b1, "%hd", elf->e_phnum), "elf->e_phnum");
  show(".short", format(b1, "%hd", elf->e_shentsize), "elf->e_shentsize");
  show(".short", format(b1, "%hd", elf->e_shnum), "elf->e_shnum");
  show(".short", format(b1, "%hd", elf->e_shstrndx), "elf->e_shstrndx");
}

static void printelfsegmentheader(int i) {
  Elf64_Phdr *phdr = GetElfSegmentHeaderAddress(elf, st->st_size, i);
  printf("/\tElf64_Phdr *phdr = GetElfSegmentHeaderAddress(elf, st->st_size, "
         "%d)\n",
         i);
  printf(".Lph%d:", i);
  show(".long",
       firstnonnull(findnamebyid(kElfSegmentTypeNames, phdr->p_type),
                    format(b1, "%#x", phdr->p_type)),
       "phdr->p_type");
  show(".long", RecreateFlags(kElfSegmentFlagNames, phdr->p_flags),
       "phdr->p_flags");
  show(".quad", format(b1, "%#x", phdr->p_offset), "phdr->p_offset");
  show(".quad", format(b1, "%#x", phdr->p_vaddr), "phdr->p_vaddr");
  show(".quad", format(b1, "%#x", phdr->p_paddr), "phdr->p_paddr");
  show(".quad", format(b1, "%#x", phdr->p_filesz), "phdr->p_filesz");
  show(".quad", format(b1, "%#x", phdr->p_memsz), "phdr->p_memsz");
  show(".quad", format(b1, "%#x", phdr->p_align), "phdr->p_align");
  fflush(stdout);
}

static void printelfsegmentheaders(void) {
  printf("\n");
  printf("\t.org\t%#x\n", elf->e_phoff);
  for (unsigned i = 0; i < elf->e_phnum; ++i) printelfsegmentheader(i);
}

static void printelfsectionheader(int i, char *shstrtab) {
  Elf64_Shdr *shdr;
  shdr = GetElfSectionHeaderAddress(elf, st->st_size, i);
  printf("/\tElf64_Shdr *shdr = GetElfSectionHeaderAddress(elf, st->st_size, "
         "%d)\n",
         i);
  printf(".Lsh%d:", i);
  show(".long", format(b1, "%d", shdr->sh_name),
       format(b2,
              "%`'s == GetElfString(elf, st->st_size, shstrtab, shdr->sh_name)",
              GetElfString(elf, st->st_size, shstrtab, shdr->sh_name)));
  show(".long",
       firstnonnull(findnamebyid(kElfSectionTypeNames, shdr->sh_type),
                    format(b1, "%d", shdr->sh_type)),
       "shdr->sh_type");
  show(".long", RecreateFlags(kElfSectionFlagNames, shdr->sh_flags),
       "shdr->sh_flags");
  show(".quad", format(b1, "%#x", shdr->sh_addr), "shdr->sh_addr");
  show(".quad", format(b1, "%#x", shdr->sh_offset), "shdr->sh_offset");
  show(".quad", format(b1, "%#x", shdr->sh_size), "shdr->sh_size");
  show(".long", format(b1, "%#x", shdr->sh_link), "shdr->sh_link");
  show(".long", format(b1, "%#x", shdr->sh_info), "shdr->sh_info");
  show(".quad", format(b1, "%#x", shdr->sh_addralign), "shdr->sh_addralign");
  show(".quad", format(b1, "%#x", shdr->sh_entsize), "shdr->sh_entsize");
  fflush(stdout);
}

static void printelfsectionheaders(void) {
  Elf64_Half i;
  char *shstrtab = GetElfSectionNameStringTable(elf, st->st_size);
  if (shstrtab) {
    printf("\n");
    printf("\t.org\t%#x\n", elf->e_shoff);
    for (i = 0; i < elf->e_shnum; ++i) {
      printelfsectionheader(i, shstrtab);
    }
    printf("\n/\t%s\n", "elf->e_shstrndx");
    printf("\t.org\t%#x\n",
           GetElfSectionHeaderAddress(elf, st->st_size, elf->e_shstrndx)
               ->sh_offset);
    for (i = 0; i < elf->e_shnum; ++i) {
      Elf64_Shdr *shdr = GetElfSectionHeaderAddress(elf, st->st_size, i);
      const char *str = GetElfString(elf, st->st_size, shstrtab, shdr->sh_name);
      show(".asciz", format(b1, "%`'s", str), NULL);
    }
  }
}

static void printelfsymbolinfo(Elf64_Sym *sym) {
  int bind = (sym->st_info >> 4) & 0xf;
  const char *bindname = findnamebyid(kElfSymbolBindNames, bind);
  int type = (sym->st_info >> 0) & 0xf;
  const char *typename = findnamebyid(kElfSymbolTypeNames, type);
  show(".byte",
       format(b1, "%s%s%s", bindname ? format(b2, "%s<<4", bindname) : "",
              bindname && typename ? "|" : "", firstnonnull(typename, "")),
       "sym->st_info");
}

static void printelfsymbolother(Elf64_Sym *sym) {
  int visibility = sym->st_other & 0x3;
  const char *visibilityname =
      findnamebyid(kElfSymbolVisibilityNames, visibility);
  int other = sym->st_other & ~0x3;
  show(".byte",
       format(b1, "%s%s%s", firstnonnull(visibilityname, ""),
              other && visibilityname ? "+" : "",
              other ? format(b2, "%d", other) : ""),
       "sym->st_other");
}

static void printelfsymbol(Elf64_Sym *sym, char *strtab, char *shstrtab) {
  show(".long", format(b1, "%d", sym->st_name),
       format(b2, "%`'s (sym->st_name)",
              GetElfString(elf, st->st_size, strtab, sym->st_name)));
  printelfsymbolinfo(sym);
  printelfsymbolother(sym);
  show(".short", format(b1, "%d", sym->st_shndx),
       format(b2, "%s sym->st_shndx",
              sym->st_shndx < 0xff00
                  ? format(b1, "%`'s",
                           GetElfString(elf, st->st_size, shstrtab,
                                        GetElfSectionHeaderAddress(
                                            elf, st->st_size, sym->st_shndx)
                                            ->sh_name))
                  : findnamebyid(kElfSpecialSectionNames, sym->st_shndx)));
  show(".quad", format(b1, "%#x", sym->st_value), "sym->st_value");
  show(".quad", format(b1, "%#x", sym->st_size), "sym->st_size");
}

static void printelfsymboltable(void) {
  size_t i, symcount = 0;
  Elf64_Sym *symtab = GetElfSymbolTable(elf, st->st_size, &symcount);
  char *strtab = GetElfStringTable(elf, st->st_size);
  char *shstrtab = GetElfSectionNameStringTable(elf, st->st_size);
  if (symtab && strtab) {
    printf("\n\n");
    printf("\t.org\t%#x\n", (intptr_t)symtab - (intptr_t)elf);
    for (i = 0; i < symcount; ++i) {
      printf(".Lsym%d:\n", i);
      printelfsymbol(&symtab[i], strtab, shstrtab);
    }
  }
}

static char *getelfsymbolname(const Elf64_Ehdr *elf, size_t mapsize,
                              const char *strtab, const char *shstrtab,
                              const Elf64_Sym *sym) {
  char *res;
  const Elf64_Shdr *shdr;
  if (elf && sym &&
      ((shstrtab && !sym->st_name &&
        ELF64_ST_TYPE(sym->st_info) == STT_SECTION &&
        (shdr = GetElfSectionHeaderAddress(elf, mapsize, sym->st_shndx)) &&
        (res = GetElfString(elf, mapsize, shstrtab, shdr->sh_name))) ||
       (strtab && (res = GetElfString(elf, mapsize, strtab, sym->st_name))))) {
    return res;
  } else {
    return NULL;
  }
}

static void printelfrelocations(void) {
  int sym;
  size_t i, j, count;
  const Elf64_Sym *syms;
  const Elf64_Rela *rela;
  const Elf64_Shdr *shdr, *symtab;
  char *strtab, *shstrtab, *symbolname;
  strtab = GetElfStringTable(elf, st->st_size);
  shstrtab = GetElfSectionNameStringTable(elf, st->st_size);
  for (i = 0; i < elf->e_shnum; ++i) {
    if ((shdr = GetElfSectionHeaderAddress(elf, st->st_size, i)) &&
        shdr->sh_type == SHT_RELA &&
        (rela = GetElfSectionAddress(elf, st->st_size, shdr))) {
      printf("\n/\t%s\n", GetElfSectionName(elf, st->st_size, shdr));
      printf("\t.org\t%#x\n", (intptr_t)rela - (intptr_t)elf);
      for (j = 0; ((uintptr_t)rela + sizeof(Elf64_Rela) <=
                   min((uintptr_t)elf + st->st_size,
                       (uintptr_t)elf + shdr->sh_offset + shdr->sh_size));
           ++rela, ++j) {
        symtab = GetElfSectionHeaderAddress(elf, st->st_size, shdr->sh_link);
        count = symtab->sh_size / symtab->sh_entsize;
        syms = GetElfSectionAddress(elf, st->st_size, symtab);
        sym = ELF64_R_SYM(rela->r_info);
        if (0 <= sym && sym < count) {
          symbolname =
              getelfsymbolname(elf, st->st_size, strtab, shstrtab, syms + sym);
        } else {
          symbolname = xasprintf("bad-sym-%d", sym);
        }
        printf("/\t%s+%#lx → %s%c%#lx\n",
               GetElfString(
                   elf, st->st_size, shstrtab,
                   GetElfSectionHeaderAddress(elf, st->st_size, shdr->sh_info)
                       ->sh_name),
               rela->r_offset, symbolname, rela->r_addend >= 0 ? '+' : '-',
               abs(rela->r_addend));
        printf("%s_%zu_%zu:\n", ".Lrela", i, j);
        show(".quad", format(b1, "%#lx", rela->r_offset), "rela->r_offset");
        show(".long",
             format(b1, "%s%s", "R_X86_64_",
                    findnamebyid(kElfNexgen32eRelocationNames,
                                 ELF64_R_TYPE(rela->r_info))),
             "ELF64_R_TYPE(rela->r_info)");
        show(".long", format(b1, "%d", ELF64_R_SYM(rela->r_info)),

             "ELF64_R_SYM(rela->r_info)");
        show(".quad", format(b1, "%#lx", rela->r_addend), "rela->r_addend");
      }
    }
  }
}

int main(int argc, char *argv[]) {
  showcrashreports();
  if (argc != 2) {
    fprintf(stderr, "usage: %s FILE\n", argv[0]);
    return 1;
  }
  path = argv[1];
  int64_t fd = open(path, O_RDONLY);
  if (fd == -1) {
    if (errno == ENOENT) {
      fprintf(stderr, "error: %`s not found\n", path);
      exit(1);
    }
    perror("open");
    exit(1);
  }
  fstat(fd, st);
  CHECK_NE(MAP_FAILED,
           (elf = mmap(NULL, st->st_size, PROT_READ, MAP_SHARED, fd, 0)));
  if (memcmp(elf->e_ident, ELFMAG, 4) != 0) {
    fprintf(stderr, "error: not an elf executable: %'s\n", path);
    exit(1);
  }
  startfile();
  printelfehdr();
  printelfsegmentheaders();
  printelfsectionheaders();
  printelfrelocations();
  printelfsymboltable();
  munmap(elf, st->st_size);
  close(fd);
  return 0;
}
