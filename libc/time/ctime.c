#include "libc/calls/weirdtypes.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"

/**
 * Represents time as string.
 * @threadunsafe
 */
char *ctime(const time_t *timep) {
  /*
  ** Section 4.12.3.2 of X3.159-1989 requires that
  **	The ctime function converts the calendar time pointed to by timer
  **	to local time in the form of a string. It is equivalent to
  **		asctime(localtime(timer))
  */
  struct tm *tmp = localtime(timep);
  return tmp ? asctime(tmp) : NULL;
}
