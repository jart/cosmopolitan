/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

char16_t *strstr16_naive(const char16_t *haystack, const char16_t *needle) {
  size_t i;
  if (haystack == needle || !*needle)
    return (void *)haystack;
  for (;;) {
    for (i = 0;; ++i) {
      if (!needle[i])
        return (/*unconst*/ char16_t *)haystack;
      if (!haystack[i])
        break;
      if (needle[i] != haystack[i])
        break;
    }
    if (!*haystack++)
      break;
  }
  return 0;
}

int main() {
  int pagesz = sysconf(_SC_PAGESIZE);
  char16_t *map = (char16_t *)mmap(0, pagesz * 2, PROT_READ | PROT_WRITE,
                                   MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  mprotect(map + pagesz / 2, pagesz, PROT_NONE);
  for (int haylen = 1; haylen < 256; ++haylen) {
    char16_t *hay = map + pagesz / 2 - (haylen + 1);
    for (int i = 0; i < haylen; ++i) {
      hay[i] = rand() & 0x0707;
      if (!hay[i])
        hay[i] = 1;
    }
    hay[haylen] = 0;
    for (int neelen = 1; neelen < haylen; ++neelen) {
      char16_t *nee = hay + (haylen + 1) - (neelen + 1);
      if (strstr16_naive(hay, nee) != strstr16(hay, nee)) {
        kprintf("strstr16(%#hhs, %#hhs) returned wrong result\n", hay, nee);
        exit(1);
      }
    }
  }
  munmap(map, pagesz * 2);
}
