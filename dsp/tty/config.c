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
#include "libc/assert.h"
#include "libc/calls/termios.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/termios.h"

/**
 * Applies configuration to teletypewriter.
 *
 * @param opt_out_oldconf is only modified if successful
 * @return 0 on success, or -1 w/ errno
 * @see ttyconfig(), ttyrestore()
 */
int ttyconfig(int ttyfd, ttyconf_f fn, int64_t arg,
              struct termios *opt_out_oldconf) {
  struct termios conf[2] = {0};
  if (tcgetattr(ttyfd, &conf[0]) != -1 &&
      fn(memcpy(&conf[1], &conf[0], sizeof(conf[0])), arg) != -1 &&
      tcsetattr(ttyfd, TCSAFLUSH, &conf[1]) != -1) {
    if (opt_out_oldconf) {
      memcpy(opt_out_oldconf, &conf[0], sizeof(conf[0]));
    }
    return 0;
  } else {
    return -1;
  }
}
