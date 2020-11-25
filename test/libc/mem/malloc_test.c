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
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/rand/rand.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/testlib.h"

#define N 1024
#define M 20

TEST(malloc, test) {
  static struct stat st;
  static volatile int i, j, k, *A[4096], fds[M], *maps[M], mapsizes[M];
  memset(fds, -1, sizeof(fds));
  memset(maps, -1, sizeof(maps));
  for (i = 0; i < N * M; ++i) {
    j = rand() % ARRAYLEN(A);
    if (A[j]) {
      ASSERT_EQ(j, A[j][0]);
      A[j] = realloc(A[j], max(sizeof(int), rand() % N));
      ASSERT_NE(NULL, A[j]);
      ASSERT_EQ(j, A[j][0]);
      free(A[j]);
      A[j] = NULL;
    } else {
      A[j] = malloc(max(sizeof(int), rand() % N));
      ASSERT_NE(NULL, A[j]);
      A[j][0] = j;
    }
    if (i % M == 0) {
      k = rand() % M;
      if (fds[k] == -1) {
        ASSERT_NE(-1, (fds[k] = open(program_invocation_name, O_RDONLY)));
        ASSERT_NE(-1, fstat(fds[k], &st));
        ASSERT_NE(MAP_FAILED,
                  (maps[k] = mmap(NULL, (mapsizes[k] = st.st_size), PROT_READ,
                                  MAP_SHARED, fds[k], 0)));
      } else {
        ASSERT_NE(-1, munmap(maps[k], mapsizes[k]));
        ASSERT_NE(-1, close(fds[k]));
        fds[k] = -1;
      }
    }
  }
  for (i = 0; i < ARRAYLEN(A); ++i) free(A[i]);
  for (i = 0; i < ARRAYLEN(maps); ++i) munmap(maps[i], mapsizes[i]);
  for (i = 0; i < ARRAYLEN(fds); ++i) close(fds[i]);
  malloc_trim(0);
}
