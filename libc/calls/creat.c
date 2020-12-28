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
#include "libc/sysv/consts/o.h"

/**
 * Creates new file, returning open()'d file descriptor.
 *
 * This function is shorthand for:
 *
 *     open(file, O_CREAT | O_WRONLY | O_TRUNC, mode)
 *
 * @param file is a UTF-8 string, which is truncated if it exists
 * @param mode is an octal user/group/other permission, e.g. 0755
 * @return a number registered with the system to track the open file,
 *     which must be stored using a 64-bit type in order to support both
 *     System V and Windows, and must be closed later on using close()
 * @see open(), touch()
 * @asyncsignalsafe
 */
nodiscard int creat(const char *file, uint32_t mode) {
  return open(file, O_CREAT | O_WRONLY | O_TRUNC, mode);
}
