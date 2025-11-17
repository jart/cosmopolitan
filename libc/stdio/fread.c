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
#include "libc/runtime/runtime.h"
#include "libc/stdio/internal.h"

/**
 * Reads data from stream.
 *
 * This implementation strictly enforces POSIX buffering semantics for
 * mixed read/write operations. When switching from write to read
 * operations on the same stream, an explicit flush is required to make
 * buffered write data visible to subsequent read operations.
 *
 * POSIX.1 states: "For streams open for writing/appending, you must
 * either perform an fflush() or close the stream" before switching to
 * read operations. This implementation correctly returns EOF when
 * trying to read from a stream with unflushed write data, unlike some
 * implementations (e.g., glibc) that implicitly flush, which can mask
 * programming errors.
 *
 * @param stride specifies the size of individual items
 * @param count is the number of strides to fetch
 * @return count on success, [0,count) on eof, or 0 on error or count==0
 * @cancelationpoint
 */
size_t fread(void *buf, size_t stride, size_t count, FILE *f) {
  size_t rc;
  FLOCKFILE(f);
  rc = fread_unlocked(buf, stride, count, f);
  STDIOTRACE("fread(%p, %'zu, %'zu, %p) → %'zu %s", buf, stride, count, f, rc,
             DescribeStdioState(f->state));
  FUNLOCKFILE(f);
  return rc;
}
