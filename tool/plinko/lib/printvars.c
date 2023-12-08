/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/print.h"

int PrintArgs(int fd, int keys, int vals, int n) {
  if (!keys) return n;
  if (keys > 0) {
    if (!(vals < 0 && Car(vals) == kClosure)) {
      if (n) {
        n += PrintChar(fd, L';');
        n += PrintChar(fd, L' ');
      }
      n += Print(fd, keys);
      n += PrintChar(fd, L'=');
      n += Print(fd, vals);
    }
    return n;
  }
  if (vals > 0) {
    if (n) {
      n += PrintChar(fd, L';');
      n += PrintChar(fd, L' ');
    }
    n += Print(fd, Car(keys));
    n += PrintChar(fd, L'=');
    n += PrintChar(fd, L'!');
    n += Print(fd, vals);
    vals = 0;
  } else {
    n += PrintArgs(fd, Car(keys), Car(vals), n);
  }
  if (!Cdr(keys)) return n;
  return PrintArgs(fd, Cdr(keys), Cdr(vals), n);
}
