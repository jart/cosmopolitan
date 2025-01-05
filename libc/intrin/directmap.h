#ifndef COSMOPOLITAN_LIBC_INTRIN_DIRECTMAP_H_
#define COSMOPOLITAN_LIBC_INTRIN_DIRECTMAP_H_
COSMOPOLITAN_C_START_

void *sys_mmap_metal(void *, size_t, int, int, int, int64_t) libcesque;
int sys_munmap_metal(void *, size_t) libcesque;
int __prot2nt(int, int) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_DIRECTMAP_H_ */
