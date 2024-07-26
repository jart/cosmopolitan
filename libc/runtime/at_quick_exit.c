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
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/thread.h"

static int count;
static void (*funcs[32])(void);
static pthread_mutex_t __quick_exit_lock_obj;

static void __quick_exit_wipe(void) {
  pthread_mutex_init(&__quick_exit_lock_obj, 0);
}

static void __quick_exit_lock(void) {
  pthread_mutex_lock(&__quick_exit_lock_obj);
}

static void __quick_exit_unlock(void) {
  pthread_mutex_unlock(&__quick_exit_lock_obj);
}

void __funcs_on_quick_exit(void) {
  void (*func)(void);
  __quick_exit_lock();
  while (count) {
    func = funcs[--count];
    __quick_exit_unlock();
    func();
    __quick_exit_lock();
  }
}

int at_quick_exit(void func(void)) {
  int res = 0;
  __quick_exit_lock();
  if (count == ARRAYLEN(funcs)) {
    res = -1;
  } else {
    funcs[count++] = func;
  }
  __quick_exit_unlock();
  return res;
}

__attribute__((__constructor__(10))) textstartup void __quick_exit_init(void) {
  pthread_atfork(__quick_exit_lock, __quick_exit_unlock, __quick_exit_wipe);
  __quick_exit_wipe();
}
