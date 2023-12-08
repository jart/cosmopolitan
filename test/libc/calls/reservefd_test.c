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
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/tls.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"

__static_yoink("zipos");
__static_yoink("libc/testlib/hyperion.txt");

#define THREADS 8

void SetUpOnce(void) {
  for (int i = 3; i < 200; ++i) {
    close(i);
  }
  errno = 0;
  ASSERT_SYS(0, 0, pledge("stdio rpath", 0));
}

void PullSomeZipFilesIntoLinkage(void) {
  gmtime(0);
}

TEST(reservefd, testGrowthOfFdsDataStructure) {
  int i, n;
  struct rlimit rlim;
  n = 1700;  // pe '2**16/40' → 1638 (likely value of g_fds.n)
  if (!getrlimit(RLIMIT_NOFILE, &rlim)) {
    n = MIN(n, rlim.rlim_cur - 3);
  } else {
    errno = 0;
  }
  for (i = 0; i < n; ++i) {
    ASSERT_SYS(0, i + 3, open("/zip/usr/share/zoneinfo/UTC", O_RDONLY));
  }
  ASSERT_GT(g_fds.n, 16);
  for (i = 0; i < n; ++i) {
    ASSERT_SYS(0, 0, close(i + 3));
  }
}

void OnSigAlrm(int sig, siginfo_t *si, void *ctx) {
  int rc, fd;
  char buf[64];
  ASSERT_NE(-1, (fd = open("/zip/libc/testlib/hyperion.txt", O_RDONLY)));
  for (;;) {
    rc = read(fd, buf, 64);
    if (rc != -1) {
      ASSERT_EQ(64, rc);
      break;
    } else {
      ASSERT_EQ(EINTR, errno);
      errno = 0;
    }
  }
  ASSERT_EQ(0, memcmp(kHyperion, buf, 64));
  close(fd);  // can eintr which doesn't matter
}

int Worker(void *p, int tid) {
  char buf[64];
  int i, rc, fd;
  for (i = 0; i < 64; ++i) {
    ASSERT_NE(-1, (fd = open("/zip/libc/testlib/hyperion.txt", O_RDONLY)));
    usleep(_rand64() % 100);
    for (;;) {
      rc = read(fd, buf, 64);
      if (rc != -1) {
        ASSERT_EQ(64, rc);
        break;
      } else {
        ASSERT_EQ(EINTR, errno);
        errno = 0;
      }
    }
    ASSERT_EQ(0, memcmp(kHyperion, buf, 64));
    close(fd);
  }
  return 0;
}
