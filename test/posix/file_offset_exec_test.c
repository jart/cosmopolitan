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

#include <cosmo.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

// test that lseek() is shared across execve()

__static_yoink("zipos");

void on_unexpected_death(int sig) {
  int ws;
  if (wait(&ws) == -1)
    _Exit(33);
  if (!WIFEXITED(ws))
    _Exit(34);
  if (!(WEXITSTATUS(ws) & 255))
    _Exit(35);
  _Exit(WEXITSTATUS(ws));
}

int main() {

  signal(SIGCHLD, on_unexpected_death);

  // extract test program
  int exefd;
  int zipfd;
  ssize_t got;
  char exepath[] = "/tmp/file_offset_exec_prog.XXXXXX";
  if ((exefd = mkstemp(exepath)) == -1)
    return 2;
  if (fchmod(exefd, 0755))
    return 3;
  if ((zipfd = open("/zip/file_offset_exec_prog", O_RDONLY)) == -1)
    return 4;
  for (;;) {
    char chunk[512];
    if ((got = read(zipfd, chunk, sizeof(chunk))) == -1)
      return 5;
    if (!got)
      break;
    if (write(exefd, chunk, got) != got)
      return 6;
  }
  if (close(zipfd))
    return 7;
  if (close(exefd))
    return 8;

  // create file shared memory mapping for synchronization
  int mapfd;
  atomic_int *phase;
  char mappath[] = "/tmp/file_offset_exec_phase.XXXXXX";
  if ((mapfd = mkstemp(mappath)) == -1)
    return 9;
  if (ftruncate(mapfd, sizeof(atomic_int)))
    return 10;
  if ((phase = mmap(0, sizeof(atomic_int), PROT_READ | PROT_WRITE, MAP_SHARED,
                    mapfd, 0)) == MAP_FAILED)
    return 11;

  // create test file to which both processes shall be writing
  int fd;
  char path[] = "/tmp/file_offset_exec_file.XXXXXX";
  if ((fd = mkstemp(path)) == -1)
    return 12;
  if (lseek(fd, 0, SEEK_CUR) != 0)
    return 13;

  // start writing to file
  if (write(fd, "0", 1) != 1)
    return 14;
  if (lseek(fd, 0, SEEK_CUR) != 1)
    return 15;

  // spawn program
  int pid;
  if ((pid = fork()) == -1)
    return 16;
  if (!pid) {
    char str[2][12];
    char *envs[] = {0};
    char *args[] = {exepath, str[0], str[1], 0};
    sprintf(str[0], "%d", fd);
    sprintf(str[1], "%d", mapfd);
    execve(exepath, args, envs);
    _Exit(17);
  }

  for (;;)
    if (*phase == 1)
      break;

  if (write(fd, "2", 1) != 1)
    return 18;
  if (lseek(fd, 0, SEEK_CUR) != 3)
    return 19;

  *phase = 2;
  for (;;)
    if (*phase == 3)
      break;

  if (write(fd, "4", 1) != 1)
    return 20;
  if (lseek(fd, 0, SEEK_CUR) != 5)
    return 21;

  signal(SIGCHLD, SIG_DFL);
  *phase = 4;

  int ws;
  if (wait(&ws) == -1)
    return 22;
  if (!WIFEXITED(ws))
    return 23;
  if (WEXITSTATUS(ws))
    return WEXITSTATUS(ws);

  char buf[16] = {0};
  if (pread(fd, buf, 15, 0) != 5)
    return 24;
  if (lseek(fd, 0, SEEK_CUR) != 5)
    return 25;

  if (close(fd))
    return 26;

  if (unlink(path))
    return 27;

  if (unlink(exepath))
    return 28;

  if (munmap(phase, sizeof(atomic_int)))
    return 29;

  if (close(mapfd))
    return 30;

  if (unlink(mappath))
    return 31;

  if (strcmp(buf, "01234"))
    return 32;
}
