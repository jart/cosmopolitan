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
#include "libc/proc/posix_spawn.h"
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/proc/proc.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/x/x.h"
#include "third_party/nsync/mu.h"

const char kTinyLinuxExit[128] = {
    0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01, 0x00,  // ⌂ELF☻☺☺ 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //         
    0x02, 0x00, 0x3e, 0x00, 0x01, 0x00, 0x00, 0x00,  // ☻ > ☺   
    0x78, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,  // x @     
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // @       
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //         
    0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x38, 0x00,  //     @ 8 
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ☺       
    0x01, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,  // ☺   ♣   
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //         
    0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,  //   @     
    0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,  //   @     
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Ç       
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Ç       
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //  ►      
    0x6a, 0x2a, 0x5f, 0x6a, 0x3c, 0x58, 0x0f, 0x05,  // j*_j<X☼♣
};

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

char *GetHost(void) {
  static char host[256];
  unassert(!gethostname(host, sizeof(host)));
  return host;
}

long GetRss(void) {
  struct rusage ru;
  unassert(!getrusage(RUSAGE_SELF, &ru));
  return ru.ru_maxrss * 1024;
}

long GetSize(const char *path) {
  struct stat st;
  unassert(!stat(path, &st));
  return st.st_size;
}

__attribute__((__constructor__)) static void init(void) {
  switch (atoi(nulltoempty(getenv("THE_DOGE")))) {
    case 42:
      exit(42);
    default:
      break;
  }
}

TEST(posix_spawn, self) {
  int ws, pid;
  char *prog = GetProgramExecutableName();
  char *args[] = {prog, NULL};
  char *envs[] = {"THE_DOGE=42", NULL};
  EXPECT_EQ(0, posix_spawn(&pid, prog, NULL, NULL, args, envs));
  EXPECT_NE(-1, waitpid(pid, &ws, 0));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(42, WEXITSTATUS(ws));
}

TEST(posix_spawn, ape) {
  int ws, pid;
  char *prog = "./life.com";
  char *args[] = {prog, 0};
  char *envs[] = {0};
  testlib_extract("/zip/life.com", prog, 0755);
  ASSERT_EQ(0, posix_spawn(&pid, prog, NULL, NULL, args, envs));
  ASSERT_NE(-1, waitpid(pid, &ws, 0));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(42, WEXITSTATUS(ws));
}

TEST(posix_spawn, elf) {
  if (IsXnu() || IsWindows() || IsMetal()) return;
  int ws, pid;
  char *prog = "./life.elf";  // assimilate -bcef
  char *args[] = {prog, 0};
  char *envs[] = {0};
  testlib_extract("/zip/life.elf", prog, 0755);
  ASSERT_EQ(0, posix_spawn(&pid, prog, NULL, NULL, args, envs));
  ASSERT_NE(-1, waitpid(pid, &ws, 0));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(42, WEXITSTATUS(ws));
}

TEST(posix_spawn, pipe) {
  char buf[10];
  int p[2], pid, status;
  const char *pn = "./echo.com";
  posix_spawn_file_actions_t fa;
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  ASSERT_SYS(0, 0, pipe(p));
  ASSERT_EQ(0, posix_spawn_file_actions_init(&fa));
  ASSERT_EQ(0, posix_spawn_file_actions_addclose(&fa, p[0]));
  ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&fa, p[1], 1));
  ASSERT_EQ(0, posix_spawn_file_actions_addclose(&fa, p[1]));
  ASSERT_EQ(
      0, posix_spawnp(&pid, pn, &fa, 0, (char *[]){(void *)pn, "hello", 0}, 0));
  ASSERT_SYS(0, 0, close(p[1]));
  ASSERT_SYS(0, pid, waitpid(pid, &status, 0));
  ASSERT_SYS(0, 6, read(p[0], buf, sizeof(buf)));
  ASSERT_SYS(0, 0, close(p[0]));
  ASSERT_EQ(0, posix_spawn_file_actions_destroy(&fa));
}

TEST(posix_spawn, chdir) {
  int ws, pid, p[2];
  char buf[16] = {0};
  char *args[] = {"cocmd.com", "-c", "cat hello.txt", 0};
  char *envs[] = {0};
  posix_spawn_file_actions_t fa;
  testlib_extract("/zip/cocmd.com", "cocmd.com", 0755);
  ASSERT_SYS(0, 0, mkdir("subdir", 0777));
  ASSERT_SYS(0, 0, xbarf("subdir/hello.txt", "hello\n", -1));
  ASSERT_SYS(0, 0, pipe2(p, O_CLOEXEC));
  ASSERT_EQ(0, posix_spawn_file_actions_init(&fa));
  ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&fa, p[1], 1));
  ASSERT_EQ(0, posix_spawn_file_actions_addchdir_np(&fa, "subdir"));
  ASSERT_EQ(0, posix_spawn(&pid, "../cocmd.com", &fa, 0, args, envs));
  ASSERT_EQ(0, posix_spawn_file_actions_destroy(&fa));
  ASSERT_SYS(0, 0, close(p[1]));
  ASSERT_NE(-1, waitpid(pid, &ws, 0));
  ASSERT_EQ(0, ws);
  ASSERT_SYS(0, 6, read(p[0], buf, sizeof(buf)));
  ASSERT_STREQ("hello\n", buf);
  ASSERT_SYS(0, 0, close(p[0]));
}

_Thread_local atomic_int gotsome;

void OhMyGoth(int sig) {
  ++gotsome;
}

// time for a vfork() clone() signal bloodbath
TEST(posix_spawn, torture) {
  if (1) return;
  int n = 10;
  int ws, pid;
  sigset_t allsig;
  posix_spawnattr_t attr;
  posix_spawn_file_actions_t fa;
  signal(SIGUSR2, OhMyGoth);
  sigfillset(&allsig);
  if (!fileexists("echo.com")) {
    testlib_extract("/zip/echo.com", "echo.com", 0755);
  }
  // XXX: NetBSD doesn't seem to let us set the scheduler to itself ;_;
  ASSERT_EQ(0, posix_spawnattr_init(&attr));
  ASSERT_EQ(0, posix_spawnattr_setflags(
                   &attr, POSIX_SPAWN_SETSIGDEF | POSIX_SPAWN_SETSIGDEF |
                              POSIX_SPAWN_SETPGROUP | POSIX_SPAWN_SETSIGMASK |
                              (IsLinux() ? POSIX_SPAWN_SETSCHEDULER : 0)));
  ASSERT_EQ(0, posix_spawnattr_setsigmask(&attr, &allsig));
  ASSERT_EQ(0, posix_spawnattr_setsigdefault(&attr, &allsig));
  ASSERT_EQ(0, posix_spawn_file_actions_init(&fa));
  ASSERT_EQ(0, posix_spawn_file_actions_addclose(&fa, 0));
  ASSERT_EQ(
      0, posix_spawn_file_actions_addopen(&fa, 1, "/dev/null", O_WRONLY, 0644));
  ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&fa, 1, 0));
  for (int i = 0; i < n; ++i) {
    char *volatile zzz = malloc(13);
    volatile int fd = open("/dev/null", O_WRONLY);
    char *args[] = {"./echo.com", NULL};
    char *envs[] = {NULL};
    raise(SIGUSR2);
    ASSERT_EQ(0, posix_spawn(&pid, "./echo.com", &fa, &attr, args, envs));
    ASSERT_FALSE(__vforked);
    ASSERT_NE(-1, waitpid(pid, &ws, 0));
    EXPECT_FALSE(WIFSIGNALED(ws));
    EXPECT_EQ(0, WTERMSIG(ws));
    EXPECT_EQ(0, WEXITSTATUS(ws));
    close(fd);
    free(zzz);
  }
  // TODO(jart): Why does it deliver 2x the signals sometimes?
  ASSERT_NE(0, gotsome);
  ASSERT_EQ(0, posix_spawn_file_actions_destroy(&fa));
  ASSERT_EQ(0, posix_spawnattr_destroy(&attr));
}

void *Torturer(void *arg) {
  posix_spawn_torture();
  return 0;
}

TEST(posix_spawn, agony) {
  int i, n = 4;
  pthread_t *t = gc(malloc(sizeof(pthread_t) * n));
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  for (i = 0; i < n; ++i) {
    ASSERT_EQ(0, pthread_create(t + i, 0, Torturer, 0));
  }
  for (i = 0; i < n; ++i) {
    ASSERT_EQ(0, pthread_join(t[i], 0));
  }
}

void EmptySigHandler(int sig) {
}

TEST(posix_spawn, etxtbsy) {
  if (IsWindows()) return;  // can't deliver signals between processes
  if (IsXnu()) return;      // they don't appear impacted by this race condition
  if (IsNetbsd()) return;   // they don't appear impacted by this race condition
  if (IsOpenbsd()) return;  // they don't appear impacted by this race condition
  int ws, me, pid, thief;
  char *prog = "./life.com";
  char *args[] = {prog, 0};
  char *envs[] = {0};
  sigset_t ss, old;
  testlib_extract("/zip/life.com", prog, 0755);
  for (int i = 0; i < 2; ++i) {
    sigemptyset(&ss);
    sigaddset(&ss, SIGUSR1);
    sigprocmask(SIG_BLOCK, &ss, &old);
    sigdelset(&ss, SIGUSR1);
    ASSERT_SYS(0, 3, open(prog, O_RDWR));
    signal(SIGUSR1, EmptySigHandler);
    me = getpid();
    if (!(thief = fork())) {
      ASSERT_SYS(0, 0, kill(me, SIGUSR1));
      ASSERT_SYS(EINTR, -1, sigsuspend(&ss));
      _Exit(0);
    }
    EXPECT_SYS(0, 0, close(3));
    EXPECT_SYS(EINTR, -1, sigsuspend(&ss));
    EXPECT_EQ(ETXTBSY, posix_spawn(&pid, prog, NULL, NULL, args, envs));
    EXPECT_EQ(0, kill(thief, SIGUSR1));
    EXPECT_EQ(thief, wait(&ws));
    ASSERT_EQ(0, ws);
    sigprocmask(SIG_SETMASK, &old, 0);
    unassert(!errno);
  }
}

int sigchld_pid;
volatile bool sigchld_got_signal;

// 1. Test SIGCHLD is delivered due to lack of waiters.
// 2. Test wait4() returns ECHILD when nothing remains.
// 3. Test wait4() failing doesn't clobber *ws and *ru.
// 4. Check information passed in siginfo and ucontext.
void OnSigchld(int sig, siginfo_t *si, void *arg) {
  int ws;
  struct rusage ru;
  ucontext_t *ctx = arg;
  EXPECT_NE(-1, wait4(sigchld_pid, &ws, 0, &ru));
  EXPECT_SYS(ECHILD, -1, wait4(sigchld_pid, &ws, 0, &ru));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(42, WEXITSTATUS(ws));
  EXPECT_EQ(SIGCHLD, sig);
  EXPECT_EQ(SIGCHLD, si->si_signo);
  // TODO(jart): find a safer way to deliver signals on win32
  if (!IsWindows()) {
    // TODO(jart): what's up with openbsd?
    if (!IsOpenbsd()) {
      EXPECT_EQ(CLD_EXITED, si->si_code);
      EXPECT_EQ(sigchld_pid, si->si_pid);
    }
    EXPECT_EQ(getuid(), si->si_uid);
  }
  EXPECT_NE(NULL, ctx);
  sigchld_got_signal = true;
}

TEST(posix_spawn, sigchld) {
  struct sigaction newsa, oldsa;
  sigset_t oldmask, blocksigchld, unblockall;
  char *prog = GetProgramExecutableName();
  char *args[] = {prog, NULL};
  char *envs[] = {"THE_DOGE=42", NULL};
  newsa.sa_flags = SA_SIGINFO;
  newsa.sa_sigaction = OnSigchld;
  sigemptyset(&newsa.sa_mask);
  ASSERT_SYS(0, 0, sigaction(SIGCHLD, &newsa, &oldsa));
  sigemptyset(&blocksigchld);
  sigaddset(&blocksigchld, SIGCHLD);
  ASSERT_SYS(0, 0, sigprocmask(SIG_BLOCK, &blocksigchld, &oldmask));
  EXPECT_EQ(0, posix_spawn(&sigchld_pid, prog, NULL, NULL, args, envs));
  sigemptyset(&unblockall);
  EXPECT_SYS(EINTR, -1, sigsuspend(&unblockall));
  EXPECT_TRUE(sigchld_got_signal);
  EXPECT_SYS(0, 0, sigaction(SIGCHLD, &oldsa, 0));
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &oldmask, 0));
}

////////////////////////////////////////////////////////////////////////////////

void ForkExecveWait(const char *prog) {
  int ws;
  if (!fork()) {
    execve(prog, (char *[]){(char *)prog, 0}, (char *[]){0});
    _Exit(127);
  }
  ASSERT_NE(-1, wait(&ws));
  ASSERT_EQ(42, WEXITSTATUS(ws));
}

void VforkExecveWait(const char *prog) {
  int ws;
  if (!vfork()) {
    execve(prog, (char *[]){(char *)prog, 0}, (char *[]){0});
    _Exit(127);
  }
  ASSERT_NE(-1, wait(&ws));
  ASSERT_EQ(42, WEXITSTATUS(ws));
}

void PosixSpawnWait(const char *prog) {
  int ws, pid;
  char *args[] = {(char *)prog, 0};
  char *envs[] = {0};
  ASSERT_EQ(0, posix_spawn(&pid, prog, NULL, NULL, args, envs));
  ASSERT_NE(-1, waitpid(pid, &ws, 0));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(42, WEXITSTATUS(ws));
}

BENCH(posix_spawn, bench) {
  long n = 1L * 1000 * 1000;
  memset(gc(malloc(n)), -1, n);
  creat("tiny64", 0755);
  write(3, kTinyLinuxExit, 128);
  close(3);
  testlib_extract("/zip/life.com", "life.com", 0755);
  testlib_extract("/zip/life.elf", "life.elf", 0755);
  testlib_extract("/zip/life-pe.com", "life-pe.com", 0755);
  kprintf("%s %s (MODE=" MODE
          " rss=%'zu tiny64=%'zu life.com=%'zu life.elf=%'zu)\n",
          __describe_os(), GetHost(), GetRss(), GetSize("tiny64"),
          GetSize("life.com"), GetSize("life.elf"));
  ForkExecveWait("./life.com");
  EZBENCH2("posix_spawn life.com", donothing, PosixSpawnWait("./life.com"));
  EZBENCH2("vfork life.com", donothing, VforkExecveWait("./life.com"));
  EZBENCH2("fork life.com", donothing, ForkExecveWait("./life.com"));
  if (IsWindows()) {
    EZBENCH2("posix_spawn life-pe.com", donothing,
             PosixSpawnWait("./life-pe.com"));
    EZBENCH2("vfork life-pe.com", donothing, VforkExecveWait("./life-pe.com"));
    EZBENCH2("fork life-pe.com", donothing, ForkExecveWait("./life-pe.com"));
  }
  if (IsXnu() || IsWindows() || IsMetal()) return;
  EZBENCH2("posix_spawn life.elf", donothing, PosixSpawnWait("./life.elf"));
  EZBENCH2("vfork life.elf", donothing, VforkExecveWait("./life.elf"));
  EZBENCH2("fork life.elf", donothing, ForkExecveWait("./life.elf"));
#ifdef __x86_64__
  if (!IsLinux()) return;
  EZBENCH2("posix_spawn tiny64", donothing, PosixSpawnWait("tiny64"));
  EZBENCH2("vfork tiny64", donothing, VforkExecveWait("tiny64"));
  EZBENCH2("fork tiny64", donothing, ForkExecveWait("tiny64"));
#endif
}
