/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/struct/sigaction.h"
#include "libc/intrin/spinlock.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/time/time.h"

volatile bool gotctrlc;

void GotCtrlC(int sig) {
  gotctrlc = true;
}

int Worker(void *arg) {
  uint8_t *p;
  unsigned x = 0;
  struct sigaction sa = {.sa_handler = GotCtrlC};
  sigaction(SIGINT, &sa, 0);
  for (;;) {
    for (p = _base; p < _end; ++p) {
      x += *p;
      if (gotctrlc) {
        return x | x >> 8 | x >> 16 | x >> 24;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  char **tls;
  int i, n, prot, flags;
  ShowCrashReports();
  n = GetCpuCount();
  tls = gc(malloc(n * sizeof(*tls)));
  for (i = 0; i < n; ++i) {
    prot = PROT_READ | PROT_WRITE;
    flags = MAP_STACK | MAP_ANONYMOUS;
    tls[i] = __initialize_tls(malloc(64));
    clone(Worker, mmap(0, GetStackSize(), prot, flags, -1, 0), GetStackSize(),
          CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
              CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID | CLONE_SETTLS,
          0, 0, tls[i], 64, (int *)(tls[i] + 0x38));
  }
  while (!gotctrlc) {
    usleep(1000);
  }
  for (i = 0; i < n; ++i) {
    _spinlock((int *)(tls[i] + 0x38));
    free(tls[i]);
  }
}
