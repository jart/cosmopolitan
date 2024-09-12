#include <arpa/inet.h>
#include <cosmo.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {

  // Create server socket
  int server_fd;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
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
    struct pollfd fds[2] = {
        {server_fd, POLLIN | POLLOUT},
    };
    int ret = poll(fds, 1, 0);
    if (ret != 0)
      return 5;
  }

  // create client socket
  int client_fd;
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    return 6;
  if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)))
    return 7;

  {
    // poll server
    struct pollfd fds[] = {{server_fd, POLLIN | POLLOUT}};
    int ret = poll(fds, 1, -1u);
    if (ret != 1)
      return 8;
    if (!(fds[0].revents & POLLIN))
      return 9;
    if (fds[0].revents & POLLOUT)
      return 10;
    if (fds[0].revents & POLLHUP)
      return 11;
    if (fds[0].revents & POLLERR)
      return 12;
  }

  {
    // poll server with invalid thing
    struct pollfd fds[] = {
        {server_fd, POLLIN | POLLOUT},
        {666, POLLIN | POLLOUT},
    };
    int ret = poll(fds, 2, -1u);
    if (ret != 2)
      return 18;
    if (!(fds[0].revents & POLLIN))
      return 19;
    if (fds[0].revents & POLLOUT)
      return 20;
    if (fds[1].revents & POLLIN)
      return 21;
    if (fds[1].revents & POLLOUT)
      return 22;
    if (!(fds[1].revents & POLLNVAL))
      return 23;
  }

  // Clean up
  if (close(client_fd))
    return 13;
  if (close(server_fd))
    return 14;

  CheckForMemoryLeaks();
}
