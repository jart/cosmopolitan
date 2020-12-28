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
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/w.h"

#define RESTORE_TTY "\e[?1000;1002;1015;1006l\e[?25h"

/**
 * Launches GDB debugger GUI for current process.
 *
 * This function abstracts the toilsome details of configuring the best
 * possible UX for debugging your app, for varying levels of available
 * information, on most of the various platforms.
 *
 * Before calling this function, consider placing showcrashreports() in
 * your main function and calling DebugBreak() wherever you want; it's
 * safer. Also note the "GDB" environment variable can be set to empty
 * string, as a fail-safe for disabling this behavior.
 *
 * @param continuetoaddr can be a code address, 0, or -1 for auto
 * @return gdb pid if continuing, 0 if detached, or -1 w/ errno
 * @note this is called via eponymous spinlock macro wrapper
 */
relegated int(attachdebugger)(intptr_t continuetoaddr) {
  int ttyfd;
  struct StackFrame *bp;
  char pidstr[11], breakcmd[40];
  const char *se, *elf, *gdb, *rewind, *layout;
  if (IsGenuineCosmo() || !(gdb = GetGdbPath()) ||
      (ttyfd = open(_PATH_TTY, O_RDWR, 0)) == -1) {
    return -1;
  }
  write(ttyfd, RESTORE_TTY, strlen(RESTORE_TTY));
  snprintf(pidstr, sizeof(pidstr), "%u", getpid());
  layout = "layout asm";
  if ((elf = FindDebugBinary())) {
    se = "-se";
    if (fileexists(__FILE__)) layout = "layout src";
  } else {
    se = "-q";
    elf = "-q";
  }
  if (continuetoaddr) {
    if (continuetoaddr == -1) {
      bp = __builtin_frame_address(0);
      continuetoaddr = bp->addr;
    }
    rewind = "-ex";
    snprintf(breakcmd, sizeof(breakcmd), "%s *%#p", "break", continuetoaddr);
  } else {
    rewind = NULL;
    breakcmd[0] = '\0';
  }
  return spawnve(0, (int[3]){ttyfd, ttyfd, STDERR_FILENO}, gdb,
                 (char *const[]){
                     "gdb",    "--tui",
                     "-p",     pidstr,
                     se,       elf,
                     "-ex",    "set osabi GNU/Linux",
                     "-ex",    "set complaints 0",
                     "-ex",    "set confirm off",
                     "-ex",    layout,
                     "-ex",    "layout reg",
                     "-ex",    "set var g_gdbsync = 1",
                     "-q",     rewind,
                     breakcmd, "-ex",
                     "c",      NULL,
                 },
                 environ);
}
