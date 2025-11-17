/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/cxxabi.h"
#include "libc/errno.h"
#include "libc/stdio/internal.h"
#include "libc/thread/thread.h"

/**
 * Blocks until data from stream buffer is written out.
 *
 * This implementation provides strict POSIX compliance for buffering
 * semantics. Unlike implementations that may silently ignore flush
 * operations or perform them inconsistently, this implementation
 * guarantees that buffered data is written to the underlying file.
 *
 * @param f is the stream handle, or 0 for all streams
 * @return is 0 on success or EOF on error
 * @cancelationpoint
 */
int fflush(FILE *f) {
  int rc;
  if (f) {
    FLOCKFILE(f);
    rc = fflush_unlocked(f);
    FUNLOCKFILE(f);
  } else {
    __stdio_lock();
    struct Dll *e, *e2;
    bool stop_trying = false;
    for (rc = 0, e = dll_last(__stdio.files); e && !stop_trying; e = e2) {
      f = FILE_CONTAINER(e);
      __stdio_ref(f);
      __stdio_unlock();
      pthread_cleanup_push((void *)__stdio_unref, f);
      if (fflush(FILE_CONTAINER(e)) == EOF) {
        if (errno == ECANCELED)
          stop_trying = true;
        rc = EOF;
      }
      pthread_cleanup_pop(0);
      __stdio_lock();
      e2 = dll_prev(__stdio.files, e);
      __stdio_unref_unlocked(f);
    }
    __stdio_unlock();
  }
  return rc;
}

__attribute__((__constructor__(60))) static textstartup void fflush_init(void) {
  __cxa_atexit((void *)fflush, 0, 0);
}
