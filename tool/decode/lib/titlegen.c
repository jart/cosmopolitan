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
#include "libc/fmt/fmt.h"
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
    printf("-*-%-71s-*-│\n│vi:%-72s:vi│\n╞", modeline->emacs, modeline->vim);
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
