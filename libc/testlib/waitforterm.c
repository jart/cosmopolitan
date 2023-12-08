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
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

__static_yoink("strsignal");

void testlib_waitforterm(const char *file, int line, const char *code, int sig,
                         int pid) {
  int ws;
  char host[64];
  ASSERT_NE(-1, waitpid(pid, &ws, 0));
  if (WIFSIGNALED(ws)) {
    if (WTERMSIG(ws) == sig) {
      return;
    }
    kprintf("%s:%d: test failed\n"
            "\tTERMS(%s)\n"
            "\t  want WTERMSIG(%G)\n"
            "\t   got WTERMSIG(%G)\n",
            file, line, code, sig, WTERMSIG(ws));
  } else if (WIFEXITED(ws)) {
    kprintf("%s:%d: test failed\n"
            "\tTERMS(%s)\n"
            "\t  want WTERMSIG(%G)\n"
            "\t   got _Exit(%d)\n",
            file, line, code, sig, WEXITSTATUS(ws));
  } else if (WIFSTOPPED(ws)) {
    kprintf("%s:%d: test failed\n"
            "\tTERMS(%s)\n"
            "\t  want WTERMSIG(%G)\n"
            "\t   got WSTOPSIG(%G)\n",
            file, line, code, sig, WSTOPSIG(ws));
  } else {
    kprintf("%s:%d: test failed\n"
            "\tTERMS(%s)\n"
            "\t  want WTERMSIG(%G)\n"
            "\t   got ws=%#x\n",
            file, line, code, sig, ws);
  }
  if (gethostname(host, sizeof(host))) {
    strcpy(host, "unknown");
  }
  kprintf("\t%s\n", host);
  exit(1);
}
