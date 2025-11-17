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
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/temp.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

TEST(setrlimit, RLIMIT_AS) {
  struct rlimit old;
  ASSERT_SYS(0, 0, getrlimit(RLIMIT_AS, &old));
  struct rlimit neu = old;
  neu.rlim_cur = 0;
  ASSERT_SYS(0, 0, setrlimit(RLIMIT_AS, &neu));
  ASSERT_SYS(
      ENOMEM, MAP_FAILED,
      mmap(0, 1, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
  ASSERT_SYS(0, 0, setrlimit(RLIMIT_AS, &old));
}

TEST(setrlimit, RLIMIT_NOFILE) {
  int pfds[2];
  struct rlimit old;
  ASSERT_SYS(0, 0, getrlimit(RLIMIT_NOFILE, &old));
  struct rlimit neu = old;
  // FreeBSD dup() has a bug where RLIMIT_NOFILE is used to validate the
  // argument, which causes it to raise EBADF if rlim_cur is set to zero
  neu.rlim_cur = 2;
  ASSERT_SYS(0, 0, setrlimit(RLIMIT_NOFILE, &neu));
  ASSERT_SYS(EMFILE, -1, open("/", O_RDONLY | O_DIRECTORY));
  ASSERT_SYS(EMFILE, -1, dup(1));
  ASSERT_SYS(EMFILE, -1, pipe(pfds));
  ASSERT_SYS(EMFILE, -1, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, setrlimit(RLIMIT_NOFILE, &old));
}

volatile sig_atomic_t gotsig;

void onsig(int sig) {
  gotsig = sig;
}

TEST(setrlimit, RLIMIT_FSIZE) {

  // set file size limit to zero
  struct rlimit old;
  ASSERT_SYS(0, 0, getrlimit(RLIMIT_FSIZE, &old));
  struct rlimit neu = old;
  neu.rlim_cur = 0;
  ASSERT_SYS(0, 0, setrlimit(RLIMIT_FSIZE, &neu));

  // create file
  int fd;
  char path[] = "/tmp/setrlimit_test.XXXXXX";
  ASSERT_NE(-1, (fd = mkstemp(path)));

  // attempt to write to file
  signal(SIGXFSZ, onsig);
  ASSERT_SYS(EFBIG, -1, write(fd, "x", 1));
  ASSERT_EQ(SIGXFSZ, gotsig);
  gotsig = 0;

  // make sure data didn't get written
  struct stat st;
  ASSERT_SYS(0, 0, fstat(fd, &st));
  ASSERT_EQ(0, st.st_size);

  // increase file size limit
  neu.rlim_cur = 1;
  ASSERT_SYS(0, 0, setrlimit(RLIMIT_FSIZE, &neu));

  // attempt to write to file
  ASSERT_SYS(0, 1, write(fd, "x", 1));
  ASSERT_EQ(0, gotsig);

  // make sure file size increased
  ASSERT_SYS(0, 0, fstat(fd, &st));
  ASSERT_EQ(1, st.st_size);

  // attempt to write to file, again
  ASSERT_SYS(EFBIG, -1, write(fd, "x", 1));
  ASSERT_EQ(SIGXFSZ, gotsig);
  gotsig = 0;

  // test pwrite does the right thing
  ASSERT_SYS(0, 1, pwrite(fd, "x", 1, 0));
  ASSERT_SYS(EFBIG, -1, pwrite(fd, "x", 1, 1));
  ASSERT_EQ(SIGXFSZ, gotsig);
  gotsig = 0;

  // test we're allowed to seek beyond the limit
  ASSERT_SYS(0, 2, lseek(fd, 2, SEEK_SET));

  // test we're still allowed to read beyond the limit
  neu.rlim_cur = 0;
  ASSERT_SYS(0, 0, setrlimit(RLIMIT_FSIZE, &neu));
  ASSERT_SYS(0, 0, lseek(fd, 0, SEEK_SET));
  ASSERT_SYS(0, 1, read(fd, (char[]){0}, 1));
  ASSERT_SYS(0, 1, pread(fd, (char[]){0}, 1, 0));
  ASSERT_EQ(0, gotsig);

  // remove file
  ASSERT_SYS(0, 0, close(fd));
  ASSERT_SYS(0, 0, unlink(path));

  // restore limit
  ASSERT_SYS(0, 0, setrlimit(RLIMIT_FSIZE, &old));
}

void *rlimit_cpu_soft_fork_test(void *) {
  int child;
  ASSERT_NE(-1, (child = fork()));
  if (!child) {
    ASSERT_SYS(0, 0, setrlimit(RLIMIT_CPU, &(struct rlimit){1, 10}));
    for (;;)
      pthread_yield_np();
  }
  int ws;
  ASSERT_NE(-1, wait(&ws));
  ASSERT_TRUE(WIFSIGNALED(ws));
  ASSERT_TRUE(WTERMSIG(ws) == SIGKILL || WTERMSIG(ws) == SIGXCPU);
  return 0;
}

void *rlimit_cpu_hard_fork_test(void *) {
  int child;
  ASSERT_NE(-1, (child = fork()));
  if (!child) {
    ASSERT_SYS(0, 0, setrlimit(RLIMIT_CPU, &(struct rlimit){1, 1}));
    for (;;)
      pthread_yield_np();
  }
  int ws;
  ASSERT_NE(-1, wait(&ws));
  ASSERT_TRUE(WIFSIGNALED(ws));
  ASSERT_TRUE(WTERMSIG(ws) == SIGKILL || WTERMSIG(ws) == SIGXCPU);
  return 0;
}

__attribute__((__constructor__)) static void init(void) {
  if (__argc == 2 && !strcmp(__argv[1], "--hammer-time"))
    for (;;)
      pthread_yield_np();
}

void *rlimit_cpu_hard_exec_test(void *) {
  int child;
  // TODO(jart): Why does vfork() here rarely deadlock on Windows?
  ASSERT_NE(-1, (child = fork()));
  if (!child) {
    ASSERT_SYS(0, 0, setrlimit(RLIMIT_CPU, &(struct rlimit){1, 1}));
    const char *prog = GetProgramExecutableName();
    execl(prog, prog, "--hammer-time", NULL);
    _Exit(127);
  }
  int ws;
  ASSERT_NE(-1, wait(&ws));
  ASSERT_TRUE(WIFSIGNALED(ws));
  ASSERT_TRUE(WTERMSIG(ws) == SIGKILL || WTERMSIG(ws) == SIGXCPU);
  return 0;
}

TEST(setrlimit, RLIMIT_CPU) {
  if (IsXnu())
    return;  // TOOD: Why doesn't RLIMIT_CPU on XNU work?
  pthread_t th1, th2, th3;
  ASSERT_EQ(0, pthread_create(&th1, 0, rlimit_cpu_soft_fork_test, 0));
  ASSERT_EQ(0, pthread_create(&th2, 0, rlimit_cpu_hard_fork_test, 0));
  ASSERT_EQ(0, pthread_create(&th3, 0, rlimit_cpu_hard_exec_test, 0));
  ASSERT_EQ(0, pthread_join(th1, 0));
  ASSERT_EQ(0, pthread_join(th2, 0));
  ASSERT_EQ(0, pthread_join(th3, 0));
}
