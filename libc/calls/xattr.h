#ifndef COSMOPOLITAN_LIBC_CALLS_XATTR_H_
#define COSMOPOLITAN_LIBC_CALLS_XATTR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

ssize_t flistxattr(int, char *, size_t);
ssize_t fgetxattr(int, const char *, void *, size_t);
int fsetxattr(int, const char *, const void *, size_t, int);
int fremovexattr(int, const char *);
ssize_t listxattr(const char *, char *, size_t);
ssize_t getxattr(const char *, const char *, void *, size_t);
int setxattr(const char *, const char *, const void *, size_t, int);
int removexattr(const char *, const char *);
ssize_t llistxattr(const char *, char *, size_t);
ssize_t lgetxattr(const char *, const char *, void *, size_t);
int lsetxattr(const char *, const char *, const void *, size_t, int);
int lremovexattr(const char *, const char *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_XATTR_H_ */
