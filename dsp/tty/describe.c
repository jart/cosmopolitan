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
#include "libc/fmt/fmt.h"
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
