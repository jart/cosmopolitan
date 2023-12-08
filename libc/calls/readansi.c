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
#include "libc/calls/calls.h"
#include "libc/errno.h"
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
 *     "\e[38;5;202m"    ORANGERED
 *     "\e[A"            UP
 *     "\e\e[A"          ALT-UP
 *     "\001"            CTRL-ALFA
 *     "\e\001"          ALT-CTRL-ALFA
 *     "\eOP"            PF1
 *     "\000"            NUL
 *     "\e]rm -rf /\e\\" OSC
 *     "\302\233A"       UP
 *     "\300\200"        NUL
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
 * @param p is guaranteed to receive a NUL terminator if n>0
 * @return number of bytes read (helps differentiate "\0" vs. "")
 * @see examples/ttyinfo.c
 * @see ANSI X3.64-1979
 * @see ISO/IEC 6429
 * @see FIPS-86
 * @see ECMA-48
 */
ssize_t readansi(int fd, char *p, size_t n) {
  wint_t x;
  ssize_t rc;
  int e, i, j;
  unsigned char c;
  enum { kAscii, kUtf8, kEsc, kCsi1, kCsi2, kSs, kNf, kStr, kStr2, kDone } t;
  e = errno;
  t = kAscii;
  x = i = j = 0;
  if (n) p[0] = 0;
  do {
    for (;;) {
      if (n) {
        rc = read(fd, &c, 1);
      } else {
        rc = read(fd, 0, 0);
      }
      if (rc == -1 && errno == EINTR) {
        if (!i) {
          return -1;
        }
      } else if (rc == -1) {
        return -1;
      } else if (!rc) {
        if (!i) {
          errno = e;
          return 0;
        } else {
          return eilseq();
        }
      } else {
        break;
      }
    }
    if (i + 1 < n) {
      p[i] = c;
      p[i + 1] = 0;
    } else if (i < n) {
      p[i] = 0;
    }
    ++i;
    switch (t) {
    Whoopsie:
      if (n) p[0] = c;
      t = kAscii;
      i = 1;
        /* fallthrough */
      case kAscii:
        if (c < 0200) {
          if (c == '\e') {
            t = kEsc;
          } else {
            t = kDone;
          }
        } else if (c >= 0300) {
          t = kUtf8;
          x = ThomPikeByte(c);
          j = ThomPikeLen(c) - 1;
        } else {
          /* ignore overlong sequences */
        }
        break;
      case kUtf8:
        if ((c & 0300) == 0200) {
          x = ThomPikeMerge(x, c);
          if (!--j) {
            switch (x) {
              case '\e':
                t = kEsc; /* parsed but not canonicalized */
                break;
              case 0x9b:
                t = kCsi1; /* unusual but legal */
                break;
              case 0x8e:
              case 0x8f:
                t = kSs; /* unusual but legal */
                break;
              case 0x90: /* DCS (Device Control String) */
              case 0x98: /* SOS (Start of String) */
              case 0x9d: /* OSC (Operating System Command) */
              case 0x9e: /* PM  (Privacy Message) */
              case 0x9f: /* APC (Application Program Command) */
                t = kStr;
                break;
              default:
                t = kDone;
                break;
            }
          }
        } else {
          goto Whoopsie; /* ignore underlong sequences if not eof */
        }
        break;
      case kEsc:
        if (0x20 <= c && c <= 0x2f) { /* Nf */
          /*
           * Almost no one uses ANSI Nf sequences
           * They overlaps with alt+graphic keystrokes
           * We care more about being able to type alt-/
           */
          if (c == ' ' || c == '#') {
            t = kNf;
          } else {
            t = kDone;
          }
        } else if (0x30 <= c && c <= 0x3f) { /* Fp */
          t = kDone;
        } else if (0x20 <= c && c <= 0x5F) { /* Fe */
          switch (c) {
            case '[':
              t = kCsi1;
              break;
            case 'N': /* SS2 */
            case 'O': /* SS3 */
              t = kSs;
              break;
            case 'P': /* DCS (Device Control String) */
            case 'X': /* SOS (Start of String) */
            case ']': /* DCS (Operating System Command) */
            case '^': /* PM  (Privacy Message) */
            case '_': /* DCS (Application Program Command) */
              t = kStr;
              break;
            default:
              t = kDone;
              break;
          }
        } else if (0x60 <= c && c <= 0x7e) { /* Fs */
          t = kDone;
        } else if (c == '\e') {
          if (i < 3) {
            t = kEsc; /* alt chording */
          } else {
            t = kDone; /* esc mashing */
            i = 1;
          }
        } else {
          t = kDone;
        }
        break;
      case kSs:
        t = kDone;
        break;
      case kNf:
        if (0x30 <= c && c <= 0x7e) {
          t = kDone;
        } else if (!(0x20 <= c && c <= 0x2f)) {
          goto Whoopsie;
        }
        break;
      case kCsi1:
        if (0x20 <= c && c <= 0x2f) {
          t = kCsi2;
        } else if (c == '[' && (i == 3 || (i == 4 && p[1] == '\e'))) {
          /* linux function keys */
        } else if (0x40 <= c && c <= 0x7e) {
          t = kDone;
        } else if (!(0x30 <= c && c <= 0x3f)) {
          goto Whoopsie;
        }
        break;
      case kCsi2:
        if (0x40 <= c && c <= 0x7e) {
          t = kDone;
        } else if (!(0x20 <= c && c <= 0x2f)) {
          goto Whoopsie;
        }
        break;
      case kStr:
        switch (c) {
          case '\a':
            t = kDone;
            break;
          case '\e': /* ESC */
          case 0302: /* C1 (UTF-8) */
            t = kStr2;
            break;
          default:
            break;
        }
        break;
      case kStr2:
        switch (c) {
          case '\a':
            t = kDone;
            break;
          case '\\': /* ST (ASCII) */
          case 0234: /* ST (UTF-8) */
            t = kDone;
            break;
          default:
            t = kStr;
            break;
        }
        break;
      default:
        __builtin_unreachable();
    }
  } while (t != kDone);
  errno = e;
  return i;
}
