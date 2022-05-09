/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/struct/stat.h"
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nt/struct/importobjectheader.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/xed/x86.h"
#include "tool/build/lib/dis.h"
#include "tool/build/lib/high.h"
#include "tool/build/lib/loader.h"

#define HEXWIDTH 8

const char *const kRealSymbols[] = {
    "a20",          "ape.mbrpad",     "ape.str",
    "ape_disk",     "ape_grub",       "ape_mz",
    "apesh",        "dsknfo",         "e820",
    "gdt",          "golong",         "hiload",
    "lcheck",       "longmodeloader", "pc",
    "pcread",       "pinit",          "realmodeloader",
    "rldie",        "rvputs",         "sconf",
    "sinit",        "sinit4",         "str.cpuid",
    "str.crlf",     "str.dsknfo",     "str.e820",
    "str.error",    "str.long",       "str.memory",
    "str.oldskool", "stub",           "unreal",
};

const char *const kLegacySymbols[] = {
    "ape_grub_entry",
};

bool boop;
long cursym;
Elf64_Ehdr *elf;
Elf64_Shdr *sec;
struct Dis dis[1];
struct Machine m[1];
struct Elf diself[1];
char codebuf[256];
char optspecbuf[128];
const Elf64_Sym *syms;
char *symstrs, *secstrs;
size_t i, j, k, l, size, skip, symcount;

bool IsRealSymbol(const char *s) {
  int m, l, r, x;
  l = 0;
  r = ARRAYLEN(kRealSymbols) - 1;
  while (l <= r) {
    m = (l + r) >> 1;
    x = strcmp(s, kRealSymbols[m]);
    if (x < 0) {
      r = m - 1;
    } else if (x > 0) {
      l = m + 1;
    } else {
      return true;
    }
  }
  return false;
}

bool IsLegacySymbol(const char *s) {
  int m, l, r, x;
  l = 0;
  r = ARRAYLEN(kLegacySymbols) - 1;
  while (l <= r) {
    m = (l + r) >> 1;
    x = strcmp(s, kLegacySymbols[m]);
    if (x < 0) {
      r = m - 1;
    } else if (x > 0) {
      l = m + 1;
    } else {
      return true;
    }
  }
  return false;
}

void SetSymbol(bool printit) {
  if (!boop) {
    printf("\n");
    boop = true;
  }
  if (printit) {
    printf("\e[38;5;%dm%s\e[0m:\n", g_high.label, symstrs + syms[k].st_name);
  }
  if (IsRealSymbol(symstrs + syms[k].st_name)) {
    if (m->mode != XED_MACHINE_MODE_REAL) {
      printf("\t\e[38;5;%dm.code16\e[0m\n", g_high.keyword);
    }
    m->mode = XED_MACHINE_MODE_REAL;
  } else if (IsLegacySymbol(symstrs + syms[k].st_name)) {
    if (m->mode != XED_MACHINE_MODE_LEGACY_32) {
      printf("\t\e[38;5;%dm.code32\e[0m\n", g_high.keyword);
    }
    m->mode = XED_MACHINE_MODE_LEGACY_32;
  } else {
    if (m->mode != XED_MACHINE_MODE_LONG_64) {
      printf("\t\e[38;5;%dm.code64\e[0m\n", g_high.keyword);
    }
    m->mode = XED_MACHINE_MODE_LONG_64;
  }
  cursym = k;
  if (syms[k].st_size) {
    skip = syms[k].st_size;
  } else {
    skip = -1;
    for (l = 0; l < symcount; ++l) {
      if (syms[l].st_shndx == i && syms[l].st_name &&
          syms[l].st_value > syms[cursym].st_value) {
        skip = MIN(skip, syms[l].st_value - syms[cursym].st_value);
      }
    }
    if (skip == -1) {
      skip = sec->sh_addr + sec->sh_size - syms[cursym].st_value;
    }
  }
}

void PrintSymbolName(void) {
  bool done;
  done = false;
  boop = false;
  for (k = 0; k < symcount; ++k) {
    if (syms[k].st_value == sec->sh_addr + j && syms[k].st_name) {
      if (!done && syms[k].st_size) {
        SetSymbol(true);
        done = true;
      } else {
        if (!boop) {
          printf("\n");
          boop = true;
        }
        printf("\e[38;5;%dm%s\e[0m:\n", g_high.label,
               symstrs + syms[k].st_name);
      }
    }
  }
  if (done) {
    return;
  }
  for (k = 0; k < symcount; ++k) {
    if (ELF64_ST_TYPE(syms[k].st_info) && syms[k].st_name &&
        syms[k].st_value == sec->sh_addr + j) {
      SetSymbol(false);
      return;
    }
  }
  for (k = 0; k < symcount; ++k) {
    if (syms[k].st_name && syms[k].st_value == sec->sh_addr + j) {
      SetSymbol(false);
      return;
    }
  }
  if (cursym != -1 && syms[cursym].st_size &&
      sec->sh_addr + j >= syms[cursym].st_value + syms[cursym].st_size) {
    cursym = -1;
    skip = 1;
  }
}

bool Ild(void) {
  int remain;
  remain = 15;
  if (cursym != -1 && syms[cursym].st_size) {
    remain =
        (syms[cursym].st_value + syms[cursym].st_size) - (sec->sh_addr + j);
  }
  xed_decoded_inst_zero_set_mode(dis->xedd, m->mode);
  xed_instruction_length_decode(dis->xedd, (char *)elf + sec->sh_offset + j,
                                remain);
  skip = dis->xedd->op.error ? 1 : MAX(1, dis->xedd->length);
  return !dis->xedd->op.error;
}

bool IsCode(void) {
  if (!(sec->sh_flags & SHF_EXECINSTR)) return false;
  if (cursym != -1 && ELF64_ST_TYPE(syms[cursym].st_info) == STT_OBJECT) {
    return false;
  }
  return true;
}

bool IsBss(void) {
  return sec->sh_type == SHT_NOBITS;
}

void Disassemble(void) {
  int c;
  bool istext;
  cursym = -1;
  secstrs = GetElfSectionNameStringTable(elf, size);
  symstrs = GetElfStringTable(elf, size);
  syms = GetElfSymbolTable(elf, size, &symcount);
  for (i = 0; i < elf->e_shnum; ++i) {
    sec = GetElfSectionHeaderAddress(elf, size, i);
    if (!sec->sh_size) continue;
    if (!(sec->sh_flags & SHF_ALLOC)) continue;
    printf("\n\t\e[38;5;%dm.section\e[0m %s\n", g_high.keyword,
           secstrs + sec->sh_name);
    for (j = 0; j < sec->sh_size; j += MAX(1, skip)) {
      PrintSymbolName();
      if (cursym == -1) continue;
      if (sec->sh_type == SHT_NOBITS) {
        printf("\t\e[38;5;%dm.zero\e[0m\t%ld\n", g_high.keyword, skip);
      } else if (IsCode()) {
        if (Ild()) {
          dis->addr = sec->sh_addr + j;
          DisInst(dis, codebuf, DisSpec(dis->xedd, optspecbuf));
          printf("\t%s\n", codebuf);
        } else {
          printf("\t.wut\t%ld\n", dis->xedd->op.error);
        }
      } else {
        for (k = 0; k < skip; ++k) {
          if (!(k & (HEXWIDTH - 1))) {
            if (k) {
              printf(" \e[38;5;%dm# %#.*s\e[0m\n", g_high.comment, HEXWIDTH,
                     (unsigned char *)elf + sec->sh_offset + j + k - HEXWIDTH);
            }
            printf("\t\e[38;5;%dm.byte\e[0m\t", g_high.keyword);
          } else if (k) {
            printf(",");
          }
          printf("0x%02x", ((unsigned char *)elf)[sec->sh_offset + j + k]);
        }
        if (k) {
          if (!(k & (HEXWIDTH - 1))) {
            printf(" \e[38;5;%dm# %#.*s\e[0m\n", g_high.comment, HEXWIDTH,
                   (unsigned char *)elf + sec->sh_offset + j + skip - HEXWIDTH);
          } else {
            for (l = 0; l < HEXWIDTH - (k & (HEXWIDTH - 1)); ++l) {
              printf("     ");
            }
            printf(" \e[38;5;%dm# %#.*s\e[0m\n", g_high.comment,
                   skip - ROUNDDOWN(skip, HEXWIDTH),
                   (unsigned char *)elf + sec->sh_offset + j +
                       ROUNDDOWN(skip, HEXWIDTH));
          }
        }
      }
    }
  }
}

int main(int argc, char *argv[]) {
  ShowCrashReports();
  int fd;
  void *map;
  struct stat st;
  const char *path;
  if (argc == 1) {
    fprintf(stderr, "USAGE: %s ELF\n", program_invocation_name);
    exit(1);
  } else {
    path = argv[1];
  }
  if ((fd = open(path, O_RDONLY)) == -1) {
    fprintf(stderr, "ERROR: NOT FOUND: %`'s\n", path);
    exit(1);
  }
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_NE(0, st.st_size);
  CHECK_NE(MAP_FAILED,
           (map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)));
  if (memcmp(map, ELFMAG, 4)) {
    fprintf(stderr, "ERROR: NOT AN ELF: %`'s\n", path);
    exit(1);
  }
  m->mode = XED_MACHINE_MODE_LONG_64;
  g_high.keyword = 155;
  g_high.reg = 215;
  g_high.literal = 182;
  g_high.label = 221;
  g_high.comment = 112;
  g_high.quote = 180;
  dis->m = m;
  diself->prog = path;
  LoadDebugSymbols(diself);
  DisLoadElf(dis, diself);
  LOGIFNEG1(close(fd));
  elf = map;
  size = st.st_size;
  Disassemble();
  LOGIFNEG1(munmap(map, st.st_size));
  return 0;
}
