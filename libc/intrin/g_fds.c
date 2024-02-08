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
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/ttydefaults.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/extend.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/pushpop.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/console.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/thread/thread.h"

#define OPEN_MAX 16

#ifdef __x86_64__
__static_yoink("_init_g_fds");
#endif

struct Fds g_fds;
static struct Fd g_fds_static[OPEN_MAX];

static bool TokAtoi(const char **str, long *res) {
  int c, d;
  unsigned long x = 0;
  d = **str == '-' ? -1 : 1;
  while ((c = *(*str)++)) {
    if (('0' <= c && c <= '9')) {
      x *= 10;
      x += (c - '0') * d;
    } else {
      *res = x;
      return true;
    }
  }
  return false;
}

static textwindows void SetupWinStd(struct Fds *fds, int i, uint32_t x) {
  int64_t h;
  uint32_t cm;
  h = GetStdHandle(x);
  if (!h || h == -1) return;
  fds->p[i].kind = GetConsoleMode(h, &cm) ? kFdConsole : kFdFile;
  fds->p[i].handle = h;
  atomic_store_explicit(&fds->f, i + 1, memory_order_relaxed);
}

textstartup void __init_fds(int argc, char **argv, char **envp) {
  struct Fds *fds;
  fds = &g_fds;
  fds->n = 4;
  atomic_store_explicit(&fds->f, 3, memory_order_relaxed);
  if (_weaken(_extend)) {
    fds->p = fds->e = (void *)kMemtrackFdsStart;
    fds->e =
        _weaken(_extend)(fds->p, fds->n * sizeof(*fds->p), fds->e, MAP_PRIVATE,
                         kMemtrackFdsStart + kMemtrackFdsSize);
  } else {
    fds->p = g_fds_static;
    fds->e = g_fds_static + OPEN_MAX;
  }

  // inherit standard i/o file descriptors
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
    fds->p[0].handle = __veil("r", 0x3F8ull);
    fds->p[1].handle = __veil("r", 0x3F8ull);
    fds->p[2].handle = __veil("r", 0x3F8ull);
  } else if (IsWindows()) {
    for (long i = 0; i < 3; ++i) {
      SetupWinStd(fds, i, kNtStdio[i]);
    }
  }
  fds->p[0].flags = O_RDONLY;
  fds->p[1].flags = O_WRONLY | O_APPEND;
  fds->p[2].flags = O_WRONLY | O_APPEND;

  // inherit file descriptors from cosmo parent process
  if (IsWindows()) {
    const char *fdspec;
    if ((fdspec = getenv("_COSMO_FDS"))) {
      unsetenv("_COSMO_FDS");
      for (;;) {
        long fd, kind, flags, mode, handle, pointer, type, family, protocol;
        if (!TokAtoi(&fdspec, &fd)) break;
        if (!TokAtoi(&fdspec, &handle)) break;
        if (!TokAtoi(&fdspec, &kind)) break;
        if (!TokAtoi(&fdspec, &flags)) break;
        if (!TokAtoi(&fdspec, &mode)) break;
        if (!TokAtoi(&fdspec, &pointer)) break;
        if (!TokAtoi(&fdspec, &type)) break;
        if (!TokAtoi(&fdspec, &family)) break;
        if (!TokAtoi(&fdspec, &protocol)) break;
        __ensurefds_unlocked(fd);
        struct Fd *f = fds->p + fd;
        if (f->handle && f->handle != -1 && f->handle != handle) {
          CloseHandle(f->handle);
          if (fd < 3) {
            SetStdHandle(kNtStdio[fd], handle);
          }
        }
        f->handle = handle;
        f->kind = kind;
        f->flags = flags;
        f->mode = mode;
        f->pointer = pointer;
        f->type = type;
        f->family = family;
        f->protocol = protocol;
        atomic_store_explicit(&fds->f, fd + 1, memory_order_relaxed);
      }
    }
  }
}
