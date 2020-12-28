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
#include "libc/alg/alg.h"
#include "libc/elf/elf.h"
#include "libc/elf/struct/sym.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "tool/build/lib/dis.h"

bool g_disisprog_disable;

static int DisSymCompare(const struct DisSym *a, const struct DisSym *b) {
  if (a->addr != b->addr) {
    if (a->addr < b->addr) return -1;
    if (a->addr > b->addr) return +1;
  }
  if (a->rank != b->rank) {
    if (a->rank > b->rank) return -1;
    if (a->rank < b->rank) return +1;
  }
  if (a->unique != b->unique) {
    if (a->unique < b->unique) return -1;
    if (a->unique > b->unique) return +1;
  }
  return 0;
}

static void DisLoadElfLoads(struct Dis *d, struct Elf *elf) {
  long i, j, n;
  int64_t addr;
  uint64_t size;
  Elf64_Phdr *phdr;
  j = 0;
  n = elf->ehdr->e_phnum;
  if (d->loads.n < n) {
    d->loads.n = n;
    d->loads.p = realloc(d->loads.p, d->loads.n * sizeof(*d->loads.p));
    CHECK_NOTNULL(d->loads.p);
  }
  for (i = 0; i < n; ++i) {
    phdr = GetElfSegmentHeaderAddress(elf->ehdr, elf->size, i);
    if (phdr->p_type != PT_LOAD) continue;
    d->loads.p[j].addr = phdr->p_vaddr;
    d->loads.p[j].size = phdr->p_memsz;
    d->loads.p[j].istext = (phdr->p_flags & PF_X) == PF_X;
    ++j;
  }
  d->loads.i = j;
}

static void DisLoadElfSyms(struct Dis *d, struct Elf *elf) {
  size_t i, j, n;
  int64_t stablen;
  const Elf64_Sym *st, *sym;
  bool isabs, iscode, isweak, islocal, ishidden, isprotected, isfunc, isobject;
  j = 0;
  if ((d->syms.stab = GetElfStringTable(elf->ehdr, elf->size)) &&
      (st = GetElfSymbolTable(elf->ehdr, elf->size, &n))) {
    stablen = (intptr_t)elf->ehdr + elf->size - (intptr_t)d->syms.stab;
    if (d->syms.n < n) {
      d->syms.n = n;
      d->syms.p = realloc(d->syms.p, d->syms.n * sizeof(*d->syms.p));
      CHECK_NOTNULL(d->syms.p);
    }
    for (i = 0; i < n; ++i) {
      if (ELF64_ST_TYPE(st[i].st_info) == STT_SECTION ||
          ELF64_ST_TYPE(st[i].st_info) == STT_FILE || !st[i].st_name ||
          startswith(d->syms.stab + st[i].st_name, "v_") ||
          !(0 <= st[i].st_name && st[i].st_name < stablen) || !st[i].st_value ||
          !(-0x800000000000 <= (int64_t)st[i].st_value &&
            (int64_t)st[i].st_value < 0x800000000000)) {
        continue;
      }
      isabs = st[i].st_shndx == SHN_ABS;
      isweak = ELF64_ST_BIND(st[i].st_info) == STB_WEAK;
      islocal = ELF64_ST_BIND(st[i].st_info) == STB_LOCAL;
      ishidden = st[i].st_other == STV_HIDDEN;
      isprotected = st[i].st_other == STV_PROTECTED;
      isfunc = ELF64_ST_TYPE(st[i].st_info) == STT_FUNC;
      isobject = ELF64_ST_TYPE(st[i].st_info) == STT_OBJECT;
      d->syms.p[j].unique = i;
      d->syms.p[j].size = st[i].st_size;
      d->syms.p[j].name = st[i].st_name;
      d->syms.p[j].addr = st[i].st_value;
      d->syms.p[j].rank =
          -islocal + -isweak + -isabs + isprotected + isobject + isfunc;
      d->syms.p[j].iscode = DisIsText(d, st[i].st_value) ? !isobject : isfunc;
      d->syms.p[j].isabs = isabs;
      ++j;
    }
  }
  d->syms.i = j;
}

static void DisSortSyms(struct Dis *d) {
  qsort(d->syms.p, d->syms.i, sizeof(struct DisSym), (void *)DisSymCompare);
}

static void DisCanonizeSyms(struct Dis *d) {
  int64_t i, j, a;
  if (d->syms.i) {
    i = 1;
    j = 1;
    a = d->syms.p[0].addr;
    do {
      if (d->syms.p[j].addr > a) {
        a = d->syms.p[j].addr;
        if (j > i) {
          d->syms.p[i] = d->syms.p[j];
        }
        ++i;
      }
      ++j;
    } while (j < d->syms.i);
    d->syms.p = realloc(d->syms.p, sizeof(*d->syms.p) * i);
    d->syms.i = i;
    d->syms.n = i;
  }
  for (i = 0; i < d->syms.i; ++i) {
    DEBUGF("%p-%p %s", d->syms.p[i].addr,
           d->syms.p[i].addr + (d->syms.p[i].size ? d->syms.p[i].size - 1 : 0),
           d->syms.stab + d->syms.p[i].name);
  }
}

bool DisIsProg(struct Dis *d, int64_t addr) {
  long i;
  if (g_disisprog_disable) return true;
  for (i = 0; i < d->loads.i; ++i) {
    if (addr >= d->loads.p[i].addr &&
        addr < d->loads.p[i].addr + d->loads.p[i].size) {
      return true;
    }
  }
  return false;
}

bool DisIsText(struct Dis *d, int64_t addr) {
  long i;
  for (i = 0; i < d->loads.i; ++i) {
    if (addr >= d->loads.p[i].addr &&
        addr < d->loads.p[i].addr + d->loads.p[i].size) {
      return d->loads.p[i].istext;
    }
  }
  return false;
}

long DisFindSym(struct Dis *d, int64_t addr) {
  long l, r, m, n;
  if (DisIsProg(d, addr)) {
    l = 0;
    r = d->syms.i;
    while (l < r) {
      m = (l + r) >> 1;
      if (d->syms.p[m].addr > addr) {
        r = m;
      } else {
        l = m + 1;
      }
    }
    if (r && (addr == d->syms.p[r - 1].addr ||
              (addr > d->syms.p[r - 1].addr &&
               (addr <= d->syms.p[r - 1].addr + d->syms.p[r - 1].size ||
                !d->syms.p[r - 1].size)))) {
      return r - 1;
    }
  }
  return -1;
}

long DisFindSymByName(struct Dis *d, const char *s) {
  long i;
  for (i = 0; i < d->syms.i; ++i) {
    if (strcmp(s, d->syms.stab + d->syms.p[i].name) == 0) {
      return i;
    }
  }
  return -1;
}

void DisLoadElf(struct Dis *d, struct Elf *elf) {
  LOGF("DisLoadElf");
  if (!elf || !elf->ehdr) return;
  DisLoadElfLoads(d, elf);
  DisLoadElfSyms(d, elf);
  DisSortSyms(d);
  DisCanonizeSyms(d);
}
