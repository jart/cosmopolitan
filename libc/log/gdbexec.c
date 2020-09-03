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
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/fmt/fmt.h"
#include "libc/log/gdb.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.h"

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
  if ((elf = finddebugbinary())) {
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
