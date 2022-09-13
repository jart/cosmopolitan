#include "libc/calls/weirdtypes.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"

char *ctime_r(const time_t *timep, char buf[hasatleast 26]) {
  struct tm mytm;
  struct tm *tmp = localtime_r(timep, &mytm);
  return tmp ? asctime_r(tmp, buf) : NULL;
}
