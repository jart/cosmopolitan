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
