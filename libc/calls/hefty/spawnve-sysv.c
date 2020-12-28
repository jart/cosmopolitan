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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/hefty/internal.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/fmt/conv.h"
#include "libc/mem/mem.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"

int spawnve$sysv(unsigned flags, int stdiofds[3], const char *program,
                 char *const argv[], char *const envp[]) {
  int rc, pid, fd;
  size_t i, j, len;
  int32_t tubes[3][2];
  char **argv2, MZqFpD[8];
  void *argv3;

  pid = 0;
  argv2 = NULL;
  argv3 = argv;

  /*
   * αcτµαlly pδrταblε εxεcµταblε w/ thompson shell script
   * morphology needs to be launched via command interpreter.
   */
  if (endswith(program, ".com") || endswith(program, ".exe")) {
    memset(MZqFpD, 0, sizeof(MZqFpD));
    fd = openat$sysv(AT_FDCWD, program, O_RDONLY, 0);
    read$sysv(fd, MZqFpD, sizeof(MZqFpD));
    close$sysv(fd);
    if (memcmp(MZqFpD, "MZqFpD", 6) == 0) {
      /*
       * If we got this:
       *
       *   spawn(/bin/echo, [echo, hi, there])
       *
       * It will become this:
       *
       *   spawn(/bin/sh, [sh, /bin/echo, hi, there])
       */
      len = 1;
      while (argv[len]) len++;
      if ((argv2 = malloc((2 + len + 1) * sizeof(char *)))) {
        i = 0, j = 1;
        argv2[i++] = "sh";
        argv2[i++] = program;
        while (j < len) argv2[i++] = argv[j++];
        argv2[i] = NULL;
        argv3 = argv2;
        program = "/bin/sh";
      }
    }
  }

  for (i = 0; i < 3; ++i) {
    if (stdiofds[i] == -1) {
      pid |= pipe$sysv(tubes[i]);
    }
  }

  if (pid != -1) {
    if ((pid = vfork()) == 0) {
      if (flags & SPAWN_DETACH) {
        if (setsid() == -1) abort();
        if ((rc = fork$sysv()) == -1) abort();
        if (rc > 0) _exit(0);
      }
      for (i = 0; i < 3; ++i) {
        if (stdiofds[i] == -1) {
          close$sysv(tubes[i][kIoMotion[i]]);
          fd = tubes[i][!kIoMotion[i]];
        } else {
          fd = stdiofds[i];
        }
        dup2$sysv(fd, i);
      }
      execve$sysv(program, argv3, envp);
      abort();
    }
  }

  for (i = 0; i < 3; ++i) {
    if (stdiofds[i] == -1) {
      close$sysv(tubes[i][!kIoMotion[i]]);
      fd = tubes[i][kIoMotion[i]];
      if (pid != -1) {
        stdiofds[i] = fd;
      } else {
        close$sysv(fd);
      }
    }
  }

  if (argv2) free(argv2);
  return pid;
}
