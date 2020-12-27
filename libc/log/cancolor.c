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
#include "libc/bits/weaken.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/log/log.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/version.h"
#include "libc/nt/files.h"
#include "libc/nt/pedef.internal.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/teb.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"

/**
 * Returns true if ANSI terminal colors are appropriate.
 *
 * We take an optimistic approach here. We use colors, unless we see the
 * environment variable TERM=dumb, which is set by software like Emacs.
 * It's a common antipattern to check isatty(STDERR_FILENO), since that
 * usually makes colors harder to get than they are to remove:
 *
 *      sed 's/\x1b\[[;[:digit:]]*m//g' <color.txt >uncolor.txt
 *
 * Ideally, all software should be updated to understand color, since
 * it's been formally standardized nearly as long as ASCII. Even old
 * MS-DOS supports it (but Windows didn't until Windows 10) yet even
 * tools like less may need wrapper scripts, e.g.:
 *
 *      #!/bin/sh
 *      LESSCHARSET=UTF-8 exec /usr/bin/less -RS "$@"
 *
 * It's that easy fam.
 */
bool cancolor(void) {
  static bool once;
  static bool result;
  const char *term;
  if (!once) {
    if (!result) {
      if ((term = getenv("TERM"))) {
        /* anything but emacs basically */
        result = strcmp(term, "dumb") != 0;
      } else {
        /* TODO(jart): Why does Mac bash login shell exec nuke TERM? */
        result = IsXnu();
      }
    }
    once = true;
  }
  return result;
}
