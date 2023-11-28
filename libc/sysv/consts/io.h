#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_IO_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_IO_H_
COSMOPOLITAN_C_START_

#define _IOC(a, b, c, d) (((a) << 30) | ((b) << 8) | (c) | ((d) << 16))
#define _IOC_NONE        0U
#define _IOC_WRITE       1U
#define _IOC_READ        2U

#define _IO(a, b)      _IOC(_IOC_NONE, (a), (b), 0)
#define _IOW(a, b, c)  _IOC(_IOC_WRITE, (a), (b), sizeof(c))
#define _IOR(a, b, c)  _IOC(_IOC_READ, (a), (b), sizeof(c))
#define _IOWR(a, b, c) _IOC(_IOC_READ | _IOC_WRITE, (a), (b), sizeof(c))

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_IO_H_ */
