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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nt/accounting.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

static uint32_t KnuthMultiplicativeHash32(const void *buf, size_t size) {
  size_t i;
  uint32_t h;
  const uint32_t kPhiPrime = 0x9e3779b1;
  const unsigned char *p = (const unsigned char *)buf;
  for (h = i = 0; i < size; i++) h = (p[i] + h) * kPhiPrime;
  return h;
}

static textwindows dontinline uint32_t GetUserNameHash(void) {
  char16_t buf[257];
  uint32_t size = ARRAYLEN(buf);
  GetUserName(&buf, &size);
  return KnuthMultiplicativeHash32(buf, size >> 1) & INT_MAX;
}

/**
 * Returns real user id of process.
 *
 * This never fails. On Windows, which doesn't really have this concept,
 * we return a deterministic value that's likely to work.
 *
 * @return user id (always successful)
 * @asyncsignalsafe
 * @threadsafe
 * @vforksafe
 */
uint32_t getuid(void) {
  int rc;
  if (IsMetal()) {
    rc = 0;
  } else if (!IsWindows()) {
    rc = sys_getuid();
  } else {
    rc = GetUserNameHash();
  }
  _npassert(rc >= 0);
  STRACE("%s() → %d", "getuid", rc);
  return rc;
}

/**
 * Returns real group id of process.
 *
 * This never fails. On Windows, which doesn't really have this concept,
 * we return a deterministic value that's likely to work.
 *
 * @return group id (always successful)
 * @asyncsignalsafe
 * @threadsafe
 * @vforksafe
 */
uint32_t getgid(void) {
  int rc;
  if (IsMetal()) {
    rc = 0;
  } else if (!IsWindows()) {
    rc = sys_getgid();
  } else {
    rc = GetUserNameHash();
  }
  _npassert(rc >= 0);
  STRACE("%s() → %d", "getgid", rc);
  return rc;
}
