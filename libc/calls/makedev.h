#ifndef COSMOPOLITAN_LIBC_CALLS_MAKEDEV_H_
#define COSMOPOLITAN_LIBC_CALLS_MAKEDEV_H_

uint64_t makedev(uint32_t, uint32_t) libcesque;
uint32_t major(uint64_t) libcesque;
uint32_t minor(uint64_t) libcesque;

#define major(x)      major(x)
#define minor(x)      minor(x)
#define makedev(x, y) makedev(x, y)

#endif /* COSMOPOLITAN_LIBC_CALLS_MAKEDEV_H_ */
