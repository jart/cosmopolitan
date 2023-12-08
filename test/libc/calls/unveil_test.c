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
#include "libc/calls/landlock.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/gc.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"

#define EACCES_OR_ENOENT (IsOpenbsd() ? ENOENT : EACCES)

struct stat st;

bool HasUnveilSupport(void) {
  return unveil("", 0) >= 0;
}

bool UnveilCanSecureTruncate(void) {
  int abi = unveil("", 0);
  return abi == 0 || abi >= 3;
}

void SetUpOnce(void) {
  if (!HasUnveilSupport()) {
    fprintf(stderr, "warning: unveil() not supported on this system: %m\n");
    exit(0);
  }
  testlib_enable_tmp_setup_teardown();
}

void SetUp(void) {
  // make sure zipos maps executable into memory early
  ASSERT_SYS(0, 0, stat("/zip/life.elf", &st));
}

TEST(unveil, api_differences) {
  SPAWN(fork);
  ASSERT_SYS(0, 0, mkdir("foo", 0755));
  ASSERT_SYS(0, 0, unveil("foo", "r"));
  if (IsOpenbsd()) {
    // openbsd imposes restrictions immediately
    ASSERT_SYS(ENOENT, -1, open("/", O_RDONLY | O_DIRECTORY));
  } else {
    // restrictions on linux don't go into effect until unveil(0,0)
    ASSERT_SYS(0, 3, open(".", O_RDONLY | O_DIRECTORY));
    ASSERT_SYS(0, 0, close(3));
  }
  ASSERT_SYS(0, 0, unveil(0, 0));
  // error numbers are inconsistent
  ASSERT_SYS(EACCES_OR_ENOENT, -1, open("/", O_RDONLY | O_DIRECTORY));
  // wut
  if (IsLinux()) {
    ASSERT_SYS(0, 3, open("/", O_PATH));  // wut
    ASSERT_SYS(0, 0, stat("/", &st));     // wut
  }
  EXITS(0);
}

TEST(unveil, rx_readOnlyPreexistingExecutable_worksFine) {
  SPAWN(fork);
  ASSERT_SYS(0, 0, mkdir("folder", 0755));
  testlib_extract("/zip/life.elf", "folder/life.elf", 0755);
  ASSERT_SYS(0, 0, unveil("folder", "rx"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  SPAWN(fork);
  execl("folder/life.elf", "folder/life.elf", 0);
  kprintf("execve failed! %s\n", strerror(errno));
  _Exit(127);
  EXITS(42);
  EXITS(0);
}

TEST(unveil, r_noExecutePreexistingExecutable_raisesEacces) {
  SPAWN(fork);
  ASSERT_SYS(0, 0, mkdir("folder", 0755));
  testlib_extract("/zip/life.elf", "folder/life.elf", 0755);
  ASSERT_SYS(0, 0, unveil("folder", "r"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  SPAWN(fork);
  ASSERT_SYS(EACCES, -1, execl("folder/life.elf", "folder/life.elf", 0));
  EXITS(0);
  EXITS(0);
}

TEST(unveil, canBeUsedAgainAfterVfork) {
  ASSERT_SYS(0, 0, touch("bad", 0644));
  ASSERT_SYS(0, 0, touch("good", 0644));
  SPAWN(fork);
  SPAWN(vfork);
  ASSERT_SYS(0, 0, unveil("bad", "r"));
  ASSERT_SYS(0, 0, unveil("good", "r"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(0, 3, open("bad", 0));
  EXITS(0);
  ASSERT_SYS(0, 0, unveil("good", "r"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(0, 3, open("good", 0));
  ASSERT_SYS(EACCES_OR_ENOENT, -1, open("bad", 0));
  EXITS(0);
}

TEST(unveil, rwc_createExecutableFile_isAllowedButCantBeRun) {
  SPAWN(fork);
  ASSERT_SYS(0, 0, mkdir("folder", 0755));
  ASSERT_SYS(0, 0, unveil("folder", "rwc"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  testlib_extract("/zip/life.elf", "folder/life.elf", 0755);
  SPAWN(fork);
  ASSERT_SYS(0, 0, stat("folder/life.elf", &st));
  ASSERT_SYS(EACCES, -1, execl("folder/life.elf", "folder/life.elf", 0));
  EXITS(0);
  EXITS(0);
}

TEST(unveil, rwcx_createExecutableFile_canAlsoBeRun) {
  SPAWN(fork);
  ASSERT_SYS(0, 0, mkdir("folder", 0755));
  ASSERT_SYS(0, 0, unveil("folder", "rwcx"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  testlib_extract("/zip/life.elf", "folder/life.elf", 0755);
  SPAWN(fork);
  ASSERT_SYS(0, 0, stat("folder/life.elf", &st));
  execl("folder/life.elf", "folder/life.elf", 0);
  kprintf("execve failed! %s\n", strerror(errno));
  _Exit(127);
  EXITS(42);
  EXITS(0);
}

TEST(unveil, dirfdHacking_doesntWork) {
  SPAWN(fork);
  ASSERT_SYS(0, 0, mkdir("jail", 0755));
  ASSERT_SYS(0, 0, mkdir("garden", 0755));
  ASSERT_SYS(0, 0, touch("garden/secret.txt", 0644));
  ASSERT_SYS(0, 3, open("garden", O_RDONLY | O_DIRECTORY));
  ASSERT_SYS(0, 0, unveil("jail", "rw"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(EACCES_OR_ENOENT, -1, openat(3, "secret.txt", O_RDONLY));
  EXITS(0);
}

TEST(unveil, mostRestrictivePolicy) {
  if (IsOpenbsd()) return;  // openbsd behaves oddly; see docs
  SPAWN(fork);
  ASSERT_SYS(0, 0, mkdir("jail", 0755));
  ASSERT_SYS(0, 0, mkdir("garden", 0755));
  ASSERT_SYS(0, 0, touch("garden/secret.txt", 0644));
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(EACCES_OR_ENOENT, -1, open("jail", O_RDONLY | O_DIRECTORY));
  ASSERT_SYS(EACCES_OR_ENOENT, -1, open("garden/secret.txt", O_RDONLY));
  EXITS(0);
}

TEST(unveil, overlappingDirectories_inconsistentBehavior) {
  SPAWN(fork);
  ASSERT_SYS(0, 0, makedirs("f1/f2", 0755));
  testlib_extract("/zip/life.elf", "f1/f2/life.elf", 0755);
  ASSERT_SYS(0, 0, unveil("f1", "x"));
  ASSERT_SYS(0, 0, unveil("f1/f2", "r"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  if (IsOpenbsd()) {
    // OpenBSD favors the most restrictive policy
    SPAWN(fork);
    ASSERT_SYS(0, 0, stat("f1/f2/life.elf", &st));
    ASSERT_SYS(EACCES, -1, execl("f1/f2/life.elf", "f1/f2/life.elf", 0));
    EXITS(0);
  } else {
    // Landlock (Linux) uses the union of policies
    //
    // TODO(jart): this test flakes on github actions. it reports an
    //             exit code of 0! find out why this is happening...
    //             so far it's happened to MODE=rel and MODE=tiny...
    //
    // SPAWN(fork);
    // ASSERT_SYS(0, 0, stat("f1/f2/life.elf", &st));
    // execl("f1/f2/life.elf", "f1/f2/life.elf", 0);
    // kprintf("execve failed! %s\n", strerror(errno));
    // _Exit(127);
    // EXITS(42);
  }
  EXITS(0);
}

TEST(unveil, usedTwice_allowedOnLinux) {
  if (IsOpenbsd()) return;
  SPAWN(fork);
  ASSERT_SYS(0, 0, mkdir("jail", 0755));
  ASSERT_SYS(0, 0, xbarf("jail/ok.txt", "hello", 5));
  ASSERT_SYS(0, 0, mkdir("garden", 0755));
  ASSERT_SYS(0, 0, xbarf("garden/secret.txt", "hello", 5));
  ASSERT_SYS(0, 0, mkdir("heaven", 0755));
  ASSERT_SYS(0, 0, xbarf("heaven/verysecret.txt", "hello", 5));
  ASSERT_SYS(0, 0, unveil("jail", "rw"));
  ASSERT_SYS(0, 0, unveil("garden", "rw"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(0, 3, open("garden/secret.txt", O_RDONLY));
  ASSERT_SYS(0, 0, unveil("jail", "rw"));
  ASSERT_SYS(0, 0, unveil("heaven", "rw"));  // not allowed, superset of parent
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(0, 4, open("jail/ok.txt", O_RDONLY));
  ASSERT_SYS(EACCES, -1, open("garden/secret.txt", O_RDONLY));
  ASSERT_SYS(EACCES, -1, open("heaven/verysecret.txt", O_RDONLY));
  EXITS(0);
}

TEST(unveil, truncate_isForbiddenBySeccomp) {
  SPAWN(fork);
  ASSERT_SYS(0, 0, mkdir("jail", 0755));
  ASSERT_SYS(0, 0, mkdir("garden", 0755));
  ASSERT_SYS(0, 0, xbarf("garden/secret.txt", "hello", 5));
  ASSERT_SYS(0, 0, unveil("jail", "rw"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(!UnveilCanSecureTruncate() ? EPERM : EACCES_OR_ENOENT, -1,
             truncate("garden/secret.txt", 0));
  if (IsLinux()) {
    ASSERT_SYS(0, 0, stat("garden/secret.txt", &st));
    ASSERT_EQ(5, st.st_size);
  }
  EXITS(0);
}

TEST(unveil, ftruncate_isForbidden) {
  if (IsOpenbsd()) return;  // b/c O_PATH is a Linux thing
  SPAWN(fork);
  ASSERT_SYS(0, 0, mkdir("jail", 0755));
  ASSERT_SYS(0, 0, mkdir("garden", 0755));
  ASSERT_SYS(0, 0, xbarf("garden/secret.txt", "hello", 5));
  ASSERT_SYS(0, 0, unveil("jail", "rw"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(0, 3, open("garden/secret.txt", O_PATH));
  ASSERT_SYS(EBADF, -1, ftruncate(3, 0));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, stat("garden/secret.txt", &st));
  ASSERT_EQ(5, st.st_size);
  EXITS(0);
}

TEST(unveil, procfs_isForbiddenByDefault) {
  if (IsOpenbsd()) return;
  SPAWN(fork);
  ASSERT_SYS(0, 0, mkdir("jail", 0755));
  ASSERT_SYS(0, 0, unveil("jail", "rw"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(EACCES, -1, open("/proc/self/cmdline", O_RDONLY));
  EXITS(0);
}

void *Worker(void *arg) {
  ASSERT_SYS(EACCES_OR_ENOENT, -1, open("garden/secret.txt", O_RDONLY));
  return 0;
}

TEST(unveil, isInheritedAcrossThreads) {
  pthread_t t;
  SPAWN(fork);
  ASSERT_SYS(0, 0, mkdir("jail", 0755));
  ASSERT_SYS(0, 0, mkdir("garden", 0755));
  ASSERT_SYS(0, 0, xbarf("garden/secret.txt", "hello", 5));
  ASSERT_SYS(0, 0, unveil("jail", "rw"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(0, 0, pthread_create(&t, 0, Worker, 0));
  EXPECT_SYS(0, 0, pthread_join(t, 0));
  EXITS(0);
}

void *Worker2(void *arg) {
  ASSERT_SYS(0, 0, unveil("jail", "rw"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(EACCES_OR_ENOENT, -1, open("garden/secret.txt", O_RDONLY));
  return 0;
}

TEST(unveil, isThreadSpecificOnLinux_isProcessWideOnOpenbsd) {
  pthread_t t;
  SPAWN(fork);
  ASSERT_SYS(0, 0, mkdir("jail", 0755));
  ASSERT_SYS(0, 0, mkdir("garden", 0755));
  ASSERT_SYS(0, 0, xbarf("garden/secret.txt", "hello", 5));
  ASSERT_SYS(0, 0, pthread_create(&t, 0, Worker2, 0));
  EXPECT_SYS(0, 0, pthread_join(t, 0));
  if (IsOpenbsd()) {
    ASSERT_SYS(ENOENT, -1, open("garden/secret.txt", O_RDONLY));
  } else {
    ASSERT_SYS(0, 3, open("garden/secret.txt", O_RDONLY));
  }
  EXITS(0);
}

TEST(unveil, usedTwice_forbidden_worksWithPledge) {
  int ws, pid;
  bool *gotsome;
  ASSERT_NE(-1, (gotsome = _mapshared(FRAMESIZE)));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    // install our first seccomp filter
    ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath unveil", 0));
    ASSERT_SYS(0, 0, mkdir("jail", 0755));
    ASSERT_SYS(0, 0, mkdir("garden", 0755));
    ASSERT_SYS(0, 0, xbarf("garden/secret.txt", "hello", 5));
    ASSERT_SYS(0, 0, unveil("jail", "rw"));
    // committing and locking causes a new bpf filter to be installed
    ASSERT_SYS(0, 0, unveil(0, 0));
    ASSERT_SYS(EACCES_OR_ENOENT, -1, open("garden/secret.txt", O_RDONLY));
    // verify the second filter is working
    ASSERT_SYS(EACCES_OR_ENOENT, -1, open("garden/secret.txt", O_RDONLY));
    // verify the first filter is still working
    *gotsome = true;
    socket(AF_UNIX, SOCK_STREAM, 0);
    _Exit(0);
  }
  ASSERT_NE(-1, wait(&ws));
  ASSERT_TRUE(*gotsome);
  ASSERT_TRUE(WIFSIGNALED(ws));
  ASSERT_EQ(IsOpenbsd() ? SIGABRT : SIGSYS, WTERMSIG(ws));
  EXPECT_SYS(0, 0, munmap(gotsome, FRAMESIZE));
}

TEST(unveil, lotsOfPaths) {
  int i, n;
  SPAWN(fork);
  n = 100;
  for (i = 0; i < n; ++i) {
    ASSERT_SYS(0, 0, touch(xasprintf("%d", i), 0644));
    ASSERT_SYS(0, 0, touch(xasprintf("%d-", i), 0644));
  }
  for (i = 0; i < n; ++i) {
    ASSERT_SYS(0, 0, unveil(xasprintf("%d", i), "rw"));
  }
  ASSERT_SYS(0, 0, unveil(0, 0));
  for (i = 0; i < n; ++i) {
    ASSERT_SYS(0, 3, open(xasprintf("%d", i), O_RDONLY));
    ASSERT_SYS(0, 0, close(3));
    ASSERT_SYS(EACCES_OR_ENOENT, -1, open(xasprintf("%d-", i), O_RDONLY));
  }
  EXITS(0);
}

TEST(unveil, reparent) {
  return;  // need abi 2 :'(
  SPAWN(fork);
  ASSERT_SYS(0, 0, mkdir("x", 0755));
  ASSERT_SYS(0, 0, unveil("x", "rwc"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(0, 0, mkdir("x/y", 0755));
  ASSERT_SYS(0, 0, touch("x/y/z", 0644));
  ASSERT_SYS(0, 0, rename("x/y/z", "x/z"));
  EXITS(0);
}
