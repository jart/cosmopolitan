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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/nt/accounting.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"

static uint32_t KnuthMultiplicativeHash32(const void *buf, size_t size) {
  size_t i;
  uint32_t h;
  const uint32_t kPhiPrime = 0x9e3779b1;
  const unsigned char *p = (const unsigned char *)buf;
  for (h = i = 0; i < size; i++) h = (p[i] + h) * kPhiPrime;
  return h;
}

static textwindows noinline uint32_t GetUserNameHash(void) {
  char16_t buf[257];
  uint32_t size = ARRAYLEN(buf);
  GetUserName(&buf, &size);
  return KnuthMultiplicativeHash32(buf, size >> 1);
}

static uint32_t getuidgid(int at, uint32_t impl(void)) {
  if (!IsWindows()) {
    if (at != -1) {
      return getauxval(at);
    } else {
      return impl();
    }
  } else {
    return GetUserNameHash();
  }
}

/**
 * Returns real user id of process.
 *
 * This never fails. On Windows, which doesn't really have this concept,
 * we return a deterministic value that's likely to work. On Linux, this
 * is fast.
 *
 * @asyncsignalsafe
 */
uint32_t getuid(void) {
  return getuidgid(AT_UID, getuid$sysv);
}

/**
 * Returns real group id of process.
 *
 * This never fails. On Windows, which doesn't really have this concept,
 * we return a deterministic value that's likely to work. On Linux, this
 * is fast.
 *
 * @asyncsignalsafe
 */
uint32_t getgid(void) {
  return getuidgid(AT_GID, getgid$sysv);
}
