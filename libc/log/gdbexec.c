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
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/fmt/fmt.h"
#include "libc/log/gdb.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"

/**
 * Attachs GDB temporarilly, to do something like print a variable.
 */
int(gdbexec)(const char *cmd) {
  int ttyin, ttyout;
  struct StackFrame *bp;
  intptr_t continuetoaddr;
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
  continuetoaddr = bp->addr;
  snprintf(breakcmd, sizeof(breakcmd), "%s *%#p", "break", bp->addr);

  char *const args[] = {
      "gdb",
      "--nx",
      "--nh",
      "-p",
      pidstr,
      se,
      elf,
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
      cmd,
      "-ex",
      "quit",
      NULL,
  };

  return SYNCHRONIZE_DEBUGGER(spawnve(0, NULL, gdb, args, environ));
}
