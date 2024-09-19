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
 * @fileoverview recv(MSG_WAITALL) test
 */

struct sockaddr_in serv_addr;
atomic_bool g_ready_for_conn;
atomic_bool g_ready_for_data;
atomic_bool g_ready_for_more;
atomic_bool g_ready_for_exit;

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

  // check waitall + dontwait
  char buf[2];
  int rc = recv(client, buf, 2, MSG_WAITALL | MSG_DONTWAIT);
  if (rc != -1)
    exit(15);
  if (errno != EAGAIN)
    exit(16);

  // wake main thread
  g_ready_for_data = true;

  // check peek
  rc = recv(client, buf, 2, MSG_PEEK);
  if (rc == -1) {
    perror("recv1");
    exit(17);
  }
  if (rc != 1)
    exit(18);
  if (buf[0] != 'x')
    exit(19);

  // check read() has @restartable behavior
  rc = recv(client, buf, 2, MSG_WAITALL);
  if (rc == -1) {
    perror("recv2");
    exit(21);
  }
  if (rc != 2)
    exit(22);
  if (buf[0] != 'x')
    exit(23);
  if (buf[1] != 'y')
    exit(24);

  // wake main thread
  g_ready_for_more = true;

  // check normal recv won't wait
  rc = read(client, buf, 2);
  if (rc == -1) {
    perror("recv3");
    exit(25);
  }
  if (rc != 1)
    exit(26);
  if (buf[0] != 'x')
    exit(27);

  // wait for main thread
  for (;;)
    if (g_ready_for_exit)
      break;

  // close listening socket
  if (close(server))
    exit(28);
  if (close(client))
    exit(29);
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

  // send first transmission
  usleep(100e3);
  if (write(client, "x", 1) != 1)
    return 4;
  usleep(100e3);
  if (write(client, "y", 1) != 1)
    return 5;

  // wait for thread
  for (;;)
    if (g_ready_for_more)
      break;

  // send second transmission
  usleep(100e3);
  if (write(client, "x", 1) != 1)
    return 4;
  usleep(100e3);
  if (write(client, "y", 1) != 1)
    return 5;

  g_ready_for_exit = true;

  if (pthread_join(th, 0))
    return 6;
}
