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
#include "dsp/tty/tty.h"
#include "libc/dce.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/nt/console.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/consolecursorinfo.h"
#include "libc/runtime/runtime.h"

/* TODO(jart): DELETE */

static int ttysetcursor(int fd, bool visible) {
  struct NtConsoleCursorInfo ntcursor;
  char code[8] = "\e[?25l";
  if (__nocolor) return 0;
  if (visible) code[5] = 'h';
  if (IsWindows()) {
    GetConsoleCursorInfo(GetStdHandle(kNtStdOutputHandle), &ntcursor);
    ntcursor.bVisible = visible;
    SetConsoleCursorInfo(GetStdHandle(kNtStdOutputHandle), &ntcursor);
  }
  return ttysend(fd, code);
}

/**
 * Asks teletypewriter to hide blinking box.
 */
int ttyhidecursor(int fd) {
  return ttysetcursor(fd, false);
}

/**
 * Asks teletypewriter to restore blinking box.
 */
int ttyshowcursor(int fd) {
  return ttysetcursor(fd, true);
}
