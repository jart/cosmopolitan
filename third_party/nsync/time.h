#ifndef NSYNC_TIME_H_
#define NSYNC_TIME_H_
#include "libc/calls/struct/timespec.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define NSYNC_TIME_SEC(t)  ((t).tv_sec)
#define NSYNC_TIME_NSEC(t) ((t).tv_nsec)
#define NSYNC_TIME_STATIC_INIT(t, ns) \
  { (t), (ns) }

/* The type nsync_time represents the interval elapsed between two
   moments in time. Often the first such moment is an address-space-wide
   epoch, such as the Unix epoch, but clients should not rely on the
   epoch in one address space being the same as that in another.
   Intervals relative to the epoch are known as absolute times. */
typedef struct timespec nsync_time;

/* A deadline infinitely far in the future. */
extern const nsync_time nsync_time_no_deadline;

/* The zero delay, or an expired deadline. */
extern const nsync_time nsync_time_zero;

/* Return the current time since the epoch.  */
#define nsync_time_now() _timespec_real()

/* Sleep for the specified delay. Returns the unslept time which may be
   non-zero if the call was interrupted. */
nsync_time nsync_time_sleep(nsync_time delay);

/* Return a+b */
#define nsync_time_add(a, b) _timespec_add(a, b)

/* Return a-b */
#define nsync_time_sub(a, b) _timespec_sub(a, b)

/* Return +ve, 0, or -ve according to whether a>b, a==b, or a<b. */
#define nsync_time_cmp(a, b) _timespec_cmp(a, b)

/* Return the specified number of milliseconds as a time. */
#define nsync_time_ms(a) _timespec_frommillis(a)

/* Return the specified number of microseconds as a time. */
#define nsync_time_us(a) _timespec_frommicros(a)

/* Return an nsync_time constructed from second and nanosecond
   components */
#define nsync_time_s_ns(s, ns) ((nsync_time){(int64_t)(s), (unsigned)(ns)})

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* NSYNC_TIME_H_ */
