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
#include "libc/calls/termios.h"
#include "libc/sysv/consts/termios.h"

/**
 * Enables direct teletypewriter communication.
 *
 * @see ttyconfig(), ttyrestore()
 */
int ttysetraw(struct termios *conf, int64_t flags) {
  conf->c_iflag &= ~(INPCK | ISTRIP | PARMRK | INLCR | IGNCR | ICRNL | IXON);
  conf->c_lflag &= ~(IEXTEN | ICANON);
  conf->c_cflag &= ~(CSIZE | PARENB);
  conf->c_cflag |= CS8;
  conf->c_iflag |= IUTF8;
  /* if (flags & kTtyLfToCrLf) { */
  /*   /\* conf->c_oflag &= ~(OLCUC | OCRNL | ONLRET | OFILL | OFDEL); *\/ */
  /*   /\* conf->c_oflag |= ONLCR | ONOCR; *\/ */
  /*   conf->c_oflag |= ONLCR; */
  /* } else { */
  /*   conf->c_oflag &= ~OPOST; */
  /* } */
  if (!(flags & kTtySigs)) {
    conf->c_iflag &= ~(IGNBRK | BRKINT);
    conf->c_lflag &= ~(ISIG);
  }
  if (flags & kTtyEcho) {
    conf->c_lflag |= ECHO | ECHONL;
  } else {
    conf->c_lflag &= ~(ECHO | ECHONL);
  }
  return 0;
}
