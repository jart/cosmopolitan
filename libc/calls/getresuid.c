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
#include "libc/intrin/strace.internal.h"

/**
 * Gets real, effective, and "saved" user ids.
 *
 * @param real receives real user id, or null to do nothing
 * @param effective receives effective user id, or null to do nothing
 * @param saved receives saved user id, or null to do nothing
 * @return 0 on success or -1 w/ errno
 * @see setresuid()
 */
int getresuid(uint32_t *real, uint32_t *effective, uint32_t *saved) {
  int rc, uid;
  if (IsWindows()) {
    uid = getuid();
    if (real) *real = uid;
    if (effective) *effective = uid;
    if (saved) *saved = uid;
    rc = 0;
  } else if (saved) {
    rc = sys_getresuid(real, effective, saved);
  } else {
    if (real) *real = sys_getuid();
    if (effective) *effective = sys_geteuid();
    rc = 0;
  }
  STRACE("getresuid([%d], [%d], [%d]) → %d% m", real ? *real : 0,
         effective ? *effective : 0, saved ? *saved : 0, rc);
  return rc;
}
