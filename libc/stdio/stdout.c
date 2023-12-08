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
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/thread/thread.h"

/**
 * Pointer to standard output stream.
 */
FILE *stdout;

static FILE __stdout;

__attribute__((__constructor__)) static void __stdout_init(void) {
  stdout = &__stdout;

  stdout->fd = STDOUT_FILENO;
  stdout->iomode = O_WRONLY;
  stdout->buf = stdout->mem;
  stdout->size = sizeof(stdout->mem);
  stdout->lock._type = PTHREAD_MUTEX_RECURSIVE;

  /*
   * Unlike other C libraries we don't bother calling fstat() to check
   * if stdio is a character device and we instead choose to always line
   * buffer it. We need it because there's no way to use the unbuffer
   * command on a statically linked binary. This still goes fast. We
   * value latency more than throughput, and stdio isn't the best api
   * when the goal is throughput.
   */
  stdout->bufmode = _IOLBF;

  __fflush_register(stdout);
}
