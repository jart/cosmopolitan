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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"

/**
 * Returns current working directory.
 *
 * If the `PWD` environment variable is set, and it's correct, then
 * that'll be returned in its exact canonical or non-canonical form
 * instead of calling getcwd().
 *
 * @return pointer that must be free()'d, or NULL w/ errno
 */
char *get_current_dir_name(void) {
  const char *res;
  struct stat st1, st2;
  if ((res = getenv("PWD")) && *res &&  //
      !stat(res, &st1) &&               //
      !stat(".", &st2) &&               //
      st1.st_dev == st2.st_dev &&       //
      st1.st_ino == st2.st_ino) {
    return strdup(res);
  } else {
    return getcwd(0, 0);
  }
}
