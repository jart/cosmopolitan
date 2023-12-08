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
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

#define PROCESSES  8
#define ITERATIONS 100000

struct SharedMemory {
  pthread_mutex_t mutex;
  volatile long x;
} * shm;

void Worker(void) {
  long t;
  for (int i = 0; i < ITERATIONS; ++i) {
    pthread_mutex_lock(&shm->mutex);
    t = shm->x;
    t += 1;
    shm->x = t;
    pthread_mutex_unlock(&shm->mutex);
  }
}

TEST(lockipc, mutex) {
  int e, rc, ws, pid;

  // create shared memory
  shm = _mapshared(FRAMESIZE);

  // create shared mutex
  pthread_mutexattr_t mattr;
  pthread_mutexattr_init(&mattr);
  pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_NORMAL);
  pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&shm->mutex, &mattr);
  pthread_mutexattr_destroy(&mattr);

  // create processes
  for (int i = 0; i < PROCESSES; ++i) {
    ASSERT_NE(-1, (rc = fork()));
    if (!rc) {
      Worker();
      _Exit(0);
    }
  }

  // wait for processes to finish
  for (;;) {
    e = errno;
    if ((pid = waitpid(-1, &ws, 0)) != -1) {
      if (WIFSIGNALED(ws)) {
        kprintf("process %d terminated with %G\n", pid, WTERMSIG(ws));
        testlib_incrementfailed();
      } else if (WEXITSTATUS(ws)) {
        kprintf("process %d exited with %d\n", pid, WEXITSTATUS(ws));
        testlib_incrementfailed();
      }
    } else {
      ASSERT_EQ(ECHILD, errno);
      errno = e;
      break;
    }
  }

  EXPECT_EQ(PROCESSES * ITERATIONS, shm->x);
  ASSERT_EQ(0, pthread_mutex_destroy(&shm->mutex));
  ASSERT_SYS(0, 0, munmap(shm, FRAMESIZE));
}
