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
#include "libc/assert.h"
#include "libc/elf/def.h"
#include "libc/elf/struct/phdr.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/uart.internal.h"
#include "libc/runtime/e820.internal.h"
#include "libc/runtime/metalprintf.internal.h"
#include "libc/runtime/pc.internal.h"
#include "libc/runtime/runtime.h"

#define INVERT(x) (BANE + PHYSICAL(x))
#define NOPAGE ((uint64_t)-1)

struct ReclaimedPage {
  uint64_t next;
};

/**
 * Allocates new page of physical memory.
 */
noasan texthead uint64_t __new_page(struct mman *mm) {
  uint64_t p = mm->frp;
  if (p != NOPAGE) {
    uint64_t q;
    struct ReclaimedPage *rp = (struct ReclaimedPage *)(BANE + p);
    _unassert(p == (p & PAGE_TA));
    q = rp->next;
    _unassert(q == (q & PAGE_TA));
    mm->frp = q;
    return p;
  }
  if (mm->pdpi == mm->e820n) {
    return 0;
  }
  while (mm->pdp >= mm->e820[mm->pdpi].addr + mm->e820[mm->pdpi].size) {
    if (++mm->pdpi == mm->e820n) return 0;
    mm->pdp = MAX(mm->pdp, mm->e820[mm->pdpi].addr);
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
    if (!(*e & (PAGE_V | PAGE_RSRV))) {
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
static noasan textreal void __normalize_e820(struct mman *mm, uint64_t top) {
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
  top = ROUNDUP(top, 4096);
  mm->pdp = MAX(top, mm->e820[0].addr);
  mm->pdpi = 0;
  mm->e820n = n;
  mm->frp = NOPAGE;
}

/**
 * Identity maps an area of physical memory to its negative address.
 */
noasan textreal uint64_t *__invert_memory_area(struct mman *mm,
                                               uint64_t *pml4t,
                                               uint64_t ps, uint64_t size,
                                               uint64_t pte_flags) {
  uint64_t pe = ps + size, p, *m = NULL;
  ps = ROUNDDOWN(ps, 4096);
  pe = ROUNDUP(pe, 4096);
  for (p = ps; p != pe; p += 4096) {
    m = __get_virtual(mm, pml4t, BANE + p, true);
    if (m && !(*m & (PAGE_V | PAGE_RSRV))) {
      *m = p | PAGE_V | PAGE_RSRV | pte_flags;
    }
  }
  return m;
}

/**
 * Increments the reference count for a page of physical memory.
 */
noasan void __ref_page(struct mman *mm, uint64_t *pml4t, uint64_t p) {
  uint64_t *m, e;
  m = __invert_memory_area(mm, pml4t, p, 4096, PAGE_RW | PAGE_XD);
  if (m) {
    e = *m;
    if ((e & PAGE_REFC) != PAGE_REFC) {
      e += PAGE_1REF;
      *m = e;
    }
  }
}

/**
 * Increments the reference counts for an area of physical memory.
 */
noasan void __ref_pages(struct mman *mm, uint64_t *pml4t, uint64_t ps,
                        uint64_t size) {
  uint64_t p = ROUNDDOWN(ps, 4096), e = ROUNDUP(ps + size, 4096);
  while (p != e) {
    __ref_page(mm, pml4t, p);
    p += 4096;
  }
}

/**
 * Reclaims a page of physical memory for later use.
 */
static noasan void __reclaim_page(struct mman *mm, uint64_t p) {
  struct ReclaimedPage *rp = (struct ReclaimedPage *)(BANE + p);
  _unassert(p == (p & PAGE_TA));
  rp->next = mm->frp;
  mm->frp = p;
}

/**
 * Decrements the reference count for a page of physical memory.  Frees the
 * page if there are no virtual addresses (excluding the negative space)
 * referring to it.
 */
noasan void __unref_page(struct mman *mm, uint64_t *pml4t, uint64_t p) {
  uint64_t *m, e;
  m = __invert_memory_area(mm, pml4t, p, 4096, PAGE_RW | PAGE_XD);
  if (m) {
    e = *m;
    if ((e & PAGE_REFC) != PAGE_REFC) {
      e -= PAGE_1REF;
      *m = e;
      if ((e & PAGE_REFC) == 0) __reclaim_page(mm, p);
    }
  }
}

/**
 * Identity maps all usable physical memory to its negative address.
 */
static noasan textreal void __invert_memory(struct mman *mm, uint64_t *pml4t) {
  uint64_t i, j, *m, p, pe;
  for (i = 0; i < mm->e820n; ++i) {
    uint64_t ps = mm->e820[i].addr, size = mm->e820[i].size;
    /* ape/ape.S has already mapped the first 2 MiB of physical memory. */
    if (ps < 0x200000 && ps + size <= 0x200000) continue;
    __invert_memory_area(mm, pml4t, ps, size, PAGE_RW | PAGE_XD);
  }
}

/**
 * Exports information about the offset of a field within a structure type,
 * so that assembly language routines can use it.  This macro can be invoked
 * from inside a function whose code is known to be emitted.
 */
#define export_offsetof(type, member)                    \
  do {                                                   \
    asm volatile(".globl \"" #type "::" #member "\"\n\t" \
                 ".set \"" #type "::" #member "\",%c0"   \
                 : /* no outputs */                      \
                 : "i"(offsetof(type, member)));         \
  } while (0)

noasan textreal void __setup_mman(struct mman *mm, uint64_t *pml4t,
                                  uint64_t top) {
  export_offsetof(struct mman, pc_drive_base_table);
  export_offsetof(struct mman, pc_drive_last_sector);
  export_offsetof(struct mman, pc_drive_last_head);
  export_offsetof(struct mman, pc_drive);
  export_offsetof(struct mman, e820);
  export_offsetof(struct mman, e820_end);
  export_offsetof(struct mman, bad_idt);
  export_offsetof(struct mman, pc_drive_next_sector);
  export_offsetof(struct mman, pc_drive_next_cylinder);
  export_offsetof(struct mman, pc_drive_next_head);
  export_offsetof(struct mman, pc_video_type);
  export_offsetof(struct mman, pc_video_stride);
  export_offsetof(struct mman, pc_video_width);
  export_offsetof(struct mman, pc_video_height);
  export_offsetof(struct mman, pc_video_framebuffer);
  export_offsetof(struct mman, pc_video_framebuffer_size);
  export_offsetof(struct mman, pc_video_curs_info);
  export_offsetof(struct mman, pc_video_char_height);
  __normalize_e820(mm, top);
  __invert_memory(mm, pml4t);
}

/**
 * Maps APE-defined ELF program headers into memory and clears BSS.
 */
noasan textreal void __map_phdrs(struct mman *mm, uint64_t *pml4t, uint64_t b,
                                 uint64_t top) {
  struct Elf64_Phdr *p;
  uint64_t i, f, v, m, *e;
  extern char ape_phdrs[] __attribute__((__weak__));
  extern char ape_phdrs_end[] __attribute__((__weak__));
  __setup_mman(mm, pml4t, top);
  for (p = (struct Elf64_Phdr *)INVERT(ape_phdrs), m = 0;
       p < (struct Elf64_Phdr *)INVERT(ape_phdrs_end); ++p) {
    if (p->p_type == PT_LOAD || p->p_type == PT_GNU_STACK) {
      f = PAGE_RSRV | PAGE_U;
      if (p->p_flags & PF_W)
        f |= PAGE_V | PAGE_RW;
      else if (p->p_flags & (PF_R | PF_X))
        f |= PAGE_V;
      if (!(p->p_flags & PF_X)) f |= PAGE_XD;
      for (i = 0; i < p->p_memsz; i += 4096) {
        if (i < p->p_filesz) {
          v = b + p->p_offset + i;
          m = MAX(m, v);
        } else {
          v = __clear_page(BANE + __new_page(mm));
        }
        *__get_virtual(mm, pml4t, p->p_vaddr + i, true) = (v & PAGE_TA) | f;
        __ref_page(mm, pml4t, v & PAGE_TA);
      }
    }
  }
  mm->pdp = MAX(mm->pdp, m);
}

/**
 * Reclaims memory pages which were used at boot time but which can now be
 * made available for the application.
 */
noasan textreal void __reclaim_boot_pages(struct mman *mm, uint64_t skip_start,
                                          uint64_t skip_end) {
  uint64_t p = mm->frp, q = IMAGE_BASE_REAL, i, n = mm->e820n, b, e;
  for (i = 0; i < n; ++i) {
    b = mm->e820[i].addr;
    if (b >= IMAGE_BASE_PHYSICAL) break;
    e = MIN(IMAGE_BASE_PHYSICAL, b + mm->e820[i].size);
    q = MAX(IMAGE_BASE_REAL, b);
    while (q < e) {
      struct ReclaimedPage *rp;
      if (q == skip_start) {
        q = skip_end;
        if (q >= e) break;
      }
      rp = (struct ReclaimedPage *)(BANE + q);
      rp->next = p;
      p = q;
      q += 4096;
    }
  }
  mm->frp = p;
}
