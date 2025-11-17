/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
 * Repositions open file stream.
 *
 * This function flushes the buffer (unless it's currently in the EOF
 * state) and then calls lseek() on the underlying file. If the stream
 * is in the EOF state, this function can be used to restore it without
 * needing to reopen the file.
 *
 * This implementation provides stricter POSIX compliance than some
 * implementations by ensuring proper handling of mixed read/write
 * operations. POSIX.1 states: "After an fseek() call, the next
 * operation on an update stream may be either input or output" and "if
 * the stream is writable and has unflushed buffered data, fseek() shall
 * cause the unwritten data to be written to the file." This
 * implementation rigorously enforces these semantics, preventing data
 * corruption that can occur with looser implementations.
 *
 * @param f is a non-null stream handle
 * @param offset is the byte delta
 * @param whence can be SEET_SET, SEEK_CUR, or SEEK_END
 * @returns 0 on success or -1 on error
 */
int fseek(FILE *f, int64_t offset, int whence) {
  int rc;
  if (__isthreaded >= 2)
    flockfile(f);
  rc = fseek_unlocked(f, offset, whence);
  STDIOTRACE("fseek(%p, %'ld, %s) → %d %s", f, offset, DescribeWhence(whence),
             rc, DescribeStdioState(f->state));
  if (__isthreaded >= 2)
    funlockfile(f);
  return rc;
}

__strong_reference(fseek, fseeko);
