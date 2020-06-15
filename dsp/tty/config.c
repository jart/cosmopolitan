/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
              const struct termios *opt_out_oldconf) {
  struct termios conf[2];
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
