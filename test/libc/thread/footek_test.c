#define USE        POSIX
#define ITERATIONS 100000
#define THREADS    30

#define SPIN            1
#define FUTEX           2
#define FUTEX_SHARED    3
#define POSIX           4
#define POSIX_RECURSIVE 5
#define RWLOCK          6
#define RWLOCK_SHARED   7

#ifdef __COSMOPOLITAN__
#include <cosmo.h>
#endif

#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#ifdef __linux__
#include <linux/futex.h>
#include <sys/syscall.h>
static inline long nsync_futex_wait_(atomic_int *uaddr, int val, char pshare,
                                     int clock,
                                     const struct timespec *timeout) {
  return syscall(SYS_futex, uaddr, pshare ? FUTEX_WAIT : FUTEX_WAIT_PRIVATE,
                 val, timeout, NULL, 0);
}
static inline long nsync_futex_wake_(atomic_int *uaddr, int num_to_wake,
                                     char pshare) {
  return syscall(SYS_futex, uaddr, pshare ? FUTEX_WAKE : FUTEX_WAKE_PRIVATE,
                 num_to_wake, NULL, NULL, 0);
}
#endif

// x86 fleet
// with spin lock
// 30 threads / 100000 iterations
//
// footek_test on Linux 6.8 AMD Ryzen Threadripper PRO 7995WX
//        1,570,224 us real
//       42,690,880 us user
//            1,999 us sys
//
// footek_test on rhel7.test.           423 µs    2'638 µs     912'241 µs
//          897,815 us real
//        1,763,705 us user
//            9,696 us sys
//
// footek_test on xnu.test.          98'468 µs    5'242 µs   5'191'724 µs
//        4,225,726 us real
//       16,679,456 us user
//           16,265 us sys
//
// footek_test on freebsd.test.         690 µs    3'011 µs   2'925'997 µs
//        2,916,033 us real
//       17,236,103 us user
//                0 us sys
//
// footek_test on netbsd.test.        1'151 µs    2'634 µs   1'014'867 µs
//          790,332 us real
//        2,359,967 us user
//                0 us sys
//
// footek_test on openbsd.test.         557 µs    3'020 µs   2'554'648 µs
//        2,332,724 us real
//        9,150,000 us user
//           10,000 us sys
//
// footek_test on win10.test.           962 µs    9'698 µs   2'751'905 µs
//        2,528,863 us real
//       56,546,875 us user
//        1,671,875 us sys

// x86 fleet
// with futexes
// 30 threads / 100000 iterations
//
// footek_test on Linux 6.8 AMD Ryzen Threadripper PRO 7995WX
//          100,746 us real
//          234,451 us user
//        2,638,333 us sys
//
// footek_test on rhel7.test.           376 µs    2'259 µs     153'024 µs
//          146,015 us real
//          169,427 us user
//           68,939 us sys
//
// footek_test on xnu.test.          11'143 µs    9'159 µs     164'865 µs
//          144,917 us real
//          383,317 us user
//          191,203 us sys
//
// footek_test on freebsd.test.         394 µs    2'165 µs     256'227 µs
//          244,286 us real
//          405,395 us user
//          956,122 us sys
//
// footek_test on netbsd.test.          502 µs    2'020 µs     261'895 µs
//          209,095 us real
//          616,634 us user
//            9,945 us sys
//
// footek_test on openbsd.test.         457 µs    2'737 µs     396'342 µs
//          344,876 us real
//           50,000 us user
//        1,240,000 us sys
//
// footek_test on win10.test.           462 µs   59'528 µs   1'348'265 µs
//        1,193,906 us real
//       17,546,875 us user
//        3,000,000 us sys

// x86 fleet
// with posix
// 30 threads / 100000 iterations
//
// footek_test on Linux 6.8 AMD Ryzen Threadripper PRO 7995WX (glibc)
//          111,560 us real
//          153,985 us user
//        2,988,121 us sys
//
// footek_test on Linux 6.8 AMD Ryzen Threadripper PRO 7995WX (musl)
//          392,765 us real
//        1,885,558 us user
//        9,667,865 us sys
//
// footek_test on Linux 6.8 AMD Ryzen Threadripper PRO 7995WX (cosmo)
//           40,965 us real
//           47,168 us user
//           25,398 us sys
//
// footek_test on rhel7.test.           683 µs    1'340 µs     105'977 µs
//          101,934 us real
//          104,771 us user
//            4,068 us sys
//
// footek_test on xnu.test.           2'054 µs    5'352 µs     210'306 µs
//          181,540 us real
//          216,236 us user
//          127,344 us sys
//
// footek_test on freebsd.test. (cosmo)
//          126,803 us real
//            3,100 us user
//          176,744 us sys
//
// footek_test on freebsd.test. (freebsd libc)
//          219,073 us real
//          158,103 us user
//        1,146,252 us sys
//
// footek_test on netbsd.test.          350 µs    3'570 µs     262'186 µs
//          199,882 us real
//          138,178 us user
//          329,501 us sys
//
// footek_test on openbsd.test. (cosmo)
//          138,619 us real
//           30,000 us user
//          110,000 us sys
//
// footek_test on openbsd.test. (openbsd libc)
//          385,431 us real
//           80,000 us user
//        1,350,000 us sys
//
// footek_test on win10.test. (cosmo)
//          156,382 us real
//          312,500 us user
//           31,250 us sys
//
// footek_test on win10.test. (cygwin)
//        9,334,610 us real
//        1,562,000 us user
//        6,093,000 us sys

// arm fleet
// with spin lock
// 30 threads / 100000 iterations
//
// footek_test on studio.test.          961 µs   12'907 µs   1'287'983 µs
//        1,282,084 us real
//       29,359,582 us user
//           34,553 us sys
//
// footek_test on pi.test.              459 µs   16'376 µs   4'095'512 µs
//        4,070,988 us real
//       16,203,990 us user
//            7,999 us sys
//
// footek_test on freebsdarm.test.      502 µs   16'446 µs   7'051'545 µs
//        7,012,493 us real
//       27,936,725 us user
//            7,871 us sys

// arm fleet
// with futexes
// 30 threads / 100000 iterations
//
// footek_test on studio.test.          585 µs   13'597 µs      57'473 µs
//           46,481 us real
//           68,745 us user
//          586,871 us sys
//
// footek_test on pi5.test.             335 µs   13'034 µs     432'358 µs
//          389,619 us real
//          839,848 us user
//          679,112 us sys
//
// footek_test on pi.test.              479 µs   16'539 µs     476'395 µs
//          463,799 us real
//        1,259,267 us user
//          547,681 us sys
//
// footek_test on freebsdarm.test.      364 µs   16'898 µs   1'288'594 µs
//        1,256,134 us real
//        3,770,473 us user
//        1,214,755 us sys

// arm fleet
// with posix
// 30 threads / 100000 iterations
//
// footek_test on Apple M2 Ultra (Apple Libc)
//           45,443 us real
//           30,201 us user
//          864,650 us sys
//
// footek_test on Apple M2 Ultra (Cosmo Libc)
//           65,118 us real
//           77,891 us user
//        1,023,575 us sys
//
// footek_test on pi5.test.             407 µs   12'661 µs     198'133 µs
//          152,791 us real
//          143,678 us user
//           14,736 us sys
//
// footek_test on studio.test.          463 µs   13'286 µs     234'742 µs
//          227,916 us real
//          294,162 us user
//          155,062 us sys
//
// footek_test on pi.test.              374 µs   15'720 µs     249'245 µs
//          233,504 us real
//          301,072 us user
//          187,153 us sys
//
// footek_test on freebsdarm.test.      328 µs   16'614 µs     918'647 µs
//          877,124 us real
//        1,377,338 us user
//          798,230 us sys

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
  }
  if (word == 1)
    word = atomic_exchange_explicit(futex, 2, memory_order_acquire);
  while (word > 0) {
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
#if USE == FUTEX || USE == FUTEX_SHARED
    cosmo_futex_wait(
        futex, 2,
        USE == FUTEX_SHARED ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE,
        0, 0);
#else
    pthread_yield_np();
#endif
    pthread_setcancelstate(cs, 0);
    word = atomic_exchange_explicit(futex, 2, memory_order_acquire);
  }
}

void unlock(atomic_int *futex) {
  int word = atomic_fetch_sub_explicit(futex, 1, memory_order_release);
  if (word == 2) {
    atomic_store_explicit(futex, 0, memory_order_release);
#if USE == FUTEX || USE == FUTEX_SHARED
    cosmo_futex_wake(
        futex, 1,
        USE == FUTEX_SHARED ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE);
#endif
  }
}

int g_chores;
atomic_int g_lock;
pthread_mutex_t g_locker;
pthread_rwlock_t g_rwlocker;

void *worker(void *arg) {
  for (int i = 0; i < ITERATIONS; ++i) {
#if USE == POSIX || USE == POSIX_RECURSIVE
    pthread_mutex_lock(&g_locker);
    ++g_chores;
    pthread_mutex_unlock(&g_locker);
#elif USE == RWLOCK || USE == RWLOCK_SHARED
    pthread_rwlock_wrlock(&g_rwlocker);
    ++g_chores;
    pthread_rwlock_unlock(&g_rwlocker);
#else
    lock(&g_lock);
    ++g_chores;
    unlock(&g_lock);
#endif
  }
  return 0;
}

struct timeval tub(struct timeval a, struct timeval b) {
  a.tv_sec -= b.tv_sec;
  if (a.tv_usec < b.tv_usec) {
    a.tv_usec += 1000000;
    a.tv_sec--;
  }
  a.tv_usec -= b.tv_usec;
  return a;
}

long tomicros(struct timeval x) {
  return x.tv_sec * 1000000ul + x.tv_usec;
}

int main() {
  struct timeval start;
  gettimeofday(&start, 0);

  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
#if USE == POSIX_RECURSIVE
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#else
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_DEFAULT);
#endif
  pthread_mutex_init(&g_locker, &attr);
  pthread_mutexattr_destroy(&attr);

  pthread_rwlockattr_t rwattr;
  pthread_rwlockattr_init(&rwattr);
#if USE == RWLOCK_SHARED
  pthread_rwlockattr_setpshared(&rwattr, PTHREAD_PROCESS_SHARED);
#endif
  pthread_rwlock_init(&g_rwlocker, &rwattr);
  pthread_rwlockattr_destroy(&rwattr);

  pthread_t th[THREADS];
  for (int i = 0; i < THREADS; ++i)
    pthread_create(&th[i], 0, worker, 0);
  for (int i = 0; i < THREADS; ++i)
    pthread_join(th[i], 0);
  assert(g_chores == THREADS * ITERATIONS);

  struct rusage ru;
  struct timeval end;
  gettimeofday(&end, 0);
  getrusage(RUSAGE_SELF, &ru);
  printf("%16ld us real\n"
         "%16ld us user\n"
         "%16ld us sys\n",
         tomicros(tub(end, start)),  //
         tomicros(ru.ru_utime),      //
         tomicros(ru.ru_stime));

  pthread_rwlock_destroy(&g_rwlocker);
  pthread_mutex_destroy(&g_locker);

#ifdef __COSMOPOLITAN__
  CheckForMemoryLeaks();
#endif
}
