// Copyright 2025 Justine Alexandra Roberts Tunney
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

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// test that posix advisory locks can be interrupted by signals

int fd;
int gotsig;

void onsig(int sig) {
  gotsig = sig;
}

int main(int argc, char *argv[]) {

  struct sigaction sa;
  sa.sa_flags = 0;
  sa.sa_handler = onsig;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGUSR1, &sa, 0))
    exit(1);

  char path[] = "/tmp/fcntl_eintr_test.XXXXXX";
  if ((fd = mkstemp(path)) == -1)
    exit(2);

  if (fcntl(fd, F_SETLK,
            &(struct flock){
                .l_type = F_WRLCK,
                .l_start = 0,
                .l_len = 1,
            }))
    exit(3);

  int child = fork();
  if (child == -1)
    exit(10);
  if (!child) {
    if (!fcntl(fd, F_SETLKW,
               &(struct flock){
                   .l_type = F_WRLCK,
                   .l_start = 0,
                   .l_len = 1,
               }))
      exit(11);
    if (errno != EINTR)
      exit(12);
    if (gotsig != SIGUSR1)
      exit(13);
    _Exit(0);
  }

  sleep(1);
  if (kill(child, SIGUSR1))
    exit(4);

  int ws;
  if (wait(&ws) != child)
    exit(5);
  if (WIFEXITED(ws))
    if (WEXITSTATUS(ws))
      exit(WEXITSTATUS(ws));
  if (WIFSIGNALED(ws))
    raise(WTERMSIG(ws));

  if (close(fd))
    exit(7);
  if (unlink(path))
    exit(8);
}
