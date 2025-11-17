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
#include "libc/fmt/conv.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/ftw.h"
#include "libc/stdio/stdio.h"

size_t g_sum;

static int visit(const char *fpath,      //
                 const struct stat *st,  //
                 int typeflag,           //
                 struct FTW *ftwbuf) {   //
  if (typeflag == FTW_F)
    g_sum += st->st_size;
  return 0;
}

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    g_sum = 0;
    if (nftw(argv[i], visit, 128, FTW_PHYS)) {
      perror(argv[i]);
      exit(1);
    }
    char ss[32];
    sizefmt(ss, g_sum, 1000);
    printf("%-7s %s\n", ss, argv[i]);
  }
}
