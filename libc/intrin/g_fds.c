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
#include "libc/calls/state.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/extend.internal.h"
#include "libc/intrin/pushpop.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/thread/thread.h"

STATIC_YOINK("_init_g_fds");

struct Fds g_fds;

static textwindows dontinline void SetupWinStd(struct Fds *fds, int i, int x) {
  int64_t h;
  h = GetStdHandle(x);
  if (!h || h == -1) return;
  fds->p[i].kind = pushpop(kFdFile);
  fds->p[i].handle = h;
  atomic_store_explicit(&fds->f, i + 1, memory_order_relaxed);
}

textstartup void InitializeFileDescriptors(void) {
  struct Fds *fds;
  __fds_lock_obj._type = PTHREAD_MUTEX_RECURSIVE;
  pthread_atfork(_weaken(__fds_lock), _weaken(__fds_unlock),
                 _weaken(__fds_funlock));
  fds = VEIL("r", &g_fds);
  fds->p = fds->e = (void *)kMemtrackFdsStart;
  fds->n = 4;
  atomic_store_explicit(&fds->f, 3, memory_order_relaxed);
  fds->e = _extend(fds->p, fds->n * sizeof(*fds->p), fds->e, MAP_PRIVATE,
                   kMemtrackFdsStart + kMemtrackFdsSize);
  if (IsMetal()) {
    extern const char vga_console[];
    fds->f = 3;
    if (_weaken(vga_console)) {
      fds->p[0].kind = pushpop(kFdConsole);
      fds->p[1].kind = pushpop(kFdConsole);
      fds->p[2].kind = pushpop(kFdConsole);
    } else {
      fds->p[0].kind = pushpop(kFdSerial);
      fds->p[1].kind = pushpop(kFdSerial);
      fds->p[2].kind = pushpop(kFdSerial);
    }
    fds->p[0].handle = VEIL("r", 0x3F8ull);
    fds->p[1].handle = VEIL("r", 0x3F8ull);
    fds->p[2].handle = VEIL("r", 0x3F8ull);
  } else if (IsWindows()) {
    SetupWinStd(fds, 0, kNtStdInputHandle);
    SetupWinStd(fds, 1, kNtStdOutputHandle);
    SetupWinStd(fds, 2, kNtStdErrorHandle);
  }
  fds->p[1].flags = O_WRONLY | O_APPEND;
  fds->p[2].flags = O_WRONLY | O_APPEND;
}
