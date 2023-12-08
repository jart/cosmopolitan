/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/errno.h"
#include "libc/proc/posix_spawn.h"
#include "libc/proc/posix_spawn.internal.h"

/**
 * Sets posix_spawn() flags.
 *
 * @param attr was initialized by posix_spawnattr_init()
 * @param flags may have any of the following
 *     - `POSIX_SPAWN_RESETIDS`
 *     - `POSIX_SPAWN_SETPGROUP`
 *     - `POSIX_SPAWN_SETSIGDEF`
 *     - `POSIX_SPAWN_SETSIGMASK`
 *     - `POSIX_SPAWN_SETSCHEDPARAM`
 *     - `POSIX_SPAWN_SETSCHEDULER`
 *     - `POSIX_SPAWN_SETSID`
 *     - `POSIX_SPAWN_SETRLIMIT`
 * @return 0 on success, or errno on error
 * @raise EINVAL if `flags` has invalid bits
 */
int posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags) {
  if (flags & ~(POSIX_SPAWN_USEVFORK |       //
                POSIX_SPAWN_RESETIDS |       //
                POSIX_SPAWN_SETPGROUP |      //
                POSIX_SPAWN_SETSIGDEF |      //
                POSIX_SPAWN_SETSIGMASK |     //
                POSIX_SPAWN_SETSCHEDPARAM |  //
                POSIX_SPAWN_SETSCHEDULER |   //
                POSIX_SPAWN_SETSID |         //
                POSIX_SPAWN_SETRLIMIT)) {
    return EINVAL;
  }
  (*attr)->flags = flags;
  return 0;
}
