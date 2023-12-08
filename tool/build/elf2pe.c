/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/rela.h"
#include "libc/elf/struct/shdr.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/serialize.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/dll.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/pedef.internal.h"
#include "libc/nt/struct/imagedatadirectory.internal.h"
#include "libc/nt/struct/imagedosheader.internal.h"
#include "libc/nt/struct/imagefileheader.internal.h"
#include "libc/nt/struct/imageimportdescriptor.internal.h"
#include "libc/nt/struct/imageoptionalheader.internal.h"
#include "libc/nt/struct/imagesectionheader.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdckdint.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/getopt/getopt.internal.h"

// see tool/hello/hello-pe.c for an example program this can link
// make -j8 m=tiny o/tiny/tool/hello/hello-pe.com

#pragma GCC diagnostic ignored "-Wstringop-overflow"

#define VERSION                     \
  "elf2pe v0.1\n"                   \
  "copyright 2023 justine tunney\n" \
  "https://github.com/jart/cosmopolitan\n"

#define MANUAL                           \
  " -o OUTPUT INPUT\n"                   \
  "\n"                                   \
  "DESCRIPTION\n"                        \
  "\n"                                   \
  "  Converts ELF executables to PE\n"   \
  "\n"                                   \
  "FLAGS\n"                              \
  "\n"                                   \
  "  -h         show usage\n"            \
  "  -o OUTPUT  set output path\n"       \
  "  -D PATH    embed dos/bios stub\n"   \
  "  -S SIZE    size of stack commit\n"  \
  "  -R SIZE    size of stack reserve\n" \
  "\n"

#define MAX_ALIGN 65536

#define ALIGN_VIRT(p, a) ROUNDUP(p, (long)(a))
#define ALIGN_FILE(p, a) (char *)ROUNDUP((uintptr_t)(p), (long)(a))

#define FUNC_CONTAINER(e)    DLL_CONTAINER(struct Func, elem, e)
#define LIBRARY_CONTAINER(e) DLL_CONTAINER(struct Library, elem, e)
#define SECTION_CONTAINER(e) DLL_CONTAINER(struct Section, elem, e)
#define SEGMENT_CONTAINER(e) DLL_CONTAINER(struct Segment, elem, e)

struct ImagePointer {
  char *fp;
  int64_t vp;
};

struct Func {
  const char *name;
  struct Dll elem;
  uint64_t *ilt;
  Elf64_Sym *symbol;
};

struct Library {
  const char *name;
  struct Dll *funcs;
  struct Dll elem;
  struct NtImageImportDescriptor *idt;
  uint64_t *ilt;
  size_t iltbytes;
};

struct Section {
  int prot;
  int index;
  char *name;
  Elf64_Shdr *shdr;
  Elf64_Rela *relas;
  Elf64_Xword relacount;
  struct Dll elem;
};

struct Segment {
  int prot;
  char *ptr_new;
  bool hasnobits;
  bool hasprogbits;
  Elf64_Xword align;
  Elf64_Off offset_min;
  Elf64_Off offset_max;
  Elf64_Addr vaddr_min;
  Elf64_Addr vaddr_max;
  Elf64_Addr vaddr_new_min;
  Elf64_Addr vaddr_new_max;
  struct Dll *sections;
  struct Dll elem;
  struct NtImageSectionHeader *pesection;
};

struct Elf {
  union {
    char *map;
    Elf64_Ehdr *ehdr;
  };
  size_t size;
  const char *path;
  char *strtab;
  char *secstrs;
  Elf64_Sym *symtab;
  Elf64_Shdr *symhdr;
  Elf64_Xword align;
  Elf64_Xword symcount;
  struct Dll *imports;
  struct Dll *segments;
  Elf64_Half text_count;
  Elf64_Half rdata_count;
  Elf64_Half data_count;
  Elf64_Half bss_count;
};

static const char *prog;
static const char *outpath;
static const char *stubpath;
static long FLAG_SizeOfStackCommit = 64 * 1024;
static long FLAG_SizeOfStackReserve = 8 * 1024 * 1024;

static wontreturn void Die(const char *thing, const char *reason) {
  tinyprint(2, thing, ": ", reason, "\n", NULL);
  exit(1);
}

static wontreturn void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

static wontreturn void ShowUsage(int rc, int fd) {
  tinyprint(fd, VERSION, "\nUSAGE\n\n  ", prog, MANUAL, NULL);
  exit(rc);
}

static wontreturn void DieOom(void) {
  Die("makepe", "out of memory");
}

static void *Calloc(size_t n) {
  void *p;
  if (!(p = calloc(1, n))) DieOom();
  return p;
}

static void *Realloc(void *p, size_t n) {
  if (!(p = realloc(p, n))) DieOom();
  return p;
}

static struct Func *NewFunc(void) {
  struct Func *s;
  s = Calloc(sizeof(struct Func));
  dll_init(&s->elem);
  return s;
}

static struct Library *NewLibrary(void) {
  struct Library *s;
  s = Calloc(sizeof(struct Library));
  dll_init(&s->elem);
  return s;
}

static struct Section *NewSection(void) {
  struct Section *s;
  s = Calloc(sizeof(struct Section));
  dll_init(&s->elem);
  return s;
}

static struct Segment *NewSegment(void) {
  struct Segment *s;
  s = Calloc(sizeof(struct Segment));
  dll_init(&s->elem);
  return s;
}

static Elf64_Addr RelocateVaddrWithinSegment(struct Elf *elf,
                                             Elf64_Addr vaddr_old,
                                             struct Segment *segment) {
  unassert(segment->vaddr_min <= vaddr_old && vaddr_old < segment->vaddr_max);
  Elf64_Addr vaddr_new =
      vaddr_old + (segment->vaddr_new_min - segment->vaddr_min);
  unassert(segment->vaddr_new_min <= vaddr_new &&
           vaddr_new < segment->vaddr_new_max);
  return vaddr_new;
}

static Elf64_Addr RelocateVaddr(struct Elf *elf, Elf64_Addr vaddr) {
  for (struct Dll *e = dll_first(elf->segments); e;
       e = dll_next(elf->segments, e)) {
    struct Segment *segment = SEGMENT_CONTAINER(e);
    if (segment->vaddr_min <= vaddr && vaddr < segment->vaddr_max) {
      return RelocateVaddrWithinSegment(elf, vaddr, segment);
    }
  }
  return -1;
}

static Elf64_Phdr *GetTlsPhdr(struct Elf *elf) {
  Elf64_Phdr *phdr;
  for (int i = 0; i < elf->ehdr->e_phnum; ++i) {
    if ((phdr = GetElfProgramHeaderAddress(elf->ehdr, elf->size, i)) &&
        phdr->p_type == PT_TLS) {
      return phdr;
    }
  }
  Die(elf->path, "ELF has TLS relocations but no PT_TLS program header");
}

static Elf64_Addr RelocateTlsVaddr(struct Elf *elf, Elf64_Addr vaddr) {
  Elf64_Addr res;
  if ((res = RelocateVaddr(elf, vaddr)) != -1) {
    return res;
  } else {
    Die(elf->path, "ELF PT_TLS program header doesn't overlap with any of "
                   "the loaded segments we're copying");
  }
}

static Elf64_Addr GetTpAddr(struct Elf *elf) {
  unassert(elf->ehdr->e_machine == EM_NEXGEN32E ||
           elf->ehdr->e_machine == EM_S390);
  Elf64_Phdr *p = GetTlsPhdr(elf);
  return RelocateTlsVaddr(
      elf, (p->p_vaddr + p->p_memsz + (p->p_align - 1)) & -p->p_align);
}

static Elf64_Addr GetDtpAddr(struct Elf *elf) {
  unassert(elf->ehdr->e_machine != EM_PPC64 &&
           elf->ehdr->e_machine != EM_RISCV);
  Elf64_Phdr *p = GetTlsPhdr(elf);
  return RelocateTlsVaddr(elf, p->p_vaddr);
}

static void RelocateRela(struct Elf *elf, struct Segment *segment,
                         struct Section *section, Elf64_Rela *rela) {
  Elf64_Addr place_vaddr =
      RelocateVaddrWithinSegment(elf, rela->r_offset, segment);
  Elf64_Addr symbol_vaddr = elf->symtab[ELF64_R_SYM(rela->r_info)].st_value;
  char *place_ptr = segment->ptr_new + (place_vaddr - segment->vaddr_new_min);
  switch (ELF64_R_TYPE(rela->r_info)) {
    case R_X86_64_NONE:    // do nothing
    case R_X86_64_COPY:    // do nothing
    case R_X86_64_SIZE32:  // isn't impacted
    case R_X86_64_SIZE64:  // isn't impacted
      break;
    case R_X86_64_64: {  // S + A
      uint64_t abs64;
      if (ckd_add(&abs64, symbol_vaddr, rela->r_addend)) {
        fprintf(stderr,
                "%s: ELF R_X86_64_64 relocation %lx + %ld overflowed "
                "at %lx in section %s\n",
                elf->path, symbol_vaddr, rela->r_addend, place_vaddr,
                section->name);
        exit(1);
      }
      memcpy(place_ptr, &abs64, sizeof(abs64));
      break;
    }
    case R_X86_64_32: {  // S + A
      uint32_t abs32;
      if (ckd_add(&abs32, symbol_vaddr, rela->r_addend)) {
        fprintf(stderr,
                "%s: ELF R_X86_64_32 relocation %lx + %ld overflowed "
                "at %lx in section %s\n",
                elf->path, symbol_vaddr, rela->r_addend, place_vaddr,
                section->name);
        exit(1);
      }
      memcpy(place_ptr, &abs32, sizeof(abs32));
      break;
    }
    case R_X86_64_32S: {  // S + A
      int32_t abs32s;
      if (ckd_add(&abs32s, symbol_vaddr, rela->r_addend)) {
        fprintf(stderr,
                "%s: ELF R_X86_64_32S relocation %lx + %ld overflowed "
                "at %lx in section %s\n",
                elf->path, symbol_vaddr, rela->r_addend, place_vaddr,
                section->name);
        exit(1);
      }
      memcpy(place_ptr, &abs32s, sizeof(abs32s));
      break;
    }
    case R_X86_64_16: {  // S + A
      uint16_t abs16;
      if (ckd_add(&abs16, symbol_vaddr, rela->r_addend)) {
        fprintf(stderr,
                "%s: ELF R_X86_64_16 relocation %lx + %ld overflowed "
                "at %lx in section %s\n",
                elf->path, symbol_vaddr, rela->r_addend, place_vaddr,
                section->name);
        exit(1);
      }
      memcpy(place_ptr, &abs16, sizeof(abs16));
      break;
    }
    case R_X86_64_8: {  // S + A
      uint8_t abs8;
      if (ckd_add(&abs8, symbol_vaddr, rela->r_addend)) {
        fprintf(stderr,
                "%s: ELF R_X86_64_8 relocation %lx + %ld overflowed "
                "at %lx in section %s\n",
                elf->path, symbol_vaddr, rela->r_addend, place_vaddr,
                section->name);
        exit(1);
      }
      memcpy(place_ptr, &abs8, sizeof(abs8));
      break;
    }
    case R_X86_64_PC64: {  // S + A - P
      int64_t pc64;
      Elf64_Sxword tmp;
      if (ckd_add(&tmp, symbol_vaddr, rela->r_addend) ||
          ckd_sub(&pc64, tmp, place_vaddr)) {
        fprintf(stderr,
                "%s: ELF R_X86_64_PC64 relocation %lx + %ld - %lx overflowed "
                "at %lx in section %s\n",
                elf->path, symbol_vaddr, rela->r_addend, place_vaddr,
                place_vaddr, section->name);
        exit(1);
      }
      memcpy(place_ptr, &pc64, sizeof(pc64));
      break;
    }
    case R_X86_64_PC32:  // S + A - P
    case R_X86_64_PLT32: {
      int32_t pc32;
      Elf64_Sxword tmp;
      if (ckd_add(&tmp, symbol_vaddr, rela->r_addend) ||
          ckd_sub(&pc32, tmp, place_vaddr)) {
        fprintf(stderr,
                "%s: ELF R_X86_64_PC32 relocation %lx + %ld - %lx overflowed "
                "at %lx in section %s\n",
                elf->path, symbol_vaddr, rela->r_addend, place_vaddr,
                place_vaddr, section->name);
        exit(1);
      }
      memcpy(place_ptr, &pc32, sizeof(pc32));
      break;
    }
    case R_X86_64_PC16: {  // S + A - P
      int16_t pc16;
      Elf64_Sxword tmp;
      if (ckd_add(&tmp, symbol_vaddr, rela->r_addend) ||
          ckd_sub(&pc16, tmp, place_vaddr)) {
        fprintf(stderr,
                "%s: ELF R_X86_64_PC16 relocation %lx + %ld - %lx overflowed "
                "at %lx in section %s\n",
                elf->path, symbol_vaddr, rela->r_addend, place_vaddr,
                place_vaddr, section->name);
        exit(1);
      }
      memcpy(place_ptr, &pc16, sizeof(pc16));
      break;
    }
    case R_X86_64_PC8: {  // S + A - P
      int8_t pc8;
      Elf64_Sxword tmp;
      if (ckd_add(&tmp, symbol_vaddr, rela->r_addend) ||
          ckd_sub(&pc8, tmp, place_vaddr)) {
        fprintf(stderr,
                "%s: ELF R_X86_64_PC8 relocation %lx + %ld - %lx overflowed "
                "at %lx in section %s\n",
                elf->path, symbol_vaddr, rela->r_addend, place_vaddr,
                place_vaddr, section->name);
        exit(1);
      }
      memcpy(place_ptr, &pc8, sizeof(pc8));
      break;
    }
    case R_X86_64_DTPOFF32: {  // S + A - T
      int32_t pc32;
      Elf64_Addr dtp;
      Elf64_Sxword tmp;
      dtp = GetDtpAddr(elf);
      if (ckd_add(&tmp, symbol_vaddr, rela->r_addend) ||
          ckd_sub(&pc32, tmp, dtp)) {
        fprintf(
            stderr,
            "%s: ELF R_X86_64_DTPOFF32 relocation %lx + %ld - %lx overflowed "
            "at %lx in section %s\n",
            elf->path, symbol_vaddr, rela->r_addend, dtp, place_vaddr,
            section->name);
        exit(1);
      }
      memcpy(place_ptr, &pc32, sizeof(pc32));
      break;
    }
    case R_X86_64_DTPOFF64: {  // S + A - T
      int64_t pc64;
      Elf64_Addr dtp;
      Elf64_Sxword tmp;
      dtp = GetDtpAddr(elf);
      if (ckd_add(&tmp, symbol_vaddr, rela->r_addend) ||
          ckd_sub(&pc64, tmp, dtp)) {
        fprintf(
            stderr,
            "%s: ELF R_X86_64_DTPOFF64 relocation %lx + %ld - %lx overflowed "
            "at %lx in section %s\n",
            elf->path, symbol_vaddr, rela->r_addend, dtp, place_vaddr,
            section->name);
        exit(1);
      }
      memcpy(place_ptr, &pc64, sizeof(pc64));
      break;
    }
    case R_X86_64_TPOFF32: {  // S + A - T
      int32_t pc32;
      Elf64_Addr tp;
      Elf64_Sxword tmp;
      tp = GetTpAddr(elf);
      if (ckd_add(&tmp, symbol_vaddr, rela->r_addend) ||
          ckd_sub(&pc32, tmp, tp)) {
        fprintf(
            stderr,
            "%s: ELF R_X86_64_TPOFF32 relocation %lx + %ld - %lx overflowed "
            "at %lx in section %s\n",
            elf->path, symbol_vaddr, rela->r_addend, tp, place_vaddr,
            section->name);
        exit(1);
      }
      memcpy(place_ptr, &pc32, sizeof(pc32));
      break;
    }
    case R_X86_64_TPOFF64: {  // S + A - T
      int64_t pc64;
      Elf64_Addr tp;
      Elf64_Sxword tmp;
      tp = GetTpAddr(elf);
      if (ckd_add(&tmp, symbol_vaddr, rela->r_addend) ||
          ckd_sub(&pc64, tmp, tp)) {
        fprintf(
            stderr,
            "%s: ELF R_X86_64_TPOFF64 relocation %lx + %ld - %lx overflowed "
            "at %lx in section %s\n",
            elf->path, symbol_vaddr, rela->r_addend, tp, place_vaddr,
            section->name);
        exit(1);
      }
      memcpy(place_ptr, &pc64, sizeof(pc64));
      break;
    }
    default:
      fprintf(stderr, "%s: don't understand ELF relocation type %d\n",
              elf->path, ELF64_R_TYPE(rela->r_info));
  }
}

static void RelocateSection(struct Elf *elf, struct Segment *segment,
                            struct Section *section) {
  for (Elf64_Xword i = 0; i < section->relacount; ++i) {
    RelocateRela(elf, segment, section, section->relas + i);
  }
}

static void RelocateSegment(struct Elf *elf, struct Segment *segment) {
  unassert(segment->ptr_new);
  unassert(segment->hasprogbits);
  for (struct Dll *e = dll_first(segment->sections); e;
       e = dll_next(segment->sections, e)) {
    struct Section *section = SECTION_CONTAINER(e);
    RelocateSection(elf, segment, section);
  }
}

static void RelocateElf(struct Elf *elf) {
  for (Elf64_Xword i = 0; i < elf->symcount; ++i) {
    if (elf->symtab[i].st_shndx != SHN_ABS &&
        elf->symtab[i].st_shndx != SHN_UNDEF &&
        ELF64_ST_TYPE(elf->symtab[i].st_info) != STT_TLS) {
      elf->symtab[i].st_value = RelocateVaddr(elf, elf->symtab[i].st_value);
    }
  }
  for (struct Dll *e = dll_first(elf->segments); e;
       e = dll_next(elf->segments, e)) {
    struct Segment *segment = SEGMENT_CONTAINER(e);
    if (segment->hasprogbits) {
      RelocateSegment(elf, segment);
    }
  }
}

static struct Elf64_Sym *FindGlobal(struct Elf *elf, const char *name) {
  Elf64_Xword i;
  for (i = elf->symhdr->sh_info; i < elf->symcount; ++i) {
    if (elf->symtab[i].st_name &&
        !strcmp(elf->strtab + elf->symtab[i].st_name, name)) {
      return elf->symtab + i;
    }
  }
  return 0;
}

static int GetRelaSectionIndex(struct Elf *elf, int i) {
  int j;
  Elf64_Shdr *shdr;
  for (j = 0; j < elf->ehdr->e_shnum; ++j) {
    if ((shdr = GetElfSectionHeaderAddress(elf->ehdr, elf->size, j)) &&
        shdr->sh_type == SHT_RELA && shdr->sh_info == i) {
      return j;
    }
  }
  return -1;
}

static struct Section *LoadSection(struct Elf *elf, int index,
                                   Elf64_Shdr *shdr) {
  int rela_index;
  Elf64_Shdr *rela_shdr;
  struct Section *section;
  section = NewSection();
  section->index = index;
  section->shdr = shdr;
  section->prot = PROT_READ;
  section->name = elf->secstrs + shdr->sh_name;
  if (shdr->sh_flags & SHF_WRITE) {
    section->prot |= PROT_WRITE;
  }
  if (shdr->sh_flags & SHF_EXECINSTR) {
    section->prot |= PROT_EXEC;
  }
  if ((rela_index = GetRelaSectionIndex(elf, index)) != -1 &&
      (rela_shdr =
           GetElfSectionHeaderAddress(elf->ehdr, elf->size, rela_index)) &&
      (section->relas =
           GetElfSectionAddress(elf->ehdr, elf->size, rela_shdr))) {
    section->relacount = rela_shdr->sh_size / sizeof(Elf64_Rela);
  }
  return section;
}

static void LoadSectionsIntoSegments(struct Elf *elf) {
  int i;
  Elf64_Shdr *shdr;
  bool hasdataseg = false;
  struct Segment *segment = 0;
  for (i = 0; i < elf->ehdr->e_shnum; ++i) {
    if ((shdr = GetElfSectionHeaderAddress(elf->ehdr, elf->size, i)) &&
        (shdr->sh_type == SHT_PROGBITS || shdr->sh_type == SHT_NOBITS) &&
        !((shdr->sh_flags & SHF_TLS) && shdr->sh_type == SHT_NOBITS) &&
        (shdr->sh_flags & SHF_ALLOC)) {
      struct Section *section;
      section = LoadSection(elf, i, shdr);
      if (segment && (segment->prot != section->prot ||
                      (segment->hasnobits && shdr->sh_type == SHT_PROGBITS))) {
        dll_make_last(&elf->segments, &segment->elem);
        segment = 0;
      }
      if (!segment) {
        segment = NewSegment();
        segment->prot = section->prot;
        segment->vaddr_min = section->shdr->sh_addr;
        if (shdr->sh_type == SHT_PROGBITS)
          segment->offset_min = section->shdr->sh_offset;
        hasdataseg |= segment->prot == (PROT_READ | PROT_WRITE);
      }
      segment->hasnobits |= shdr->sh_type == SHT_NOBITS;
      segment->hasprogbits |= shdr->sh_type == SHT_PROGBITS;
      segment->vaddr_max = section->shdr->sh_addr + section->shdr->sh_size;
      if (shdr->sh_type == SHT_PROGBITS)
        segment->offset_max = section->shdr->sh_offset + section->shdr->sh_size;
      segment->align = MAX(segment->align, section->shdr->sh_addralign);
      elf->align = MAX(elf->align, segment->align);
      dll_make_last(&segment->sections, &section->elem);
    }
  }
  if (segment) {
    dll_make_last(&elf->segments, &segment->elem);
  }
  if (elf->imports && !hasdataseg) {
    // if the program we're linking is really tiny and it doesn't have
    // either a .data or .bss section but it does import function from
    // libraries, then create a synthetic .data segment for the pe iat
    segment = NewSegment();
    segment->align = 8;
    segment->hasprogbits = true;
    segment->prot = PROT_READ | PROT_WRITE;
    dll_make_last(&elf->segments, &segment->elem);
  }
}

static bool ParseDllImportSymbol(const char *symbol_name,
                                 const char **out_dll_name,
                                 const char **out_func_name) {
  size_t n;
  char *dll_name;
  const char *dolla;
  if (!startswith(symbol_name, "dll$")) return false;
  symbol_name += 4;
  dolla = strchr(symbol_name, '$');
  if (!dolla) return false;
  n = dolla - symbol_name;
  dll_name = memcpy(Calloc(n + 1), symbol_name, n);
  *out_dll_name = dll_name;
  *out_func_name = dolla + 1;
  return true;
}

static struct Library *FindImport(struct Elf *elf, const char *name) {
  struct Dll *e;
  for (e = dll_first(elf->imports); e; e = dll_next(elf->imports, e)) {
    struct Library *library = LIBRARY_CONTAINER(e);
    if (!strcmp(name, library->name)) {
      return library;
    }
  }
  return 0;
}

static void LoadDllImports(struct Elf *elf) {
  Elf64_Xword i;
  struct Func *func;
  const char *dll_name;
  const char *func_name;
  struct Library *library;
  for (i = 0; i < elf->symcount; ++i) {
    Elf64_Sym *symbol = elf->symtab + i;
    if (symbol->st_name && ParseDllImportSymbol(elf->strtab + symbol->st_name,
                                                &dll_name, &func_name)) {
      if (symbol->st_value || symbol->st_shndx != SHN_UNDEF)
        Die(elf->path, "ELF executable declared a dll: import symbol without "
                       "annotating it weak extern");
      if (!(library = FindImport(elf, dll_name))) {
        library = NewLibrary();
        library->name = dll_name;
        dll_make_last(&elf->imports, &library->elem);
      }
      func = NewFunc();
      func->name = func_name;
      func->symbol = symbol;
      dll_make_last(&library->funcs, &func->elem);
    }
  }
}

static struct Elf *OpenElf(const char *path) {
  int fd;
  struct Elf *elf;
  elf = Calloc(sizeof(*elf));
  elf->path = path;
  if ((fd = open(path, O_RDONLY)) == -1) DieSys(path);
  if ((elf->size = lseek(fd, 0, SEEK_END)) == -1) DieSys(path);
  elf->map = mmap(0, elf->size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (elf->map == MAP_FAILED) DieSys(path);
  if (!IsElf64Binary(elf->ehdr, elf->size)) Die(path, "not an elf64 binary");
  elf->symhdr =
      GetElfSymbolTable(elf->ehdr, elf->size, SHT_SYMTAB, &elf->symcount);
  elf->symtab = GetElfSectionAddress(elf->ehdr, elf->size, elf->symhdr);
  if (!elf->symtab) Die(path, "elf doesn't have symbol table");
  elf->strtab = GetElfStringTable(elf->ehdr, elf->size, ".strtab");
  if (!elf->strtab) Die(path, "elf doesn't have string table");
  elf->secstrs = GetElfSectionNameStringTable(elf->ehdr, elf->size);
  if (!elf->strtab) Die(path, "elf doesn't have section string table");
  LoadDllImports(elf);
  LoadSectionsIntoSegments(elf);
  close(fd);
  return elf;
}

static void PrintElf(struct Elf *elf) {
  struct Dll *e, *g;
  printf("\n");
  printf("%s\n", elf->path);
  printf("sections\n");
  for (e = dll_first(elf->segments); e; e = dll_next(elf->segments, e)) {
    struct Segment *segment = SEGMENT_CONTAINER(e);
    for (g = dll_first(segment->sections); g;
         g = dll_next(segment->sections, g)) {
      struct Section *section = SECTION_CONTAINER(g);
      printf("\t%s\n", section->name);
    }
    printf("\t\talign     = %ld\n", segment->align);
    printf("\t\tvaddr_old = %lx\n", segment->vaddr_min);
    printf("\t\tvaddr_new = %lx\n", segment->vaddr_new_min);
    printf("\t\tprot      = %s\n", DescribeProtFlags(segment->prot));
  }
  printf("imports\n");
  for (e = dll_first(elf->imports); e; e = dll_next(elf->imports, e)) {
    struct Library *library = LIBRARY_CONTAINER(e);
    printf("\t%s\n", library->name);
    for (g = dll_first(library->funcs); g; g = dll_next(library->funcs, g)) {
      struct Func *func = FUNC_CONTAINER(g);
      printf("\t\t%s\n", func->name);
    }
  }
}

static void PickPeSectionName(char *p, struct Elf *elf,
                              struct Segment *segment) {
  int n;
  switch (segment->prot) {
    case PROT_EXEC:
    case PROT_EXEC | PROT_READ:
    case PROT_EXEC | PROT_READ | PROT_WRITE:
      p = stpcpy(p, ".text");
      n = ++elf->text_count;
      break;
    case PROT_READ:
      p = stpcpy(p, ".rdata");
      n = ++elf->rdata_count;
      break;
    case PROT_READ | PROT_WRITE:
      if (segment->hasprogbits) {
        p = stpcpy(p, ".data");
        n = ++elf->data_count;
      } else {
        p = stpcpy(p, ".bss");
        n = ++elf->bss_count;
      }
      break;
    default:
      notpossible;
  }
  if (n > 1) {
    FormatInt32(p, n);
  }
}

static uint32_t GetPeSectionCharacteristics(struct Segment *s) {
  uint32_t x = 0;
  if (s->prot & PROT_EXEC) {
    x |= kNtPeSectionCntCode | kNtPeSectionMemExecute;
  } else if (s->hasprogbits) {
    x |= kNtPeSectionCntInitializedData;
  }
  if (s->prot & PROT_READ) {
    x |= kNtPeSectionMemRead;
  }
  if (s->prot & PROT_WRITE) {
    x |= kNtPeSectionMemWrite;
  }
  if (s->hasnobits) {
    x |= kNtPeSectionCntUninitializedData;
  }
  return x;
}

// converts static elf executable to portable executable
//
// the trick to generating a portable executable is to maintain the file
// pointer and virtual address pointers separately, as the image is made
//
// during this process, we're going to be inserting and removing padding
// to both the file layout and virtual address space, that weren't there
// originally in the elf image that ld linked. in order for this to work
// the executable needs to be linked in `ld -q` mode, since it'll retain
// the .rela sections we'll need later to fixup the binary.
static struct ImagePointer GeneratePe(struct Elf *elf, char *fp, int64_t vp) {

  Elf64_Sym *entry;
  if (!(entry = FindGlobal(elf, "__win32_start")) &&
      !(entry = FindGlobal(elf, "WinMain")))
    Die(elf->path, "ELF didn't define global `__win32_start` PE entrypoint or "
                   "alternatively a `WinMain` function");

  if (elf->align > MAX_ALIGN)
    Die(elf->path, "ELF specified an alignment greater than 64k which isn't "
                   "supported by the PE format");

  vp = ALIGN_VIRT(vp, 65536);
  struct NtImageDosHeader *mzhdr;
  mzhdr = (struct NtImageDosHeader *)fp;
  fp += sizeof(struct NtImageDosHeader);
  memcpy(mzhdr, "MZ", 2);

  // embed the ms-dos stub and/or bios bootloader
  if (stubpath) {
    int fd = open(stubpath, O_RDONLY);
    if (fd == -1) DieSys(stubpath);
    for (;;) {
      ssize_t got = read(fd, fp, 512);
      if (got == -1) DieSys(stubpath);
      if (!got) break;
      fp += got;
    }
    if (close(fd)) DieSys(stubpath);
  }

  // output portable executable magic
  fp = ALIGN_FILE(fp, 8);
  mzhdr->e_lfanew = fp - (char *)mzhdr;
  fp = WRITE32LE(fp, 'P' | 'E' << 8);

  // output coff header
  struct NtImageFileHeader *filehdr;
  filehdr = (struct NtImageFileHeader *)fp;
  fp += sizeof(struct NtImageFileHeader);
  filehdr->Machine = kNtImageFileMachineNexgen32e;
  filehdr->TimeDateStamp = 1690072024;
  filehdr->Characteristics =
      kNtPeFileExecutableImage | kNtImageFileLargeAddressAware |
      kNtPeFileRelocsStripped | kNtPeFileLineNumsStripped |
      kNtPeFileLocalSymsStripped;

  // output "optional" header
  struct NtImageOptionalHeader *opthdr;
  opthdr = (struct NtImageOptionalHeader *)fp;
  fp += sizeof(struct NtImageOptionalHeader);
  opthdr->Magic = kNtPe64bit;
  opthdr->MajorLinkerVersion = 14;
  opthdr->MinorLinkerVersion = 35;
  opthdr->ImageBase = vp;
  opthdr->FileAlignment = 512;
  opthdr->SectionAlignment = MAX(4096, elf->align);
  opthdr->MajorOperatingSystemVersion = 6;
  opthdr->MajorSubsystemVersion = 6;
  opthdr->Subsystem = kNtImageSubsystemWindowsCui;
  opthdr->DllCharacteristics = kNtImageDllcharacteristicsNxCompat |
                               kNtImageDllcharacteristicsHighEntropyVa;
  opthdr->SizeOfStackReserve =
      MAX(FLAG_SizeOfStackReserve, FLAG_SizeOfStackCommit);
  opthdr->SizeOfStackCommit = FLAG_SizeOfStackCommit;

  // output data directory entries
  if (elf->imports) {
    opthdr->NumberOfRvaAndSizes = 2;
    fp += opthdr->NumberOfRvaAndSizes * sizeof(struct NtImageDataDirectory);
  }

  // finish optional header
  filehdr->SizeOfOptionalHeader = fp - (char *)opthdr;

  // output section headers
  struct NtImageSectionHeader *sections;
  sections = (struct NtImageSectionHeader *)fp;
  struct NtImageSectionHeader *idata_section;
  idata_section = (struct NtImageSectionHeader *)fp;
  if (elf->imports) {
    fp += sizeof(struct NtImageSectionHeader);
    ++filehdr->NumberOfSections;
  }
  for (struct Dll *e = dll_first(elf->segments); e;
       e = dll_next(elf->segments, e)) {
    struct Segment *segment = SEGMENT_CONTAINER(e);
    segment->pesection = (struct NtImageSectionHeader *)fp;
    fp += sizeof(struct NtImageSectionHeader);
    ++filehdr->NumberOfSections;
  }

  // finish headers
  fp = ALIGN_FILE(fp, opthdr->FileAlignment);
  opthdr->SizeOfHeaders = fp - (char *)mzhdr;
  vp += opthdr->SizeOfHeaders;

  // output .idata section
  if (elf->imports) {
    vp = ALIGN_VIRT(vp, opthdr->SectionAlignment);
    char *fbegin = fp;
    int64_t vbegin = vp;
    idata_section->VirtualAddress = vp - opthdr->ImageBase;
    idata_section->PointerToRawData = fbegin - (char *)mzhdr;
    //////////////////////////////////////////////////////////////////////
    strcpy(idata_section->Name, ".idata");
    idata_section->Characteristics =
        kNtPeSectionMemRead | kNtPeSectionCntInitializedData;
    // output import descriptor for each dll
    for (struct Dll *e = dll_first(elf->imports); e;
         e = dll_next(elf->imports, e)) {
      struct Library *library = LIBRARY_CONTAINER(e);
      library->idt = (struct NtImageImportDescriptor *)fp;
      fp += sizeof(struct NtImageImportDescriptor);
      vp += sizeof(struct NtImageImportDescriptor);
    }
    fp += sizeof(struct NtImageImportDescriptor);
    vp += sizeof(struct NtImageImportDescriptor);
    opthdr->DataDirectory[kNtImageDirectoryEntryImport].VirtualAddress =
        idata_section->VirtualAddress;
    opthdr->DataDirectory[kNtImageDirectoryEntryImport].Size = vp - vbegin;
    // output import lookup table for each dll
    for (struct Dll *e = dll_first(elf->imports); e;
         e = dll_next(elf->imports, e)) {
      struct Library *library = LIBRARY_CONTAINER(e);
      library->idt->ImportLookupTable = vp - opthdr->ImageBase;
      library->ilt = (uint64_t *)fp;
      for (struct Dll *g = dll_first(library->funcs); g;
           g = dll_next(library->funcs, g)) {
        struct Func *func = FUNC_CONTAINER(g);
        func->ilt = (uint64_t *)fp;
        fp += sizeof(uint64_t);
        vp += sizeof(uint64_t);
      }
      fp += sizeof(uint64_t);
      vp += sizeof(uint64_t);
      library->iltbytes = fp - (char *)library->ilt;
    }
    // output the hint name table
    for (struct Dll *e = dll_first(elf->imports); e;
         e = dll_next(elf->imports, e)) {
      struct Library *library = LIBRARY_CONTAINER(e);
      for (struct Dll *g = dll_first(library->funcs); g;
           g = dll_next(library->funcs, g)) {
        struct Func *func = FUNC_CONTAINER(g);
        *func->ilt = vp - opthdr->ImageBase;
        fp += sizeof(uint16_t);  // hint
        vp += sizeof(uint16_t);
        size_t n = strlen(func->name);
        fp = mempcpy(fp, func->name, n + 1);
        vp += n + 1;
        fp = ALIGN_FILE(fp, 2);
        vp = ALIGN_VIRT(vp, 2);
      }
    }
    // output the dll names
    for (struct Dll *e = dll_first(elf->imports); e;
         e = dll_next(elf->imports, e)) {
      struct Library *library = LIBRARY_CONTAINER(e);
      size_t n = strlen(library->name);
      library->idt->DllNameRva = vp - opthdr->ImageBase;
      fp = mempcpy(fp, library->name, n + 1);
      vp += n + 1;
    }
    //////////////////////////////////////////////////////////////////////
    idata_section->Misc.VirtualSize = vp - vbegin;
    fp = ALIGN_FILE(fp, opthdr->FileAlignment);
    idata_section->SizeOfRawData = fp - fbegin;
  }

  // output elf segment sections
  for (struct Dll *e = dll_first(elf->segments); e;
       e = dll_next(elf->segments, e)) {
    struct Segment *segment = SEGMENT_CONTAINER(e);
    vp = ALIGN_VIRT(vp, opthdr->SectionAlignment);
    char *fbegin = fp;
    int64_t vbegin = vp;
    segment->pesection->VirtualAddress = vp - opthdr->ImageBase;
    segment->pesection->PointerToRawData = fbegin - (char *)mzhdr;
    //////////////////////////////////////////////////////////////////////
    if (segment->prot == (PROT_READ | PROT_WRITE)) {
      // sneak InputAddressTable into .data or .bss section
      for (struct Dll *e = dll_first(elf->imports); e;
           e = dll_next(elf->imports, e)) {
        struct Library *library = LIBRARY_CONTAINER(e);
        library->idt->ImportAddressTable = vp - opthdr->ImageBase;
        fp = mempcpy(fp, library->ilt, library->iltbytes);
        segment->hasprogbits = true;
        for (struct Dll *g = dll_first(library->funcs); g;
             g = dll_next(library->funcs, g)) {
          struct Func *func = FUNC_CONTAINER(g);
          func->symbol->st_value = vp;
          vp += 8;
        }
        vp += 8;
      }
      fp = ALIGN_FILE(fp, segment->align);
      vp = ALIGN_VIRT(vp, segment->align);
    }
    PickPeSectionName(segment->pesection->Name, elf, segment);
    segment->vaddr_new_min = vp;
    if (segment->vaddr_min <= entry->st_value &&
        entry->st_value < segment->vaddr_max) {
      opthdr->AddressOfEntryPoint =
          vp + (entry->st_value - segment->vaddr_min) - opthdr->ImageBase;
    }
    if (segment->hasprogbits) {
      segment->ptr_new = fp;
      fp = mempcpy(fp, elf->map + segment->offset_min,
                   segment->offset_max - segment->offset_min);
    }
    vp += segment->vaddr_max - segment->vaddr_min;
    segment->vaddr_new_max = vp;
    segment->pesection->Characteristics = GetPeSectionCharacteristics(segment);
    //////////////////////////////////////////////////////////////////////
    segment->pesection->Misc.VirtualSize = vp - vbegin;
    fp = ALIGN_FILE(fp, opthdr->FileAlignment);
    segment->pesection->SizeOfRawData = fp - fbegin;
  }

  // compute relocations
  RelocateElf(elf);

  // compute informative sizes
  // the windows executive ignores these fields, but they can't hurt.
  for (int i = 0; i < filehdr->NumberOfSections; ++i) {
    if (sections[i].Characteristics & kNtPeSectionCntCode) {
      opthdr->SizeOfCode += sections[i].SizeOfRawData;
      if (!opthdr->BaseOfCode) {
        opthdr->BaseOfCode = sections[i].VirtualAddress;
      }
    }
    if (sections[i].Characteristics & kNtPeSectionCntInitializedData) {
      opthdr->SizeOfInitializedData += sections[i].SizeOfRawData;
    }
    if ((sections[i].Characteristics & kNtPeSectionCntUninitializedData) &&
        sections[i].Misc.VirtualSize > sections[i].SizeOfRawData) {
      opthdr->SizeOfUninitializedData +=
          sections[i].Misc.VirtualSize - sections[i].SizeOfRawData;
    }
  }

  // finish image
  vp = ALIGN_VIRT(vp, opthdr->SectionAlignment);
  opthdr->SizeOfImage = vp - opthdr->ImageBase;
  return (struct ImagePointer){fp, vp};
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "ho:D:R:S:")) != -1) {
    switch (opt) {
      case 'o':
        outpath = optarg;
        break;
      case 'D':
        stubpath = optarg;
        break;
      case 'S':
        FLAG_SizeOfStackCommit = sizetol(optarg, 1024);
        break;
      case 'R':
        FLAG_SizeOfStackReserve = sizetol(optarg, 1024);
        break;
      case 'h':
        ShowUsage(0, 1);
      default:
        ShowUsage(1, 2);
    }
  }
  if (!outpath) {
    Die(prog, "need output path");
  }
  if (optind == argc) {
    Die(prog, "missing input argument");
  }
}

static void Pwrite(int fd, const void *data, size_t size, uint64_t offset) {
  ssize_t rc;
  const char *p, *e;
  for (p = data, e = p + size; p < e; p += (size_t)rc, offset += (size_t)rc) {
    if ((rc = pwrite(fd, p, e - p, offset)) == -1) {
      DieSys(outpath);
    }
  }
}

int main(int argc, char *argv[]) {
#ifdef MODE_DBG
  ShowCrashReports();
#endif
  // get program name
  prog = argv[0];
  if (!prog) prog = "elf2pe";
  // process flags
  GetOpts(argc, argv);
  // translate executable
  struct Elf *elf = OpenElf(argv[optind]);
  char *buf = memalign(MAX_ALIGN, 134217728);
  struct ImagePointer ip = GeneratePe(elf, buf, 0x00400000);
  if (creat(outpath, 0755) == -1) DieSys(elf->path);
  Pwrite(3, buf, ip.fp - buf, 0);
  if (close(3)) DieSys(elf->path);
  // PrintElf(elf);
}
