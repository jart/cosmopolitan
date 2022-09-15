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
╠──────────────────────────────────────────────────────────────────────────────╣
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█▀█░▀█▀░█░█░█▀█░█░░░█░░░█░█░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█░▄░░█░░█░█░█▀█░█░░░█░░░▀█▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░▀░▀░▀▀▀░░▀░░▀▀▀░▀░▀░▀▀▀░▀▀▀░░▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█▀█░█▀█░▀█▀░█▀█░█▀█░█░░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀▀░█ █░██▀░░█░░█▀█░█▀█░█░░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░▀░░░▀▀▀░▀░▀░░▀░░▀░▀░▀▀▀░▀▀▀░▀▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀▀░█░█░█▀▀░█▀█░█░█░▀█▀░█▀█░█▀█░█░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░▄▄░░░▐█░░│
│░░░░░░░█▀▀░▄▀▄░█▀▀░█░▄░█░█░░█░░█▀█░█▀█░█░░█▀▀░░░░░░░░░░░░▄▄▄░░░▄██▄░░█▀░░░█░▄░│
│░░░░░░░▀▀▀░▀░▀░▀▀▀░▀▀▀░▀▀▀░░▀░░▀░▀░▀▀▀░▀▀░▀▀▀░░░░░░░░░░▄██▀█▌░██▄▄░░▐█▀▄░▐█▀░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▐█▀▀▌░░░▄▀▌░▌░█░▌░░▌░▌░░│
╠──────────────────────────────────────────────────────▌▀▄─▐──▀▄─▐▄─▐▄▐▄─▐▄─▐▄─│
│ αcτµαlly pδrταblε εxεcµταblε § no-frills virtual memory management           │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "ape/relocations.h"
#include "libc/elf/def.h"
#include "libc/elf/struct/phdr.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/uart.internal.h"
#include "libc/runtime/e820.internal.h"
#include "libc/runtime/metalprintf.internal.h"
#include "libc/runtime/pc.internal.h"
#include "libc/runtime/runtime.h"

/**
 * Allocates new page of physical memory.
 */
noasan texthead uint64_t __new_page(struct mman *mm) {
  uint64_t p;
  if (mm->pdpi == mm->e820n) {
    /* TODO: reclaim free pages */
    return 0;
  }
  while (mm->pdp >= mm->e820[mm->pdpi].addr + mm->e820[mm->pdpi].size) {
    if (++mm->pdpi == mm->e820n) return 0;
    mm->pdp = mm->e820[mm->pdpi].addr;
  }
  p = mm->pdp;
  mm->pdp += 4096;
  return p;
}

/**
 * Returns pointer to page table entry for page at virtual address.
 * Additional page tables are allocated if needed as a side-effect.
 */
noasan textreal uint64_t *__get_virtual(struct mman *mm, uint64_t *t,
                                        int64_t vaddr, bool maketables) {
  uint64_t *e, p;
  unsigned char h;
  for (h = 39;; h -= 9) {
    e = t + ((vaddr >> h) & 511);
    if (h == 12) return e;
    if (!(*e & PAGE_V)) {
      if (!maketables) return NULL;
      if (!(p = __new_page(mm))) return NULL;
      __clear_page(BANE + p);
      *e = p | PAGE_V | PAGE_RW;
    }
    t = (uint64_t *)(BANE + (*e & PAGE_TA));
  }
}

/**
 * Sorts, rounds, and filters BIOS memory map.
 */
static noasan textreal void __normalize_e820(struct mman *mm) {
  uint64_t a, b;
  uint64_t x, y;
  unsigned i, j, n;
  for (n = i = 0; mm->e820[i].size; ++i) {
    mm->e820[n] = mm->e820[i];
    x = mm->e820[n].addr;
    y = mm->e820[n].addr + mm->e820[n].size;
    a = ROUNDUP(x, 4096);
    b = ROUNDDOWN(y, 4096);
    if (b > a && mm->e820[i].type == kMemoryUsable) {
      b -= a;
      mm->e820[n].addr = a;
      mm->e820[n].size = b;
      ++n;
    }
  }
  for (i = 1; i < n; ++i) {
    for (j = i; j > 0 && mm->e820[i].addr < mm->e820[j - 1].addr; --j) {
      mm->e820[j] = mm->e820[j - 1];
    }
    mm->e820[j] = mm->e820[i];
  }
  mm->pdp = MAX(0x80000, mm->e820[0].addr);
  mm->pdpi = 0;
  mm->e820n = n;
}

/**
 * Identity maps all usable physical memory to its negative address.
 */
static noasan textreal void __invert_memory(struct mman *mm, uint64_t *pml4t) {
  uint64_t i, j, *m, p, pe;
  for (i = 0; i < mm->e820n; ++i) {
    for (p = mm->e820[i].addr, pe = mm->e820[i].addr + mm->e820[i].size;
         p != pe + 0x200000; p += 4096) {
      m = __get_virtual(mm, pml4t, BANE + p, true);
      if (m && !(*m & PAGE_V)) {
        *m = p | PAGE_V | PAGE_RW;
      }
    }
  }
}

noasan textreal void __setup_mman(struct mman *mm, uint64_t *pml4t) {
  __normalize_e820(mm);
  __invert_memory(mm, pml4t);
}

/**
 * Maps APE-defined ELF program headers into memory and clears BSS.
 */
noasan textreal void __map_phdrs(struct mman *mm, uint64_t *pml4t, uint64_t b) {
  struct Elf64_Phdr *p;
  uint64_t i, f, v, m, *e;
  extern char ape_phdrs[] __attribute__((__weak__));
  extern char ape_phdrs_end[] __attribute__((__weak__));
  __setup_mman(mm, pml4t);
  for (p = (struct Elf64_Phdr *)REAL(ape_phdrs), m = 0;
       p < (struct Elf64_Phdr *)REAL(ape_phdrs_end); ++p) {
    if (p->p_type == PT_LOAD || p->p_type == PT_GNU_STACK) {
      f = PAGE_V | PAGE_U;
      if (p->p_flags & PF_W) f |= PAGE_RW;
      for (i = 0; i < p->p_memsz; i += 4096) {
        if (i < p->p_filesz) {
          v = b + p->p_offset + i;
          m = MAX(m, v);
        } else {
          v = __clear_page(BANE + __new_page(mm));
        }
        *__get_virtual(mm, pml4t, p->p_vaddr + i, true) = (v & PAGE_TA) | f;
      }
    }
  }
  mm->pdp = MAX(mm->pdp, m);
}
