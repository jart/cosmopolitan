/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/strace.internal.h"
#include "libc/dce.h"
#include "libc/intrin/promises.internal.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/nr.h"

/**
 * Terminates process, ignoring destructors and atexit() handlers.
 *
 * When running on bare metal, this function will reboot your computer
 * by hosing the interrupt descriptors and triple faulting the system.
 *
 * @param exitcode is masked with 255 on unix (but not windows)
 * @asyncsignalsafe
 * @threadsafe
 * @vforksafe
 * @noreturn
 */
privileged wontreturn void _Exit(int exitcode) {
  int i;
  STRACE("_Exit(%d)", exitcode);
  if (!IsWindows() && !IsMetal()) {
    // On Linux _Exit1 (exit) must be called in pledge("") mode. If we
    // call _Exit (exit_group) when we haven't used pledge("stdio") then
    // it'll terminate the process instead. On OpenBSD we must not call
    // _Exit1 (__threxit) because only _Exit (exit) is whitelisted when
    // operating in pledge("") mode.
    if (!(IsLinux() && !PLEDGED(STDIO))) {
      asm volatile("syscall"
                   : /* no outputs */
                   : "a"(__NR_exit_group), "D"(exitcode)
                   : "rcx", "r11", "memory");
    }
    // Inline _Exit1() just in case _Exit() isn't allowed by pledge()
    asm volatile("syscall"
                 : /* no outputs */
                 : "a"(__NR_exit), "D"(exitcode)
                 : "rcx", "r11", "memory");
  } else if (IsWindows()) {
    ExitProcess(exitcode);
  }
  asm("push\t$0\n\t"
      "push\t$0\n\t"
      "cli\n\t"
      "lidt\t(%rsp)");
  for (;;) asm("ud2");
}
