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

// arm fleet
// with futexes
// 30 threads / 100000 iterations
//
//           46,481 us real
//           68,745 us user
//          586,871 us sys
// footek_test on studio.test.          585 µs   13'597 µs      57'473 µs
//          389,619 us real
//          839,848 us user
//          679,112 us sys
// footek_test on pi5.test.             335 µs   13'034 µs     432'358 µs
//          463,799 us real
//        1,259,267 us user
//          547,681 us sys
// footek_test on pi.test.              479 µs   16'539 µs     476'395 µs
//        1,256,134 us real
//        3,770,473 us user
//        1,214,755 us sys
// footek_test on freebsdarm.test.      364 µs   16'898 µs   1'288'594 µs

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

#define SPIN  1
#define FUTEX 2
#define NSYNC 3

#define USE NSYNC

#define THREADS    10
#define ITERATIONS 50000

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
#if USE == FUTEX
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
#if USE == FUTEX
    nsync_futex_wake_(futex, 1, 0);
#endif
  }
}

int g_chores;
atomic_int g_lock;
pthread_mutex_t g_locker;

void *worker(void *arg) {
  for (int i = 0; i < ITERATIONS; ++i) {
#if USE == NSYNC
    pthread_mutex_lock(&g_locker);
    ++g_chores;
    pthread_mutex_unlock(&g_locker);
#else
    lock(&g_lock);
    ++g_chores;
    unlock(&g_lock);
#endif
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

// x86 fleet
// with pthread_mutex_t
// 30 threads / 100000 iterations
//
//          177,702 us real
//          183,488 us user
//           54,921 us sys
// footek_test on rhel7.test.           304 µs    2'225 µs     185'809 µs
//          191,346 us real
//           43,746 us user
//          257,012 us sys
// footek_test on freebsd.test.         405 µs    2'186 µs     200'568 µs
//          194,344 us real
//          228,235 us user
//          143,203 us sys
// footek_test on xnu.test.          33'207 µs    5'164 µs     220'693 µs
//          199,882 us real
//          138,178 us user
//          329,501 us sys
// footek_test on netbsd.test.          350 µs    3'570 µs     262'186 µs
//          291,255 us real
//           70,000 us user
//          440,000 us sys
// footek_test on openbsd.test.         628 µs    3'232 µs     342'136 µs
//          250,072 us real
//          437,500 us user
//           93,750 us sys
// footek_test on win10.test.           996 µs   10'949 µs     398'435 µs

// arm fleet
// with pthread_mutex_t
// 30 threads / 100000 iterations
//
//           88,681 us real
//          163,500 us user
//           22,183 us sys
// footek_test on studio.test.          651 µs   15'086 µs      98'632 µs
//          157,701 us real
//          215,597 us user
//           46,436 us sys
// footek_test on pi5.test.             296 µs   13'222 µs     159'805 µs
//          699,863 us real
//        1,027,981 us user
//          648,353 us sys
// footek_test on pi.test.              419 µs   16'716 µs     721'851 µs
//          843,858 us real
//        1,432,362 us user
//          696,613 us sys
// footek_test on freebsdarm.test.      349 µs   16'613 µs     876'863 µs
