/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nt/accounting.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/files.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/processinformation.h"
#include "libc/nt/struct/startupinfo.h"
#include "libc/nt/synchronization.h"
#include "libc/proc/describefds.internal.h"
#include "libc/proc/ntspawn.h"
#include "libc/proc/posix_spawn.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/pib.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(fork, testPipes) {
  int a, b;
  int ws, pid;
  int pipefds[2];
  ASSERT_NE(-1, pipe(pipefds));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    a = 31337;
    close(pipefds[0]);
    write(pipefds[1], &a, sizeof(a));
    close(pipefds[1]);
    _exit(0);
  }
  EXPECT_NE(-1, close(pipefds[1]));
  EXPECT_EQ(sizeof(b), read(pipefds[0], &b, sizeof(b)));
  EXPECT_NE(-1, close(pipefds[0]));
  EXPECT_NE(-1, waitpid(pid, &ws, 0));
  EXPECT_EQ(31337, b);
}

TEST(fork, testSharedMemory) {
  int ws, pid;
  int stackvar;
  int *sharedvar;
  int *privatevar;
  EXPECT_NE(MAP_FAILED,
            (sharedvar = mmap(0, getpagesize(), PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0)));
  EXPECT_NE(MAP_FAILED,
            (privatevar = mmap(0, getpagesize(), PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  stackvar = 1;
  *sharedvar = 1;
  *privatevar = 1;
  EXPECT_NE(-1, (pid = fork()));
  if (!pid) {
    ++stackvar;
    ++*privatevar;
    ++*sharedvar;
    _exit(0);
  }
  EXPECT_NE(-1, waitpid(pid, &ws, 0));
  EXPECT_EQ(1, stackvar);
  EXPECT_EQ(2, *sharedvar);
  EXPECT_EQ(1, *privatevar);
  EXPECT_SYS(0, 0, munmap(sharedvar, getpagesize()));
  EXPECT_SYS(0, 0, munmap(privatevar, getpagesize()));
}

static volatile bool gotsigusr1;
static volatile bool gotsigusr2;

static void OnSigusr1(int sig) {
  gotsigusr1 = true;
}

static void OnSigusr2(int sig) {
  gotsigusr2 = true;
}

TEST(fork, childToChild) {
  sigset_t mask, oldmask;
  int ws, parent, child1, child2;
  gotsigusr1 = false;
  gotsigusr2 = false;
  parent = getpid();
  signal(SIGUSR1, OnSigusr1);
  signal(SIGUSR2, OnSigusr2);
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
  sigaddset(&mask, SIGUSR2);
  sigprocmask(SIG_BLOCK, &mask, &oldmask);
  ASSERT_NE(-1, (child1 = fork()));
  if (!child1) {
    if (kill(parent, SIGUSR2)) {
      kprintf("%s:%d: error: failed to kill parent: %m\n", __FILE__, __LINE__);
      _Exit(1);
    }
    ASSERT_SYS(EINTR, -1, sigsuspend(0));
    _Exit(!gotsigusr1);
  }
  EXPECT_SYS(EINTR, -1, sigsuspend(0));
  ASSERT_NE(-1, (child2 = fork()));
  if (!child2) {
    if (kill(child1, SIGUSR1)) {
      kprintf("%s:%d: error: failed to kill child1: %m\n", __FILE__, __LINE__);
      _Exit(1);
    }
    _Exit(0);
  }
  ASSERT_NE(-1, wait(&ws));
  EXPECT_EQ(0, ws);
  ASSERT_NE(-1, wait(&ws));
  EXPECT_EQ(0, ws);
  sigprocmask(SIG_SETMASK, &oldmask, 0);
}

TEST(fork, preservesTlsMemory) {
  __get_tls()->tib_errno = 31337;
  SPAWN(fork);
  ASSERT_EQ(31337, __get_tls()->tib_errno);
  EXITS(0);
}

TEST(fork, privateExtraPageData_getsCopiedByFork) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, 1, PROT_WRITE | PROT_READ,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  p[0] = 1;
  p[1] = 2;
  SPAWN(fork);
  ASSERT_EQ(1, p[0]);
  ASSERT_EQ(2, p[1]);
  EXITS(0);
  ASSERT_SYS(0, 0, munmap(p, 1));
}

TEST(fork, sharedExtraPageData_getsResurrectedByFork) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, 1, PROT_WRITE | PROT_READ,
                                  MAP_ANONYMOUS | MAP_SHARED, -1, 0)));
  p[0] = 1;
  p[1] = 2;
  SPAWN(fork);
  ASSERT_EQ(1, p[0]);
  ASSERT_EQ(2, p[1]);
  EXITS(0);
  ASSERT_SYS(0, 0, munmap(p, 1));
}

#define CHECK_TERMSIG                                                    \
  if (WIFSIGNALED(ws)) {                                                 \
    kprintf("%s:%d: error: forked life subprocess terminated with %G\n", \
            __FILE__, __LINE__, WTERMSIG(ws));                           \
    exit(1);                                                             \
  }

#ifdef __x86_64__
static void CreateProcess_in_serial(void) {
  uint32_t status;
  char16_t cwd[PATH_MAX];
  char16_t prog[PATH_MAX];
  struct NtStartupInfo si = {
      .cb = sizeof(struct NtStartupInfo),
      .dwFlags = kNtStartfUsestdhandles,
      .hStdInput = -1,
      .hStdOutput = -1,
      .hStdError = -1,
  };
  struct NtProcessInformation pi;
  GetCurrentDirectory(PATH_MAX, cwd);
  strcpy16(prog, cwd);
  strcat16(prog, u"\\life");
  ASSERT_TRUE(CreateProcess(prog, prog, 0, 0, true, kNtCreateUnicodeEnvironment,
                            0, cwd, &si, &pi));
  ASSERT_TRUE(CloseHandle(pi.hThread));
  ASSERT_EQ(0, WaitForSingleObject(pi.hProcess, -1u));
  ASSERT_TRUE(GetExitCodeProcess(pi.hProcess, &status));
  ASSERT_EQ(42 << 8, status);
  ASSERT_TRUE(CloseHandle(pi.hProcess));
}
#endif

void fork_wait_in_serial(void) {
  int pid, ws;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid)
    _Exit(0);
  ASSERT_NE(-1, waitpid(pid, &ws, 0));
  CHECK_TERMSIG;
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(0, WEXITSTATUS(ws));
}

void vfork_execl_wait_in_serial(void) {
  int pid, ws;
  ASSERT_NE(-1, (pid = vfork()));
  if (!pid) {
    execl("./life", "./life", NULL);
    _Exit(127);
  }
  ASSERT_NE(-1, waitpid(pid, &ws, 0));
  CHECK_TERMSIG;
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(42, WEXITSTATUS(ws));
}

void vfork_wait_in_serial(void) {
  int pid, ws;
  ASSERT_NE(-1, (pid = vfork()));
  if (!pid)
    _Exit(0);
  ASSERT_NE(-1, waitpid(pid, &ws, 0));
  CHECK_TERMSIG;
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(0, WEXITSTATUS(ws));
}

void sys_fork_wait_in_serial(void) {
  int pid, ws;
  ASSERT_NE(-1, (pid = sys_fork()));
  if (!pid) {
    _Exit(0);
  }
  ASSERT_NE(-1, waitpid(pid, &ws, 0));
  CHECK_TERMSIG;
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(0, WEXITSTATUS(ws));
}

void posix_spawn_fork_in_serial(void) {
  int ws, pid;
  char *prog = "./life";
  char *args[] = {prog, NULL};
  char *envs[] = {NULL};
  ASSERT_EQ(0, posix_spawn(&pid, prog, NULL, NULL, args, envs));
  ASSERT_NE(-1, waitpid(pid, &ws, 0));
  CHECK_TERMSIG;
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(42, WEXITSTATUS(ws));
}

void posix_spawn_vfork_in_serial(void) {
  int ws, pid;
  char *prog = "./life";
  char *args[] = {prog, NULL};
  char *envs[] = {NULL};
  posix_spawnattr_t spawnattr;
  ASSERT_EQ(0, posix_spawnattr_init(&spawnattr));
  ASSERT_EQ(0, posix_spawnattr_setflags(&spawnattr, POSIX_SPAWN_USEVFORK));
  ASSERT_EQ(0, posix_spawn(&pid, prog, NULL, &spawnattr, args, envs));
  ASSERT_EQ(0, posix_spawnattr_destroy(&spawnattr));
  ASSERT_NE(-1, waitpid(pid, &ws, 0));
  CHECK_TERMSIG;
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(42, WEXITSTATUS(ws));
}

#define OPS     80
#define THREADS 8

void *do_in_threads_worker(void *arg) {
  void (*func)(void) = arg;
  for (int i = 0; i < OPS / THREADS; ++i)
    func();
  return 0;
}

void do_in_threads(void func(void)) {
  pthread_t th[THREADS];
  for (int i = 0; i < THREADS; ++i)
    ASSERT_EQ(0, pthread_create(&th[i], 0, do_in_threads_worker, func));
  for (int i = 0; i < THREADS; ++i)
    ASSERT_EQ(0, pthread_join(th[i], 0));
}

// NOTE: we leak a lot of signal delivery files because we're spawning
//       non-cosmo processes.
BENCH(fork, bench) {
  if (0 && IsWindows()) {
    testlib_extract("/zip/life-pe.ape", "life", 0755);
  } else {
    testlib_extract("/zip/life", "life", 0755);
  }

  vfork_wait_in_serial();
  vfork_execl_wait_in_serial();
  posix_spawn_fork_in_serial();
  fork_wait_in_serial();

  fprintf(stderr,
          "\nwill %dx fork() calls go faster if we divide it among %d "
          "threads?\n",
          OPS, THREADS);
  BENCHMARK(OPS, 1, fork_wait_in_serial());
  BENCHMARK(1, OPS, do_in_threads(fork_wait_in_serial));

  if (IsWindows()) {
#ifdef __x86_64__
    fprintf(stderr,
            "\nwill %dx CreateProcess() calls go faster if we divide "
            "it among %d threads?\n",
            OPS, THREADS);
    BENCHMARK(OPS, 1, CreateProcess_in_serial());
    BENCHMARK(1, OPS, do_in_threads(CreateProcess_in_serial));
#endif
  } else {
    fprintf(stderr,
            "\nwill %dx sys_fork() calls go faster if we divide it "
            "among %d threads? (if yes, blame mutexes)\n",
            OPS, THREADS);
    BENCHMARK(OPS, 1, sys_fork_wait_in_serial());
    BENCHMARK(1, OPS, do_in_threads(sys_fork_wait_in_serial));
  }

  fprintf(stderr,
          "\nwill %dx vfork() calls go faster if we divide it among %d "
          "threads?\n",
          OPS, THREADS);
  BENCHMARK(OPS, 1, vfork_wait_in_serial());
  BENCHMARK(1, OPS, do_in_threads(vfork_wait_in_serial));

  fprintf(stderr,
          "\nwill %dx posix_spawn(fork)s go faster if we divide it "
          "among %d threads?\n",
          OPS, THREADS);
  BENCHMARK(OPS, 1, posix_spawn_fork_in_serial());
  BENCHMARK(1, OPS, do_in_threads(posix_spawn_fork_in_serial));

  fprintf(stderr,
          "\nwill %dx posix_spawn(vfork)s go faster if we divide it "
          "among %d threads?\n",
          OPS, THREADS);
  BENCHMARK(OPS, 1, posix_spawn_vfork_in_serial());
  BENCHMARK(1, OPS, do_in_threads(posix_spawn_vfork_in_serial));

  fprintf(stderr,
          "\nwill %dx vfork() spawns faster if we divide it among %d "
          "threads?\n",
          OPS, THREADS);
  BENCHMARK(OPS, 1, vfork_execl_wait_in_serial());
  BENCHMARK(1, OPS, do_in_threads(vfork_execl_wait_in_serial));
}
