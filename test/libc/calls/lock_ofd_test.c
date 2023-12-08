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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/flock.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

/**
 * @fileoverview Better Advisory Locks Test
 */

#define PROCESSES  8
#define THREADS    8  // <-- THIS
#define RATIO      3
#define ITERATIONS 10

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

_Thread_local const char *kind;

bool SupportsOfdLocks(void) {
  int e;
  bool r;
  if (!IsLinux()) return false;
  // F_OFD_* was introduced in linux 3.15
  // getrandom() was introduced in linux 3.17
  // testing for getrandom() should be a sure thing w/o creating an fd
  e = errno;
  BLOCK_CANCELATION;
  r = !sys_getrandom(0, 0, 0);
  ALLOW_CANCELATION;
  errno = e;
  return r;
}

void SetUp(void) {
  if (!SupportsOfdLocks()) {
    kprintf("ofd locks not supported on this system\n");
    exit(0);
  }
}

void Log(const char *fmt, ...) {
#if 0
  va_list va;
  char b[512];
  int i, n = sizeof(b);
  va_start(va, fmt);
  i = ksnprintf(b, n, "%s pid=%d tid=%d ", kind, getpid(), gettid());
  i += kvsnprintf(b + i, MAX(0, n - i), fmt, va);
  i += ksnprintf(b + i, MAX(0, n - i), "\n");
  write(2, b, MIN(i, n));
  va_end(va);
#endif
}

void Lock(int fd, int type, long start, long len) {
  int e;
  struct flock lock = {
      .l_type = type,
      .l_whence = SEEK_SET,
      .l_start = start,
      .l_len = len,
  };
  e = errno;
  while (fcntl(fd, F_OFD_SETLK, &lock)) {
    ASSERT_TRUE(errno == EAGAIN || errno == EACCES);
    errno = e;
    Log("flock spinning on %d", fd);
  }
}

void WriteLock(int fd, long start, long len) {
  Lock(fd, F_WRLCK, start, len);
  Log("acquired write lock on %d", fd);
}

void ReadLock(int fd, long start, long len) {
  Lock(fd, F_RDLCK, start, len);
  Log("acquired read lock on %d", fd);
}

void Unlock(int fd, long start, long len) {
  Lock(fd, F_UNLCK, start, len);
  Log("released lock on %d", fd);
}

void *Reader(void *arg) {
  int i, j, fd;
  char buf[10];
  kind = arg;
  ASSERT_NE(-1, (fd = open("db", O_RDONLY)));
  for (j = 0; j < ITERATIONS; ++j) {
    ReadLock(fd, 10, 10);
    for (i = 0; i < 10; ++i) {
      ASSERT_SYS(0, 1, pread(fd, buf + i, 1, 10 + i));
      ASSERT_EQ(buf[0], buf[i]);
    }
    Unlock(fd, 10, 10);
    sched_yield();
  }
  ASSERT_SYS(0, 0, close(fd));
  return 0;
}

void *Writer(void *arg) {
  int i, j, fd;
  char buf[10];
  kind = arg;
  ASSERT_NE(-1, (fd = open("db", O_RDWR)));
  for (j = 0; j < ITERATIONS; ++j) {
    WriteLock(fd, 10, 10);
    for (i = 0; i < 10; ++i) {
      ASSERT_SYS(0, 1, pread(fd, buf + i, 1, 10 + i));
      ASSERT_EQ(buf[0], buf[i]);
    }
    for (i = 0; i < 10; ++i) {
      buf[i]++;
    }
    for (i = 0; i < 10; ++i) {
      ASSERT_SYS(0, 1, pwrite(fd, buf + i, 1, 10 + i));
    }
    Unlock(fd, 10, 10);
    sched_yield();
  }
  ASSERT_SYS(0, 0, close(fd));
  return 0;
}

TEST(posixAdvisoryLocks, threadsAndProcessesFightingForLock) {
  int i, rc, pid, fd, ws;
  pthread_t th[THREADS + 1];

  ASSERT_SYS(0, 3, creat("db", 0644));
  ASSERT_SYS(0, 0, ftruncate(3, 30));
  ASSERT_SYS(0, 0, close(3));

  for (i = 0; i < THREADS; ++i) {
    if (i % RATIO == 0) {
      ASSERT_EQ(0, pthread_create(th + i, 0, Reader, "reader thread"));
    } else {
      ASSERT_EQ(0, pthread_create(th + i, 0, Writer, "writer thread"));
    }
  }

  for (i = 0; i < PROCESSES; ++i) {
    ASSERT_NE(-1, (rc = fork()));
    if (!rc) {
      if (i % RATIO == 0) {
        Writer("writer process");
      } else {
        Reader("reader process");
      }
      _Exit(0);
    }
  }

  ASSERT_NE(-1, (fd = open("db", O_RDWR)));
  Lock(fd, F_WRLCK, 0, 10);
  Lock(fd, F_WRLCK, 20, 10);

  for (i = 0; i < THREADS; ++i) {
    ASSERT_EQ(0, pthread_join(th[i], 0));
  }

  kind = "main process";
  for (;;) {
    int e = errno;
    if ((pid = waitpid(0, &ws, 0)) != -1) {
      if (WIFSIGNALED(ws)) {
        Log("process %d terminated with %G", pid, WTERMSIG(ws));
        testlib_incrementfailed();
      } else if (WEXITSTATUS(ws)) {
        Log("process %d exited with %d", pid, WEXITSTATUS(ws));
        testlib_incrementfailed();
      }
    } else {
      ASSERT_EQ(ECHILD, errno);
      errno = e;
      break;
    }
  }

  ASSERT_SYS(0, 0, close(fd));
}
