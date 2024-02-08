/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Nhat Pham <nphamcs@gmail.com>                                 │
│ Copyright 2023 Stephen Gregoratto <dev@sgregoratto.me>                       │
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
#include "libc/calls/cachestat.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/statfs.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/rand.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

static size_t pagesize;

bool HasCachestatSupport(void) {
  return IsLinux() && cachestat(-1, 0, 0, 0) == -1 && errno == EBADF;
}

void SetUpOnce(void) {
  if (!HasCachestatSupport()) {
    kprintf("warning: cachestat not supported on this systemL %m\n");
    exit(0);
  }
  testlib_enable_tmp_setup_teardown();
  pagesize = (size_t)getauxval(AT_PAGESZ);
  // ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath", 0));
}

TEST(cachestat, testCachestatOnDevices) {
  const char *const files[] = {
      "/dev/zero", "/dev/null", "/dev/urandom", "/proc/version", "/proc",
  };
  struct cachestat_range range = {0, 4 * pagesize};
  struct cachestat cs;
  for (size_t i = 0; i < ARRAYLEN(files); i++) {
    ASSERT_SYS(0, 3, open(files[i], O_RDONLY));
    ASSERT_SYS(0, 0, cachestat(3, &range, &cs, 0));
    ASSERT_SYS(0, 0, close(3));
  }
}

TEST(cachestat, testCachestatAfterWrite) {
  size_t size = 4 * pagesize;
  char *data = gc(xmalloc(size));
  ASSERT_SYS(0, size, getrandom(data, size, 0));
  // TODO: handle EINTR like xbarf
  ASSERT_SYS(0, 3, open("tmpfilecachestat", O_CREAT | O_RDWR, 0600));
  ASSERT_SYS(0, size, write(3, data, size));
  struct cachestat_range range = {0, size};
  struct cachestat cs;
  ASSERT_SYS(0, 0, cachestat(3, &range, &cs, 0));
  ASSERT_EQ(4, cs.nr_cache + cs.nr_evicted,
            "total number of evicted pages is off.");
  ASSERT_SYS(0, 0, close(3));
}

#define TMPFS_MAGIC 0x01021994
TEST(cachestat, testCachestatSyncNoDirty) {
  size_t size = 4 * pagesize;
  char *data = gc(xmalloc(size));
  ASSERT_SYS(0, size, getrandom(data, size, 0));
  // TODO: handle EINTR like xbarf
  ASSERT_SYS(0, 3, open("tmpfilecachestat", O_CREAT | O_RDWR, 0600));
  ASSERT_SYS(0, size, write(3, data, size));
  struct cachestat_range range = {0, size};
  struct cachestat cs;
  ASSERT_SYS(0, 0, cachestat(3, &range, &cs, 0));
  ASSERT_EQ(4, cs.nr_cache + cs.nr_evicted,
            "total number of evicted pages is off.");
  struct statfs statfs;
  ASSERT_SYS(0, 0, fstatfs(3, &statfs));
  if (statfs.f_type == TMPFS_MAGIC) goto done;
  ASSERT_SYS(0, 0, fsync(3));
  ASSERT_SYS(0, 0, cachestat(3, &range, &cs, 0));
  EXPECT_EQ(0, cs.nr_dirty,
            "dirty pages should be zero after fsync, got %llu\n", cs.nr_dirty);
done:
  ASSERT_SYS(0, 0, close(3));
}

TEST(cachestat, testCachestatShmem) {
  size_t filesize = 512 * 2 * pagesize;  // 2 2MB huge pages.
  size_t compute_len = 512 * pagesize;
  unsigned long num_pages = compute_len / pagesize;
  char *data = gc(xmalloc(filesize));
  ASSERT_SYS(0, filesize, getrandom(data, filesize, 0));
  ASSERT_SYS(0, 3, shm_open("tmpshmcstat", O_CREAT | O_RDWR, 0600));
  ASSERT_SYS(0, 0, ftruncate(3, filesize));
  ASSERT_SYS(0, filesize, write(3, data, filesize));
  struct cachestat_range range = {pagesize, compute_len};
  struct cachestat cs;
  ASSERT_SYS(0, 0, cachestat(3, &range, &cs, 0));
  ASSERT_EQ(num_pages, cs.nr_cache + cs.nr_evicted,
            "total number of cached and evicted pages is off.\n");
  ASSERT_SYS(0, 0, shm_unlink("tmpshmcstat"));
  ASSERT_SYS(0, 0, close(3));
}
