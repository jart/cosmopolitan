#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

enum FdKind {
  kFdEmpty,
  kFdFile,
  kFdSocket,
  kFdProcess,
  kFdConsole,
  kFdSerial,
  kFdZip,
  kFdEpoll,
  kFdReserved
};

struct Fd {
  enum FdKind kind;
  unsigned flags;
  unsigned mode;
  int64_t handle;
  int64_t extra;
  bool zombie;
};

struct Fds {
  _Atomic(int) f; /* lowest free slot */
  size_t n;
  struct Fd *p, *e;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_ */
