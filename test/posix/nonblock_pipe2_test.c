#include <cosmo.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int pipefd[2];
  char buf[PIPE_BUF];
  char buf2[PIPE_BUF];
  ssize_t bytes_read;
  ssize_t bytes_written;

  // Create a pipe
  if (pipe2(pipefd, O_NONBLOCK) == -1)
    exit(1);

  // Test 1: Reading from an empty pipe should fail with EAGAIN
  bytes_read = read(pipefd[0], buf, PIPE_BUF);
  if (bytes_read != -1 || errno != EAGAIN)
    exit(4);

  // Test 2: Writing to the pipe
  bytes_written = write(pipefd[1], buf, PIPE_BUF);
  if (bytes_written != PIPE_BUF)
    exit(5);

  // Test 3: Reading from the pipe after writing
  bytes_read = read(pipefd[0], buf2, PIPE_BUF);
  if (bytes_read != PIPE_BUF || memcmp(buf, buf2, PIPE_BUF))
    exit(6);

  // Test 4: Fill the pipe buffer
  int ch = 10;
  size_t total_written = 0;
  for (;;) {
    memset(buf, ch, PIPE_BUF);
    bytes_written = write(pipefd[1], buf, PIPE_BUF);
    if (bytes_written == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        break;  // Pipe is full
      } else {
        exit(7);  // Unexpected error
      }
    }
    total_written += bytes_written;
  }

  // Test 5: Verify that we can read all the data we wrote
  ch = 10;
  size_t total_read = 0;
  while (total_read < total_written) {
    bytes_read = read(pipefd[0], buf2, PIPE_BUF);
    if (bytes_read == -1)
      exit(8);
    memset(buf, ch, PIPE_BUF);
    if (memcmp(buf, buf2, PIPE_BUF))
      exit(9);
    total_read += bytes_read;
  }
  if (total_read != total_written)
    exit(10);

  // Clean up
  if (close(pipefd[0]))
    exit(11);
  if (close(pipefd[1]))
    exit(12);

  CheckForMemoryLeaks();
}
