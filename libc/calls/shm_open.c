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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"

/**
 * Opens POSIX named memory object.
 *
 * @param name should begin with a `/` and shouldn't contain subsequent
 *     slash characters, and furthermore shouldn't exceed NAME_MAX, for
 *     maximum portability; POSIX defines it as opening a multi-process
 *     object and leaves all other names implementation defined; and we
 *     choose (in this implementation) to define those names as meaning
 *     the same thing while not imposing any length limit; cosmo always
 *     feeds your name through BLAKE2B to create a `.sem` file existing
 *     under `/dev/shm` if available, otherwise it will go under `/tmp`
 * @return open file descriptor, or -1 w/ errno
 */
int shm_open(const char *name, int oflag, unsigned mode) {
  int fd;
  char path[78];
  shm_path_np(name, path);
  BLOCK_CANCELATION;
  fd = openat(AT_FDCWD, path, oflag, mode);
  ALLOW_CANCELATION;
  return fd;
}
