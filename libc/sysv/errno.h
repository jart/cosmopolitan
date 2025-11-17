#ifndef COSMOPOLITAN_LIBC_INTRIN_ERRNO_H_
#define COSMOPOLITAN_LIBC_INTRIN_ERRNO_H_
COSMOPOLITAN_C_START_

int __errno_bsd2linux(int);
int __errno_freebsd2linux(int);
int __errno_host2linux(int);
int __errno_netbsd2linux(int);
int __errno_openbsd2linux(int);
int __errno_windows2linux(int);
int __errno_xnu2linux(int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_ERRNO_H_ */
