/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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

void __clear_cache2(const void *base, const void *end) {
#ifdef __aarch64__
  int icache, dcache;
  const char *p, *pe = end;
  static unsigned int ctr_el0 = 0;
  if (!ctr_el0) asm volatile("mrs\t%0,ctr_el0" : "=r"(ctr_el0));
  icache = 4 << (ctr_el0 & 15);
  dcache = 4 << ((ctr_el0 >> 16) & 15);
  for (p = (const char *)((uintptr_t)base & -dcache); p < pe; p += dcache) {
    asm volatile("dc\tcvau,%0" : : "r"(p) : "memory");
  }
  asm volatile("dsb\tish" ::: "memory");
  for (p = (const char *)((uintptr_t)base & -icache); p < pe; p += icache) {
    asm volatile("ic\tivau,%0" : : "r"(p) : "memory");
  }
  asm volatile("dsb\tish\nisb" ::: "memory");
#endif
}
