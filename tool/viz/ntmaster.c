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
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/x/xgetline.h"

#define DLL "iphlpapi"

/**
 * @fileoverview Tool for adding rnew libraries to libc/nt/master.sh
 *
 * If provided with a /tmp/syms.txt file containing one symbol name per
 * line, this tool will output the correctly tab indented shell code.
 */

int main(int argc, char *argv[]) {
  FILE *f;
  int i, n, t;
  char *sym, tabs[64];
  ShowCrashReports();
  f = fopen("/tmp/syms.txt", "r");
  memset(tabs, '\t', 64);
  while ((sym = _chomp(xgetline(f)))) {
    if (strlen(sym)) {
      printf("imp\t");

      /* what we call the symbol */
      i = printf("'%s'", sym);
      t = 0;
      n = 56;
      if (i % 8) ++t, i = ROUNDUP(i, 8);
      t += (n - i) / 8;
      printf("%.*s", t, tabs);

      /* what the kernel dll calls the symbol */
      i = printf("%s", sym);
      t = 0;
      n = 56;
      if (i % 8) ++t, i = ROUNDUP(i, 8);
      t += (n - i) / 8;
      printf("%.*s", t, tabs);

      /* dll short name */
      i = printf("%s", DLL);
      t = 0;
      n = 16;
      if (i % 8) ++t, i = ROUNDUP(i, 8);
      t += (n - i) / 8;
      printf("%.*s", t, tabs);

      /* hint */
      i = printf("0");
      t = 0;
      n = 8;
      if (i % 8) ++t, i = ROUNDUP(i, 8);
      t += (n - i) / 8;
      printf("%.*s", t, tabs);

      printf("\n");
    }
    free(sym);
  }
  return 0;
}
