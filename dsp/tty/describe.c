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
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/x/x.h"

#define BUFFY 48

static char *ttydescriber(char *b, const struct TtyIdent *ti) {
  switch (ti->id) {
    case 0:
      snprintf(b, BUFFY, "%s %d", "putty", ti->version);
      break;
    case 1:
      if (ti->version > 1000) {
        snprintf(b, BUFFY, "%s %d", "gnome terminal", ti->version);
      } else {
        snprintf(b, BUFFY, "%s %d", "mlterm", ti->version);
      }
      break;
    case kTtyIdScreen:
      snprintf(b, BUFFY, "%s %d", "gnu screen", ti->version);
      break;
    case 77:
      snprintf(b, BUFFY, "%s %d", "redhat mintty", ti->version);
      break;
    case 41:
      snprintf(b, BUFFY, "%s %d", "xterm", ti->version);
      break;
    case 82:
      snprintf(b, BUFFY, "%s %d", "rxvt", ti->version);
      break;
    default:
      snprintf(b, BUFFY, "%s %d %d", "unknown teletypewriter no.", ti->id,
               ti->version);
      break;
  }
  return b;
}

/**
 * Makes educated guess about name of teletypewriter.
 */
char *ttydescribe(char *out, size_t size, const struct TtyIdent *ti) {
  char b1[BUFFY], b2[BUFFY];
  if (ti) {
    ttydescriber(b1, ti);
    if (ti->next) {
      snprintf(out, size, "%s%s%s", ttydescriber(b2, ti->next), " inside ", b1);
    } else {
      snprintf(out, size, "%s", b1);
    }
  } else {
    snprintf(out, size, "%s", "no tty");
  }
  return out;
}
