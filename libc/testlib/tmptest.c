/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/dll.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/aspect.internal.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

static struct {
  bool is_aspected;
  struct TestAspect aspect;
  char olddir[PATH_MAX];
  char tmpdir[PATH_MAX];
} g_tmptest;

static void SetupTmpDir(const testfn_t *fn) {
  char number[21];
  FormatInt64(number, _rand64() & INT64_MAX);
  g_tmptest.tmpdir[0] = 0;
  if (*__get_tmpdir() != '/') {
    strlcat(g_tmptest.tmpdir, g_tmptest.olddir, sizeof(g_tmptest.tmpdir));
    strlcat(g_tmptest.tmpdir, "/", sizeof(g_tmptest.tmpdir));
  }
  strlcat(g_tmptest.tmpdir, __get_tmpdir(), sizeof(g_tmptest.tmpdir));
  strlcat(g_tmptest.tmpdir, program_invocation_short_name,
          sizeof(g_tmptest.tmpdir));
  strlcat(g_tmptest.tmpdir, ".", sizeof(g_tmptest.tmpdir));
  strlcat(g_tmptest.tmpdir, number, sizeof(g_tmptest.tmpdir));
  if (makedirs(g_tmptest.tmpdir, 0755) || chdir(g_tmptest.tmpdir)) {
    perror(g_tmptest.tmpdir);
    tinyprint(2, "testlib failed to setup tmpdir\n", NULL);
    exit(1);
  }
}

static void TearDownTmpDir(const testfn_t *fn) {
  if (chdir(g_tmptest.olddir)) {
    perror(g_tmptest.olddir);
    exit(1);
  }
  if (rmrf(g_tmptest.tmpdir)) {
    perror(g_tmptest.tmpdir);
    tinyprint(2, "testlib failed to tear down tmpdir\n", NULL);
    exit(1);
  }
}

static void InstallAspect(void) {
  unassert(!g_tmptest.is_aspected);
  g_tmptest.is_aspected = true;
  g_tmptest.aspect.teardown = TearDownTmpDir;
  dll_init(&g_tmptest.aspect.elem);
  dll_make_last(&testlib_aspects, &g_tmptest.aspect.elem);
  if (!getcwd(g_tmptest.olddir, sizeof(g_tmptest.olddir))) {
    perror("getcwd");
    exit(1);
  }
}

void testlib_enable_tmp_setup_teardown(void) {
  g_tmptest.aspect.setup = SetupTmpDir;
  InstallAspect();
}

void testlib_enable_tmp_setup_teardown_once(void) {
  g_tmptest.aspect.once = true;
  InstallAspect();
  SetupTmpDir(0);
}
