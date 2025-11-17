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
#include "libc/calls/calls.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/mem.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/testlib.h"

#define PATH "love"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void WriteBlockSync(int blksize) {
  static int block;
  char *buf = memalign(blksize, blksize);
  for (int i = 0; i < blksize; i += 4096)
    buf[i] = rand();
  ASSERT_SYS(0, blksize, pwrite(3, buf, blksize, (block++ % 10) * blksize));
  free(buf);
}

void ReadBlockSync(int blksize) {
  static int block;
  char *buf = memalign(blksize, blksize);
  ASSERT_SYS(0, blksize, pread(3, buf, blksize, (block++ % 10) * blksize));
  free(buf);
}

TEST(normal, test) {
  kprintf("NORMAL\n");
  struct stat st;
  ASSERT_SYS(0, 3, open(PATH, O_RDWR | O_CREAT, 0644));
  ASSERT_SYS(0, 0, fstat(3, &st));
  BENCHMARK(100, 1, WriteBlockSync(st.st_blksize));
  BENCHMARK(100, 1, ReadBlockSync(st.st_blksize));
  ASSERT_SYS(0, 0, close(3));
}

TEST(o_dsync, test) {
  kprintf("O_DSYNC\n");
  struct stat st;
  ASSERT_SYS(0, 3, open(PATH, O_RDWR | O_CREAT | O_DSYNC, 0644));
  ASSERT_SYS(0, 0, fstat(3, &st));
  BENCHMARK(100, 1, WriteBlockSync(st.st_blksize));
  BENCHMARK(100, 1, ReadBlockSync(st.st_blksize));
  ASSERT_SYS(0, 0, close(3));
}

TEST(o_direct, test) {
  if (IsXnu())
    return;
  kprintf("O_DIRECT\n");
  struct stat st;
  ASSERT_SYS(0, 3, open(PATH, O_RDWR | O_CREAT | O_DIRECT, 0644));
  ASSERT_SYS(0, 0, fstat(3, &st));
  BENCHMARK(100, 1, WriteBlockSync(st.st_blksize));
  BENCHMARK(100, 1, ReadBlockSync(st.st_blksize));
  ASSERT_SYS(0, 0, close(3));
}
