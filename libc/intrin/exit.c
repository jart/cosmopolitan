/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/dce.h"
#include "libc/intrin/promises.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/sig.h"

/**
 * Terminates process, ignoring destructors and atexit() handlers.
 *
 * Exit codes are narrowed to an unsigned char on most platforms. The
 * exceptions would be Windows, NetBSD, and OpenBSD, which should let
 * you have larger exit codes.
 *
 * When running on bare metal, this function will reboot your computer
 * by hosing the interrupt descriptors and triple faulting the system.
 *
 * @asyncsignalsafe
 * @vforksafe
 * @noreturn
 */
wontreturn void _Exit(int exitcode) {
  STRACE("_Exit(%d)", exitcode);
  if (!IsWindows() && !IsMetal()) {
    // On Linux _Exit1 (exit) must be called in pledge("") mode. If we
    // call _Exit (exit_group) when we haven't used pledge("stdio") then
    // it'll terminate the process instead. On OpenBSD we must not call
    // _Exit1 (__threxit) because only _Exit (exit) is whitelisted when
    // operating in pledge("") mode.
    if (!(IsLinux() && !PLEDGED(STDIO))) {
#ifdef __x86_64__
      asm volatile("syscall"
                   : /* no outputs */
                   : "a"(__NR_exit_group), "D"(exitcode)
                   : "rcx", "r11", "memory");
#elif defined(__aarch64__)
      register long x0 asm("x0") = exitcode;
      register long x8 asm("x8") = IsLinux() ? 94 : 1;
      asm volatile("mov\tx16,%1\n\t"
                   "svc\t0"
                   : /* no outputs */
                   : "r"(x8), "i"(1), "r"(x0)
                   : "x16", "memory");
#else
#error "unsupported architecture"
#endif
    }
    // Inline _Exit1() just in case _Exit() isn't allowed by pledge()
#ifdef __x86_64__
    asm volatile("syscall"
                 : /* no outputs */
                 : "a"(__NR_exit), "D"(exitcode)
                 : "rcx", "r11", "memory");
#else
    register long r0 asm("x0") = exitcode;
    register long x8 asm("x8") = IsLinux() ? 93 : 431;
    asm volatile("mov\tx16,%1\n\t"
                 "svc\t0"
                 : /* no outputs */
                 : "r"(x8), "i"(0x169), "r"(r0)
                 : "memory");
#endif
  } else if (IsWindows()) {
    uint32_t waitstatus;
    // What Microsoft calls an exit code, POSIX calls a status code. See
    // also the WEXITSTATUS() and WIFEXITED() macros that POSIX defines.
    waitstatus = exitcode;
    waitstatus <<= 8;
    // "The GetExitCodeProcess function returns a valid error code
    //  defined by the application only after the thread terminates.
    //  Therefore, an application should not use kNtStillActive (259) as
    //  an error code (kNtStillActive is a macro for kNtStatusPending).
    //  If a thread returns kNtStillActive (259) as an error code, then
    //  applications that test for that value could interpret it to mean
    //  that the thread is still running, and continue to test for the
    //  completion of the thread after the thread has terminated, which
    //  could put the application into an infinite loop." -Quoth MSDN
    if (waitstatus == kNtStillActive) {
      waitstatus = 0xc9af3d51u;
    }
    TerminateThisProcess(waitstatus);
  }
#ifdef __x86_64__
  asm("push\t$0\n\t"
      "push\t$0\n\t"
      "cli\n\t"
      "lidt\t(%rsp)");
  for (;;) asm("ud2");
#else
  __builtin_unreachable();
#endif
}

__strong_reference(_Exit, _exit);
