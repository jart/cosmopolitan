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
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/intrin/fds.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/kprintf.h"

static const char *__fdkind2str(int x) {
  switch (x) {
    case kFdEmpty:
      return "kFdEmpty";
    case kFdFile:
      return "kFdFile";
    case kFdSocket:
      return "kFdSocket";
    case kFdConsole:
      return "kFdConsole";
    case kFdDevNull:
      return "kFdDevNull";
    case kFdSerial:
      return "kFdSerial";
    case kFdZip:
      return "kFdZip";
    case kFdEpoll:
      return "kFdEpoll";
    case kFdDevRandom:
      return "kFdRandom";
    default:
      return "kFdWut";
  }
}

void __printfds(struct Fd *fds, size_t fdslen) {
  int i;
  char buf[128];
  for (i = 0; i < fdslen; ++i) {
    if (!fds[i].kind)
      continue;
    kprintf("%3d %s", i, __fdkind2str(fds[i].kind));
    if (fds[i].flags) {
      kprintf(" flags=%s", (DescribeOpenFlags)(buf, fds[i].flags));
    }
    if (fds[i].mode)
      kprintf(" mode=%#o", fds[i].mode);
    if (fds[i].handle)
      kprintf(" handle=%ld", fds[i].handle);
    kprintf("\n");
  }
}
