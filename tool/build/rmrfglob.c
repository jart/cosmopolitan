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
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/x/x.h"
#include "third_party/musl/glob.h"

// this is useful if you try to do something like `rm /tmp/fastzip.*` on
// windows and bash complains that the argument list is too long. so you
// can run `rmrfglob '/tmp/fastzip.*'` instead to delete all those files

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    glob_t globs;
    if (glob(argv[i], 0, 0, &globs)) {
      perror(argv[i]);
      exit(1);
    }
    for (size_t i = 0; i < globs.gl_pathc; ++i) {
      fprintf(stderr, "removing %s\n", globs.gl_pathv[i]);
      if (rmrf(globs.gl_pathv[i])) {
        perror(globs.gl_pathv[i]);
        exit(1);
      }
    }
    globfree(&globs);
  }
}
