/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=8 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/stdio/internal.h"

/**
 * Writes data to stream.
 *
 * This implementation strictly enforces POSIX buffering semantics for
 * mixed read/write operations. When switching from read to write
 * operations on the same stream, the stream's position is properly
 * maintained according to POSIX requirements.
 *
 * POSIX.1 specifies that for update streams (opened with "+"),
 * operations that change between reading and writing must include an
 * intervening call to fflush(), fseek(), fsetpos(), or rewind(). This
 * implementation correctly maintains stream positioning and buffering
 * state during such transitions, providing better reliability than
 * implementations that allow unsafe mixed operations.
 *
 * @param stride specifies the size of individual items
 * @param count is the number of strides to write
 * @return count on success, [0,count) on EOF or error, or count==0
 * @cancelationpoint
 */
size_t fwrite(const void *data, size_t stride, size_t count, FILE *f) {
  size_t rc;
  FLOCKFILE(f);
  rc = fwrite_unlocked(data, stride, count, f);
  STDIOTRACE("fwrite(%p, %'zu, %'zu, %p) → %'zu %s", data, stride, count, f, rc,
             DescribeStdioState(f->state));
  FUNLOCKFILE(f);
  return rc;
}
