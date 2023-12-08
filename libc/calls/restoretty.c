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
#include "libc/calls/cp.internal.h"
#include "libc/calls/struct/metatermios.internal.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/termios.h"
#include "libc/errno.h"
#include "libc/log/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/termios.h"

/**
 * @fileoverview Terminal Restoration Helper for System Five.
 *
 * This is used by the crash reporting functions, e.g. __die(), to help
 * ensure the terminal is in an unborked state after a crash happens.
 */

#define RESET_COLOR   "\e[0m"
#define SHOW_CURSOR   "\e[?25h"
#define DISABLE_MOUSE "\e[?1000;1002;1015;1006l"
#define ANSI_RESTORE  RESET_COLOR SHOW_CURSOR DISABLE_MOUSE

static bool __isrestorable;
static struct termios __oldtermios;

static size_t __strlen(const char *s) {
  size_t i = 0;
  while (s[i]) ++i;
  return i;
}

// called weakly by libc/calls/tcsetattr.c to avoid pledge("tty")
void __on_tcsetattr(int fd) {
  int e;
  e = errno;
  if (tcgetattr(fd, &__oldtermios) != -1) {
    __isrestorable = true;
  }
  errno = e;
}

void __restore_tty(void) {
  int e;
  if (__isrestorable && !__isworker && !__nocolor) {
    e = errno;
    BEGIN_CANCELATION_POINT;
    sys_write(0, ANSI_RESTORE, __strlen(ANSI_RESTORE));
    END_CANCELATION_POINT;
    tcsetattr(0, TCSAFLUSH, &__oldtermios);
    errno = e;
  }
}
