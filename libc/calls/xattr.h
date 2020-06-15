#ifndef COSMOPOLITAN_LIBC_CALLS_XATTR_H_
#define COSMOPOLITAN_LIBC_CALLS_XATTR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

ssize_t flistxattr(int filedes, char *list, size_t size);
ssize_t fgetxattr(int filedes, const char *name, void *value, size_t size);
int fsetxattr(int filedes, const char *name, const void *value, size_t size,
              int flags);
int fremovexattr(int filedes, const char *name);

ssize_t listxattr(const char *path, char *list, size_t size);
ssize_t getxattr(const char *path, const char *name, void *value, size_t size);
int setxattr(const char *path, const char *name, const void *value, size_t size,
             int flags);
int removexattr(const char *path, const char *name);

ssize_t llistxattr(const char *path, char *list, size_t size);
ssize_t lgetxattr(const char *path, const char *name, void *value, size_t size);
int lsetxattr(const char *path, const char *name, const void *value,
              size_t size, int flags);
int lremovexattr(const char *path, const char *name);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_XATTR_H_ */
