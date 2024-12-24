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
#include "libc/atomic.h"
#include "libc/cosmo.h"
#include "libc/errno.h"
#include "libc/intrin/strace.h"
#include "libc/mem/mem.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

struct AtFork {
  struct AtFork *p[2];
  atfork_f f[3];
};

struct AtForks {
  atomic_uint once;
  pthread_mutex_t lock;
  struct AtFork *list;
};

static struct AtForks _atforks = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
};

static void pthread_atfork_clear(void) {
  struct AtFork *a, *b;
  for (a = _atforks.list; a; a = b) {
    b = a->p[0];
    free(a);
  }
}

static void pthread_atfork_init(void) {
  atexit(pthread_atfork_clear);
}

static void _pthread_onfork(int i, const char *op) {
  struct AtFork *a;
  for (a = _atforks.list; a; a = a->p[!i]) {
    if (a->f[i]) {
      STRACE("pthread_atfork(%s, %t)", op, a->f[i]);
      a->f[i]();
    }
    _atforks.list = a;
  }
}

void _pthread_onfork_prepare(void) {
  _pthread_mutex_lock(&_atforks.lock);
  _pthread_onfork(0, "prepare");
}

void _pthread_onfork_parent(void) {
  _pthread_onfork(1, "parent");
  _pthread_mutex_unlock(&_atforks.lock);
}

void _pthread_onfork_child(void) {
  _pthread_mutex_wipe_np(&_atforks.lock);
  _pthread_onfork(2, "child");
}

/**
 * Registers fork callbacks.
 *
 * When fork happens, your prepare functions will be called in the
 * reverse order they were registered. Then, in the parent and child
 * processes, their callbacks will be called in the same order they were
 * registered.
 *
 * One big caveat with fork() is that it hard kills all threads except
 * the calling thread. So let's say one of those threads was printing to
 * stdout while it was killed. In that case, the stdout lock will still
 * be held when your child process comes alive, which means that the
 * child will deadlock next time it tries to print.
 *
 * The solution for that is simple. Every lock in your process should be
 * registered with this interface. However there's one highly important
 * thing you need to know. Locks must follow a consistent hierarchy. So
 * the order in which you register locks matters. If nested locks aren't
 * acquired in the same order globally, then rarely occurring deadlocks
 * will happen. So what we recommend is that you hunt down all the locks
 * that exist in your app and its dependencies, and register them all at
 * once from your main() function at startup. This ensures a clear order
 * and if you aren't sure what that order should be, cosmo libc has got
 * you covered. Simply link your program with the `cosmocc -mdbg` flag
 * and cosmo will detect locking violations with your `pthread_mutex_t`
 * objects and report them by printing the strongly connected component.
 * This will include the demangled symbol name of each mutex, assuming
 * the `pthread_mutex_t` objects are stored in static memory. cosmo.h
 * also exposes a deadlock API that lets you incorporate your own lock
 * object types into this error checking system, which we also use to
 * verify the entire libc runtime itself. See libc/intrin/deadlock.c.
 *
 * Special care should be taken when using this interface in libraries.
 * While it may seem tempting to use something like a `__constructor__`
 * attribute to register your mutexes in a clean and abstracted way, it
 * is only appropriate if your mutex is guarding pure memory operations
 * and poses zero risk of nesting with locks outside your library. For
 * example, calling open() or printf() while holding your lock will do
 * just that, since the C runtime functions you may consider pure will
 * actually use mutexes under the hood, which are also validated under
 * `cosmocc -mdbg` builds. So if your locks can't be made unnestable
 * pure memory operations, then you should consider revealing their
 * existence to users of your library.
 *
 * Here's an example of how pthread_atfork() can be used:
 *
 *     static struct {
 *       pthread_once_t once;
 *       pthread_mutex_t lock;
 *       // data structures...
 *     } g_lib;
 *
 *     static void lib_lock(void) {
 *       pthread_mutex_lock(&g_lib.lock);
 *     }
 *
 *     static void lib_unlock(void) {
 *       pthread_mutex_unlock(&g_lib.lock);
 *     }
 *
 *     static void lib_wipe(void) {
 *       pthread_mutex_wipe_np(&g_lib.lock);
 *     }
 *
 *     static void lib_setup(void) {
 *       pthread_mutex_init(&g_lib.lock, 0);
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
  cosmo_once(&_atforks.once, pthread_atfork_init);
  struct AtFork *a;
  if (!(a = calloc(1, sizeof(struct AtFork))))
    return ENOMEM;
  a->f[0] = prepare;
  a->f[1] = parent;
  a->f[2] = child;
  _pthread_mutex_lock(&_atforks.lock);
  a->p[0] = 0;
  a->p[1] = _atforks.list;
  if (_atforks.list)
    _atforks.list->p[0] = a;
  _atforks.list = a;
  _pthread_mutex_unlock(&_atforks.lock);
  return 0;
}
