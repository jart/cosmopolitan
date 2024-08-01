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
#include "libc/stdio/internal.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/thread/thread.h"

static FILE __stdout = {
    .fd = STDOUT_FILENO,
    .iomode = O_WRONLY,
    .buf = __stdout.mem,
    .size = sizeof(stdout->mem),
    .lock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP,

    // Unlike other C libraries we don't bother calling fstat() to check
    // if stdio is a character device and we instead choose to always
    // line buffer it. We need it because there's no way to use the
    // unbuffer command on a statically linked binary. This still goes
    // fast. We value latency more than throughput, and stdio isn't the
    // best api when the goal is throughput.
    .bufmode = _IOLBF,
};

/**
 * Pointer to standard output stream.
 */
FILE *stdout = &__stdout;

__attribute__((__constructor__(60))) static textstartup void outinit(void) {
  __fflush_register(stdout);
}
