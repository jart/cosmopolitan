#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/calls/hefty/copyfile.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/calls/struct/stat.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/time/time.h"
#include "libc/x/x.h"

/**
 * @fileoverview Overcommit tutorial.
 * You can allocate memory like a central banker prints money.
 */

/* #define kHugeAmount (10LU * 1024LU * 1024LU * 1024LU * 1024LU) */
#define kHugeAmount (1LU * 1024LU * 1024LU * 1024LU)

int copyfile2(const char *frompath, const char *topath, bool dontoverwrite) {
  struct stat st;
  ssize_t transferred;
  int rc, fromfd, tofd;
  int64_t inoffset, outoffset;
  rc = -1;
  if ((fromfd = open(frompath, O_RDONLY | O_DIRECT, 0)) != -1) {
    if (fstat(fromfd, &st) != -1 &&
        (tofd =
             open(topath,
                  O_WRONLY | O_CREAT | O_DIRECT | (dontoverwrite ? O_EXCL : 0),
                  st.st_mode & 0777)) != -1) {
      inoffset = 0;
      outoffset = 0;
      while (st.st_size &&
             (transferred = copy_file_range(fromfd, &inoffset, tofd, &outoffset,
                                            st.st_size, 0)) != -1) {
        st.st_size -= transferred;
      }
      if (!st.st_size) rc = 0;
      rc |= close(tofd);
    }
    rc |= close(fromfd);
  }
  return rc;
}

int main(int argc, char *argv[]) {
  int fd, pid;
  size_t size;
  long double t1, t2;
  const char *core, *core2, *core3;
  volatile unsigned char *mem;
  size = ROUNDUP(kHugeAmount, PAGESIZE);
  core = gc(xasprintf("%s.%s", getauxval(AT_EXECFN), "core"));
  core2 = gc(xasprintf("%s.%s", getauxval(AT_EXECFN), "core2"));
  core3 = gc(xasprintf("%s.%s", getauxval(AT_EXECFN), "core3"));
  CHECK_NE(-1, (fd = open(core, O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0600)));
  CHECK_NE(-1, ftruncate(fd, size));
  CHECK_NE(MAP_FAILED,
           (mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)));
  strcpy(&mem[0], "hello\n\n\n\n\n\n\n\n\n\n");
  strcpy(&mem[kHugeAmount / 2], "hello\n\n\n\n\n\n\n\n\n\n");
  CHECK_NE(-1, munmap(mem, size));
  CHECK_NE(-1,
           (pid = spawnve(
                0, (int[3]){STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO},
                "o/examples/stat.com",
                (char *const[]){"o/examples/stat.com", core, NULL}, environ)));
  CHECK_NE(-1, waitpid(pid, NULL, 0));
  CHECK_NE(-1, close(fd));

  t1 = dtime(CLOCK_REALTIME);
  CHECK_NE(-1, copyfile(core, core2, 0));
  t2 = dtime(CLOCK_REALTIME);
  printf("%.6Lf\n", t2 - t1);

  t1 = dtime(CLOCK_REALTIME);
  CHECK_NE(-1, copyfile2(core, core3, false));
  t2 = dtime(CLOCK_REALTIME);
  printf("%.6Lf\n", t2 - t1);

  /* CHECK_NE(-1, unlink(core)); */
  return 0;
}
