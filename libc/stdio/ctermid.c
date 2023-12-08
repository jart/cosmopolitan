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
#include "libc/nt/pedef.internal.h"
#include "libc/runtime/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * Generates path of controlling terminal.
 *
 * This function always returns /dev/tty since that's supported by all
 * supported platforms, and polyfilled on Windows.
 *
 * @param s may optionally specify an outut buffer L_ctermid in size
 * @return pointer to `s` (or image memory if `s` was null) which
 *     contains path of controlling terminal, or empty string if
 *     if this program is a win32 app running in gui mode
 */
char *ctermid(char *s) {
  const char *tty;
  if ((char)(intptr_t)v_ntsubsystem == kNtImageSubsystemWindowsGui) {
    tty = "";
  } else {
    tty = "/dev/tty";
  }
  if (s) {
    return strcpy(s, tty);
  } else {
    return (char *)tty;
  }
}
