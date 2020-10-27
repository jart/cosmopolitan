/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
