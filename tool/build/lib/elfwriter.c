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
#include "tool/build/lib/elfwriter.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/elf/def.h"
#include "libc/log/check.h"
#include "libc/mem/arraylist2.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/stdalign.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/xasprintf.h"

static const Elf64_Ehdr kObjHeader = {
    .e_ident = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3, ELFCLASS64, ELFDATA2LSB, 1,
                ELFOSABI_NONE},
    .e_type = ET_REL,
    .e_machine = EM_NEXGEN32E,
    .e_version = 1,
    .e_ehsize = sizeof(Elf64_Ehdr),
    .e_shentsize = sizeof(Elf64_Shdr),
};

static size_t AppendSection(struct ElfWriter *elf, const char *name,
                            int sh_type, int sh_flags) {
  ssize_t section =
      APPEND(&elf->shdrs->p, &elf->shdrs->i, &elf->shdrs->n,
             (&(Elf64_Shdr){.sh_type = sh_type,
                            .sh_flags = sh_flags,
                            .sh_entsize = elf->entsize,
                            .sh_addralign = elf->addralign,
                            .sh_offset = sh_type != SHT_NULL ? elf->wrote : 0,
                            .sh_name = intern(elf->shstrtab, name)}));
  CHECK_NE(-1, section);
  return section;
}

static size_t FinishSection(struct ElfWriter *elf) {
  size_t section = elf->shdrs->i - 1;
  elf->shdrs->p[section].sh_size =
      elf->wrote - elf->shdrs->p[section].sh_offset;
  return section;
}

static struct ElfWriterSymRef AppendSymbol(struct ElfWriter *elf,
                                           const char *name, int st_info,
                                           int st_other, size_t st_value,
                                           size_t st_size, size_t st_shndx,
                                           enum ElfWriterSymOrder slg) {
  ssize_t sym =
      APPEND(&elf->syms[slg]->p, &elf->syms[slg]->i, &elf->syms[slg]->n,
             (&(Elf64_Sym){.st_info = st_info,
                           .st_size = st_size,
                           .st_value = st_value,
                           .st_other = st_other,
                           .st_name = intern(elf->strtab, name),
                           .st_shndx = st_shndx}));
  CHECK_NE(-1, sym);
  return (struct ElfWriterSymRef){.slg = slg, .sym = sym};
}

static void MakeRelaSection(struct ElfWriter *elf, size_t section) {
  size_t shdr, size;
  size = (elf->relas->i - elf->relas->j) * sizeof(Elf64_Rela);
  elfwriter_align(elf, alignof(Elf64_Rela), sizeof(Elf64_Rela));
  shdr = elfwriter_startsection(
      elf,
      gc(xasprintf("%s%s", ".rela",
                   &elf->shstrtab->p[elf->shdrs->p[section].sh_name])),
      SHT_RELA, SHF_INFO_LINK);
  elf->shdrs->p[shdr].sh_info = section;
  elfwriter_reserve(elf, size);
  elfwriter_commit(elf, size);
  FinishSection(elf);
  elf->relas->j = elf->relas->i;
}

static void WriteRelaSections(struct ElfWriter *elf, size_t symtab) {
  uint32_t sym;
  size_t i, j, k;
  Elf64_Rela *rela;
  for (j = 0, i = 0; i < elf->shdrs->i; ++i) {
    if (elf->shdrs->p[i].sh_type == SHT_RELA) {
      elf->shdrs->p[i].sh_link = symtab;
      for (rela = (Elf64_Rela *)((char *)elf->map + elf->shdrs->p[i].sh_offset);
           rela <
           (Elf64_Rela *)((char *)elf->map + (elf->shdrs->p[i].sh_offset +
                                              elf->shdrs->p[i].sh_size));
           rela++, j++) {
        sym = elf->relas->p[j].symkey.sym;
        for (k = 0; k < elf->relas->p[j].symkey.slg; ++k) {
          sym += elf->syms[k]->i;
        }
        rela->r_offset = elf->relas->p[j].offset;
        rela->r_info = ELF64_R_INFO(sym, elf->relas->p[j].type);
        rela->r_addend = elf->relas->p[j].addend;
      }
    }
  }
  assert(j == elf->relas->i);
}

static size_t FlushStrtab(struct ElfWriter *elf, const char *name,
                          struct Interner *strtab) {
  size_t size = strtab->i * sizeof(strtab->p[0]);
  elfwriter_align(elf, 1, 0);
  AppendSection(elf, name, SHT_STRTAB, 0);
  mempcpy(elfwriter_reserve(elf, size), strtab->p, size);
  elfwriter_commit(elf, size);
  return FinishSection(elf);
}

static void FlushTables(struct ElfWriter *elf) {
  size_t i, size, symtab;
  elfwriter_align(elf, alignof(Elf64_Sym), sizeof(Elf64_Sym));
  symtab = AppendSection(elf, ".symtab", SHT_SYMTAB, 0);
  for (i = 0; i < ARRAYLEN(elf->syms); ++i) {
    size = elf->syms[i]->i * sizeof(Elf64_Sym);
    if (size) {
      memcpy(elfwriter_reserve(elf, size), elf->syms[i]->p, size);
    }
    elfwriter_commit(elf, size);
  }
  FinishSection(elf);
  elf->shdrs->p[symtab].sh_link = FlushStrtab(elf, ".strtab", elf->strtab);
  elf->ehdr->e_shstrndx = FlushStrtab(elf, ".shstrtab", elf->shstrtab);
  WriteRelaSections(elf, symtab);
  size = elf->shdrs->i * sizeof(elf->shdrs->p[0]);
  elfwriter_align(elf, alignof(Elf64_Shdr), sizeof(elf->shdrs->p[0]));
  elf->ehdr->e_shoff = elf->wrote;
  elf->ehdr->e_shnum = elf->shdrs->i;
  elf->shdrs->p[symtab].sh_info =
      elf->syms[kElfWriterSymSection]->i + elf->syms[kElfWriterSymLocal]->i;
  mempcpy(elfwriter_reserve(elf, size), elf->shdrs->p, size);
  elfwriter_commit(elf, size);
}

struct ElfWriter *elfwriter_open(const char *path, int mode, int arch) {
  struct ElfWriter *elf;
  CHECK_NOTNULL((elf = calloc(1, sizeof(struct ElfWriter))));
  CHECK_NOTNULL((elf->path = strdup(path)));
  CHECK_NE(-1, (elf->fd = open(elf->path, O_CREAT | O_TRUNC | O_RDWR, mode)));
  CHECK_NE(-1, ftruncate(elf->fd, (elf->mapsize = FRAMESIZE)));
  CHECK_NE(MAP_FAILED, (elf->map = mmap((void *)(intptr_t)kFixedmapStart,
                                        elf->mapsize, PROT_READ | PROT_WRITE,
                                        MAP_SHARED | MAP_FIXED, elf->fd, 0)));
  elf->ehdr = memcpy(elf->map, &kObjHeader, (elf->wrote = sizeof(kObjHeader)));
  if (!arch) {
#ifdef __x86_64__
    arch = EM_NEXGEN32E;
#elif defined(__aarch64__)
    arch = EM_AARCH64;
#elif defined(__powerpc64__)
    arch = EM_PPC64;
#elif defined(__riscv)
    arch = EM_RISCV;
#elif defined(__s390x__)
    arch = EM_S390;
#else
#error "unsupported architecture"
#endif
  }
  elf->ehdr->e_machine = arch;
  elf->strtab = newinterner();
  elf->shstrtab = newinterner();
  intern(elf->strtab, "");
  intern(elf->shstrtab, "");
  intern(elf->shstrtab, ".shstrtab");
  return elf;
}

void elfwriter_close(struct ElfWriter *elf) {
  size_t i;
  FlushTables(elf);
  CHECK_NE(-1, msync(elf->map, elf->wrote, MS_ASYNC));
  CHECK_NE(-1, munmap(elf->map, elf->mapsize));
  CHECK_NE(-1, ftruncate(elf->fd, elf->wrote));
  CHECK_NE(-1, close(elf->fd));
  freeinterner(elf->shstrtab);
  freeinterner(elf->strtab);
  free(elf->shdrs->p);
  free(elf->relas->p);
  free(elf->path);
  for (i = 0; i < ARRAYLEN(elf->syms); ++i) {
    free(elf->syms[i]->p);
  }
  free(elf);
}

void elfwriter_align(struct ElfWriter *elf, size_t addralign, size_t entsize) {
  elf->entsize = entsize;
  elf->addralign = addralign;
  elf->wrote = ROUNDUP(elf->wrote, addralign);
}

size_t elfwriter_startsection(struct ElfWriter *elf, const char *name,
                              int sh_type, int sh_flags) {
  size_t shdr = AppendSection(elf, name, sh_type, sh_flags);
  AppendSymbol(elf, "",
               sh_type != SHT_NULL ? ELF64_ST_INFO(STB_LOCAL, STT_SECTION)
                                   : ELF64_ST_INFO(STB_LOCAL, STT_NOTYPE),
               STV_DEFAULT, 0, 0, shdr, kElfWriterSymSection);
  return shdr;
}

void *elfwriter_reserve(struct ElfWriter *elf, size_t size) {
  size_t need, greed;
  need = elf->wrote + size;
  greed = elf->mapsize;
  if (need > greed) {
    do {
      greed = greed + (greed >> 1);
    } while (need > greed);
    greed = ROUNDUP(greed, FRAMESIZE);
    CHECK_NE(-1, ftruncate(elf->fd, greed));
    CHECK_NE(MAP_FAILED, mmap((char *)elf->map + elf->mapsize,
                              greed - elf->mapsize, PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_FIXED, elf->fd, elf->mapsize));
    elf->mapsize = greed;
  }
  return (char *)elf->map + elf->wrote;
}

void elfwriter_commit(struct ElfWriter *elf, size_t size) {
  elf->wrote += size;
}

void elfwriter_finishsection(struct ElfWriter *elf) {
  size_t section = FinishSection(elf);
  if (elf->relas->j < elf->relas->i) MakeRelaSection(elf, section);
}

/**
 * Appends symbol.
 *
 * This function should be called between elfwriter_startsection() and
 * elfwriter_finishsection(). If that's not possible, then this can be
 * called after elfwriter_open() and then elfwriter_setsection() can be
 * called later to fix-up the section id.
 */
struct ElfWriterSymRef elfwriter_appendsym(struct ElfWriter *elf,
                                           const char *name, int st_info,
                                           int st_other, size_t st_value,
                                           size_t st_size) {
  return AppendSymbol(
      elf, name, st_info, st_other, st_value, st_size, elf->shdrs->i - 1,
      ELF64_ST_BIND(st_info) == STB_LOCAL ? kElfWriterSymLocal
                                          : kElfWriterSymGlobal);
}

void elfwriter_setsection(struct ElfWriter *elf, struct ElfWriterSymRef sym,
                          uint16_t st_shndx) {
  elf->syms[sym.slg]->p[sym.sym].st_shndx = st_shndx;
}

struct ElfWriterSymRef elfwriter_linksym(struct ElfWriter *elf,
                                         const char *name, int st_info,
                                         int st_other) {
  return AppendSymbol(elf, name, st_info, st_other, 0, 0, 0,
                      kElfWriterSymGlobal);
}

void elfwriter_appendrela(struct ElfWriter *elf, uint64_t r_offset,
                          struct ElfWriterSymRef symkey, uint32_t type,
                          int64_t r_addend) {
  CHECK_NE(-1, APPEND(&elf->relas->p, &elf->relas->i, &elf->relas->n,
                      (&(struct ElfWriterRela){.type = type,
                                               .symkey = symkey,
                                               .offset = r_offset,
                                               .addend = r_addend})));
}

uint32_t elfwriter_relatype_abs32(const struct ElfWriter *elf) {
  switch (elf->ehdr->e_machine) {
    case EM_NEXGEN32E:
      return R_X86_64_32;
    case EM_AARCH64:
      return R_AARCH64_ABS32;
    case EM_PPC64:
      return R_PPC64_ADDR32;
    case EM_RISCV:
      return R_RISCV_32;
    case EM_S390:
      return R_390_32;
    default:
      notpossible;
  }
}

uint32_t elfwriter_relatype_pc32(const struct ElfWriter *elf) {
  switch (elf->ehdr->e_machine) {
    case EM_NEXGEN32E:
      return R_X86_64_PC32;
    case EM_AARCH64:
      return R_AARCH64_PREL32;
    case EM_PPC64:
      return R_PPC64_REL32;
    case EM_RISCV:
      return R_RISCV_RELATIVE;
    case EM_S390:
      return R_390_PC32;
    default:
      notpossible;
  }
}
