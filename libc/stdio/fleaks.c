/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/fmt/itoa.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"

#define MIN_CLANDESTINE_FD 100  // e.g. kprintf's dup'd handle

void CheckForFileLeaks(void) {
  char msg[512];
  char *p = msg;
  char *pe = msg + 256;
  bool gotsome = false;
  for (int fd = 3; fd < MIN_CLANDESTINE_FD; ++fd) {
    if (fcntl(fd, F_GETFL) != -1) {
      if (!gotsome) {
        p = stpcpy(p, program_invocation_short_name);
        p = stpcpy(p, ": FILE DESCRIPTOR LEAKS:");
        gotsome = true;
      }
      if (p + 1 + 12 + 1 < pe) {
        *p++ = ' ';
        p = FormatInt32(p, fd);
      } else {
        break;
      }
    }
  }
  if (gotsome) {
    *p++ = '\n';
    *p = 0;
    write(2, msg, p - msg);
    char proc[64];
    p = proc;
    p = stpcpy(p, "ls -hal /proc/");
    p = FormatInt32(p, getpid());
    p = stpcpy(p, "/fd");
    system(proc);
    exit(1);
  }
}
