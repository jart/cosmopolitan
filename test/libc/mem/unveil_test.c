/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/io.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

STATIC_YOINK("zip_uri_support");

#define EACCES_OR_ENOENT (IsOpenbsd() ? ENOENT : EACCES)

#define SPAWN()                    \
  {                                \
    int ws, pid;                   \
    ASSERT_NE(-1, (pid = fork())); \
    if (!pid) {

#define EXITS(rc)                 \
  _Exit(0);                       \
  }                               \
  ASSERT_NE(-1, wait(&ws));       \
  ASSERT_TRUE(WIFEXITED(ws));     \
  ASSERT_EQ(rc, WEXITSTATUS(ws)); \
  }

char testlib_enable_tmp_setup_teardown;

struct stat st;

static bool SupportsLandlock(void) {
  int e = errno;
  bool r = landlock_create_ruleset(0, 0, LANDLOCK_CREATE_RULESET_VERSION) >= 0;
  errno = e;
  return r;
}

__attribute__((__constructor__)) static void init(void) {
  if (!(IsLinux() && SupportsLandlock()) && !IsOpenbsd()) exit(0);
}

void SetUp(void) {
  // make sure zipos maps executable into memory early
  ASSERT_SYS(0, 0, stat("/zip/life.elf", &st));
}

int extract(const char *from, const char *to, int mode) {
  int fdin, fdout;
  if ((fdin = open(from, O_RDONLY)) == -1) return -1;
  if ((fdout = creat(to, mode)) == -1) {
    close(fdin);
    return -1;
  }
  if (_copyfd(fdin, fdout, -1) == -1) {
    close(fdout);
    close(fdin);
    return -1;
  }
  return close(fdout) | close(fdin);
}

TEST(unveil, api_differences) {
  SPAWN();
  ASSERT_SYS(0, 0, stat("/", &st));
  ASSERT_SYS(0, 0, unveil(".", "rw"));
  if (IsOpenbsd()) {
    // openbsd imposes restrictions immediately
    ASSERT_SYS(ENOENT, -1, open("/", O_RDONLY | O_DIRECTORY));
  } else {
    // restrictions on linux don't go into effect until unveil(0,0)
    ASSERT_SYS(0, 3, open("/", O_RDONLY | O_DIRECTORY));
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
  SPAWN();
  ASSERT_SYS(0, 0, mkdir("folder", 0755));
  ASSERT_SYS(0, 0, extract("/zip/life.elf", "folder/life.elf", 0755));
  ASSERT_SYS(0, 0, unveil("folder", "rx"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  SPAWN();
  execl("folder/life.elf", "folder/life.elf", 0);
  kprintf("execve failed! %s\n", strerror(errno));
  _Exit(127);
  EXITS(42);
  EXITS(0);
}

TEST(unveil, r_noExecutePreexistingExecutable_raisesEacces) {
  SPAWN();
  ASSERT_SYS(0, 0, mkdir("folder", 0755));
  ASSERT_SYS(0, 0, extract("/zip/life.elf", "folder/life.elf", 0755));
  ASSERT_SYS(0, 0, unveil("folder", "r"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  SPAWN();
  ASSERT_SYS(EACCES, -1, execl("folder/life.elf", "folder/life.elf", 0));
  EXITS(0);
  EXITS(0);
}

TEST(unveil, rwc_createExecutableFile_isAllowedButCantBeRun) {
  SPAWN();
  ASSERT_SYS(0, 0, mkdir("folder", 0755));
  ASSERT_SYS(0, 0, unveil("folder", "rwc"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(0, 0, extract("/zip/life.elf", "folder/life.elf", 0755));
  SPAWN();
  ASSERT_SYS(0, 0, stat("folder/life.elf", &st));
  ASSERT_SYS(EACCES, -1, execl("folder/life.elf", "folder/life.elf", 0));
  EXITS(0);
  EXITS(0);
}

TEST(unveil, rwcx_createExecutableFile_canAlsoBeRun) {
  SPAWN();
  ASSERT_SYS(0, 0, mkdir("folder", 0755));
  ASSERT_SYS(0, 0, unveil("folder", "rwcx"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(0, 0, extract("/zip/life.elf", "folder/life.elf", 0755));
  SPAWN();
  ASSERT_SYS(0, 0, stat("folder/life.elf", &st));
  execl("folder/life.elf", "folder/life.elf", 0);
  kprintf("execve failed! %s\n", strerror(errno));
  _Exit(127);
  EXITS(42);
  EXITS(0);
}

TEST(unveil, dirfdHacking_doesntWork) {
  SPAWN();
  ASSERT_SYS(0, 0, mkdir("jail", 0755));
  ASSERT_SYS(0, 0, mkdir("garden", 0755));
  ASSERT_SYS(0, 0, touch("garden/secret.txt", 0644));
  ASSERT_SYS(0, 3, open("garden", O_RDONLY | O_DIRECTORY));
  ASSERT_SYS(0, 0, unveil("jail", "rw"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  ASSERT_SYS(EACCES_OR_ENOENT, -1, openat(3, "secret.txt", O_RDONLY));
  EXITS(0);
}

TEST(unveil, overlappingDirectories_inconsistentBehavior) {
  SPAWN();
  ASSERT_SYS(0, 0, makedirs("f1/f2", 0755));
  ASSERT_SYS(0, 0, extract("/zip/life.elf", "f1/f2/life.elf", 0755));
  ASSERT_SYS(0, 0, unveil("f1", "x"));
  ASSERT_SYS(0, 0, unveil("f1/f2", "r"));
  ASSERT_SYS(0, 0, unveil(0, 0));
  if (IsOpenbsd()) {
    // OpenBSD favors the most restrictive policy
    SPAWN();
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
    // SPAWN();
    // ASSERT_SYS(0, 0, stat("f1/f2/life.elf", &st));
    // execl("f1/f2/life.elf", "f1/f2/life.elf", 0);
    // kprintf("execve failed! %s\n", strerror(errno));
    // _Exit(127);
    // EXITS(42);
  }
  EXITS(0);
}
