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
#include "libc/fmt/fmt.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/unicode/unicode.h"
#include "libc/x/x.h"

int a, b, w, i;
char name[512];

void DisplayUnicodeCharacter(void) {
  int c, cw;
  c = i;
  cw = wcwidth(c);
  if (cw < 1) {
    c = '?';
    cw = 1;
  }
  if (w) {
    if (w + 1 + cw > 80) {
      printf("\n");
      w = 0;
    } else {
      fputc(' ', stdout);
      w += 1 + cw;
    }
  }
  if (!w) {
    printf("%08x ", i);
    w = 9 + cw;
  }
  fputwc(c, stdout);
}

void DisplayUnicodeBlock(void) {
  if (a == 0x10000) {
    printf("\n\n\n\n\n\n\n                                "
           "ASTRAL PLANES\n\n\n\n\n");
  }
  if (a == 0x0590 /* hebrew */) return;
  if (a == 0x0600 /* arabic */) return;
  if (a == 0x08a0 /* arabic */) return;
  if (a == 0x0750 /* arabic */) return;
  if (a == 0x0700 /* syriac */) return;
  if (a == 0x10800 /* cypriot */) return;
  printf("\n\n%-60s%20s\n"
         "──────────────────────────────────────────────"
         "──────────────────────────────────\n",
         name, gc(xasprintf("%04x .. %04x", a, b)));
  w = 0;
  for (i = a; i <= b; ++i) {
    DisplayUnicodeCharacter();
  }
}

int main(int argc, char *argv[]) {
  FILE *f;
  char *line;
  size_t linesize;
  printf("\n\n\n\n\n                                UNICODE PLANES\n\n\n\n");
  f = fopen("libc/unicode/blocks.txt", "r");
  line = NULL;
  linesize = 0;
  while (!feof(f)) {
    if (getline(&line, &linesize, f) == -1) break;
    if (sscanf(line, "%x..%x; %s", &a, &b, name) != 3) continue;
    DisplayUnicodeBlock();
  }
  free(line);
  fclose(f);
  return 0;
}
