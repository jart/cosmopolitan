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
#include "libc/calls/calls.h"
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/intrin/kprintf.h"
#include "libc/macho.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/getopt/getopt.internal.h"

#define VERSION                     \
  "objbincopy v1.0\n"               \
  "copyright 2023 justine tunney\n" \
  "https://github.com/jart/cosmopolitan\n"

#define MANUAL                                                   \
  " -o OUTPUT INPUT\n"                                           \
  "\n"                                                           \
  "DESCRIPTION\n"                                                \
  "\n"                                                           \
  "  Fast `objcopy -SO binary` that doesn't insert bloat.\n"     \
  "\n"                                                           \
  "  This program is for times where the unix linker is being\n" \
  "  used to create executables, that define their own custom\n" \
  "  executable headers. The ld program outputs such programs\n" \
  "  as an executable wrapped inside an executable.  Normally\n" \
  "  the only way to get it out is using `objcopy -SO binary`\n" \
  "  except that it has the undesirable impact of adding lots\n" \
  "  of bloat to the output file, in order to make its layout\n" \
  "  the same as the virtual memory layout. That's useful for\n" \
  "  things like naive firmware loaders but isnt a great idea\n" \
  "  when our goal is to generate files like ELF and PE which\n" \
  "  support loading segments, from overlapping file regions.\n" \
  "  Therefore, this program performs a naive objcopy of your\n" \
  "  ELF PT_LOAD segments without considering virtual layout.\n" \
  "\n"                                                           \
  "FLAGS\n"                                                      \
  "\n"                                                           \
  "  -h         show usage\n"                                    \
  "  -o OUTPUT  set output path\n"                               \
  "  -m         create Mach-O executable\n"                      \
  "  -f         coerce EI_OSABI to FreeBSD\n"                    \
  "\n"                                                           \
  "ARGUMENTS\n"                                                  \
  "\n"                                                           \
  "  OUTPUT     where to save the unwrapped executable\n"        \
  "  INPUT      is an elf executable made by the unix linker\n"  \
  "\n"

#ifdef MODE_DBG
#define DEBUG(...) kprintf("DEBUG: " __VA_ARGS__)
#else
#define DEBUG(...) (void)0
#endif

#define IsStaticStringEqual(buf, str) \
  (strnlen(buf, sizeof(buf)) == strlen(str) && !memcmp(buf, str, strlen(str)))

static int outfd;
static bool want_macho;
static const char *prog;
static bool want_freebsd;
static const char *outpath;

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

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "hmfo:")) != -1) {
    switch (opt) {
      case 'o':
        outpath = optarg;
        break;
      case 'f':
        want_freebsd = true;
        break;
      case 'm':
        want_macho = true;
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

static int PhdrFlagsToProt(Elf64_Word flags) {
  int prot = PROT_NONE;
  if (flags & PF_R) prot |= PROT_READ;
  if (flags & PF_W) prot |= PROT_WRITE;
  if (flags & PF_X) prot |= PROT_EXEC;
  return prot;
}

static void Write(const void *data, size_t size) {
  ssize_t rc;
  const char *p, *e;
  for (p = data, e = p + size; p < e; p += (size_t)rc) {
    if ((rc = write(outfd, p, e - p)) == -1) {
      DieSys(outpath);
    }
  }
}

// apple imposes very strict requirements which forbid creativity to the
// greatest possible extent. this routine is designed to help us know if
// something we've built won't be accepted by the xnu kernel.
static void ValidateMachoSection(const char *inpath,         //
                                 Elf64_Ehdr *elf,            //
                                 Elf64_Shdr *shdr,           //
                                 struct MachoHeader *macho,  //
                                 Elf64_Off filesize) {
  int i;
  char *end;
  bool found_uuid;
  bool found_segment;
  uint64_t lastvaddr;
  uint64_t lastoffset;
  bool found_pagezero;
  bool found_unixthread;
  struct MachoLoadCommand *cmd;
  if (!shdr) return;
  if (elf->e_machine != EM_NEXGEN32E) {
    Die(inpath, ".macho section only supported for ELF x86_64");
  }
  if (!macho) Die(inpath, "corrupted .macho section content");
  if (shdr->sh_size < sizeof(struct MachoHeader)) {
    Die(inpath, ".macho section too small for mach-o header");
  }
  if (macho->magic != 0xFEEDFACE + 1) {
    Die(inpath, ".macho header magic wasn't 0xFEEDFACE+1");
  }
  if (macho->arch != MAC_CPU_NEXGEN32E) {
    Die(inpath, "mach-o arch wasn't MAC_CPU_NEXGEN32E");
  }
  if (shdr->sh_size != sizeof(struct MachoHeader) + macho->loadsize) {
    Die(inpath, ".macho section size not equal to sizeof(header) + loadsize");
  }
  lastvaddr = 0;
  lastoffset = 0;
  found_uuid = false;
  found_segment = false;
  found_pagezero = false;
  found_unixthread = false;
  end = (char *)(macho + 1) + macho->loadsize;
  cmd = (struct MachoLoadCommand *)(macho + 1);
  for (i = 0; i < macho->loadcount; ++i) {
    if ((char *)cmd + sizeof(struct MachoLoadCommand *) > end ||
        (char *)cmd + cmd->size > end) {
      Die(inpath, "mach-o load commands overflowed loadsize");
    }
    if (cmd->command == MAC_LC_SEGMENT_64) {
      size_t namelen;
      struct MachoLoadSegment *loadseg;
      loadseg = (struct MachoLoadSegment *)cmd;
      if (loadseg->sectioncount) {
        Die(inpath, "don't bother with mach-o sections");
      }
      namelen = strnlen(loadseg->name, sizeof(loadseg->name));
      if (!loadseg->name[0]) {
        Die(inpath, "mach-o load segment missing name");
      }
      if (filesize || (loadseg->vaddr && loadseg->memsz)) {
        if (loadseg->vaddr < lastvaddr) {
          Die(inpath,
              "the virtual memory regions defined by mach-o load segment "
              "commands aren't allowed to overlap and must be specified "
              "monotonically");
        }
        if (loadseg->vaddr + loadseg->memsz < loadseg->vaddr) {
          Die(inpath, "mach-o segment memsz overflows");
        }
        if (loadseg->filesz > loadseg->memsz) {
          Die(inpath, "mach-o segment filesz exceeds memsz");
        }
        lastvaddr = loadseg->vaddr + loadseg->memsz;
        if (loadseg->vaddr & 4095) {
          Die(inpath, "mach-o segment vaddr must be page aligned");
        }
      }
      if (filesize) {
        if (loadseg->offset < lastoffset) {
          Die(inpath,
              "the file segments defined by mach-o load segment commands "
              "aren't allowed to overlap and must be specified monotonically");
        }
        if (loadseg->filesz > filesize) {
          Die(inpath, "mach-o segment filesz exceeds file size");
        }
        if (loadseg->offset + loadseg->filesz < loadseg->offset) {
          Die(inpath, "mach-o segment offset + filesz overflows");
        }
        if (loadseg->offset + loadseg->filesz > filesize) {
          Die(inpath, "mach-o segment overlaps end of file");
        }
        lastoffset = loadseg->offset + loadseg->filesz;
      }
      if (namelen == strlen("__PAGEZERO") &&
          !memcmp(loadseg->name, "__PAGEZERO", namelen)) {
        found_pagezero = true;
        if (i != 0) {
          Die(inpath, "mach-o __PAGEZERO must be first load command");
        }
      } else {
        if (!found_segment) {
          found_segment = true;
          if (loadseg->offset) {
            Die(inpath, "the first mach-o load segment (that isn't page zero) "
                        "must begin loading the executable from offset zero");
          }
        }
      }
    } else if (cmd->command == MAC_LC_UUID) {
      uint64_t *uuid;
      found_uuid = true;
      if (cmd->size != sizeof(*cmd) + 16) {
        Die(inpath, "MAC_LC_UUID size wrong");
      }
      uuid = (uint64_t *)(cmd + 1);
      if (!uuid[0] && !uuid[1]) {
        uuid[0] = _rand64();
        uuid[1] = _rand64();
      }
    } else if (cmd->command == MAC_LC_UNIXTHREAD) {
      uint64_t *registers;
      struct MachoLoadThreadCommand *thread;
      if (cmd->size != sizeof(*thread) + 21 * 8) {
        Die(inpath, "MAC_LC_UNIXTHREAD size should be 4+4+4+4+21*8");
      }
      thread = (struct MachoLoadThreadCommand *)cmd;
      if (thread->flavor != MAC_THREAD_NEXGEN32E) {
        Die(inpath, "MAC_LC_UNIXTHREAD flavor should be MAC_THREAD_NEXGEN32E");
      }
      if (thread->count != 21 * 8 / 4) {
        Die(inpath, "MAC_LC_UNIXTHREAD count should be 21*8/4");
      }
      registers = (uint64_t *)(thread + 1);
      if (!registers[16]) {
        Die(inpath, "MAC_LC_UNIXTHREAD doesn't specify RIP register");
      }
      found_unixthread = true;
    } else {
      Die(inpath, "unsupported mach-o load command");
    }
    cmd = (struct MachoLoadCommand *)((char *)cmd + cmd->size);
  }
  if (!found_uuid) {
    Die(inpath, "mach-o missing MAC_LC_UUID");
  }
  if (!found_unixthread) {
    Die(inpath, "mach-o missing MAC_LC_UNIXTHREAD");
  }
  if (!found_pagezero) {
    Die(inpath, "mach-o missing __PAGEZERO load segment command");
  }
  if ((char *)cmd != end) {
    Die(inpath, "mach-o loadsize greater than load commands");
  }
}

static struct MachoLoadSegment *GetNextMachoLoadSegment(
    struct MachoLoadCommand **load, int *count) {
  struct MachoLoadCommand *cmd;
  while (*count) {
    --*count;
    cmd = *load;
    *load = (struct MachoLoadCommand *)((char *)cmd + cmd->size);
    if (cmd->command == MAC_LC_SEGMENT_64) {
      struct MachoLoadSegment *loadseg;
      loadseg = (struct MachoLoadSegment *)cmd;
      if (!IsStaticStringEqual(loadseg->name, "__PAGEZERO")) {
        return loadseg;
      }
    }
  }
  return 0;
}

static void HandleElf(const char *inpath, Elf64_Ehdr *elf, size_t esize) {
  char *secstrs;
  int i, loadcount;
  Elf64_Off maxoff;
  Elf64_Phdr *phdr;
  Elf64_Shdr *macho_shdr;
  struct MachoHeader *macho;
  struct MachoLoadCommand *loadcommand;
  struct MachoLoadSegment *loadsegment;
  if (elf->e_type != ET_EXEC && elf->e_type != ET_DYN) {
    Die(inpath, "elf binary isn't an executable");
  }
  if (!(secstrs = GetElfSectionNameStringTable(elf, esize))) {
    Die(inpath, "elf section name string table not found");
  }
  macho_shdr = FindElfSectionByName(elf, esize, secstrs, ".macho");
  macho = GetElfSectionAddress(elf, esize, macho_shdr);
  // ValidateMachoSection(inpath, elf, macho_shdr, macho, 0);
  loadcommand = (struct MachoLoadCommand *)(macho + 1);
  loadcount = macho->loadcount;
  if (want_freebsd) {
    elf->e_ident[EI_OSABI] = ELFOSABI_FREEBSD;
  }
  elf->e_shoff = 0;
  elf->e_shnum = 0;
  elf->e_shstrndx = 0;
  elf->e_shentsize = 0;
  for (maxoff = i = 0; i < elf->e_phnum; ++i) {
    phdr = GetElfProgramHeaderAddress(elf, esize, i);
    if (!phdr) Die(inpath, "corrupted elf header");
    if (phdr->p_type == PT_INTERP) Die(inpath, "PT_INTERP isn't supported");
    if (phdr->p_type == PT_DYNAMIC) Die(inpath, "PT_DYNAMIC isn't supported");
    if (!phdr->p_filesz) continue;
    maxoff = MAX(maxoff, phdr->p_offset + phdr->p_filesz);
    if (macho && phdr->p_type == PT_LOAD) {
      if (!(loadsegment = GetNextMachoLoadSegment(&loadcommand, &loadcount))) {
        Die(inpath, "there must exist a MAC_LC_SEGMENT_64 for every PT_LOAD "
                    "when the .macho section is defined");
      }
      loadsegment->vaddr = phdr->p_vaddr;
      loadsegment->memsz = phdr->p_memsz;
      loadsegment->offset = phdr->p_offset;
      loadsegment->filesz = phdr->p_filesz;
      loadsegment->initprot |= PhdrFlagsToProt(phdr->p_flags);
      if (loadsegment->initprot == PROT_EXEC) {
        loadsegment->initprot |= PROT_READ;
      }
      loadsegment->maxprot |= loadsegment->initprot;
    }
  }
  // ValidateMachoSection(inpath, elf, macho_shdr, macho, maxoff);
  Write((char *)elf, maxoff);
  if (want_macho) {
    if (!macho_shdr || !macho) {
      Die(inpath, "requested Mach-O output but .macho section not found");
    }
    if (lseek(outfd, 0, SEEK_SET)) {
      DieSys(inpath);
    }
    // TODO(jart): Add a check that ensures we aren't overwriting
    //             anything except ELF headers and the old machoo
    Write((char *)elf + macho_shdr->sh_offset, macho_shdr->sh_size);
  }
}

static void HandleInput(const char *inpath) {
  int infd;
  void *map;
  ssize_t size;
  if ((infd = open(inpath, O_RDONLY)) == -1) {
    DieSys(inpath);
  }
  if ((size = lseek(infd, 0, SEEK_END)) == -1) {
    DieSys(inpath);
  }
  if (size) {
    if ((map = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, infd, 0)) ==
        MAP_FAILED) {
      DieSys(inpath);
    }
    if (IsElf64Binary(map, size)) {
      HandleElf(inpath, map, size);
    } else {
      Die(prog, "not an elf64 binary");
    }
    if (munmap(map, size)) {
      DieSys(inpath);
    }
  }
  if (close(infd)) {
    DieSys(inpath);
  }
}

int main(int argc, char *argv[]) {
  int i;
  prog = argv[0];
  if (!prog) prog = "objbincopy";
  GetOpts(argc, argv);
  if ((outfd = creat(outpath, 0755)) == -1) {
    DieSys(outpath);
  }
  for (i = optind; i < argc; ++i) {
    HandleInput(argv[i]);
  }
  if (close(outfd)) {
    DieSys(outpath);
  }
}
