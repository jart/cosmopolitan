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
#include "libc/intrin/weaken.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

/**
 * Registers fork() handlers.
 *
 * Parent and child functions are called in the same order they're
 * registered. Prepare functions are called in reverse order.
 *
 * Here's an example of how pthread_atfork() can be used:
 *
 *     static struct {
 *       pthread_once_t once;
 *       pthread_mutex_t lock;
 *       // data structures...
 *     } g_lib;
 *
 *     static void lib_wipe(void) {
 *       pthread_mutex_init(&g_lib.lock, 0);
 *     }
 *
 *     static void lib_lock(void) {
 *       pthread_mutex_lock(&g_lib.lock);
 *     }
 *
 *     static void lib_unlock(void) {
 *       pthread_mutex_unlock(&g_lib.lock);
 *     }
 *
 *     static void lib_setup(void) {
 *       lib_wipe();
 *       pthread_atfork(lib_lock, lib_unlock, lib_wipe);
 *     }
 *
 *     static void lib_init(void) {
 *       pthread_once(&g_lib.once, lib_setup);
 *     }
 *
 *     void lib(void) {
 *       lib_init();
 *       lib_lock();
 *       // do stuff...
 *       lib_unlock();
 *     }
 *
 * @param prepare is run by fork() before forking happens
 * @param parent is run by fork() after forking happens in parent process
 * @param child is run by fork() after forking happens in childe process
 * @return 0 on success, or errno on error
 * @raise ENOMEM if we require more vespene gas
 */
int pthread_atfork(atfork_f prepare, atfork_f parent, atfork_f child) {
  if (_weaken(_pthread_atfork)) {
    return _weaken(_pthread_atfork)(prepare, parent, child);
  } else {
    return 0;
  }
}
