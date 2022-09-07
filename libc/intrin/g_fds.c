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
#include "libc/calls/internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/intrin/pthread.h"
#include "libc/intrin/pushpop.h"
#include "libc/intrin/spinlock.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/consts/o.h"

STATIC_YOINK("_init_g_fds");

struct Fds g_fds;
static pthread_mutex_t __fds_lock_obj;

void(__fds_lock)(void) {
  pthread_mutex_lock(&__fds_lock_obj);
}

void(__fds_unlock)(void) {
  pthread_mutex_unlock(&__fds_lock_obj);
}

static textwindows dontinline void SetupWinStd(struct Fds *fds, int i, int x) {
  int64_t h;
  h = GetStdHandle(x);
  if (!h || h == -1) return;
  fds->__init_p[i].kind = pushpop(kFdFile);
  fds->__init_p[i].handle = h;
  fds->f = i + 1;
}

textstartup void InitializeFileDescriptors(void) {
  struct Fds *fds;
  __fds_lock_obj.type = PTHREAD_MUTEX_RECURSIVE;
  fds = VEIL("r", &g_fds);
  pushmov(&fds->n, ARRAYLEN(fds->__init_p));
  fds->f = 3;
  fds->p = fds->__init_p;
  if (IsMetal()) {
    extern const char vga_console[];
    pushmov(&fds->f, 3ull);
    if (weaken(vga_console)) {
      fds->__init_p[0].kind = pushpop(kFdConsole);
      fds->__init_p[1].kind = pushpop(kFdConsole);
      fds->__init_p[2].kind = pushpop(kFdConsole);
    } else {
      fds->__init_p[0].kind = pushpop(kFdSerial);
      fds->__init_p[1].kind = pushpop(kFdSerial);
      fds->__init_p[2].kind = pushpop(kFdSerial);
    }
    fds->__init_p[0].handle = VEIL("r", 0x3F8ull);
    fds->__init_p[1].handle = VEIL("r", 0x3F8ull);
    fds->__init_p[2].handle = VEIL("r", 0x3F8ull);
  } else if (IsWindows()) {
    SetupWinStd(fds, 0, kNtStdInputHandle);
    SetupWinStd(fds, 1, kNtStdOutputHandle);
    SetupWinStd(fds, 2, kNtStdErrorHandle);
  }
  fds->__init_p[1].flags = O_WRONLY | O_APPEND;
  fds->__init_p[2].flags = O_WRONLY | O_APPEND;
}
