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

#include <arpa/inet.h>
#include <cosmo.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @fileoverview send(MSG_NOSIGNAL) test
 *
 * It's possible when writing to a socket for SIGPIPE to be raised. It
 * can happen for a variety of reasons. The one reason that has broad
 * consensus across OSes and is officially documented, is if shutdown()
 * is used on the local end.
 */

struct sockaddr_in serv_addr;
atomic_bool g_ready_for_conn;
atomic_bool g_ready_for_data;
atomic_bool g_ready_for_more;
atomic_bool g_ready_for_exit;
volatile sig_atomic_t g_got_signal;

void onsig(int sig) {
  g_got_signal = sig;
}

void *server_thread(void *arg) {
  socklen_t len;
  int server, client;
  struct sockaddr_in cli_addr;

  // create listening socket
  server = socket(AF_INET, SOCK_STREAM, 0);
  if (server == -1) {
    perror("socket");
    exit(10);
  }

  // initialize server address
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  serv_addr.sin_port = htons(0);

  // bind socket
  if (bind(server, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) {
    perror("bind");
    exit(11);
  }

  // get assigned port
  len = sizeof(serv_addr);
  if (getsockname(server, (struct sockaddr *)&serv_addr, &len)) {
    perror("getsockname");
    exit(12);
  }

  // listen on the socket
  if (listen(server, SOMAXCONN)) {
    perror("listen");
    exit(13);
  }

  // wake main thread
  g_ready_for_conn = true;

  // accept connection
  len = sizeof(cli_addr);
  client = accept(server, (struct sockaddr *)&cli_addr, &len);
  if (client == -1) {
    perror("accept");
    exit(14);
  }

  // wake main thread
  g_ready_for_data = true;

  // wait for thread
  for (;;)
    if (g_ready_for_exit)
      break;

  // close sockets
  if (close(client))
    exit(29);
  if (close(server))
    exit(28);

  return 0;
}

int main() {

  // create server thread
  pthread_t th;
  if (pthread_create(&th, 0, server_thread, 0))
    return 1;

  // wait for thread
  for (;;)
    if (g_ready_for_conn)
      break;

  // create socket
  int client = socket(AF_INET, SOCK_STREAM, 0);
  if (client == -1) {
    perror("socket");
    return 2;
  }

  // connect to server
  if (connect(client, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    perror("connect");
    return 3;
  }

  // wait for thread
  for (;;)
    if (g_ready_for_data)
      break;

  // handle signals
  struct sigaction sa;
  sa.sa_flags = 0;
  sa.sa_handler = onsig;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGPIPE, &sa, 0);

  // half close socket
  if (shutdown(client, SHUT_WR))
    return 15;

  // send first transmission
  int rc;
  for (;;) {
    rc = write(client, "x", 1);
    if (rc == 1)
      continue;
    if (rc != -1)
      return 4;
    if (errno != EPIPE) {
      perror("write");
      return 5;
    }
    // NetBSD is oddly lazy about sending SIGPIPE
    if (IsNetbsd())
      for (;;)
        if (g_got_signal)
          break;
    if (g_got_signal != SIGPIPE) {
      fprintf(stderr, "expected SIGPIPE but got %s\n", strsignal(g_got_signal));
      return 6;
    }
    g_got_signal = 0;
    break;
  }

  // send first transmission
  rc = send(client, "x", 1, MSG_NOSIGNAL);
  if (rc != -1)
    return 7;
  if (errno != EPIPE)
    return 8;
  if (g_got_signal)
    return 9;

  g_ready_for_exit = true;

  if (pthread_join(th, 0))
    return 6;
}
