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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/nt/accounting.h"
#include "libc/runtime/runtime.h"
#include "libc/str/knuthmultiplicativehash.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"

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
