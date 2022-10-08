#ifndef NSYNC_TESTING_TIME_EXTRA_H_
#define NSYNC_TESTING_TIME_EXTRA_H_
#include "third_party/nsync/time.h"

/* Return a malloced nul-terminated string representing time t, using
   "decimals" decimal places.  */
char *nsync_time_str(nsync_time t, int decimals);

/* Return t as a double. */
double nsync_time_to_dbl(nsync_time t);

/* Return a time corresponding to double d. */
nsync_time nsync_time_from_dbl(double d);

#endif /*NSYNC_TESTING_TIME_EXTRA_H_*/
