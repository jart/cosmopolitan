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
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"

/**
 * Handles failure of CHECK_xx() macros in -DNDEBUG mode.
 *
 * This handler (1) makes binaries smaller by not embedding source code;
 * and therefore (2) less likely to leak sensitive information. This can
 * still print backtraces with function names if the .com.dbg file is in
 * the same folder.
 *
 * @see libc/log/thunks/__check_fail_ndebug.S
 */
relegated void ___check_fail_ndebug(uint64_t want, uint64_t got,
                                    const char *opchar) {
  char bx[21];
  int lasterr;
  lasterr = errno;
  __start_fatal_ndebug();
  __print_string("check failed: 0x");
  __print(bx, uint64toarray_radix16(want, bx));
  __print_string(" ");
  __print_string(opchar);
  __print_string(" 0x");
  __print(bx, uint64toarray_radix16(got, bx));
  __print_string(" (");
  __print(bx, int64toarray_radix10(lasterr, bx));
  __print_string(")\r\n");
}
