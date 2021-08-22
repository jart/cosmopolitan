#ifndef COSMOPOLITAN_LIBC_CALLS_MAKEDEV_H_
#define COSMOPOLITAN_LIBC_CALLS_MAKEDEV_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

uint64_t makedev(uint32_t, uint32_t);
uint32_t major(uint64_t);
uint32_t minor(uint64_t);

#define major(x)      major(x)
#define minor(x)      minor(x)
#define makedev(x, y) makedev(x, y)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_MAKEDEV_H_ */
