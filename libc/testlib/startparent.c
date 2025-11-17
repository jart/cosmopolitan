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
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/subprocess.h"
#include "libc/thread/thread.h"

// this code is used to ensure that if we have code like this:
//
//     SPAWN(fork);
//     ASSERT_TRUE(true);
//     PARENT();
//     ASSERT_TRUE(false);
//     WAIT(exit, 0);
//
// then the assertion failing in the parent will kill the child

static int thechilde;
static pthread_once_t once = PTHREAD_ONCE_INIT;

static void onexit(void) {
  if (thechilde)
    kill(thechilde, SIGKILL);
}

static void setup(void) {
  atexit(onexit);
  pthread_atfork(0, 0, testlib_stopsparent);
}

void testlib_startparent(int child) {
  pthread_once(&once, setup);
  thechilde = child;
}

void testlib_stopsparent(void) {
  thechilde = 0;
}
