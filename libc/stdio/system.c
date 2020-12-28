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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * Launches program with system command interpreter.
 *
 * @param cmdline is an interpreted Turing-complete command
 * @return -1 if child process couldn't be created, otherwise a wait
 *     status that can be accessed using macros like WEXITSTATUS(s)
 */
int system(const char *cmdline) {
  char comspec[128];
  int rc, pid, wstatus;
  const char *prog, *arg;
  if (weaken(fflush)) weaken(fflush)(NULL);
  if (cmdline) {
    if ((pid = fork()) == -1) return -1;
    if (!pid) {
      strcpy(comspec, kNtSystemDirectory);
      strcat(comspec, "cmd.exe");
      prog = !IsWindows() ? _PATH_BSHELL : comspec;
      arg = !IsWindows() ? "-c" : "/C";
      execve(prog, (char *const[]){prog, arg, cmdline, NULL}, environ);
      _exit(errno);
    } else if (wait4(pid, &wstatus, 0, NULL) != -1) {
      return wstatus;
    } else {
      return -1;
    }
  } else if (IsWindows()) {
    return true;
  } else {
    return fileexists(_PATH_BSHELL);
  }
}
