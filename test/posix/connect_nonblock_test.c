#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  char buffer[1024];
  fd_set wset;
  int listenfd, connfd, sockfd;
  int s, error;
  pid_t pid;
  socklen_t len;
  struct sockaddr_in serv_addr, cli_addr;
  uint16_t port;

  printf("\n");

  /* Create listening socket */
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    perror("socket() failed");
    exit(1);
  }

  /* Initialize server address */
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  serv_addr.sin_port = htons(0);

  /* Bind socket */
  if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("bind");
    exit(2);
  }

  /* Get the assigned port number */
  len = sizeof(serv_addr);
  if (getsockname(listenfd, (struct sockaddr *)&serv_addr, &len) < 0) {
    perror("getsockname");
    exit(3);
  }
  port = ntohs(serv_addr.sin_port);

  /* Listen on the socket */
  if (listen(listenfd, 1) < 0) {
    perror("listen");
    exit(4);
  }

  /* Fork a child process */
  pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(5);
  } else if (pid == 0) {
    /* Child process: acts as the client */
    close(listenfd); /* Close the listening socket in the child */

    /* Create socket */
    sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sockfd < 0) {
      perror("socket");
      exit(6);
    }

    /* Initialize server address */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* 127.0.0.1 */
    serv_addr.sin_port = htons(port);                   /* Assigned port */

    /* Try calling read() before connection is established */
    s = read(sockfd, buffer, sizeof(buffer));
    if (s < 0) {
      if (errno == ENOTCONN) {
        printf("read #1 enotconn\n");
        /* good */
      } else {
        perror("read #1");
        exit(6);
      }
    } else {
      printf("read #1 succeeded\n");
      exit(6);
    }

#if 0
    /* Try calling read() before connection is established */
    s = write(sockfd, buffer, sizeof(buffer));
    if (s < 0) {
      if (errno == ENOTCONN) {
        /* good */
      } else {
        perror("write");
      }
    } else {
      printf("Wrote %d bytes: %.*s\n", s, s, buffer);
    }
#endif

    /* Attempt to connect */
    s = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (s == 0) {
      printf("connect #1 success\n");
    } else if (s < 0 && errno == EINPROGRESS) {
      printf("connect #1 einprogress\n");
    } else {
      perror("connect #1");
      exit(10);
    }

    /* Try calling read() before connection is established */
    s = read(sockfd, buffer, sizeof(buffer));
    if (s < 0) {
      if (errno == EAGAIN) {
        printf("read #2 eagain\n");
      } else {
        perror("read #2");
        exit(10);
      }
    } else {
      printf("read #2 succeeded\n");
      exit(10);
    }

    /* Try calling connect() again to trigger EALREADY */
    s = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (!s) {
      printf("connect #2 succeeded\n");
    } else if (s < 0 && errno == EALREADY) {
      printf("connect #2 ealready\n");
    } else if (s < 0 && errno == EISCONN) {
      printf("connect #2 eisconn\n");
    } else if (s < 0) {
      perror("connect #2");
      exit(11);
    }

    /* Try calling connect() again to trigger EALREADY */
    s = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (!s) {
      printf("connect #3 succeeded\n");
    } else if (errno == EALREADY) {
      printf("connect #3 ealready\n");
    } else if (errno == EISCONN) {
      printf("connect #3 eisconn\n");
    } else {
      perror("connect");
      exit(11);
    }

    /* Try calling read() before connection is established */
    s = read(sockfd, buffer, sizeof(buffer));
    if (s < 0) {
      if (errno == EAGAIN) {
        /* good */
      } else {
        perror("read");
      }
    } else {
      printf("Read %d bytes: %.*s\n", s, s, buffer);
    }

    /* Use select() to wait for the socket to be writable */
    FD_ZERO(&wset);
    FD_SET(sockfd, &wset);

    s = select(sockfd + 1, NULL, &wset, NULL, 0);
    if (s == 0) {
      printf("not possible\n");
      exit(11);
    } else if (s < 0) {
      perror("select");
      exit(12);
    }

    /* Check if socket is writable */
    if (FD_ISSET(sockfd, &wset)) {
      /* Check for error */
      len = sizeof(error);
      if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
        exit(13);
      if (error) {
        printf("connection failed after select(): %s\n", strerror(error));
        exit(14);
      }
    } else {
      exit(16);
    }

    /* Try calling connect() again to trigger EISCONN */
    s = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (!s) {
      printf("connect #4 succeeded\n");
    } else if (s < 0 && errno == EISCONN) {
      printf("connect #4 eisconn\n");
    } else if (s < 0) {
      exit(17);
    }

    if (close(sockfd))
      exit(15);
    exit(0);
  } else {
    /* Accept connection */
    len = sizeof(cli_addr);
    connfd = accept(listenfd, (struct sockaddr *)&cli_addr, &len);
    if (connfd < 0) {
      close(listenfd);
      wait(NULL);
      exit(18);
    }

    /* Read data from client */
    s = read(connfd, buffer, sizeof(buffer));
    if (s < 0) {
      exit(51);
    } else if (!s) {
      /* got close */
    } else {
      exit(50);
    }

    /* Close connected socket */
    if (close(connfd)) {
      close(listenfd);
      wait(NULL);
      exit(19);
    }

    /* Close listening socket */
    if (close(listenfd)) {
      wait(NULL);
      exit(20);
    }

    /* Wait for child process to finish */
    int status;
    if (waitpid(pid, &status, 0) < 0)
      exit(21);

    printf("\n");
    if (WIFEXITED(status)) {
      exit(WEXITSTATUS(status)); /* Return child's exit status */
    } else {
      exit(22);
    }
  }

  exit(23);
}
