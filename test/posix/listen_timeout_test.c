#include <arpa/inet.h>
#include <cosmo.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int listenfd, connfd;
  struct sockaddr_in serv_addr;
  struct timeval timeout;
  socklen_t len;
  struct sockaddr_in cli_addr;

  // only linux really does this
  if (!IsLinux())
    return 0;

  // create listening socket
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    perror("socket");
    exit(1);
  }

  // initialize server address
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  serv_addr.sin_port = htons(0);

  // bind socket
  if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("bind");
    close(listenfd);
    exit(2);
  }

  // listen on the socket
  if (listen(listenfd, 5) < 0) {
    perror("listen");
    close(listenfd);
    exit(3);
  }

  // accept for 200ms
  timeout.tv_sec = 0;
  timeout.tv_usec = 200e3;
  if (setsockopt(listenfd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                 sizeof(timeout))) {
    perror("setsockopt");
    close(listenfd);
    exit(4);
  }

  // Accept connection
  len = sizeof(cli_addr);
  connfd = accept(listenfd, (struct sockaddr *)&cli_addr, &len);
  if (connfd < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      /* printf("accept() timed out\n"); */
    } else {
      perror("accept");
    }
  } else {
    printf("Connection accepted from client.\n");
    // Close connected socket
    close(connfd);
  }

  // Close listening socket
  close(listenfd);
}
