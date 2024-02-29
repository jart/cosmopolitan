/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/intrin/bsf.h"
#include "libc/intrin/cxaatexit.internal.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"

/**
 * Prints global destructors.
 *
 * @param pred can be null to match all
 */
void __cxa_printexits(FILE *f, void *pred) {
  char name[23];
  unsigned i, mask;
  const char *symbol;
  struct CxaAtexitBlock *b;
  fprintf(f, "\n");
  fprintf(f, "                       GLOBAL DESTRUCTORS                   \n");
  fprintf(f, "      callback                arg                pred       \n");
  fprintf(f, "---------------------- ------------------ ------------------\n");
  __cxa_lock();
  if ((b = __cxa_blocks.p)) {
    do {
      mask = b->mask;
      while (mask) {
        i = _bsf(mask);
        mask &= ~(1u << i);
        if (!pred || pred == b->p[i].pred) {
          symbol = GetSymbolByAddr((intptr_t)b->p[i].fp);
          if (symbol) {
            snprintf(name, sizeof(name), "%s", symbol);
          } else {
            snprintf(name, sizeof(name), "0x%016lx", (unsigned long)b->p[i].fp);
          }
          fprintf(f, "%-22s 0x%016lx 0x%016lx\n", name,
                  (unsigned long)b->p[i].arg, (unsigned long)b->p[i].pred);
        }
      }
    } while ((b = b->next));
  }
  __cxa_unlock();
  fprintf(f, "\n");
}
