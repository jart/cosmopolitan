/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

/**
 * @fileoverview stronger mktemp for shell scripts, e.g.
 *
 *     o//tool/build/mktemper.com ${TMPDIR:-/tmp}/fooXXXXXXXXXXXXX.o
 *
 * Is a good way to name an intermediate object file.
 */

static wontreturn void Die(const char *thing, const char *reason) {
  tinyprint(2, thing, ": ", reason, "\n", NULL);
  exit(1);
}

static wontreturn void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

int main(int argc, char *argv[]) {
  const char *prog = program_invocation_short_name;
  if (argc == 1) Die(prog, "missing argument");
  if (argc != 2) Die(prog, "too many arguments");
  char *template = argv[1];
  char *substring = strstr(template, "XXXXXXXXXXXXX");
  if (!substring) Die(prog, "template missing XXXXXXXXXXXXX substring");
  uint64_t w;
  if (getrandom(&w, 8, 0) != 8) DieSys("getrandom");
  for (int i = 0; i < 13; ++i) {
    substring[i] = "0123456789abcdefghijklmnopqrstuvwxyz"[w % 36];
    w /= 36;
  }
  int fd;
  if ((fd = open(template, O_RDWR | O_CREAT | O_EXCL, 0600)) == -1) {
    DieSys(template);
  }
  close(fd);
  tinyprint(1, template, "\n", NULL);
}
