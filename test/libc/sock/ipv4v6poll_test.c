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
#include <unistd.h>

#define BUFFER_SIZE 1024

// States:
// 0: Initial state
// 1: IPv4 listener ready
// 2: IPv6 listener ready
// 3: Both listeners ready, main can connect
// 4: Main connected, IPv4 can send
// 5: IPv4 sent, IPv6 can send
// 6: All communication complete
atomic_int global_state = 0;

typedef struct {
  int port;
  int client_sock;
} listener_data;

void *ipv4_listener(void *arg) {
  listener_data *data = (listener_data *)arg;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("IPv4 socket creation failed");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;  // Random port

  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("IPv4 bind failed");
    exit(EXIT_FAILURE);
  }

  socklen_t len = sizeof(addr);
  if (getsockname(sockfd, (struct sockaddr *)&addr, &len) < 0) {
    perror("getsockname failed");
    exit(EXIT_FAILURE);
  }

  data->port = ntohs(addr.sin_port);
  // printf("IPv4 listening on port %d\n", data->port);

  if (listen(sockfd, 1) < 0) {
    perror("IPv4 listen failed");
    exit(EXIT_FAILURE);
  }

  // Signal that IPv4 listener is ready
  atomic_fetch_add(&global_state, 1);

  // Wait for IPv6 to be ready before accepting
  while (atomic_load(&global_state) < 3) {
    // Busy wait
  }

  data->client_sock = accept(sockfd, NULL, NULL);
  if (data->client_sock < 0) {
    perror("IPv4 accept failed");
    exit(EXIT_FAILURE);
  }

  while (atomic_load(&global_state) < 4) {
    // Wait for main to signal it's connected
  }

  const char *message = "Hello from IPv4!";
  unassert(send(data->client_sock, message, strlen(message), 0) > 0);

  unassert(!close(sockfd));
  return NULL;
}

void *ipv6_listener(void *arg) {
  listener_data *data = (listener_data *)arg;
  int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("IPv6 socket creation failed");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in6 addr = {0};
  addr.sin6_family = AF_INET6;
  addr.sin6_addr = in6addr_loopback;
  addr.sin6_port = 0;  // Random port

  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("IPv6 bind failed");
    exit(EXIT_FAILURE);
  }

  socklen_t len = sizeof(addr);
  if (getsockname(sockfd, (struct sockaddr *)&addr, &len) < 0) {
    perror("getsockname failed");
    exit(EXIT_FAILURE);
  }

  data->port = ntohs(addr.sin6_port);
  // printf("IPv6 listening on port %d\n", data->port);

  if (listen(sockfd, 1) < 0) {
    perror("IPv6 listen failed");
    exit(EXIT_FAILURE);
  }

  // Signal that IPv6 listener is ready and wait for IPv4
  int expected = 1;
  while (!atomic_compare_exchange_weak(&global_state, &expected, 3)) {
    expected = 1;  // Reset expected value if CAS failed
  }

  data->client_sock = accept(sockfd, NULL, NULL);
  if (data->client_sock < 0) {
    perror("IPv6 accept failed");
    exit(EXIT_FAILURE);
  }

  while (atomic_load(&global_state) < 5) {
    // Wait for IPv4 to send its message
  }

  const char *message = "Hello from IPv6!";
  unassert(send(data->client_sock, message, strlen(message), 0) > 0);

  unassert(!close(sockfd));
  return NULL;
}

int main() {
  ShowCrashReports();

  pthread_t ipv4_thread, ipv6_thread;
  listener_data ipv4_data = {0}, ipv6_data = {0};

  if (pthread_create(&ipv4_thread, NULL, ipv4_listener, &ipv4_data) != 0) {
    perror("Failed to create IPv4 thread");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&ipv6_thread, NULL, ipv6_listener, &ipv6_data) != 0) {
    perror("Failed to create IPv6 thread");
    exit(EXIT_FAILURE);
  }

  // Wait for both listeners to be ready
  while (atomic_load(&global_state) < 3) {
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

  // Signal that main thread is connected
  atomic_store(&global_state, 4);

  struct pollfd fds[2];
  fds[0].fd = ipv4_sock;
  fds[0].events = POLLIN;
  fds[1].fd = ipv6_sock;
  fds[1].events = POLLIN;

  char buffer[BUFFER_SIZE];

  while (atomic_load(&global_state) < 6) {
    if (poll(fds, 2, -1) > 0) {
      if (fds[0].revents & POLLIN) {
        ssize_t n = recv(ipv4_sock, buffer, BUFFER_SIZE - 1, 0);
        unassert(n != -1);
        buffer[n] = '\0';
        // printf("Received from IPv4: %s\n", buffer);
        unassert(atomic_load(&global_state) == 4);
        atomic_store(&global_state, 5);
      }
      if (fds[1].revents & POLLIN) {
        ssize_t n = recv(ipv6_sock, buffer, BUFFER_SIZE - 1, 0);
        unassert(n != -1);
        buffer[n] = '\0';
        // printf("Received from IPv6: %s\n", buffer);
        unassert(atomic_load(&global_state) == 5);
        atomic_store(&global_state, 6);
      }
    }
  }

  unassert(!close(ipv4_sock));
  unassert(!close(ipv6_sock));

  unassert(!pthread_join(ipv4_thread, NULL));
  unassert(!pthread_join(ipv6_thread, NULL));

  CheckForMemoryLeaks();
  return 0;
}
