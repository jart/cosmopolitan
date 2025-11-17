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
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nt/files.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  if (!IsWindows()) {
    fprintf(stderr, "%s: skipping because not windows\n",
            program_invocation_name);
    exit(0);
  }
  if (GetFileAttributes(u"C:\\Windows") == -1u) {
    fprintf(stderr, "%s: skipping because C:\\Windows doesn't exist\n",
            program_invocation_name);
    exit(0);
  }
  if (GetFileAttributes(u"C:\\C") != -1u) {
    fprintf(stderr, "%s: skipping because C:\\C exists\n",
            program_invocation_name);
    exit(0);
  }
  testlib_enable_tmp_setup_teardown();
}

TEST(mkntpath, test_root_rel_drive) {

  // cd into $COSMOSDRIVE or $SYSTEMDRIVE
  ASSERT_SYS(0, 0, chdir("/"));

  // make sure we can access drive letters as relative paths
  struct stat st;
  ASSERT_SYS(0, 0, stat("C", &st));
  ASSERT_SYS(0, 0, stat("C/", &st));
  ASSERT_SYS(0, 0, stat("C/Windows", &st));

  // make sure c isn't listed under c
  char *cosmosdrive = getenv("COSMOSDRIVE");
  putenv("COSMOSDRIVE=C:");
  bool has_c = false;
  DIR *dir;
  ASSERT_NE(NULL, (dir = opendir("C")));
  struct dirent *ent;
  while ((ent = readdir(dir)))
    if (!strcasecmp(ent->d_name, "C"))
      has_c = true;
  ASSERT_SYS(0, 0, closedir(dir));
  ASSERT_FALSE(has_c);
  if (cosmosdrive) {
    setenv("COSMOSDRIVE", cosmosdrive, true);
  } else {
    unsetenv("COSMOSDRIVE");
  }
}

TEST(mkntpath, test_root_rel_drive_dirfd) {

  // TODO(jart): why does this fail when building monorepo on Windows?
  if (IsWindows())
    return;

  // cd into $COSMOSDRIVE or $SYSTEMDRIVE
  ASSERT_SYS(0, 3, open("/", O_RDONLY));

  // make sure we can access drive letters as relative paths
  struct stat st;
  ASSERT_SYS(0, 0, fstatat(3, "C", &st, 0));
  ASSERT_SYS(0, 0, fstatat(3, "C/", &st, 0));
  ASSERT_SYS(0, 0, fstatat(3, "C/Windows", &st, 0));
}
