#ifndef COSMOPOLITAN_LIBC_THREAD_FREEBSD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_THREAD_FREEBSD_INTERNAL_H_
#include "libc/bits/asmflag.h"
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview FreeBSD Threading
 *
 * @note even though FreeBSD uses a 64-bit type for thread IDs the
 *     maximum legal range is PID_MAX + 2 (100001) through INT_MAX
 */

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

static inline wontreturn void thr_exit(int64_t *opt_out_state) {
  long ax, di;
  asm volatile("syscall"
               : "=a"(ax)
               : "0"(431), "D"(opt_out_state)
               : "memory", "cc");
  unreachable;
}

static inline int thr_new(struct thr_param *param, int param_size) {
  bool failed;
  long ax, di, si;
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax), "=D"(di), "=S"(si)
               : "1"(455), "2"(param), "3"(param_size)
               : "rcx", "rdx", "r8", "r9", "r10", "r11", "memory");
  if (failed) ax = -ax;
  return ax;
}

static inline int thr_kill(int64_t id, int sig) {
  bool failed;
  long ax, di, si;
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax), "=D"(di), "=S"(si)
               : "1"(433), "2"(id), "3"(sig)
               : "rcx", "rdx", "r8", "r9", "r10", "r11", "memory");
  if (failed) ax = -ax;
  return ax;
}

static inline int thr_suspend(const struct timespec *opt_timeout) {
  bool failed;
  long ax, di;
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax), "=D"(di)
               : "1"(442), "2"(opt_timeout)
               : "rcx", "rdx", "rsi", "r8", "r9", "r10", "r11", "memory");
  if (failed) ax = -ax;
  return ax;
}

static inline int thr_wake(int64_t id) {
  bool failed;
  long ax, di;
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax), "=D"(di)
               : "1"(443), "2"(id)
               : "rcx", "rdx", "rsi", "r8", "r9", "r10", "r11", "memory");
  if (failed) ax = -ax;
  return ax;
}

static inline int thr_set_name(int64_t id, const char *name) {
  bool failed;
  long ax, di, si;
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax), "=D"(di), "=S"(si)
               : "1"(464), "2"(id), "3"(name)
               : "rcx", "rdx", "r8", "r9", "r10", "r11", "memory");
  if (failed) ax = -ax;
  return ax;
}

static inline int thr_kill2(int pid, int64_t id, int sig) {
  bool failed;
  long ax, di, si, dx;
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax), "=D"(di), "=S"(si),
                 "=d"(dx)
               : "1"(481), "2"(pid), "3"(id), "4"(sig)
               : "rcx", "r8", "r9", "r10", "r11", "memory");
  if (failed) ax = -ax;
  return ax;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_FREEBSD_INTERNAL_H_ */
