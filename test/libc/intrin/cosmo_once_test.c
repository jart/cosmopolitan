/*-*-mode:c++;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8-*-│
│ vi: set et ft=c++ ts=2 sts=2 sw=2 fenc=utf-8                             :vi │
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
#include <assert.h>
#include <cosmo.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <sys/resource.h>
#include <time.h>

#define N 32

int y;
atomic_int x;
cosmo_once_t once;
pthread_barrier_t b;

void InitFactory(void) {
  npassert(!atomic_load_explicit(&x, memory_order_relaxed));
  npassert(!y++);
  usleep(10000);
}

void *Worker(void *arg) {
  pthread_barrier_wait(&b);
  npassert(!cosmo_once(&once, InitFactory));
  npassert(y == 1);
  atomic_fetch_add_explicit(&x, 1, memory_order_relaxed);
  return 0;
}

void test_torture() {
  pthread_t th[N];
  once = 0;
  x = y = 0;
  npassert(!pthread_barrier_init(&b, 0, N));
  for (int i = 0; i < N; ++i)
    npassert(!pthread_create(th + i, 0, Worker, 0));
  for (int i = 0; i < N; ++i)
    npassert(!pthread_join(th[i], 0));
  npassert(y == 1);
  npassert(atomic_load_explicit(&x, memory_order_relaxed) == N);
  npassert(!pthread_barrier_destroy(&b));
}

int main() {
  struct timeval start;
  gettimeofday(&start, 0);
  cosmo_stack_setmaxstacks(N);

  // todo(jart): why does setrlimit() in compile.ape cause mmap() to
  //             fail for no good reason in qemu-aarch64?
  int omg = 32;
  if (IsQemuUser())
    omg = 8;

  for (int i = 0; i < omg; ++i)
    test_torture();

  struct rusage ru;
  struct timeval end;
  gettimeofday(&end, 0);
  getrusage(RUSAGE_SELF, &ru);
  printf("%16ld us real\n"
         "%16ld us user\n"
         "%16ld us sys\n",
         timeval_tomicros(timeval_sub(end, start)),  //
         timeval_tomicros(ru.ru_utime),              //
         timeval_tomicros(ru.ru_stime));
}
