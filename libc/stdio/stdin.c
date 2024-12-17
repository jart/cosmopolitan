/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/stat.h"
#include "libc/intrin/dll.h"
#include "libc/stdio/internal.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/thread/thread.h"

__static_yoink("fflush");

static char __stdin_buf[BUFSIZ];

static FILE __stdin = {
    .fd = STDIN_FILENO,
    .oflags = O_RDONLY,
    .bufmode = _IOFBF,
    .buf = __stdin_buf,
    .size = sizeof(__stdin_buf),
    .lock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP,
    .elem = {&__stdin.elem, &__stdin.elem},
};

/**
 * Pointer to standard input stream.
 */
FILE *stdin = &__stdin;

__attribute__((__constructor__(60))) static textstartup void stdin_init(void) {
  struct stat st;
  if (fstat(STDIN_FILENO, &st) || S_ISCHR(st.st_mode))
    stdin->bufmode = _IONBF;
  dll_make_last(&__stdio.files, &__stdin.elem);
}
