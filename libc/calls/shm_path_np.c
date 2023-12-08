/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/str/blake2.h"
#include "libc/str/str.h"

/**
 * Returns filesystem pathname of named semaphore.
 *
 * @param name is `name` of semaphore which should begin with slash
 * @param buf is temporary storage with at least 78 bytes
 * @return pointer to file system path
 * @raise ENAMETOOLONG if constructed path would exceed `size`
 */
void shm_path_np(const char *name, char buf[hasatleast 78]) {
  char *p;
  unsigned n;
  const char *a;
  uint8_t digest[BLAKE2B256_DIGEST_LENGTH];
  a = "/tmp/", n = 5;
  if (IsLinux() && isdirectory("/dev/shm")) {
    a = "/dev/shm/", n = 9;
  }
  BLAKE2B256(name, strlen(name), digest);
  p = mempcpy(buf, a, n);
  p = hexpcpy(p, digest, BLAKE2B256_DIGEST_LENGTH);
  mempcpy(p, ".sem", 5);
}
