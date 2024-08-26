/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/atomic.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/thread.h"

static void (*funcs[32])(void);
static int count;
static pthread_spinlock_t lock;
pthread_spinlock_t *const __at_quick_exit_lockptr = &lock;

void __funcs_on_quick_exit(void) {
  void (*func)(void);
  pthread_spin_lock(&lock);
  while (count) {
    func = funcs[--count];
    pthread_spin_unlock(&lock);
    func();
    pthread_spin_lock(&lock);
  }
}

int at_quick_exit(void func(void)) {
  int res = 0;
  pthread_spin_lock(&lock);
  if (count == ARRAYLEN(funcs)) {
    res = -1;
  } else {
    funcs[count++] = func;
  }
  pthread_spin_unlock(&lock);
  return res;
}
