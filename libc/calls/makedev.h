#ifndef COSMOPOLITAN_LIBC_CALLS_MAKEDEV_H_
#define COSMOPOLITAN_LIBC_CALLS_MAKEDEV_H_
COSMOPOLITAN_C_START_

uint32_t major(uint64_t) pureconst;
uint32_t minor(uint64_t) pureconst;
uint64_t makedev(uint32_t, uint32_t) pureconst;

#define major   major
#define minor   minor
#define makedev makedev

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_MAKEDEV_H_ */
