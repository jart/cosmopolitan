/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

// make sure that running `popen_test --ftrace --strace` doesn't crash
//
// function and system call tracing are invasive runtime features that
// can easily break if interrupting the other magical, deeply embedded
// parts of the runtime, like mutations to the rbtree ftrace needs for
// validating stack pointers (kisdangerous() locks the mmap lock), and
// that's why we use dontinstrument in so many places in the codebase.
//
// we like popen_test because it tests the intersection of forking and
// threads, and it activates other subsystems like the signal / itimer
// worker threads on windows. if we can ftrace and strace it, then you
// can be assured cosmo's tracing support works right on all platforms

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(trace, test) {
  unsetenv("MAKEFLAGS");  // avoid testmain.c 254 status
  testlib_extract("/zip/popen_test", "popen_test", 0755);
  testlib_extract("/zip/popen_test.dbg", "popen_test.dbg", 0755);
  if (!fork()) {
    close(1);
    close(2);
    open("log", O_CREAT | O_TRUNC | O_WRONLY | O_APPEND, 0644);
    dup(1);
    execl("./popen_test", "./popen_test", "--ftrace", "--strace", NULL);
    _Exit(128);
  }
  int ws;
  unassert(wait(&ws));
  if (WIFSIGNALED(ws)) {
    fprintf(stderr,
            "%s:%d: error: trace_test got %s signal running "
            "popen_test --strace --ftrace (see %s for output)\n",
            __FILE__, __LINE__, strsignal(WTERMSIG(ws)), realpath("log", 0));
    _Exit(1);
  }
  if (WEXITSTATUS(ws)) {
    fprintf(stderr,
            "%s:%d: error: trace_test got %d exit status running "
            "popen_test --strace --ftrace (see %s for output)\n",
            __FILE__, __LINE__, WEXITSTATUS(ws), realpath("log", 0));
    _Exit(1);
  }
}
