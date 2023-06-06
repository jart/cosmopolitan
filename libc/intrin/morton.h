#ifndef COSMOPOLITAN_LIBC_BITS_MORTON_H_
#define COSMOPOLITAN_LIBC_BITS_MORTON_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
#ifdef COSMO

#define morton   __morton
#define unmorton __unmorton

unsigned long morton(unsigned long, unsigned long) libcesque;
axdx_t unmorton(unsigned long) libcesque;

#endif /* COSMO */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_MORTON_H_ */
