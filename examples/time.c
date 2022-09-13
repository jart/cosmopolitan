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
#include "libc/calls/struct/timespec.h"
#include "libc/fmt/itoa.h"
#include "libc/log/log.h"
#include "libc/math.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/time/time.h"
#include "libc/x/xspawn.h"

/**
 * @fileoverview command for showing how long a command takes
 *
 * This offers the following improvements over the existing `time`
 * command that's incorporated into most shells:
 *
 * - This will show microseconds if seconds < 1
 * - This will launch APE binaries on systems with broken libc execv()
 */

#define WRITE(FD, STR) write(FD, STR, strlen(STR))

void OnChild(void *arg) {
  char **argv = arg;
  execv(argv[0], argv);
  _exit(127);
}

long double GetTimeval(struct timeval t) {
  return t.tv_sec + t.tv_usec * 1e-6l;
}

void PrintMetric(const char *name, long double d) {
  char buf[256], *p = buf;
  long mins, secs, mils, mics;
  mins = d / 60;
  secs = fmodl(d, 60);
  mils = fmodl(d * 1000, 1000);
  mics = fmodl(d * 1000000, 1000);
  p = stpcpy(p, name), *p++ = '\t';
  p = FormatInt64(p, mins), *p++ = 'm';
  p = FormatInt64(p, secs), *p++ = '.';
  *p++ = '0' + mils / 100;
  *p++ = '0' + mils / 10 % 10;
  *p++ = '0' + mils % 10;
  if (!secs) {
    *p++ = '0' + mics / 100;
    *p++ = '0' + mics / 10 % 10;
    *p++ = '0' + mics % 10;
  }
  *p++ = 's';
  *p++ = '\n';
  write(2, buf, p - buf);
}

int main(int argc, char *argv[]) {
  int ws;
  char *exepath;
  struct rusage r;
  long double real;
  char exebuf[PATH_MAX];
  if (argc >= 2) {
    if ((exepath = commandv(argv[1], exebuf, sizeof(exebuf)))) {
      real = nowl();
      argv[1] = exepath;
      if ((ws = xvspawn(OnChild, argv + 1, &r)) != -1) {
        PrintMetric("real", nowl() - real);
        PrintMetric("user", GetTimeval(r.ru_utime));
        PrintMetric("sys", GetTimeval(r.ru_stime));
        if (WIFEXITED(ws)) {
          return WEXITSTATUS(ws);
        } else {
          return 128 + WTERMSIG(ws);
        }
      } else {
        perror("xvspawn");
        return 127;
      }
    } else {
      perror(argv[1]);
      return 127;
    }
  } else {
    WRITE(2, "Usage: ");
    WRITE(2, argv[0]);
    WRITE(2, " PROG [ARGS...]\n");
    return EX_USAGE;
  }
}
