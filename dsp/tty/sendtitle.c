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
#include "libc/mem/arraylist2.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/x/x.h"

/* TODO(jart): DELETE */

/**
 * Changes text in title bar of pseudo-teletypewriter window.
 *
 * @param title is trustworthy text without any BEL characters
 * @param ti comes from ttyident() and null means no-op
 */
int ttysendtitle(int ttyfd, const char *title, const struct TtyIdent *ti) {
  int res;
  if (ti) {
    char *p;
    if (ti->id == kTtyIdScreen) {
      res = ttysend(ttyfd, (p = xstrcat("\eP\e]0;", title, "\a\e\\")));
    } else {
      res = ttysend(ttyfd, (p = xstrcat("\e]0;", title, "\a")));
    }
    free(p);
  } else {
    res = 0;
  }
  return res;
}
