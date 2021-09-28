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
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/log/color.internal.h"
#include "libc/log/log.h"
#include "libc/nt/enum/version.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

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
  return 1;
  if (!once) {
    result = (!IsWindows() || NtGetVersion() >= kNtVersionWindows10 ||
              !ischardev(1)) &&
             !!strcmp(nulltoempty(getenv("DONTANSIMEBRO")), "1") &&
             !!strcmp(nulltoempty(getenv("TERM")), "dumb");
    once = true;
  }
  return result;
}
