#include <arpa/inet.h>
#include <cosmo.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {

  // create server socket
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

  // create client socket
  int client_fd;
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    return 6;
  if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)))
    return 7;

  // accept client and send data
  int server_client_fd;
  if ((server_client_fd = accept(server_fd, 0, 0)) == -1)
    return 8;
  if (write(server_client_fd, "hi", 2) != 2)
    return 9;

  // poll to be safe (important for mac/bsd)
  struct pollfd fds[] = {{client_fd, POLLIN}};
  if (poll(fds, 1, -1u) != 1)
    return 10;

  // ask how many bytes we can read
  uint32_t bytes_available;
  if (ioctl(client_fd, FIONREAD, &bytes_available))
    return 11;
  if (bytes_available != 2) {
    printf("wut %d\n", bytes_available);
    return 12;
  }

  // clean up
  if (close(client_fd))
    return 13;
  if (close(server_fd))
    return 14;

  CheckForMemoryLeaks();
}
