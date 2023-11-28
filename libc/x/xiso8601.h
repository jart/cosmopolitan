#ifndef COSMOPOLITAN_LIBC_X_XISO8601_H_
#define COSMOPOLITAN_LIBC_X_XISO8601_H_
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
COSMOPOLITAN_C_START_

char *xiso8601i(int) mallocesque;
char *xiso8601tv(struct timeval *) mallocesque;
char *xiso8601ts(struct timespec *) mallocesque;

#if __STDC_VERSION__ + 0 >= 201112
#define xiso8601(TS) \
  _Generic(*(TS), struct timeval : xiso8601tv, default : xiso8601ts)(TS)
#endif /* C11 */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_X_XISO8601_H_ */
