#include <assert.h>
#include <cosmo.h>
#include <linux/futex.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>
#include "third_party/nsync/futex.internal.h"

// THIS IS AN EXAMPLE OF HOW TO USE COSMOPOLITAN FUTEXES TO IMPLEMENT
// YOUR OWN MUTEXES FROM SCRATCH. LOOK AT HOW MUCH BETTER THIS IT CAN
// MAKE THINGS COMPARED TO SPIN LOCKS. ALGORITHM FROM ULRICH DREPPER.

// arm fleet
// with futexes
// 30 threads / 100000 iterations
//
//          242,604 us real
//        4,222,946 us user
//        1,079,229 us sys
// footek_test on studio.test.          630 µs   17'415 µs     256'782 µs
//        1,362,557 us real
//        3,232,978 us user
//        2,104,824 us sys
// footek_test on pi.test.              611 µs   21'708 µs   1'385'129 µs
//        1,346,482 us real
//        3,370,513 us user
//        1,992,383 us sys
// footek_test on freebsdarm.test.      427 µs   19'967 µs   1'393'476 µs

// arm fleet
// without futexes
// 30 threads / 100000 iterations
//
//        1,282,084 us real
//       29,359,582 us user
//           34,553 us sys
// footek_test on studio.test.          961 µs   12'907 µs   1'287'983 µs
//        4,070,988 us real
//       16,203,990 us user
//            7,999 us sys
// footek_test on pi.test.              459 µs   16'376 µs   4'095'512 µs
//        7,012,493 us real
//       27,936,725 us user
//            7,871 us sys
// footek_test on freebsdarm.test.      502 µs   16'446 µs   7'051'545 µs

// x86 fleet
// with futexes
// 30 threads / 100000 iterations
//
//          146,015 us real
//          169,427 us user
//           68,939 us sys
// footek_test on rhel7.test.           376 µs    2'259 µs     153'024 µs
//          144,917 us real
//          383,317 us user
//          191,203 us sys
// footek_test on xnu.test.          11'143 µs    9'159 µs     164'865 µs
//          244,286 us real
//          405,395 us user
//          956,122 us sys
// footek_test on freebsd.test.         394 µs    2'165 µs     256'227 µs
//          209,095 us real
//          616,634 us user
//            9,945 us sys
// footek_test on netbsd.test.          502 µs    2'020 µs     261'895 µs
//          344,876 us real
//           50,000 us user
//        1,240,000 us sys
// footek_test on openbsd.test.         457 µs    2'737 µs     396'342 µs
//        1,193,906 us real
//       17,546,875 us user
//        3,000,000 us sys
// footek_test on win10.test.           462 µs   59'528 µs   1'348'265 µs

// x86 fleet
// without futexes
// 30 threads / 100000 iterations
//
//          897,815 us real
//        1,763,705 us user
//            9,696 us sys
// footek_test on rhel7.test.           423 µs    2'638 µs     912'241 µs
//          790,332 us real
//        2,359,967 us user
//                0 us sys
// footek_test on netbsd.test.        1'151 µs    2'634 µs   1'014'867 µs
//        2,332,724 us real
//        9,150,000 us user
//           10,000 us sys
// footek_test on openbsd.test.         557 µs    3'020 µs   2'554'648 µs
//        2,528,863 us real
//       56,546,875 us user
//        1,671,875 us sys
// footek_test on win10.test.           962 µs    9'698 µs   2'751'905 µs
//        2,916,033 us real
//       17,236,103 us user
//                0 us sys
// footek_test on freebsd.test.         690 µs    3'011 µs   2'925'997 µs
//        4,225,726 us real
//       16,679,456 us user
//           16,265 us sys
// footek_test on xnu.test.          98'468 µs    5'242 µs   5'191'724 µs

#define USE_FUTEX  1
#define THREADS    30
#define ITERATIONS 30000

#define MUTEX_LOCKED(word)  ((word) & 8)
#define MUTEX_WAITING(word) ((word) & 16)

#define MUTEX_LOCK(word)        ((word) | 8)
#define MUTEX_SET_WAITING(word) ((word) | 16)
#define MUTEX_UNLOCK(word)      ((word) & ~(8 | 16))

void lock(atomic_int *futex) {
  int word, cs;
  for (int i = 0; i < 4; ++i) {
    word = 0;
    if (atomic_compare_exchange_strong_explicit(
            futex, &word, 1, memory_order_acquire, memory_order_acquire))
      return;
    pthread_pause_np();
  }
  if (word == 1)
    word = atomic_exchange_explicit(futex, 2, memory_order_acquire);
  while (word > 0) {
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
#if USE_FUTEX
    nsync_futex_wait_(futex, 2, 0, 0);
#endif
    pthread_setcancelstate(cs, 0);
    word = atomic_exchange_explicit(futex, 2, memory_order_acquire);
  }
}

void unlock(atomic_int *futex) {
  int word = atomic_fetch_sub_explicit(futex, 1, memory_order_release);
  if (word == 2) {
    atomic_store_explicit(futex, 0, memory_order_release);
#if USE_FUTEX
    nsync_futex_wake_(futex, 1, 0);
#endif
  }
}

int g_chores;
atomic_int g_lock;
pthread_mutex_t g_locker;

void *worker(void *arg) {
  for (int i = 0; i < ITERATIONS; ++i) {
    lock(&g_lock);
    ++g_chores;
    unlock(&g_lock);
  }
  return 0;
}

int main() {
  struct timeval start;
  gettimeofday(&start, 0);

  pthread_t th[THREADS];
  for (int i = 0; i < THREADS; ++i)
    pthread_create(&th[i], 0, worker, 0);
  for (int i = 0; i < THREADS; ++i)
    pthread_join(th[i], 0);
  npassert(g_chores == THREADS * ITERATIONS);

  struct rusage ru;
  struct timeval end;
  gettimeofday(&end, 0);
  getrusage(RUSAGE_SELF, &ru);
  printf("%,16ld us real\n"
         "%,16ld us user\n"
         "%,16ld us sys\n",
         timeval_tomicros(timeval_sub(end, start)),  //
         timeval_tomicros(ru.ru_utime),              //
         timeval_tomicros(ru.ru_stime));

  CheckForMemoryLeaks();
}

// COMPARE ULRICH DREPPER'S LOCKING ALGORITHM WITH MIKE BURROWS *NSYNC
// WHICH IS WHAT COSMOPOLITAN LIBC USES FOR YOUR POSIX THREADS MUTEXES

// x86 fleet
// with pthread_mutex_t
// 30 threads / 100000 iterations
//
//          186,976 us real
//           43,609 us user
//          205,585 us sys
// footek_test on freebsd.test.         410 µs    2'054 µs     195'339 µs
//          238,902 us real
//          235,743 us user
//           97,881 us sys
// footek_test on rhel7.test.           343 µs    2'339 µs     246'926 µs
//          201,285 us real
//          249,612 us user
//          141,230 us sys
// footek_test on xnu.test.           1'960 µs    5'350 µs     265'758 µs
//          303,363 us real
//           60,000 us user
//          410,000 us sys
// footek_test on openbsd.test.         545 µs    3'023 µs     326'200 µs
//          386,085 us real
//          586,455 us user
//          466,991 us sys
// footek_test on netbsd.test.          344 µs    2'421 µs     413'440 µs
//          245,010 us real
//          437,500 us user
//          140,625 us sys
// footek_test on win10.test.           300 µs   18'574 µs     441'225 µs

// arm fleet
// with pthread_mutex_t
// 30 threads / 100000 iterations
//
//           87,132 us real
//          183,517 us user
//           20,020 us sys
// footek_test on studio.test.          560 µs   12'418 µs      92'825 µs
//          679,374 us real
//          957,678 us user
//          605,078 us sys
// footek_test on pi.test.              462 µs   16'574 µs     702'833 µs
//          902,343 us real
//        1,459,706 us user
//          781,140 us sys
// footek_test on freebsdarm.test.      400 µs   16'261 µs     970'022 µs
