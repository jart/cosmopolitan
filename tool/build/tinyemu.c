/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/log/check.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/x/x.h"
#include "tool/build/lib/loader.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/syscall.h"

static void AddHostFd(struct Machine *m, int fd) {
  int i = m->fds.i++;
  CHECK_NE(-1, (m->fds.p[i].fd = dup(fd)));
  m->fds.p[i].cb = &kMachineFdCbHost;
}

int main(int argc, char *argv[]) {
  int rc;
  struct Elf elf;
  struct Machine *m;
  if (argc < 2) {
    fputs("Usage: ", stderr);
    fputs(argv[0], stderr);
    fputs(" PROG [ARGS...]\n", stderr);
    return EX_USAGE;
  }
  m = NewMachine();
  LoadProgram(m, argv[1], argv + 2, environ, &elf);
  m->fds.p = xcalloc((m->fds.n = 8), sizeof(struct MachineFd));
  AddHostFd(m, STDIN_FILENO);
  AddHostFd(m, STDOUT_FILENO);
  AddHostFd(m, STDERR_FILENO);
  if (!(rc = setjmp(m->onhalt))) {
    for (;;) {
      LoadInstruction(m);
      ExecuteInstruction(m);
    }
  } else {
    return rc;
  }
}
