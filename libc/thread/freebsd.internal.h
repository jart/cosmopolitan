#ifndef COSMOPOLITAN_LIBC_THREAD_FREEBSD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_THREAD_FREEBSD_INTERNAL_H_
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/intrin/asmflag.h"

/**
 * @fileoverview FreeBSD Threading
 *
 * @note even though FreeBSD uses a 64-bit type for thread IDs the
 *     maximum legal range is PID_MAX + 2 (100001) through INT_MAX
 */

#define THR_SUSPENDED    1
#define THR_SYSTEM_SCOPE 2

#define UMTX_OP_WAIT              2
#define UMTX_OP_WAIT_UINT         11
#define UMTX_OP_WAIT_UINT_PRIVATE 15
#define UMTX_OP_WAKE              3
#define UMTX_OP_WAKE_PRIVATE      16
#define UMTX_ABSTIME              1

COSMOPOLITAN_C_START_

struct rtprio {
  uint16_t type; /* scheduling class */
  uint16_t prio;
};

struct thr_param {
  void (*start_func)(void *);
  void *arg;
  char *stack_base;
  uint64_t stack_size;
  char *tls_base;
  uint64_t tls_size;
  int64_t *child_tid;
  int64_t *parent_tid;
  int32_t flags;
  struct rtprio *rtp;
};

struct _umtx_time {
  struct timespec _timeout;
  uint32_t _flags;
  uint32_t _clockid;
};

int sys_umtx_timedwait_uint(_Atomic(int) *, int, bool, const struct timespec *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_THREAD_FREEBSD_INTERNAL_H_ */
