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
#include <stdlib.h>
#include <sys/resource.h>
#include <time.h>

////////////////////////////////////////////////////////////////////////////////
// test pthread_once() works with c++ exceptions

struct MyException {};

pthread_once_t g_once = PTHREAD_ONCE_INIT;
unsigned g_called;

void boom(void) {
  ++g_called;
  throw MyException();
}

void noop(void) {
  ++g_called;
}

void test_exception_throwing() {
  try {
    npassert(!pthread_once(&g_once, boom));
    npassert(false);
  } catch (MyException) {
    npassert(!pthread_once(&g_once, noop));
    npassert(g_called == 2);
  }
}

////////////////////////////////////////////////////////////////////////////////
// test pthread_once() works with pthread_exit()

pthread_once_t my_once = PTHREAD_ONCE_INIT;
int my_called;

void die(void) {
  ++my_called;
  pthread_exit(0);
}

void again(void) {
  ++my_called;
}

void *mythread(void *arg) {
  pthread_once(&my_once, die);
  abort();
}

void test_pthread_exit() {
  pthread_t th;
  npassert(!pthread_create(&th, 0, mythread, 0));
  npassert(!pthread_join(th, 0));
  npassert(!pthread_once(&my_once, again));
  npassert(my_called == 2);
}

////////////////////////////////////////////////////////////////////////////////
// test pthread_once() works with pthread_cancel()

pthread_once_t global_once = PTHREAD_ONCE_INIT;
int global_called;

void waitforit(void) {
  ++global_called;
  for (;;)
    usleep(1);
}

void again2(void) {
  ++global_called;
}

void *globalthread(void *arg) {
  pthread_once(&global_once, waitforit);
  abort();
}

void test_pthread_cancel() {
  pthread_t th;
  npassert(!pthread_create(&th, 0, globalthread, 0));
  npassert(!pthread_cancel(th));
  npassert(!pthread_join(th, 0));
  npassert(!pthread_once(&global_once, again2));
  npassert(global_called == 2);
}

////////////////////////////////////////////////////////////////////////////////

#define N 32

int y;
atomic_int x;
pthread_barrier_t b;
pthread_once_t once = PTHREAD_ONCE_INIT;

void InitFactory(void) {
  npassert(!atomic_load_explicit(&x, memory_order_relaxed));
  npassert(!y++);
  usleep(10000);
}

void *Worker(void *arg) {
  pthread_barrier_wait(&b);
  npassert(!pthread_once(&once, InitFactory));
  npassert(y == 1);
  atomic_fetch_add_explicit(&x, 1, memory_order_relaxed);
  return 0;
}

void test_torture() {
  pthread_t th[N];
  once = (pthread_once_t)PTHREAD_ONCE_INIT;
  x = y = 0;
  npassert(!pthread_barrier_init(&b, 0, N));
  for (int i = 0; i < N; ++i)
    unassert(!pthread_create(th + i, 0, Worker, 0));
  for (int i = 0; i < N; ++i)
    npassert(!pthread_join(th[i], 0));
  npassert(y == 1);
  npassert(atomic_load_explicit(&x, memory_order_relaxed) == N);
  npassert(!pthread_barrier_destroy(&b));
}

////////////////////////////////////////////////////////////////////////////////

int main() {
  struct timeval start;
  gettimeofday(&start, 0);
  cosmo_stack_setmaxstacks(N);

  // todo(jart): why does setrlimit() in compile.ape cause mmap() to
  //             fail for no good reason in qemu-aarch64?
  int omg = 32;
  if (IsQemuUser())
    omg = 8;

  test_exception_throwing();
  test_pthread_exit();
  test_pthread_cancel();
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
