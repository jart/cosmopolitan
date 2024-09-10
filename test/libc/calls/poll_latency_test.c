/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include <arpa/inet.h>
#include <assert.h>
#include <cosmo.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define NUM_MEASUREMENTS 10
#define BUFFER_SIZE      sizeof(struct timespec)

atomic_int global_state;

typedef struct {
  int port;
  int client_sock;
} listener_data;

void *sender_thread(void *arg) {
  listener_data *data = (listener_data *)arg;
  int sockfd = socket(data->port == 0 ? AF_INET : AF_INET6, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  void *addr;
  struct sockaddr_in addr_v4 = {0};
  struct sockaddr_in6 addr_v6 = {0};
  socklen_t addr_len;

  if (data->port == 0) {  // IPv4
    addr_v4.sin_family = AF_INET;
    addr_v4.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr_v4.sin_port = 0;
    addr = &addr_v4;
    addr_len = sizeof(addr_v4);
  } else {  // IPv6
    addr_v6.sin6_family = AF_INET6;
    addr_v6.sin6_addr = in6addr_loopback;
    addr_v6.sin6_port = 0;
    addr = &addr_v6;
    addr_len = sizeof(addr_v6);
  }

  if (bind(sockfd, addr, addr_len) < 0) {
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }

  if (getsockname(sockfd, addr, &addr_len) < 0) {
    perror("getsockname failed");
    exit(EXIT_FAILURE);
  }

  data->port = ntohs(data->port == 0 ? addr_v4.sin_port : addr_v6.sin6_port);

  if (listen(sockfd, 1) < 0) {
    perror("Listen failed");
    exit(EXIT_FAILURE);
  }

  atomic_fetch_add(&global_state, 1);
  data->client_sock = accept(sockfd, NULL, NULL);
  if (data->client_sock < 0) {
    perror("Accept failed");
    exit(EXIT_FAILURE);
  }
  atomic_fetch_add(&global_state, 1);

  struct timespec ts;
  for (int i = 0; i < NUM_MEASUREMENTS; i++) {
    while (atomic_load(&global_state)) {
    }
    atomic_fetch_add(&global_state, 1);
    clock_gettime(CLOCK_MONOTONIC, &ts);
    send(data->client_sock, &ts, sizeof(ts), 0);
  }

  close(data->client_sock);
  close(sockfd);
  return NULL;
}

int main() {
  ShowCrashReports();

  pthread_t ipv4_thread, ipv6_thread;
  listener_data ipv4_data = {0},
                ipv6_data = {1};  // Use port 0 for IPv4, 1 for IPv6

  global_state = -5;

  if (pthread_create(&ipv4_thread, NULL, sender_thread, &ipv4_data) != 0) {
    perror("Failed to create IPv4 thread");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&ipv6_thread, NULL, sender_thread, &ipv6_data) != 0) {
    perror("Failed to create IPv6 thread");
    exit(EXIT_FAILURE);
  }

  // Wait for both listeners to be ready
  while (atomic_load(&global_state) < -3) {
    // Busy wait
  }

  int ipv4_sock = socket(AF_INET, SOCK_STREAM, 0);
  int ipv6_sock = socket(AF_INET6, SOCK_STREAM, 0);

  struct sockaddr_in ipv4_addr = {0};
  ipv4_addr.sin_family = AF_INET;
  ipv4_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  ipv4_addr.sin_port = htons(ipv4_data.port);

  struct sockaddr_in6 ipv6_addr = {0};
  ipv6_addr.sin6_family = AF_INET6;
  ipv6_addr.sin6_addr = in6addr_loopback;
  ipv6_addr.sin6_port = htons(ipv6_data.port);

  if (connect(ipv4_sock, (struct sockaddr *)&ipv4_addr, sizeof(ipv4_addr)) <
      0) {
    perror("IPv4 connect failed");
    exit(EXIT_FAILURE);
  }

  if (connect(ipv6_sock, (struct sockaddr *)&ipv6_addr, sizeof(ipv6_addr)) <
      0) {
    perror("IPv6 connect failed");
    exit(EXIT_FAILURE);
  }

  // Wait for both listeners to be ready
  while (atomic_load(&global_state) < -1) {
    // Busy wait
  }
  atomic_fetch_add(&global_state, 1);

  struct pollfd fds[2];
  fds[0].fd = ipv4_sock;
  fds[0].events = POLLIN;
  fds[1].fd = ipv6_sock;
  fds[1].events = POLLIN;

  struct timespec ts_sent, ts_now;
  double total_latency = 0.0;
  int total_measurements = 0;

  while (total_measurements < 2 * NUM_MEASUREMENTS) {
    int ready = poll(fds, 2, -1);
    if (ready < 0) {
      perror("Poll failed");
      exit(EXIT_FAILURE);
    }

    clock_gettime(CLOCK_MONOTONIC, &ts_now);

    for (int i = 0; i < 2; i++) {
      if (fds[i].revents & POLLIN) {
        ssize_t n = recv(fds[i].fd, &ts_sent, sizeof(ts_sent), 0);
        if (n == sizeof(ts_sent)) {
          total_latency += timespec_tonanos(timespec_sub(ts_now, ts_sent));
          total_measurements++;
          atomic_fetch_sub(&global_state, 1);
        }
      }
    }
  }

  double mean_latency = total_latency / total_measurements;
  printf("Mean poll() latency: %.2f ns\n", mean_latency);

  unassert(!close(ipv4_sock));
  unassert(!close(ipv6_sock));

  unassert(!pthread_join(ipv4_thread, NULL));
  unassert(!pthread_join(ipv6_thread, NULL));

  CheckForMemoryLeaks();
}
