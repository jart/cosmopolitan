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
#include "ape/lib/pc.h"
#include "libc/nexgen32e/uart.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"

static void fin(FILE *f) {
  f->buf[f->end] = inb(f->fd);
  f->end = (f->end + 1) & (f->size - 1);
}

static void fout(FILE *f) {
  outb(f->fd, f->buf[f->beg]);
  f->beg = (f->beg + 1) & (f->size - 1);
}

static int serialstdio(FILE *f, unsigned char status, void action(FILE *)) {
  int block = 1;
  unsigned tally = 0;
  while (f->end != f->beg) {
    if (!(inb(f->fd + UART_LSR) & status)) {
      if (!block) break;
      asm("pause");
    } else {
      action(f);
      tally++;
    }
  }
  return (int)tally;
}

int fsreadbuf(FILE *f) {
  return serialstdio(f, UART_TTYDA, fin);
}
int fswritebuf(FILE *f) {
  return serialstdio(f, UART_TTYTXR, fout);
}
