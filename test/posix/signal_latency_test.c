// Copyright 2024 Justine Alexandra Roberts Tunney
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

#include <assert.h>
#include <cosmo.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define ITERATIONS 10000

atomic_bool got_sigusr2;
pthread_t sender_thread;
pthread_t receiver_thread;
struct timespec send_time;
double latencies[ITERATIONS];

void sender_signal_handler(int signo) {
  got_sigusr2 = true;
}

void receiver_signal_handler(int signo) {
  struct timespec receive_time;
  if (clock_gettime(CLOCK_MONOTONIC, &receive_time) == -1)
    exit(1);

  long sec_diff = receive_time.tv_sec - send_time.tv_sec;
  long nsec_diff = receive_time.tv_nsec - send_time.tv_nsec;
  double latency_ns = sec_diff * 1e9 + nsec_diff;

  static int iteration = 0;
  if (iteration < ITERATIONS)
    latencies[iteration++] = latency_ns;

  // Send SIGUSR2 back to sender_thread
  if (pthread_kill(sender_thread, SIGUSR2))
    exit(2);

  // Exit if we're done.
  if (iteration >= ITERATIONS)
    pthread_exit(0);
}

void *sender_func(void *arg) {
  // Block SIGUSR2
  sigset_t block_set;
  sigemptyset(&block_set);
  sigaddset(&block_set, SIGUSR2);
  if (pthread_sigmask(SIG_BLOCK, &block_set, 0))
    exit(3);

  // Install signal handler for SIGUSR2
  struct sigaction sa;
  sa.sa_handler = sender_signal_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGUSR2, &sa, 0))
    exit(4);

  for (int i = 0; i < ITERATIONS; i++) {
    if (clock_gettime(CLOCK_MONOTONIC, &send_time))
      exit(5);

    // Send SIGUSR1 to receiver_thread
    got_sigusr2 = false;
    if (pthread_kill(receiver_thread, SIGUSR1))
      exit(6);

    // Unblock SIGUSR2 and wait for it
    sigset_t wait_set;
    sigemptyset(&wait_set);
    while (!got_sigusr2)
      if (sigsuspend(&wait_set) && errno != EINTR)
        exit(7);
  }

  return 0;
}

void *receiver_func(void *arg) {
  // Install signal handler for SIGUSR1
  struct sigaction sa;
  sa.sa_handler = receiver_signal_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGUSR1, &sa, 0))
    exit(8);

  // Block all signals except SIGUSR1
  sigset_t block_set;
  sigfillset(&block_set);
  sigdelset(&block_set, SIGUSR1);
  if (pthread_sigmask(SIG_SETMASK, &block_set, 0))
    exit(9);

  // Wait indefinitely for signals
  while (1)
    pause();

  return 0;
}

int compare(const void *a, const void *b) {
  const double *x = a, *y = b;
  if (*x < *y)
    return -1;
  else if (*x > *y)
    return 1;
  else
    return 0;
}

int main() {

  // this test probably exposes a bug in openbsd
  if (IsOpenbsd())
    return 0;

  // Block SIGUSR1 and SIGUSR2 in main thread
  sigset_t block_set;
  sigemptyset(&block_set);
  sigaddset(&block_set, SIGUSR1);
  sigaddset(&block_set, SIGUSR2);
  if (pthread_sigmask(SIG_BLOCK, &block_set, 0))
    exit(10);

  // Create receiver thread first
  if (pthread_create(&receiver_thread, 0, receiver_func, 0))
    exit(11);

  // Create sender thread
  if (pthread_create(&sender_thread, 0, sender_func, 0))
    exit(12);

  // Wait for threads to finish
  if (pthread_join(sender_thread, 0))
    exit(13);
  if (pthread_join(receiver_thread, 0))
    exit(14);

  // Compute mean latency
  double total_latency = 0;
  for (int i = 0; i < ITERATIONS; i++)
    total_latency += latencies[i];
  double mean_latency = total_latency / ITERATIONS;

  // Sort latencies to compute percentiles
  qsort(latencies, ITERATIONS, sizeof(double), compare);

  double p50 = latencies[(int)(0.50 * ITERATIONS)];
  double p90 = latencies[(int)(0.90 * ITERATIONS)];
  double p95 = latencies[(int)(0.95 * ITERATIONS)];
  double p99 = latencies[(int)(0.99 * ITERATIONS)];

  printf("Mean latency: %.2f ns\n", mean_latency);
  printf("50th percentile latency: %.2f ns\n", p50);
  printf("90th percentile latency: %.2f ns\n", p90);
  printf("95th percentile latency: %.2f ns\n", p95);
  printf("99th percentile latency: %.2f ns\n", p99);
}
