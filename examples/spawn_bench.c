#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/weirdtypes.h"
#include "libc/mem/mem.h"
#include "libc/proc/posix_spawn.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

#define ITERATIONS 10

_Alignas(128) int a;
_Alignas(128) int b;
_Alignas(128) atomic_int lock;

static struct timespec SubtractTime(struct timespec a, struct timespec b) {
  a.tv_sec -= b.tv_sec;
  if (a.tv_nsec < b.tv_nsec) {
    a.tv_nsec += 1000000000;
    a.tv_sec--;
  }
  a.tv_nsec -= b.tv_nsec;
  return a;
}

static time_t ToNanoseconds(struct timespec ts) {
  return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

static char *Ithoa(char p[27], unsigned long x) {
  char m[26];
  unsigned i;
  i = 0;
  do {
    m[i++] = x % 10 + '0';
    x = x / 10;
  } while (x);
  for (;;) {
    *p++ = m[--i];
    if (!i) break;
    if (!(i % 3)) *p++ = ',';
  }
  *p = '\0';
  return p;
}

#define BENCH(name, x)                                             \
  {                                                                \
    int i;                                                         \
    char ibuf[27];                                                 \
    struct timespec t1, t2;                                        \
    clock_gettime(CLOCK_REALTIME, &t1);                            \
    for (i = 0; i < ITERATIONS; ++i) {                             \
      x;                                                           \
    }                                                              \
    clock_gettime(CLOCK_REALTIME, &t2);                            \
    Ithoa(ibuf, ToNanoseconds(SubtractTime(t2, t1)) / ITERATIONS); \
    printf("%-50s %16s ns\n", name, ibuf);                         \
  }

void PosixSpawnWait(const char *prog) {
  int ws, err, pid;
  char *args[] = {(char *)prog, 0};
  char *envs[] = {0};
  if ((err = posix_spawn(&pid, prog, NULL, NULL, args, envs))) {
    perror(prog);
    exit(1);
  }
  if (waitpid(pid, &ws, 0) == -1) {
    perror("waitpid");
    exit(1);
  }
  if (!(WIFEXITED(ws) && WEXITSTATUS(ws) == 42)) {
    puts("bad exit status");
    exit(1);
  }
}

void ForkExecWait(const char *prog) {
  int ws;
  if (!fork()) {
    execve(prog, (char *[]){(char *)prog, 0}, (char *[]){0});
    _Exit(127);
  }
  if (wait(&ws) == -1) {
    perror("wait");
    exit(1);
  }
  if (!(WIFEXITED(ws) && WEXITSTATUS(ws) == 42)) {
    puts("bad exit status");
    exit(1);
  }
}

char *FillMemory(char *p, long n) {
  return memset(p, -1, n);
}
char *(*pFillMemory)(char *, long) = FillMemory;

int main(int argc, char *argv[]) {
  long n;
  void *p;
  const char *prog;

  if (argc <= 1) {
    prog = "tiny64";
  } else {
    prog = argv[1];
  }

  BENCH("fork() + exec(tiny)", ForkExecWait(prog));
  BENCH("posix_spawn(tiny)", PosixSpawnWait(prog));

  n = 128L * 1024 * 1024;
  p = pFillMemory(
      mmap(0, n, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0), n);
  BENCH("mmap(128mb, MAP_SHARED) + fork() + exec(tiny)", ForkExecWait(prog));
  BENCH("mmap(128mb, MAP_SHARED) + posix_spawn(tiny)", PosixSpawnWait(prog));
  munmap(p, n);

  n = 128L * 1024 * 1024;
  p = pFillMemory(malloc(n), n);
  BENCH("malloc(128mb) + fork() + exec(tiny)", ForkExecWait(prog));
  BENCH("malloc(128mb) + posix_spawn(tiny)", PosixSpawnWait(prog));
  free(p);
}
