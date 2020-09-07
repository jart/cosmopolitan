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
#include "libc/alg/alg.h"
#include "libc/alg/arraylist2.h"
#include "libc/elf/elf.h"
#include "libc/elf/struct/sym.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "tool/build/lib/dis.h"

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
  long i;
  int64_t addr;
  uint64_t size;
  Elf64_Phdr *phdr;
  struct DisLoad l;
  d->loads.i = 0;
  for (i = 0; i < elf->ehdr->e_phnum; ++i) {
    phdr = getelfsegmentheaderaddress(elf->ehdr, elf->size, i);
    if (phdr->p_type != PT_LOAD) continue;
    l.addr = phdr->p_vaddr;
    l.size = phdr->p_memsz;
    l.istext = (phdr->p_flags & PF_X) == PF_X;
    APPEND(&d->loads.p, &d->loads.i, &d->loads.n, &l);
  }
}

static void DisLoadElfSyms(struct Dis *d, struct Elf *elf) {
  size_t i, n;
  int64_t stablen;
  struct DisSym t;
  const Elf64_Sym *st, *sym;
  bool isabs, iscode, isweak, islocal, ishidden, isprotected, isfunc, isobject;
  d->syms.i = 0;
  if ((d->syms.stab = getelfstringtable(elf->ehdr, elf->size)) &&
      (st = getelfsymboltable(elf->ehdr, elf->size, &n))) {
    stablen = (intptr_t)elf->ehdr + elf->size - (intptr_t)d->syms.stab;
    for (i = 0; i < n; ++i) {
      if (!st[i].st_name) continue;
      if (!(0 <= st[i].st_name && st[i].st_name < stablen)) continue;
      if (ELF64_ST_TYPE(st[i].st_info) == STT_SECTION) continue;
      isabs = st[i].st_shndx == SHN_ABS;
      isweak = ELF64_ST_BIND(st[i].st_info) == STB_WEAK;
      islocal = ELF64_ST_BIND(st[i].st_info) == STB_LOCAL;
      ishidden = st[i].st_other == STV_HIDDEN;
      isprotected = st[i].st_other == STV_PROTECTED;
      isfunc = ELF64_ST_TYPE(st[i].st_info) == STT_FUNC;
      isobject = ELF64_ST_TYPE(st[i].st_info) == STT_OBJECT;
      t.unique = i;
      t.size = st[i].st_size;
      t.name = st[i].st_name;
      t.addr = st[i].st_value;
      t.rank = -islocal + -isweak + -isabs + isprotected + isobject + isfunc;
      t.iscode = DisIsText(d, st[i].st_value) ? !isobject : isfunc;
      APPEND(&d->syms.p, &d->syms.i, &d->syms.n, &t);
    }
  }
  qsort(d->syms.p, d->syms.i, sizeof(struct DisSym), (void *)DisSymCompare);
}

bool DisIsProg(struct Dis *d, int64_t addr) {
  long i;
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
  size_t i, l, r, m, n;
  if (DisIsProg(d, addr)) {
    l = 0;
    r = d->syms.i;
    while (l < r) {
      m = (l + r) >> 1;
      if (d->syms.p[m].addr < addr) {
        l = m + 1;
      } else {
        r = m;
      }
    }
    if (d->syms.p[l].addr == addr) {
      return l;
    }
    l = MAX(0, (long)l - 10);
    for (n = 0, i = l; i < d->syms.i && n < 20; ++i, ++n) {
      if (addr >= d->syms.p[i].addr &&
          addr < d->syms.p[i].addr + d->syms.p[i].size) {
        return i;
      }
    }
    for (n = 0, i = l; i < d->syms.i && n < 20; ++i, ++n) {
      if (addr >= d->syms.p[i].addr &&
          (i + 1 == d->syms.i || addr < d->syms.p[i + 1].addr)) {
        return i;
      }
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
  if (!elf || !elf->ehdr) return;
  DisLoadElfLoads(d, elf);
  DisLoadElfSyms(d, elf);
}
