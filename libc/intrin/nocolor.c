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
#include "libc/dce.h"
#include "libc/log/internal.h"
#include "libc/nt/version.h"
#include "libc/runtime/runtime.h"

#define IsDumb(s) \
  (s[0] == 'd' && s[1] == 'u' && s[2] == 'm' && s[3] == 'b' && !s[4])

/**
 * Indicates if ANSI terminal colors are inappropriate.
 *
 * Normally this variable should be false. We only set it to true if
 * we're running on an old version of Windows or the environment
 * variable `TERM` is set to `dumb`.
 *
 * We think colors should be the norm, since most software is usually
 * too conservative about removing them. Rather than using `isatty`
 * consider using sed for instances where color must be removed:
 *
 *      sed 's/\x1b\[[;[:digit:]]*m//g' <color.txt >uncolor.txt
 *
 * For some reason, important software is configured by default in many
 * operating systems, to not only disable colors, but utf-8 too! Here's
 * an example of how a wrapper script can fix that for `less`.
 *
 *      #!/bin/sh
 *      LESSCHARSET=UTF-8 exec /usr/bin/less -RS "$@"
 *
 * Thank you for using colors!
 */
bool __nocolor;

optimizesize textstartup void __nocolor_init(int argc, char **argv, char **envp,
                                             intptr_t *auxv) {
  char *s;
  __nocolor = (IsWindows() && !IsAtLeastWindows10()) ||
              ((s = getenv("TERM")) && IsDumb(s));
}

const void *const __nocolor_ctor[] initarray = {
    __nocolor_init,
};
