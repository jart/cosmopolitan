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
 * @fileoverview SO_RCVTIMEO + SA_RESTART interaction test
 *
 * This code tests that setting a read timeout on a socket will cause
 * read() to change its signal handling behavior from @restartable to
 * @norestart. This is currently the case on GNU/Systemd and Windows.
 */

struct sockaddr_in serv_addr;
atomic_bool g_ready_for_conn;
atomic_bool g_ready_for_data;
atomic_bool g_ready_for_more;
atomic_bool g_ready_for_exit;
atomic_bool got_sigusr1;
atomic_bool got_sigusr2;

void on_sigusr1(int sig) {
  got_sigusr1 = true;
}

void on_sigusr2(int sig) {
  got_sigusr2 = true;
}

void *server_thread(void *arg) {
  int server, client;
  struct timeval timeout;
  socklen_t len;
  struct sockaddr_in cli_addr;

  // create listening socket
  server = socket(AF_INET, SOCK_STREAM, 0);
  if (server == -1) {
    perror("socket");
    exit(31);
  }

  // initialize server address
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  serv_addr.sin_port = htons(0);

  // bind socket
  if (bind(server, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) {
    perror("bind");
    exit(32);
  }

  // get assigned port
  len = sizeof(serv_addr);
  if (getsockname(server, (struct sockaddr *)&serv_addr, &len)) {
    perror("getsockname");
    exit(30);
  }

  // listen on the socket
  if (listen(server, SOMAXCONN)) {
    perror("listen");
    exit(33);
  }

  // wake main thread
  g_ready_for_conn = true;

  // accept connection
  len = sizeof(cli_addr);
  client = accept(server, (struct sockaddr *)&cli_addr, &len);
  if (client == -1) {
    perror("accept");
    exit(35);
  }

  // wake main thread
  g_ready_for_data = true;

  // check read() has @restartable behavior
  char buf[1];
  int rc = read(client, buf, 1);
  if (rc != -1)
    exit(35);
  if (errno != EINTR)
    exit(36);
  if (!got_sigusr1)
    exit(37);
  if (!got_sigusr2)
    exit(38);
  got_sigusr1 = false;
  got_sigusr2 = false;

  // install a socket receive timeout
  timeout.tv_sec = 5000000;
  timeout.tv_usec = 0;
  if (setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                 sizeof(timeout) + (!IsNetbsd() && !IsQemuUser()))) {
    perror("setsockopt");
    exit(34);
  }

  // wake main thread
  g_ready_for_more = true;

  // check read() has @norestart behavior
  rc = read(client, buf, 1);
  if (rc != -1)
    exit(35);
  if (errno != EINTR)
    exit(36);
  if (!got_sigusr1)
    exit(37);

  // here's the whammy
  if (IsLinux() || IsWindows()) {
    if (got_sigusr2)
      exit(38);
  } else {
    if (!got_sigusr2)
      exit(38);
  }

  // wait for main thread
  for (;;)
    if (g_ready_for_exit)
      break;

  // close listening socket
  if (close(server))
    exit(40);
  if (close(client))
    exit(39);
  return 0;
}

int main() {

  // handle signals
  struct sigaction sa = {0};
  sa.sa_handler = on_sigusr1;
  sa.sa_flags = SA_RESTART;
  sigaction(SIGUSR1, &sa, 0);
  sa.sa_handler = on_sigusr2;
  sa.sa_flags = 0;
  sigaction(SIGUSR2, &sa, 0);

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

  usleep(100e3);
  if (pthread_kill(th, SIGUSR1))
    return 4;

  usleep(100e3);
  if (pthread_kill(th, SIGUSR2))
    return 5;

  // wait for thread
  for (;;)
    if (g_ready_for_more)
      break;

  usleep(100e3);
  if (pthread_kill(th, SIGUSR1))
    return 4;

  usleep(400e3);
  if (pthread_kill(th, SIGUSR2))
    return 5;

  g_ready_for_exit = true;

  if (pthread_join(th, 0))
    return 20;
}
