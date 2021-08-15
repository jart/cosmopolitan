#ifndef COSMOPOLITAN_LIBC_CALLS_MOUNT_H_
#define COSMOPOLITAN_LIBC_CALLS_MOUNT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int mount(const char *, const char *, const char *, unsigned long,
          const void *);
int unmount(const char *, int);

#ifdef _GNU_SOURCE
int umount(const char *);
int umount2(const char *, int);
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_MOUNT_H_ */
