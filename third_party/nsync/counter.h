#ifndef NSYNC_COUNTER_H_
#define NSYNC_COUNTER_H_
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/time.h"
COSMOPOLITAN_C_START_

/* An nsync_counter represents an unsigned integer that can count up and down,
   and wake waiters when zero.  */
typedef struct nsync_counter_s_ *nsync_counter;

/* Return a freshly allocated nsync_counter with the specified value,
   of NULL if an nsync_counter cannot be created.

   Any non-NULL returned value should be passed to nsync_counter_free() when no
   longer needed.  */
nsync_counter nsync_counter_new(uint32_t value);

/* Free resources associated with c.  Requires that c was allocated by
   nsync_counter_new(), and no concurrent or future operations are applied to
   c.  */
void nsync_counter_free(nsync_counter c);

/* Add delta to c, and return its new value.  It is a checkable runtime error
   to decrement c below 0, or to increment c (i.e., apply a delta > 0) after a
   waiter has waited.  */
uint32_t nsync_counter_add(nsync_counter c, int32_t delta);

/* Return the current value of c.  */
uint32_t nsync_counter_value(nsync_counter c);

/* Wait until c has value 0, or until abs_deadline, then return
   the value of c.  It is a checkable runtime error to increment c after
   a waiter may have been woken due to the counter reaching zero.
   If abs_deadline==nsync_time_no_deadline, the deadline
   is far in the future. */
uint32_t nsync_counter_wait(nsync_counter c, nsync_time abs_deadline);

COSMOPOLITAN_C_END_
#endif /* NSYNC_COUNTER_H_ */
