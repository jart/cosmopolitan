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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/log/color.internal.h"
#include "libc/log/gdb.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"

/**
 * Launches GDB debugger GUI for current process.
 *
 * This function abstracts the toilsome details of configuring the best
 * possible UX for debugging your app, for varying levels of available
 * information, on most of the various platforms.
 *
 * Before calling this function, consider placing ShowCrashReports() in
 * your main function and calling DebugBreak() wherever you want; it's
 * safer. Also note the "GDB" environment variable can be set to empty
 * string, as a fail-safe for disabling this behavior.
 *
 * @param continuetoaddr can be a code address, 0, or -1 for auto
 * @return gdb pid if continuing, 0 if detached, or -1 w/ errno
 * @note this is called via eponymous spinlock macro wrapper
 */
relegated int(AttachDebugger)(intptr_t continuetoaddr) {
  int pid, ttyfd;
  struct StackFrame *bp;
  char pidstr[12], breakcmd[40];
  const char *se, *elf, *gdb, *rewind, *layout;
  __restore_tty();
  if (!IsLinux() ||             //
      IsGenuineBlink() ||       //
      !(gdb = GetGdbPath()) ||  //
      !isatty(0) ||             //
      !isatty(1) ||             //
      (ttyfd = sys_openat(AT_FDCWD, _PATH_TTY, O_RDWR | O_CLOEXEC, 0)) == -1) {
    return enosys();
  }
  FormatUint32(pidstr, getpid());
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
    FormatHex64(stpcpy(breakcmd, "break *"), continuetoaddr, 1);
  } else {
    rewind = NULL;
    breakcmd[0] = '\0';
  }
  if (!(pid = fork())) {
    sys_dup2(ttyfd, 0, 0);
    sys_dup2(ttyfd, 1, 0);
    __sys_execve(gdb,
                 (char *const[]){
                     "gdb",      "--tui",
                     "-p",       pidstr,
                     (char *)se, (char *)elf,
                     "-ex",      "set osabi GNU/Linux",
                     "-ex",      "set complaints 0",
                     "-ex",      "set confirm off",
                     "-ex",      (char *)layout,
                     "-ex",      "layout reg",
                     "-ex",      "set var g_gdbsync = 1",
                     "-q",       (char *)rewind,
                     breakcmd,   "-ex",
                     "c",        NULL,
                 },
                 environ);
    abort();
  }
  return pid;
}
