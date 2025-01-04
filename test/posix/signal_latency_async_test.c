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

#include <cosmo.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define ITERATIONS 10000

pthread_t sender_thread;
pthread_t receiver_thread;
struct timespec send_time;
atomic_int sender_got_signal;
atomic_int receiver_got_signal;
double latencies[ITERATIONS];

void sender_signal_handler(int signo) {
  sender_got_signal = 1;
}

void receiver_signal_handler(int signo) {
  receiver_got_signal = 1;
}

void *sender_func(void *arg) {
  for (int i = 0; i < ITERATIONS; i++) {

    // Wait a bit sometimes
    if (rand() % 2) {
      volatile unsigned v = 0;
      for (;;)
        if (++v == 4000)
          break;
    }

    // Ping receiver
    clock_gettime(CLOCK_MONOTONIC, &send_time);
    if (pthread_kill(receiver_thread, SIGUSR1))
      exit(6);

    // Wait for pong
    for (;;)
      if (atomic_load_explicit(&sender_got_signal, memory_order_relaxed))
        break;
    sender_got_signal = 0;
  }

  return 0;
}

void *receiver_func(void *arg) {
  static int iteration = 0;
  do {
    // wait for signal handler to be called
    if (atomic_exchange_explicit(&receiver_got_signal, 0,
                                 memory_order_acq_rel)) {

      // record received time
      struct timespec receive_time;
      clock_gettime(CLOCK_MONOTONIC, &receive_time);
      long sec_diff = receive_time.tv_sec - send_time.tv_sec;
      long nsec_diff = receive_time.tv_nsec - send_time.tv_nsec;
      double latency_ns = sec_diff * 1e9 + nsec_diff;
      latencies[iteration++] = latency_ns;

      // pong sender
      if (pthread_kill(sender_thread, SIGUSR2))
        exit(2);
    }
  } while (iteration < ITERATIONS);
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

  // install handlers
  struct sigaction sa;
  sa.sa_handler = receiver_signal_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGUSR1, &sa, 0);
  sa.sa_handler = sender_signal_handler;
  sigaction(SIGUSR2, &sa, 0);

  // create receiver thread first
  if (pthread_create(&receiver_thread, 0, receiver_func, 0))
    exit(11);

  // create sender thread
  if (pthread_create(&sender_thread, 0, sender_func, 0))
    exit(12);

  // wait for threads to finish
  if (pthread_join(sender_thread, 0))
    exit(13);
  if (pthread_join(receiver_thread, 0))
    exit(14);

  // compute mean latency
  double total_latency = 0;
  for (int i = 0; i < ITERATIONS; i++)
    total_latency += latencies[i];
  double mean_latency = total_latency / ITERATIONS;

  // sort latencies to compute percentiles
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
