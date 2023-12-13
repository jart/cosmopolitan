/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "tool/decode/lib/titlegen.h"

const struct Modeline kModelineAsm = {
    " mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8 ",
    " set et ft=asm ts=8 sw=8 fenc=utf-8 "};

/**
 * Displays one of those ANSI block source dividers we love so much.
 */
void showtitle(const char *brand, const char *tool, const char *title,
               const char *description, const struct Modeline *modeline) {
  char buf[512], *p;
  p = stpcpy(buf, brand);
  if (tool) {
    p = stpcpy(stpcpy(p, " § "), tool);
    if (title) {
      p = stpcpy(stpcpy(p, " » "), title);
    }
  }
  printf("/*");
  if (modeline) {
    printf("-*-%-71s-*-│\n│ vi:%-70s:vi │\n╞", modeline->emacs, modeline->vim);
    for (unsigned i = 0; i < 78; ++i) printf("═");
    printf("╡\n│ %-76s ", buf);
  } else {
    for (unsigned i = 0; i < 75; ++i) printf("─");
    printf("│─╗\n│ %-73s ─╬─", buf);
  }
  printf("│\n╚─");
  for (unsigned i = 0; i < 75; ++i) printf("─");
  printf("%s", modeline ? "─" : "│");
  if (description) {
    /* TODO(jart): paragraph fill */
    printf("─╝\n%s ", description);
  } else {
  }
  printf("*/\n");
}
