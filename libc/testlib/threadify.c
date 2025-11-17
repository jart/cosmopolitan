// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "libc/assert.h"
#include "libc/errno.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

static struct {
  int threads;
  int iterations;
} g_threadify;

static void *_testlib_threadify_worker(void *arg) {
  for (int i = 0; i < g_threadify.iterations; ++i)
    if (_weaken(testlib_runalltests))
      _weaken(testlib_runalltests)();
  return 0;
}

void _testlib_threadify_run(void) {
  unassert(g_threadify.threads > 0);
  unassert(g_threadify.iterations > 0);
  unassert(!_weaken(testlib_enable_tmp_setup_teardown));
  pthread_t *th = malloc(g_threadify.threads * sizeof(pthread_t));
  for (int i = 0; i < g_threadify.threads; ++i) {
    errno_t err;
    do {
      err = pthread_create(&th[i], 0, _testlib_threadify_worker, 0);
    } while (err == EAGAIN);
    ASSERT_EQ(0, err);
  }
  for (int i = 0; i < g_threadify.threads; ++i)
    ASSERT_EQ(0, pthread_join(th[i], 0));
  free(th);
}

/**
 * Automate thread torture testing.
 */
void testlib_threadify(int threads, int iterations) {
  g_threadify.threads = threads;
  g_threadify.iterations = iterations;
}
