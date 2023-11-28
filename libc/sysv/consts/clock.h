#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_CLOCK_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_CLOCK_H_
COSMOPOLITAN_C_START_

extern const int CLOCK_BOOTTIME;
extern const int CLOCK_BOOTTIME_ALARM;
extern const int CLOCK_MONOTONIC;
extern const int CLOCK_MONOTONIC_COARSE;
extern const int CLOCK_MONOTONIC_FAST;
extern const int CLOCK_MONOTONIC_PRECISE;
extern const int CLOCK_MONOTONIC_RAW;
extern const int CLOCK_PROCESS_CPUTIME_ID;
extern const int CLOCK_PROF;
extern const int CLOCK_REALTIME_ALARM;
extern const int CLOCK_REALTIME_COARSE;
extern const int CLOCK_REALTIME_FAST;
extern const int CLOCK_REALTIME_PRECISE;
extern const int CLOCK_SECOND;
extern const int CLOCK_TAI;
extern const int CLOCK_THREAD_CPUTIME_ID;
extern const int CLOCK_UPTIME;
extern const int CLOCK_UPTIME_FAST;
extern const int CLOCK_UPTIME_PRECISE;

COSMOPOLITAN_C_END_

#define CLOCK_REALTIME  0
#define CLOCK_MONOTONIC CLOCK_MONOTONIC

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_CLOCK_H_ */
