#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_FIO_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_FIO_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const uint32_t FIONREAD; /* one of the few encouraged ioctls */
extern const uint32_t FIONBIO;  /* use fcntl(fd, F_SETFL, O_NONBLOCK) */
extern const uint32_t FIOCLEX;  /* use fcntl(fd, F_SETFD, FD_CLOEXEC) */
extern const uint32_t FIONCLEX; /* use fcntl(fd, F_SETFD, 0) */
extern const uint32_t FIOASYNC; /* todo: fcntl(fd, F_SETOWN, pid) */

#define FIONREAD FIONREAD

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_FIO_H_ */
