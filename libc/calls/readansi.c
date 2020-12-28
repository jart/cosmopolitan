/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/str/thompike.h"
#include "libc/sysv/errfuns.h"

/**
 * Reads single keystroke or control sequence from character device.
 *
 * When reading ANSI UTF-8 text streams, characters and control codes
 * are oftentimes encoded as multi-byte sequences. This function knows
 * how long each sequence is, so that each read consumes a single thing
 * from the underlying file descriptor, e.g.
 *
 *     "a"               ALFA
 *     "\316\261"        ALPHA
 *     "\033[A"          CURSOR UP
 *     "\033[38;5;202m"  ORANGERED
 *     "\eOP"            PF1
 *
 * This routine generalizes to ascii, utf-8, chorded modifier keys,
 * function keys, color codes, c0/c1 control codes, cursor movement,
 * mouse movement, etc.
 *
 * Userspace buffering isn't required, since ANSI escape sequences and
 * UTF-8 are decoded without peeking. Noncanonical overlong encodings
 * can cause the stream to go out of sync. This function recovers such
 * events by ignoring continuation bytes at the beginning of each read.
 *
 * String control sequences, e.g. "\e_hello\e\\" currently are not
 * tokenized as a single read. Lastly note, this function has limited
 * support for UNICODE representations of C0/C1 control codes, e.g.
 *
 *     "\000"            NUL
 *     "\300\200"        NUL
 *     "\302\233A"       CURSOR UP
 *
 * @param buf is guaranteed to receive a NUL terminator if size>0
 * @return number of bytes read (helps differentiate "\0" vs. "")
 * @see examples/ttyinfo.c
 * @see ANSI X3.64-1979
 * @see ISO/IEC 6429
 * @see FIPS-86
 * @see ECMA-48
 */
ssize_t readansi(int fd, char *buf, size_t size) {
  int i, j;
  uint8_t c;
  enum { kAscii, kUtf8, kEsc, kCsi, kSs } t;
  if (size) buf[0] = 0;
  for (j = i = 0, t = kAscii;;) {
    if (i + 2 >= size) return enomem();
    if (read(fd, &c, 1) != 1) return -1;
    buf[i++] = c;
    buf[i] = 0;
    switch (t) {
      case kAscii:
        if (c < 0200) {
          if (c == '\e') {
            t = kEsc;
          } else {
            return i;
          }
        } else if (c >= 0300) {
          t = kUtf8;
          j = ThomPikeLen(c) - 1;
        }
        break;
      case kUtf8:
        if (!--j) return i;
        break;
      case kEsc:
        switch (c) {
          case '[':
            t = kCsi;
            break;
          case 'N':
          case 'O':
            t = kSs;
            break;
          case 0x20 ... 0x2F:
            break;
          default:
            return i;
        }
        break;
      case kCsi:
        switch (c) {
          case ':':
          case ';':
          case '<':
          case '=':
          case '>':
          case '?':
          case '0' ... '9':
            break;
          default:
            return i;
        }
        break;
      case kSs:
        return i;
      default:
        unreachable;
    }
  }
}
