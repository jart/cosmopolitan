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
#include "libc/str/str.h"
#include "libc/time/struct/tm.h"

/**
 * @fileoverview High performance ISO-8601 timestamp formatter.
 */

char *GetTimestamp(void) {
  int x;
  struct timespec ts;
  _Thread_local static long last;
  _Thread_local static char s[27];
  _Thread_local static struct tm tm;
  clock_gettime(0, &ts);
  if (ts.tv_sec != last) {
    localtime_r(&ts.tv_sec, &tm);
    x = tm.tm_year + 1900;
    s[0] = '0' + x / 1000;
    s[1] = '0' + x / 100 % 10;
    s[2] = '0' + x / 10 % 10;
    s[3] = '0' + x % 10;
    s[4] = '-';
    x = tm.tm_mon + 1;
    s[5] = '0' + x / 10;
    s[6] = '0' + x % 10;
    s[7] = '-';
    x = tm.tm_mday;
    s[8] = '0' + x / 10;
    s[9] = '0' + x % 10;
    s[10] = 'T';
    x = tm.tm_hour;
    s[11] = '0' + x / 10;
    s[12] = '0' + x % 10;
    s[13] = ':';
    x = tm.tm_min;
    s[14] = '0' + x / 10;
    s[15] = '0' + x % 10;
    s[16] = ':';
    x = tm.tm_sec;
    s[17] = '0' + x / 10;
    s[18] = '0' + x % 10;
    s[19] = '.';
    s[26] = 0;
    last = ts.tv_sec;
  }
  x = ts.tv_nsec;
  s[20] = '0' + x / 100000000;
  s[21] = '0' + x / 10000000 % 10;
  s[22] = '0' + x / 1000000 % 10;
  s[23] = '0' + x / 100000 % 10;
  s[24] = '0' + x / 10000 % 10;
  s[25] = '0' + x / 1000 % 10;
  return s;
}

int main(int argc, char *argv[]) {
  char buf[128], *p = buf;
  p = stpcpy(p, GetTimestamp());
  p = stpcpy(p, "\n");
  write(1, buf, p - buf);
}
