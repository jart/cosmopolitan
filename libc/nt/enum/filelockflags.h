#ifndef COSMOPOLITAN_LIBC_NT_ENUM_FILELOCKFLAGS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_FILELOCKFLAGS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtFileLockFlags {
  kNtLockfileFailImmediately = 1,
  kNtLockfileExclusiveLock = 2,
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_FILELOCKFLAGS_H_ */
