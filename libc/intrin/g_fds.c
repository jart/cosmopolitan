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
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/ttydefaults.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/extend.internal.h"
#include "libc/intrin/getenv.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/pushpop.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/runtime.h"
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

static int Atoi(const char *str) {
  int i;
  for (i = 0; '0' <= *str && *str <= '9'; ++str) {
    i *= 10;
    i += *str - '0';
  }
  return i;
}

static textwindows dontinline void SetupWinStd(struct Fds *fds, int i, int x,
                                               int sockset) {
  int64_t h;
  h = GetStdHandle(x);
  if (!h || h == -1) return;
  fds->p[i].kind = ((1 << i) & sockset) ? pushpop(kFdSocket) : pushpop(kFdFile);
  fds->p[i].handle = h;
  atomic_store_explicit(&fds->f, i + 1, memory_order_relaxed);
}

textstartup void __init_fds(int argc, char **argv, char **envp) {
  struct Fds *fds;
  __fds_lock_obj._type = PTHREAD_MUTEX_RECURSIVE;
  fds = __veil("r", &g_fds);
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
    int sockset = 0;
    struct Env var;
    var = __getenv(envp, "__STDIO_SOCKETS");
    if (var.s) {
      int i = var.i + 1;
      do {
        envp[i - 1] = envp[i];
      } while (envp[i]);
      sockset = Atoi(var.s);
    }
    if (sockset && !_weaken(socket)) {
#ifdef SYSDEBUG
      kprintf("%s: parent process passed sockets as stdio, but this program"
              " can't use them since it didn't link the socket() function\n",
              argv[0]);
      _Exit(1);
#else
      sockset = 0;  // let ReadFile() fail
#endif
    }
    SetupWinStd(fds, 0, kNtStdInputHandle, sockset);
    SetupWinStd(fds, 1, kNtStdOutputHandle, sockset);
    SetupWinStd(fds, 2, kNtStdErrorHandle, sockset);
  }
  fds->p[1].flags = O_WRONLY | O_APPEND;
  fds->p[2].flags = O_WRONLY | O_APPEND;
  __vintr = CTRL('C');
  __vquit = CTRL('\\');
}
