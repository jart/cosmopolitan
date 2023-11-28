#ifndef COSMOPOLITAN_LIBC_CALLS_PRCTL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_PRCTL_INTERNAL_H_
#include "libc/dce.h"
#include "libc/sysv/consts/nrlinux.h"
COSMOPOLITAN_C_START_

forceinline int sys_prctl(int op, long a, long b, long c, long d) {
  int rc;
#ifdef __x86_64__
  register long r10 asm("r10") = c;
  register long r8 asm("r8") = d;
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(__NR_linux_prctl), "D"(op), "S"(a), "d"(b), "r"(r10),
                 "r"(r8)
               : "rcx", "r11", "memory");
#elif defined(__aarch64__)
  register long r0 asm("x0") = op;
  register long r1 asm("x1") = a;
  register long r2 asm("x2") = b;
  register long r3 asm("x3") = c;
  register long r4 asm("x4") = d;
  register long res_x0 asm("x0");
  asm volatile("mov\tx8,%1\n\t"
               "svc\t0"
               : "=r"(res_x0)
               : "i"(__NR_linux_prctl), "r"(r0), "r"(r1), "r"(r2), "r"(r3),
                 "r"(r4)
               : "x8", "memory");
  rc = res_x0;
#endif
  return rc;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_PRCTL_INTERNAL_H_ */
