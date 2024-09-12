#include <arpa/inet.h>
#include <cosmo.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void on_signal(int sig) {
}

int main() {

  // Create server socket
  int server_fd;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  if ((server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1)
    return 1;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  address.sin_port = 0;  // let os assign random port
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)))
    return 2;
  if (getsockname(server_fd, (struct sockaddr *)&address,
                  (socklen_t *)&addrlen))
    return 3;
  if (listen(server_fd, SOMAXCONN))
    return 4;

  {
    // poll server
    struct pollfd fds[] = {{server_fd, POLLIN | POLLOUT}};
    int ret = poll(fds, 1, 0);
    if (ret != 0)
      return 5;
  }

  // verify server socket is non-blocking
  if (accept(server_fd, 0, 0) != -1)
    return 20;
  if (errno != EAGAIN && errno != EWOULDBLOCK)
    return 21;

  // create client socket
  int client_fd;
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    return 6;
  if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)))
    return 7;

  // prevent race condition
  // impacts platforms like openbsd
  fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL) & ~O_NONBLOCK);

  // accept client
  int server_client_fd;
  if ((server_client_fd = accept(server_fd, 0, 0)) == -1)
    return 8;

  // check that non-blocking wasn't inherited from listener
  char buf[1];
  sigaction(SIGALRM, &(struct sigaction){.sa_handler = on_signal}, 0);
  ualarm(100000, 0);
  if (read(server_client_fd, buf, 1) != -1)
    return 9;
  if (errno != EINTR)
    return 10;

  // Clean up
  if (close(server_client_fd))
    return 12;
  if (close(client_fd))
    return 13;
  if (close(server_fd))
    return 14;

  CheckForMemoryLeaks();
}
