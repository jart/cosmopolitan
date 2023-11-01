/* pshm_ucase_bounce.c
   Adapted from SHM_OPEN(3) in Linux Programmer's Manual
   Licensed under GNU General Public License v2 or later */

#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/semaphore.h"

#define SHM_PATH    "/fc7261622dd420d8"
#define STRING_SEND "hello"
#define STRING_RECV "HELLO"

struct shmbuf {
  sem_t sem1;    /* POSIX unnamed semaphore */
  sem_t sem2;    /* POSIX unnamed semaphore */
  size_t cnt;    /* Number of bytes used in 'buf' */
  char buf[256]; /* Data being transferred */
};

atomic_bool *ready;

wontreturn void Bouncer(void) {

  /* Create shared memory object and set its size to the size
     of our structure. */
  int fd = shm_open(SHM_PATH, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("shm_open(bouncer)");
    exit(1);
  }
  if (ftruncate(fd, sizeof(struct shmbuf)) == -1) {
    perror("ftruncate");
    exit(1);
  }

  /* Map the object into the caller's address space. */
  struct shmbuf *shmp =
      mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shmp == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  /* Initialize semaphores as process-shared, with value 0. */
  if (sem_init(&shmp->sem1, 1, 0) == -1) {
    perror("sem_init-sem1");
    exit(1);
  }
  if (sem_init(&shmp->sem2, 1, 0) == -1) {
    perror("sem_init-sem2");
    exit(1);
  }

  /* Let other process know it's ok to open. */
  *ready = true;

  /* Wait for 'sem1' to be posted by peer before touching
     shared memory. */
  if (sem_wait(&shmp->sem1) == -1) {
    perror("sem_wait");
    exit(1);
  }

  /* Convert data in shared memory into upper case. */
  for (int j = 0; j < shmp->cnt; j++) {
    shmp->buf[j] = toupper((unsigned char)shmp->buf[j]);
  }

  /* Post 'sem2' to tell the peer that it can now
     access the modified data in shared memory. */
  if (sem_post(&shmp->sem2) == -1) {
    perror("sem_post");
    exit(1);
  }

  exit(0);
}

wontreturn void Sender(void) {

  /* Wait for file to exist. */
  while (!*ready) donothing;

  /* Open the existing shared memory object and map it
     into the caller's address space. */
  int fd = shm_open(SHM_PATH, O_RDWR, 0);
  if (fd == -1) {
    perror("shm_open(sender)");
    exit(1);
  }

  struct shmbuf *shmp =
      mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shmp == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  /* Copy data into the shared memory object. */
  shmp->cnt = strlen(STRING_SEND);
  strcpy(shmp->buf, STRING_SEND);

  /* Tell peer that it can now access shared memory. */
  if (sem_post(&shmp->sem1) == -1) {
    perror("sem_post");
    exit(1);
  }

  /* Wait until peer says that it has finished accessing
     the shared memory. */
  if (sem_wait(&shmp->sem2) == -1) {
    perror("sem_wait");
    exit(1);
  }

  /* Write modified data in shared memory to standard output. */
  if (strcmp(shmp->buf, STRING_RECV)) {
    tinyprint(2, program_invocation_name,
              ": received wrong string: ", shmp->buf, "\n", NULL);
    exit(1);
  }

  /* Unlink the shared memory object. Even if the peer process
     is still using the object, this is okay. The object will
     be removed only after all open references are closed. */
  if (shm_unlink(SHM_PATH)) {
    if (IsWindows() && errno == EACCES) {
      // TODO(jart): Make unlink() work better on Windows.
    } else {
      perror("shm_unlink");
      exit(1);
    }
  }

  exit(0);
}

int pid1;
int pid2;

void OnExit(void) {
  kill(pid1, SIGKILL);
  kill(pid2, SIGKILL);
  shm_unlink(SHM_PATH);
}

void OnTimeout(int sig) {
  tinyprint(2, program_invocation_name, ": test timed out!\n", NULL);
  exit(1);
}

int main(int argc, char *argv[]) {

  // create synchronization object
  ready = _mapshared(1);

  // create bouncer
  pid1 = fork();
  if (pid1 == -1) {
    perror("fork");
    exit(1);
  }
  if (!pid1) {
    Bouncer();
  }

  // create sender
  pid2 = fork();
  if (pid1 == -1) {
    perror("fork");
    kill(pid1, SIGKILL);
    exit(1);
  }
  if (!pid2) {
    Sender();
  }

  // set limit
  atexit(OnExit);
  signal(SIGALRM, OnTimeout);
  alarm(1);

  // wait for children
  for (;;) {
    int child, status;
    child = wait(&status);
    if (child == -1) {
      if (errno == ECHILD) {
        break;
      }
      perror("wait");
      exit(1);
    }
    if (status) {
      if (WIFEXITED(status)) {
        exit(WEXITSTATUS(status));
      } else {
        raise(WTERMSIG(status));
        exit(128 + WTERMSIG(status));
      }
    }
  }

  // report success
  exit(0);
}
