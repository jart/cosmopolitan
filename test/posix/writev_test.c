#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int main() {
  int sockfd, newsockfd, clientfd;
  struct sockaddr_in serv_addr, cli_addr;
  socklen_t clilen;
  struct iovec iov[2];
  char buffer[256];
  ssize_t n;
  const char *str1 = "";
  const char *str2 = "Hello from server!\n";
  const char *str3 = "Hello from client!\n";

  // Create server socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    return 18;
  }

  // Bind server socket to localhost:PORT
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // localhost
  serv_addr.sin_port = 0;
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("bind");
    close(sockfd);
    return 19;
  }
  uint32_t addrsize = sizeof(serv_addr);
  if (getsockname(sockfd, (struct sockaddr *)&serv_addr, &addrsize)) {
    perror("getsockname");
    return 20;
  }

  // Listen for incoming connections
  if (listen(sockfd, 1) < 0) {
    perror("listen");
    close(sockfd);
    return 21;
  }

  // Create client socket
  clientfd = socket(AF_INET, SOCK_STREAM, 0);
  if (clientfd < 0) {
    perror("socket");
    close(sockfd);
    return 22;
  }

  // Connect client socket to server
  if (connect(clientfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("connect");
    close(sockfd);
    close(clientfd);
    return 23;
  }

  // Accept connection on server side
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
  if (newsockfd < 0) {
    perror("accept");
    close(sockfd);
    close(clientfd);
    return 34;
  }

  // Server writes to client using writev with zero-length iovec
  iov[0].iov_base = (void *)str1;
  iov[0].iov_len = 0;  // Zero-length iovec
  iov[1].iov_base = (void *)str2;
  iov[1].iov_len = strlen(str2);

  n = writev(newsockfd, iov, 2);
  if (n != 19)
    return 13;

  // Client reads data from server
  memset(buffer, 0, sizeof(buffer));
  n = read(clientfd, buffer, sizeof(buffer) - 1);
  if (n < 0) {
    perror("read");
    return 14;
  } else {
    if (n != 19)
      return 8;
  }

  // Client writes to server using writev with zero-length iovec
  iov[0].iov_base = (void *)str1;
  iov[0].iov_len = 0;  // Zero-length iovec
  iov[1].iov_base = (void *)str3;
  iov[1].iov_len = strlen(str3);

  n = writev(clientfd, iov, 2);
  if (n != 19)
    return 9;

  // Server reads data from client
  memset(buffer, 0, sizeof(buffer));
  n = read(newsockfd, buffer, sizeof(buffer) - 1);
  if (n < 0) {
    perror("ERROR reading from client");
    return 10;
  } else if (n != 19) {
    return 11;
  }

  // Close all sockets
  close(newsockfd);
  close(clientfd);
  close(sockfd);
}
