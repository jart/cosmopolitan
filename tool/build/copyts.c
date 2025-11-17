// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"

static const char *help = "\
Copyright 2025 Justine Alexandra Roberts Tunney - ISC license\n\
File Timestamp Copier 1.0 (2025-05-08). Usage:\n\
copyts FROMPATH TOPATH\n\
\n";

int main(int argc, char *argv[]) {

  if (argc != 3 ||
      (argc == 2 && !strcmp(argv[1], "-h") && !strcmp(argv[1], "--help"))) {
    fputs(help, stderr);
    exit(1);
  }

  const char *frompath = argv[1];
  const char *topath = argv[2];

  // get atim/mtim
  struct stat st;
  if (stat(frompath, &st)) {
    perror(frompath);
    exit(1);
  }

  // set atim/mtim
  const struct timespec ts[2] = {st.st_atim, st.st_mtim};
  if (utimensat(AT_FDCWD, topath, ts, 0)) {
    perror(topath);
    exit(1);
  }
}
