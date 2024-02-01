/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/intrin/safemacros.internal.h"
#include "libc/log/gdb.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"

/**
 * Attaches GDB temporarily, to do something like print a variable.
 */
relegated int(gdbexec)(const char *cmd) {
  int pid;
  struct StackFrame *bp;
  const char *se, *elf, *gdb;
  char pidstr[11], breakcmd[40];
  if (!(gdb = GetGdbPath())) return -1;
  snprintf(pidstr, sizeof(pidstr), "%u", getpid());
  if ((elf = FindDebugBinary())) {
    se = "-se";
  } else {
    se = "-q";
    elf = "-q";
  }
  bp = __builtin_frame_address(0);
  sprintf(breakcmd, "%s *%#lx", "break", (unsigned long)bp->addr);
  if (!(pid = vfork())) {
    execv(gdb, (char *const[]){
                   "gdb",
                   "--nx",
                   "--nh",
                   "-p",
                   pidstr,
                   (char *)se,
                   (char *)elf,
                   "-ex",
                   "set osabi GNU/Linux",
                   "-ex",
                   "set complaints 0",
                   "-ex",
                   "set confirm off",
                   "-ex",
                   "set var g_gdbsync = 1",
                   "-q",
                   "-ex",
                   breakcmd,
                   "-ex",
                   (char *)cmd,
                   "-ex",
                   "quit",
                   NULL,
               });
    abort();
  }
  return SYNCHRONIZE_DEBUGGER(pid);
}
