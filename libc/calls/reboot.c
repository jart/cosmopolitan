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
#include "libc/calls/calls.h"
#include "libc/calls/struct/framebuffervirtualscreeninfo.h"
#include "libc/dce.h"
#include "libc/nt/enum/version.h"
#include "libc/nt/system.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/errfuns.h"

#define kNtShutdownForceOthers   1
#define kNtShutdownForceSelf     2
#define kNtShutdownGraceOverride 0x20
#define kNtShutdownHybrid        0x200

int32_t sys_reboot_linux(int32_t, int32_t, int32_t) asm("sys_reboot");
int32_t sys_reboot_bsd(int32_t, const char *) asm("sys_reboot");

/**
 * Reboots system.
 *
 * The `howto` argument may be one of the following:
 *
 * - `RB_AUTOBOOT`
 * - `RB_POWER_OFF`
 * - `RB_HALT_SYSTEM`
 * - `RB_SW_SUSPEND` (linux and windows only)
 * - `RB_KEXEC` (linux only)
 * - `RB_ENABLE_CAD` (linux only)
 * - `RB_DISABLE_CAD` (linux only)
 *
 * There's an implicit `sync()` operation before the reboot happens.
 * This can be prevented by or'ing `howto` with `RB_NOSYNC`. Setting
 * this option will also prevent apps on Windows from having time to
 * close themselves.
 */
int reboot(int howto) {
  bool ok, immediately;
  if (howto != -1) {
    if (!(howto & 0x20000000)) {
      sync();
      immediately = false;
    } else {
      howto &= ~0x20000000;
      immediately = true;
    }
    if (!IsWindows()) {
      if (IsLinux()) {
        return sys_reboot_linux(0xFEE1DEAD, 0x28121969, howto);
      } else {
        return sys_reboot_bsd(howto, 0);
      }
    } else {
      if (howto == 0xD000FCE2u) {
        ok = !!SetSuspendState(0, 0, 0);
      } else {
        howto |= kNtShutdownForceSelf;
        howto |= kNtShutdownForceOthers;
        if (NtGetVersion() >= kNtVersionWindows8) {
          howto |= kNtShutdownHybrid;
        }
        if (immediately) {
          ok = !!InitiateShutdown(0, 0, 0, howto | kNtShutdownGraceOverride, 0);
        } else {
          ok = !!InitiateShutdown(0, 0, 20, howto, 0);
        }
      }
      if (ok) {
        return 0;
      } else {
        return -1;
      }
    }
  } else {
    return einval();
  }
}
